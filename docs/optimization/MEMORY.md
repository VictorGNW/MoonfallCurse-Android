# Gerenciamento de memória Vulkan

- Monitorar RSS, heaps, buffers, texturas, número de alocações e caches.
- Reutilizar e subalocar quando apropriado, respeitando sincronização.
- Aplicar limites para caches, sem liberar recursos ativos.
- Evitar criar/destruir grandes recursos a cada frame ou resize.
- Diferenciar pressão de RAM do limite de número de alocações.
- Preservar dados e estabilidade em dispositivos com memória menor.
