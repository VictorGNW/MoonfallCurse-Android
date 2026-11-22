# Negociação de capabilities Vulkan

- Consultar API version, extensions, VkPhysicalDeviceFeatures/Properties, limits, formatos e features associadas.
- Decidir pipeline, shaders, sampler, formatos e sincronização por capabilities, não apenas nome da GPU.
- Registrar caminho selecionado e motivo do fallback, sem vazar informações pessoais.
- Features opcionalmente habilitadas exigem suporte no physical device e extensões quando aplicável.
- Workarounds por driver/vendor só quando a detecção de capacidades for insuficiente e houver evidência de bug.
- Modern e Legacy devem preservar seus contratos separadamente.
