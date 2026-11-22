# Adreno — otimizações e compatibilidade

- Considerar driver do sistema e Turnip caminhos distintos e opcionais; não trocar driver de outras GPUs.
- Validar formatos ASTC/ETC2, precisão, extensões Vulkan, sampler e sincronização.
- SGSR v1 é de origem Qualcomm, mas requer benchmark com driver e resolução reais.
- Testar pipelines/cache de cada driver em namespaces separados; fallback ao sistema após falha de driver opcional.
- Não usar nomes de modelos como substitutos de vkGetPhysicalDeviceFeatures/Properties.
- Comparar P95/P99, GPU time, consumo e estabilidade.
