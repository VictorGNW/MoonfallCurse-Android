# Frame generation 2x — requisitos de segurança

**Status:** recurso planejado; não marcar como implementado.
- Separar FPS da simulação, renderização e apresentação.
- Medir latência de toque, pacing e impacto térmico; FPS gerado não equivale a resposta nativa.
- Desligar geração se não houver base FPS estável ou se provocar artefatos e jank.
- Evitar duplicar pacers ou superfícies sem controle de sincronização.
- Preservar imagem real, HUD e apresentação correta ao pausar/retomar.
- Validar sem gameplay autônomo, utilizando testes curtos determinísticos ou logs fornecidos.
