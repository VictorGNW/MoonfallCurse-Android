# Diagnóstico de stutter e frame pacing

Status: roteiro de medição, não conclusão sobre a causa.

1. Registrar commit, build, dispositivo/GPU/driver, modo Vulkan, preset, resolução, limite de FPS e cache frio/quente.
2. Separar tempo de simulation, render_cpu, submit_cpu, gpu_execution (se medível), present_wait e present_interval.
3. Coletar P50/P95/P99, contagem de hitches e eventos de shader miss, criação de pipeline e operações de cache.
4. Correlacionar eventos sem assumir causa. Uma chamada vkQueueSubmit demorada não prova tempo GPU alto.
5. Não somar tempos CPU e GPU como se fossem necessariamente sequenciais; há execução sobreposta.
6. Evitar logging por frame em builds normais; instrumentação detalhada é opt-in.

Hipóteses: hitch CPU pode vir de shader/cache/lock; hitch GPU de overdraw/bandwidth; intervalo irregular de present de pacer/vsync; degradação longa de pressão térmica/memória. Verificar cada hipótese com menor teste determinístico possível.

Caso histórico Edge 60: hitch de até ~114,9 ms e submit ~275 ms, ainda sem causalidade estabelecida.
