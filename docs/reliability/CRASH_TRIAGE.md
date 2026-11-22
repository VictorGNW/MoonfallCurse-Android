# Triagem de crashes Android

Registrar build/commit, ABI, Android, driver, Vulkan API, stack/logcat e último evento gráfico. Classificar:
- Inicialização/Forge/ROM/asset.
- Vulkan instance/device/swapchain.
- Shader/PICA/pipeline.
- Memória e resource lifetime.
- Pausa/retomada de Activity.
- Sistema ou crash sem causa identificada.

Primeiro revisar logs e ocorrências anteriores, depois isolar um teste mínimo determinístico. Não executar gameplay sem pedido do usuário.
