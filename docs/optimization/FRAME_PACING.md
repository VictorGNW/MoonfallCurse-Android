# Frame pacing — princípios

- Medir frame interval apresentado e tempo de execução CPU/GPU separadamente.
- Evitar busy-wait, múltiplos limitadores e fila de frames maior que o necessário.
- Reduzir bloqueios causados por compilação, I/O e locks na thread gráfica.
- Recuperar pacing após hitch sem acelerar incorretamente a simulação.
- Comparar P50/P95/P99, contagem de hitches e input latency em cenários equivalentes.
