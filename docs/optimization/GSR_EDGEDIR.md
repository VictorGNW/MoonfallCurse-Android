# GSR EdgeDir — SGSR v1

**Status: proposta; ainda não validado no MoonfallCurse-Android.**
Referência: https://github.com/SnapdragonGameStudios/snapdragon-gsr/tree/main/sgsr/v1

SGSR v1 é um upscaler espacial com sharpening integrado num único pass. Edge Direction ajusta amostragem segundo direção das bordas, mas pode aumentar custo e diminuir ocupação; otimização Adreno não garante vantagem em Mali ou Xclipse.

## Integração proposta
1. Renderizar a cena 3D na resolução interna configurada.
2. Aplicar AA de acordo com ordem validada (SMAA/FXAA/desligado).
3. Executar SGSR EdgeDir num pass de upscale, sem sincronização redundante ou intermediários injustificados.
4. Compor HUD/texto/controles touch na resolução final, quando arquitetura permitir.
5. Apresentar pelo pacer já existente.

Conferir formatos, espaço de cor, precisão, layouts, extensões, parâmetros e versão exata do shader. Preservar escala convencional quando o shader ou driver falhar. Avaliar FP16 só com suporte e ganho comprovados.

## Validação
Comparar saída nativa, escala convencional e EdgeDir com mesma resolução de entrada/saída, cena, AA e temperatura. Medir tempo do pass, P95/P99, energia, uso de memória, artefatos em vegetação/diagonais, halos e shimmering. Validar Adreno stock/Turnip quando disponíveis, Mali e Xclipse separadamente.

Não impor configurações gerais baseadas apenas no Edge 60. Antes de importar shader oficial, conferir licença BSD-3-Clause e avisos de terceiros (THIRD_PARTY_NOTICES.md).
