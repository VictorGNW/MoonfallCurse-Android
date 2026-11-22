# Compilação APK por GitHub Actions — artifact, sem nova release

Workflow: [Android APK artifact](../.github/workflows/android-apk-artifact.yml).

## Estado e limites

- A versão publicada mais recente em 22/11/2026 é `v0.5.0-alpha.2`. O branch `MM3D-Android` ainda declara `0.5.0-alpha.1`. Como os históricos divergem, por segurança o workflow usa a tag publicada como ref padrão; o campo `source_ref` permite escolher explicitamente outra branch/commit.
- O APK completo **não pode ser reconstruído só da árvore pública**: `tools/android/android_release.py` exige translated sources, registro A32 `a32_generated_overlay_r18` e pacote desktop de referência compatível. Não substituir esses itens por stubs.
- O workflow não cria GitHub Release, tag nem publica assets: apenas envia o APK resultante para o histórico da execução em **Actions > Artifacts** (retenção de 7 dias).
- A build não é qualificação de hardware: após gerar, instalar/testar no dispositivo apropriado com autorização do usuário.
- Em repositório público, artifacts do GitHub Actions não devem ser tratados como armazenamento privado de dados sigilosos. **Jamais incluir ROM, keys, conteúdo privado ou saves no upload**.

## Entradas privadas necessárias

Disponibilizar **fora do repositório** um ZIP do publisher acessível por URL HTTPS restrita; configurar `MM3D_PUBLISHER_BUNDLE_URL` como **GitHub Actions Secret**. Para autenticação por bearer token, configurar opcionalmente `MM3D_PUBLISHER_BUNDLE_TOKEN`. Pode-se configurar `MM3D_PUBLISHER_BUNDLE_SHA256` em **Actions Variables** para validar o download.

O ZIP deve conter somente três diretórios na raiz:

```text
translated-sources/  # traduzido do build correspondente do título
a32-registry/       # a32_generated_overlay_r18 correspondente
desktop-reference/  # pacote de release desktop correto, com manifesto e fontes exigidos
```

A distribuição de material de jogo protegido deve respeitar os direitos aplicáveis. Não armazenar ROMs nem credenciais nesse ZIP. Se os diretórios estiverem ausentes, o build termina antes de qualquer APK ser publicado.

### Assinatura permanente (APK)

Reutilizar os **Secrets** já usados em [android-signing-check.yml](../.github/workflows/android-signing-check.yml):
`TRIAEVUM_ANDROID_KEYSTORE_B64`, `TRIAEVUM_ANDROID_KEYSTORE_PASS`, `TRIAEVUM_ANDROID_KEY_ALIAS`, `TRIAEVUM_ANDROID_KEY_PASS`, e **Variable** `TRIAEVUM_ANDROID_SIGNER_SHA256`.

O pacote Forge exige **outra chave**, Ed25519, cuja chave pública já seja confiável ao Forge distribuído. Para o workflow, fornecer `TRIAEVUM_RELEASE_SIGNING_KEY_B64` em Secret; **não gerar uma chave nova** sem ajustar a confiança do instalador. Um APK assinado não basta quando o Forge package está sem assinatura válida.

A opção `unsigned_candidate` é apenas para inspeção técnica: não produz APK atualizável sobre a instalação assinada.

## Uso

1. Concluir Secrets, entradas de publisher e fonte desejada.
2. Abrir **Actions → Moonfall Curse Android APK (Actions artifact only) → Run workflow**.
3. Selecionar o ref de código correto; executar somente após mudanças que justifiquem compilação.
4. Quando passar, baixar o artifact `Moonfall-Curse-Android-*-arm64` na própria execução.
5. Conferir `BUILD_INFO.txt`, `SHA256SUMS.txt`, assinatura e logs. O comando de release já verifica o certificado em builds assinados.

A execução é propositalmente impedida de gastar minutos de build quando entradas ou assinaturas estiverem faltando. Não executar gameplay autônomo, nem disparar compilações completas por cada commit.
