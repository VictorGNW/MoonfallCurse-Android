# Compatibilidade de GPUs — matriz inicial

| Arquitetura | Evidência | Situação |
|---|---|---|
| Mali-G615 MC2, driver r44p1 | Edge 60, Android 17, Vulkan 1.3.247 | Jogo inicia; hitches e shader misses a investigar |
| Adreno 619, Vulkan 1.1.128 | Dispositivo disponível para teste | Sem validação confirmada; depende de Legacy ou compatibilidade efetiva |
| Xclipse 540 | Relato de crash | Causa desconhecida; investigar logs e extensão/feature em uso |
| Outras GPUs | Sem dados | Não testado; não declarar incompatível |

Consultar Vulkan features/extensions/limits/formats em vez de hardcode por modelo; isolar workarounds por driver. Preservar caminhos rápidos em dispositivos potentes. Atualizar cada linha com build, driver e logs antes de afirmar compatibilidade.
