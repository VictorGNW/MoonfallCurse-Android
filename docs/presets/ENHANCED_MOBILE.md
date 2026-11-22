# Preset Enhanced for Mobile — proposta

**Não implementado/validado neste documento.**

| Recurso | Configuração inicialmente proposta |
|---|---|
| Resolução interna | 720p, ajustável por aspecto/densidade |
| Upscaler | SGSR v1 GSR EdgeDir, com fallback convencional |
| Anti-aliasing | SMAA; opções FXAA/desligado |
| Frame generation | 2x, opcional e sujeito a validação |
| Anisotrópica | 8x apenas se suportado e viável |
| Mipmaps | 4 níveis como hipótese; avaliar cadeia adequada por textura |
| Visão estendida | Ativa se custo e correção permitirem |
| HUD/touch | Preferencialmente na resolução final |

Não forçar em todas as GPUs nem confundir FPS gerado com FPS nativo. Preservar qualidade superior para GPUs potentes e opções simples para entrada. Medir P95/P99, consumo, memória, latência e qualidade visual em cenários equivalentes. Não habilitar opções fictícias.
