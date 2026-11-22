# Recuperação segura Vulkan

- Registrar VkResult e etapa (instance/device/surface/swapchain/pipeline/present).
- Tratar VK_ERROR_OUT_OF_DATE_KHR e SUBOPTIMAL_KHR de acordo com o ciclo de vida da swapchain.
- Falhas de shader/feature opcional devem permitir fallback, sem tela preta.
- VK_ERROR_DEVICE_LOST requer tratamento explícito, sem presumir que o dispositivo pode ser recuperado automaticamente.
- Evitar loops infinitos de reconfiguração.
- Configuração segura não deve apagar saves ou caches válidos.
