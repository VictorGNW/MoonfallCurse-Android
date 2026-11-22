# I/O e inicialização

- Medir etapas do startup até o primeiro frame; não assumir que todo atraso é shader.
- Evitar I/O síncrono em render/input threads; carregar e salvar caches em momentos seguros.
- Minimizar alocações repetidas e preparação redundante.
- Cache persistente deve ser atômico, versionado e verificável.
- Não aumentar tempo de inicialização excessivamente por warmup especulativo.
- Preservar funcionamento do Forge e caminhos Android de arquivos.
