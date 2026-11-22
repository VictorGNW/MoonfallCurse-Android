# Estado do desenvolvimento — referência de 2026-11-22

## Evidência histórica dos diagnósticos
- Motorola Edge 60, Android 17, Dimensity 7300, Mali-G615 MC2, Vulkan 1.3.247, driver r44p1.
- Build 0.5.0-alpha.2 (500102), Forge instalado, primeiro frame em ~4,8 s.
- Sessões de ~126 e ~250 s terminadas pelo usuário, sem crash registrado nesses logs.
- Misses TRIAEVUM_FRAGMENT_ARTIFACT_MISS e Native PICA AOT.
- Frame pacing ~33,33 ms, hitches de até ~114,9 ms e submit máximo ~275 ms.
- Flush NRI adiado ~171 MB acima de limite ~134 MB; RSS ~1,7–2,1 GiB.
- Avisos guest patch, fullscreen SDL, máscaras de grama e materiais de reflexo.
- Adreno 619 (Vulkan 1.1.128): teste planejado, sem resultado confirmado.
- Xclipse 540: crash reportado, causa não estabelecida.

## Planejado; não declarar como pronto
- SGSR v1 com EdgeDir e fallback de escala convencional.
- Preset Enhanced for Mobile, SMAA/FXAA e frame generation 2x.
- Renderizador Legacy Vulkan 1.1 independente.
- Integrações opcionais de driver Turnip de PR externa.

Para atualizar: registrar commit, branch, versão, dispositivo, driver, cenário, tempo, evidência e limitações. Não assumir correlação como causalidade.
