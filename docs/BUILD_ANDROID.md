# Build Android — guia de validação

Não inventar comandos; ler Gradle, CMake e scripts da branch antes de executar.
1. Identificar módulo afetado e dependências Forge, Java/Kotlin e C++.
2. Preferir teste unitário e compilação incremental ao build completo.
3. Build e instalação de APK somente quando a alteração impactar integração Android.
4. Usar chave de assinatura persistente existente; segredos fora do Git.
5. Preservar applicationId, ABI arm64-v8a, assets, atualização e saves.
6. Registrar commit, ferramentas, comando, resultado e APK apenas se realmente produzido.

Não iniciar sessões de gameplay para validar mudanças sem solicitação.
