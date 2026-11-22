# Problemas conhecidos — observações históricas

- Edge 60: misses TRIAEVUM_FRAGMENT_ARTIFACT_MISS/Native PICA AOT.
- Hitches de até ~114,9 ms com pacing perto de 33,33 ms, origem não comprovada.
- Flush NRI adiado ~171 MB acima de limite ~134 MB; investigar sem assumir relação causal com os hitches.
- Avisos de guest patch, SDL fullscreen rejeitado, máscaras de grama e materiais de reflexão.
- Xclipse 540: crash reportado, sem correção validada.
- Adreno 619 Vulkan 1.1.128: compatibilidade ainda a testar.

Ver DEVELOPMENT_STATUS.md e upstream/UPSTREAM_BUGS.md. Registrar build/driver/log para cada atualização.
