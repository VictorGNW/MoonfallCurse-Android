# Renderizador Legacy Vulkan 1.1 — proposta separada

**Status: planejado, não implementado/validado.**

Objetivo: ampliar compatibilidade mantendo o renderizador moderno intacto.

- Enumerar features, extensões, formatos e limites reais; versão 1.1 isolada não determina potência da GPU.
- Criar caminhos alternativos para recursos modernos ausentes e falhar com mensagem clara quando inviável.
- Desenvolver por etapas: inicialização, surface/swapchain, frame mínimo, primeiro frame, pause/resume e caches.
- Não usar gameplay autônomo como teste.
- Frame generation 2x inicialmente desativado, com reavaliação por capacidade, estabilidade e custo.
- SGSR v1 EdgeDir pode ser considerado se recursos e shaders forem compatíveis; não proibir somente por Vulkan 1.1, mas manter escala convencional por padrão.
- Avaliar AA leve (FXAA/SMAA) por custo mensurado; preservar caminhos modernos em GPUs capazes.

Referência de investigação: Adreno 619 Vulkan 1.1.128, sem validação aqui.
