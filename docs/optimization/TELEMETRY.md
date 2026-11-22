# Telemetria local, leve e privada

Registrar dados técnicos agregados: API Vulkan, driver, configuração, frame times, misses, alocações, cache e erro. Não coletar identificadores pessoais, saves nem ROM.
- Contadores baratos durante o frame loop; logs detalhados sob opção de diagnóstico.
- Usar relógio monotônico e unidades explícitas.
- Separar evidência de CPU, GPU e apresentação.
- Preferir resumo por sessão e gatilhos de hitch para reduzir overhead.
- Não atribuir causa antes de medir simultaneamente eventos relacionados.
