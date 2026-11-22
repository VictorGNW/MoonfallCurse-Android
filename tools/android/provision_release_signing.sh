#!/usr/bin/env bash
set -euo pipefail
umask 077

repo=VictorGNW/MoonfallCurse-Android
if [[ $# != 3 || ( $1 != --reuse && $1 != --generate ) ]]; then
    echo "Usage: $0 --reuse EXISTING_KEYSTORE BACKUP_PATH" >&2
    echo "       $0 --generate NEW_KEYSTORE BACKUP_PATH" >&2
    exit 2
fi
mode=$1
repo_root=$(git -C "$(dirname "$0")" rev-parse --show-toplevel)
keystore=$(realpath -m -- "$2")
backup=$(realpath -m -- "$3")
for path in "$keystore" "$backup"; do
    case "$path" in
        "$repo_root"|"$repo_root/"*) echo "Use keystore and backup paths outside the checkout." >&2; exit 2 ;;
    esac
done
[[ "$keystore" != "$backup" && ! -e "$backup" ]] || { echo "Choose a new backup path." >&2; exit 2; }
[[ -d "$(dirname "$backup")" ]] || { echo "Create the private backup directory first." >&2; exit 2; }

secrets=$(gh secret list --repo "$repo" --app actions --json name --jq '.[].name')
if grep -Eq '^TRIAEVUM_ANDROID_(KEYSTORE_B64|KEYSTORE_PASS|KEY_ALIAS|KEY_PASS)$' <<<"$secrets"; then
    echo "Android signing secrets already exist. Inspect the existing configuration before modifying them." >&2
    exit 1
fi
variables=$(gh variable list --repo "$repo" --json name --jq '.[].name')
if grep -Fxq TRIAEVUM_ANDROID_SIGNER_SHA256 <<<"$variables"; then
    echo "A permanent signing fingerprint already exists. Recover that key before provisioning." >&2
    exit 1
fi

read -r -p 'Signing key alias: ' TRIAEVUM_ANDROID_KEY_ALIAS
read -rs -p 'Keystore password (keep in your password manager): ' TRIAEVUM_ANDROID_KEYSTORE_PASS
printf '\n' >&2
[[ -n "$TRIAEVUM_ANDROID_KEY_ALIAS" && -n "$TRIAEVUM_ANDROID_KEYSTORE_PASS" ]] || exit 2
export TRIAEVUM_ANDROID_KEYSTORE_PASS
if [[ "$mode" == --generate ]]; then
    [[ ! -e "$keystore" ]] || { echo "Keystore already exists." >&2; exit 2; }
    keytool -genkeypair -keystore "$keystore" -storetype PKCS12 \
        -storepass:env TRIAEVUM_ANDROID_KEYSTORE_PASS -alias "$TRIAEVUM_ANDROID_KEY_ALIAS" \
        -keyalg RSA -keysize 4096 -validity 36500 -dname 'CN=Moonfall Curse Android'
    TRIAEVUM_ANDROID_KEY_PASS=$TRIAEVUM_ANDROID_KEYSTORE_PASS
else
    [[ -f "$keystore" ]] || { echo "Keystore file missing." >&2; exit 2; }
    read -rs -p 'Private key password (Enter to use the keystore password): ' TRIAEVUM_ANDROID_KEY_PASS
    printf '\n' >&2
    TRIAEVUM_ANDROID_KEY_PASS=${TRIAEVUM_ANDROID_KEY_PASS:-$TRIAEVUM_ANDROID_KEYSTORE_PASS}
fi
export TRIAEVUM_ANDROID_KEY_PASS

# A CSR proves the password unlocks the private key, without exporting it.
keytool -certreq -keystore "$keystore" -alias "$TRIAEVUM_ANDROID_KEY_ALIAS" \
    -storepass:env TRIAEVUM_ANDROID_KEYSTORE_PASS -keypass:env TRIAEVUM_ANDROID_KEY_PASS >/dev/null
fingerprint=$(keytool -exportcert -keystore "$keystore" -alias "$TRIAEVUM_ANDROID_KEY_ALIAS" \
    -storepass:env TRIAEVUM_ANDROID_KEYSTORE_PASS | sha256sum | cut -d ' ' -f1)
# Preserve only the chosen private entry as an encrypted PKCS12 backup.
keytool -importkeystore -noprompt -srckeystore "$keystore" -srcalias "$TRIAEVUM_ANDROID_KEY_ALIAS" \
    -srcstorepass:env TRIAEVUM_ANDROID_KEYSTORE_PASS -srckeypass:env TRIAEVUM_ANDROID_KEY_PASS \
    -destkeystore "$backup" -deststoretype PKCS12 -deststorepass:env TRIAEVUM_ANDROID_KEYSTORE_PASS -destkeypass:env TRIAEVUM_ANDROID_KEYSTORE_PASS
chmod 600 "$backup"
backup_fingerprint=$(keytool -exportcert -keystore "$backup" -alias "$TRIAEVUM_ANDROID_KEY_ALIAS" \
    -storepass:env TRIAEVUM_ANDROID_KEYSTORE_PASS | sha256sum | cut -d ' ' -f1)
[[ "$backup_fingerprint" == "$fingerprint" ]]
keytool -certreq -keystore "$backup" -alias "$TRIAEVUM_ANDROID_KEY_ALIAS" \
    -storepass:env TRIAEVUM_ANDROID_KEYSTORE_PASS >/dev/null

base64 -w0 "$backup" | gh secret set TRIAEVUM_ANDROID_KEYSTORE_B64 --repo "$repo"
printf '%s' "$TRIAEVUM_ANDROID_KEYSTORE_PASS" | gh secret set TRIAEVUM_ANDROID_KEYSTORE_PASS --repo "$repo"
printf '%s' "$TRIAEVUM_ANDROID_KEY_ALIAS" | gh secret set TRIAEVUM_ANDROID_KEY_ALIAS --repo "$repo"
printf '%s' "$TRIAEVUM_ANDROID_KEYSTORE_PASS" | gh secret set TRIAEVUM_ANDROID_KEY_PASS --repo "$repo"
gh variable set TRIAEVUM_ANDROID_SIGNER_SHA256 --repo "$repo" --body "$fingerprint"
printf 'Android signing certificate SHA-256: %s\n' "$fingerprint"
