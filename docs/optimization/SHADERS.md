# Shaders — revisão de custo e correção

- Identificar falhas de preparação PICA/SPIR-V e contagem de misses.
- Evitar recompilação desnecessária; verificar identidade e invalidação de artefatos.
- Considerar precisão mediump/FP16 somente com suporte e equivalência visual suficiente.
- Medir tempo CPU de geração/compilação e GPU de execução separadamente.
- Preservar fallback caso compilação antecipa falhe.
