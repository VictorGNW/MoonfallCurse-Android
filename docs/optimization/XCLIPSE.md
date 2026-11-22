# Samsung Xclipse — compatibilidade

- Investigar crash relatado da Xclipse 540 sem presumir que seja comum a toda a família.
- Registrar driver, versão Vulkan, extensões, formatos e etapa exata da falha.
- Validar SPIR-V, pipeline creation, sincronização e swapchain em logs quando disponíveis.
- Não aplicar workarounds globais por resultado de um modelo isolado.
- Manter fallback e preservar otimizações de Adreno/Mali.
