# Protocolo SGSR v1 EdgeDir

**A implementar e validar.** Comparar:
- Render nativo: cena em resolução final.
- Upscale convencional: mesma resolução interna que EdgeDir.
- SGSR EdgeDir: mesma cena, escala, AA, resolução e temperatura.

Coletar GPU time do pass e frame total, P95/P99, memória, compilação, consumo e qualidade em diagonais, vegetação, halos, texto e shimmer. Se falhar shader ou driver, manter fallback. Validar Adreno, Mali e Xclipse separadamente e respeitar licença BSD-3-Clause da implementação oficial.
