# Pressão de memória Android

- Tratar pressão do sistema como evento dinâmico: liberar caches descartáveis, não conteúdo essencial.
- Separar RAM de processo, memória GPU e persistência em disco.
- Definir limites de cache sob hipóteses mensuráveis; evitar flush bloqueante na render thread.
- Verificar retomada de Activity após baixa memória e possíveis reinicializações.
- Preservar saves e configurações com gravações atômicas.
- Verificar orçamento em aparelhos de menor memória e não limitar dispositivos potentes sem necessidade.
