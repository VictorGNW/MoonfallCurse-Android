# Renderizador moderno — contrato de preservação

Status: diretriz para revisar código existente; não é inventário confirmado de features.

- Manter o caminho moderno otimizado para recursos Vulkan disponíveis, meta 1.2+ sujeita à conferência do código.
- Validar capacidades reais e negociar extensões individuais, sem desativação global por um bug específico.
- Revisar a origem e ciclo de vida de VkDevice, swapchain, queues, buffers, command pools, NRI/SDL e caches.
- Evitar waits de device no frame loop sem justificativa; preservar sincronização correta.
- Compartilhar a mesma implementação do driver Vulkan entre consumidores, especialmente em cenários Turnip.
- Versionar caches para não reutilizar dados incompatíveis após atualização de shader, driver ou backend.
- Não impor SGSR EdgeDir como padrão antes de medição em Mali, Adreno e Xclipse.
- Preservar fidelidade gráfica, latência e frame pacing ao otimizar.
