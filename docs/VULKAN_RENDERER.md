# Vulkan/NRI — pontos de auditoria

Localizar no código: VkInstance/VkDevice, queues, swapchain, backend NRI, pipeline cache, descriptor pools, memória e apresentação.
- Seleção Vulkan por capacidades reais; fallback por recurso ausente.
- Reduzir barriers e waits redundantes sem sacrificar correctness.
- Separar driver de sistema, Turnip e versões de shader no cache.
- Preservar caminhos tile-based e modernos sem hardcodes por aparelho.
- Medir frame times, memória e correção visual antes de desativar qualquer feature.
- Tratar device loss ou erro recuperável com diagnóstico claro; não apagar saves.
