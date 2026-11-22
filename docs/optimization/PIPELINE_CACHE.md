# Pipeline cache

- Distinguir cache de driver, cache de shaders, artefatos PICA e pipeline NRI.
- Definir identidade de compatibilidade por GPU/driver, renderer, versão de shader e configurações relevantes.
- Persistir de modo atômico e com limites de tamanho; não aceitar dados corruptos.
- Evitar flush excessivo ou serialização bloqueante durante frames.
- Medir misses a frio/quente e custos de preparação.
- Não afirmar benefício sem resultados; manter fallback após invalidação.
