# Ciclo de vida dos recursos Vulkan

- Inventariar allocations, VkBuffers, VkImages, descriptors e pipelines por cenário.
- Contabilizar número de alocações e bytes; GPU pode falhar ao atingir maxMemoryAllocationCount mesmo com VRAM livre.
- Reutilizar pools/suballocations apenas com sincronização correta.
- Garantir destruição após fences/timeline apropriadas; não usar recurso após liberação.
- Não invalidar caches durante uso pela GPU.
- Investigar crescimento durante sessões prolongadas sem iniciar gameplay autônomo.
- Relato relacionado: https://github.com/coccofresco/TriAevum/issues/160
