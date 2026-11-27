#!/usr/bin/env bash
set -euo pipefail
umask 077

repo=VictorGNW/MoonfallCurse-Android
if [[ $# != 3 || $1 != --reuse ]]; then
    echo "Usage: $0 --reuse EXISTING_KEYSTORE BACKUP_PATH" >&2
    exit 2
fi
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

pinned=$(<"$repo_root/ports/android/signing/mm3d-apk-cert.sha256")
[[ -f "$keystore" && "$pinned" =~ ^[0-9a-f]{64}$ ]] || {
    echo "The release keystore and pinned signing certificate are required." >&2
    exit 2
}
gh auth status --hostname github.com >/dev/null 2>&1 || {
    echo "Use an authenticated GitHub CLI session with permission to configure this repository." >&2
    exit 1
}

read -r -p 'Signing key alias: ' TRIAEVUM_ANDROID_KEY_ALIAS
read -rs -p 'Keystore password (keep in your password manager): ' TRIAEVUM_ANDROID_KEYSTORE_PASS
printf '\n' >&2
[[ -n "$TRIAEVUM_ANDROID_KEY_ALIAS" && -n "$TRIAEVUM_ANDROID_KEYSTORE_PASS" ]] || exit 2
export TRIAEVUM_ANDROID_KEYSTORE_PASS
read -rs -p 'Private key password (Enter to use the keystore password): ' TRIAEVUM_ANDROID_KEY_PASS
printf '\n' >&2
TRIAEVUM_ANDROID_KEY_PASS=${TRIAEVUM_ANDROID_KEY_PASS:-$TRIAEVUM_ANDROID_KEYSTORE_PASS}
export TRIAEVUM_ANDROID_KEY_PASS

# A CSR proves the password unlocks the private key, without exporting it.
keytool -certreq -keystore "$keystore" -alias "$TRIAEVUM_ANDROID_KEY_ALIAS" \
    -storepass:env TRIAEVUM_ANDROID_KEYSTORE_PASS -keypass:env TRIAEVUM_ANDROID_KEY_PASS >/dev/null
fingerprint=$(keytool -exportcert -keystore "$keystore" -alias "$TRIAEVUM_ANDROID_KEY_ALIAS" \
    -storepass:env TRIAEVUM_ANDROID_KEYSTORE_PASS | sha256sum | cut -d ' ' -f1)
if [[ "$fingerprint" != "$pinned" ]]; then
    echo "This keystore does not match the published Moonfall Curse APK." >&2
    exit 1
fi
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

base64 < "$backup" | tr -d '\n' | gh secret set TRIAEVUM_ANDROID_KEYSTORE_B64 --repo "$repo"
printf '%s' "$TRIAEVUM_ANDROID_KEYSTORE_PASS" | gh secret set TRIAEVUM_ANDROID_KEYSTORE_PASS --repo "$repo"
printf '%s' "$TRIAEVUM_ANDROID_KEY_ALIAS" | gh secret set TRIAEVUM_ANDROID_KEY_ALIAS --repo "$repo"
printf '%s' "$TRIAEVUM_ANDROID_KEYSTORE_PASS" | gh secret set TRIAEVUM_ANDROID_KEY_PASS --repo "$repo"
gh variable set TRIAEVUM_ANDROID_SIGNER_SHA256 --repo "$repo" --body "$fingerprint"
printf 'Android signing certificate SHA-256: %s\n' "$fingerprint"
