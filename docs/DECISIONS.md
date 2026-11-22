# Registro de decisões arquiteturais

## Decisões atuais / diretrizes
- Compatibilidade e frame pacing sustentado vêm antes do FPS médio isolado.
- Edge 60 é dispositivo de referência, não alvo exclusivo.
- SGSR v1 EdgeDir é a proposta principal de upscaling; manter fallback convencional até medir em várias GPUs.
- Renderizador Legacy Vulkan 1.1 deverá ser independente do moderno e negociar recursos reais.
- Preservar caminhos modernos e correções específicas de drivers isoladas.
- Evitar aumentar threads e pré-compilação sem revisar lifecycle e consumo.
- Não executar gameplay autônomo sem solicitação.
- Preservar Gradle, Forge, assinatura e saves.

Ao tomar nova decisão, registrar data/commit, problema, opções, evidência, trade-offs e condições de reversão.
