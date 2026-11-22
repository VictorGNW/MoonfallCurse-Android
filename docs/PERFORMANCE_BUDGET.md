# Orçamento e critérios de desempenho

**Metas de análise, não resultados obtidos.**
- Alvo de 30 FPS: 33,33 ms/frame; 60 FPS: 16,67 ms/frame.
- Coletar P50/P95/P99, hitches >1,5x e >3x orçamento, tempo de CPU e GPU quando disponível.
- Contabilizar shader misses, compilação de pipelines, submissão e espera da apresentação.
- Acompanhar alocações Vulkan simultâneas, memória, tamanho do cache e crescimento ao longo da sessão.
- Medir consumo sustentado e tendência térmica quando disponíveis.
- Não confundir FPS simulados, renderizados e apresentados, especialmente com frame generation.
- Comparar no mesmo cenário, resolução, aquecimento e driver; nunca generalizar uma GPU para as outras.
