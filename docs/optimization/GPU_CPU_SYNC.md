# Sincronização CPU/GPU

- Investigar vkQueueSubmit, fences, semáforos, barreiras e waits em caminhos críticos.
- Não usar vkDeviceWaitIdle por frame salvo justificativa funcional.
- Validar ownership de recursos e momento de destruição/reutilização.
- Diferenciar tempo CPU da chamada Vulkan de duração real de execução GPU.
- Evitar alterações que reduzam barriers mas introduzam data races, layout incorreto ou corrupção.
- Medir antes/depois e preservar drivers menos tolerantes a bugs de sincronização.
