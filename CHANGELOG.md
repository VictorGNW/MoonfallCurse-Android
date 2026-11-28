# Changelog do Moonfall Curse Android

## 0.5.0-alpha.2 · Teste Turnip R5

Pré-release ARM64 para Android 10 ou posterior com Vulkan compatível. As mudanças abaixo comparam esta build com a [0.5.0-alpha.2](https://github.com/VictorGNW/MoonfallCurse-Android/releases/tag/v0.5.0-alpha.2).

### Gráficos e preset móvel

- **SGSR v1 Edge Direction** como opção independente do antisserrilhado. O renderizador Vulkan amplia a cena 3D antes de compor o HUD quando o caminho é compatível. A composição convencional continua disponível.
- **Enhanced for Mobile** revisado. Usa escala interna de 80% como base e mira 720 linhas internas quando as dimensões e os limites de escala permitem. Combina SGSR com SMAA 1×, usa FXAA quando o SMAA não está disponível e mantém a interpolação 2×, o FOV de 1,10× e a distância de desenho estendida. Reaplique o preset para adotar os novos valores se já tiver configurações salvas.
- **Antisserrilhado simplificado no Android.** O menu oferece Desligado, FXAA e SMAA 1×. Configurações antigas de MSAA e TAA são migradas para FXAA.
- **Filtragem de texturas** em um único interruptor. Quando ativada, usa até 8× de anisotropia conforme a capacidade da GPU e até quatro níveis de mipmap disponíveis na textura.

### Turnip e compatibilidade Vulkan

- O instalador permite escolher o driver do sistema, o Turnip integrado em Adreno série 600 compatível ou um ZIP Turnip importado em Adreno compatível. O APK desta pré-release inclui **Mesa Turnip 26.3.0 R5**.
- SDL, NRI e as chamadas Vulkan do runtime usam o carregador selecionado. Os caches do driver do sistema e do Turnip ficam separados. Se uma tentativa com Turnip falhar antes do primeiro quadro, a próxima abertura volta ao driver do sistema.
- A recriação da cadeia de apresentação pode recuperar a superfície Android perdida. O runtime limita as consultas repetidas quando o compositor reporta `SUBOPTIMAL`; redimensionamentos e `OUT_OF_DATE` continuam sendo tratados. Transferências, profundidade transitória e VRS seguem as capacidades consultadas no dispositivo.

### Pacotes de texturas

- Importação de texturas DDS BC1, BC2, BC3, BC4, BC5 e BC7 com preservação dos mipmaps fornecidos e da indicação sRGB em cabeçalhos DX10.
- Transcodificação de texturas de cor DDS para **ASTC 4×4** habilitada por padrão no Android quando o driver Vulkan 1.2 ou posterior oferece os recursos exigidos, incluindo ASTC, FP16 e Int8. O caminho RGBA8 é usado quando a transcodificação está indisponível ou foi desligada.
- O Forge oferece a opção **Desativar transcodificação BCN caso ter problemas com texturas**. A mudança vale na próxima abertura do jogo.

### Shaders, cache e memória

- A gravação periódica do cache Vulkan do NRI espera novos pipelines e respeita o intervalo mínimo. O limite de tamanho é aplicado antes da alocação do snapshot, o último cache compatível é preservado e a gravação é pulada após perda do dispositivo.
- A consulta aos artefatos de shader e ao pacote AOT reaproveita a identidade do shader. O diagnóstico registra separadamente em qual camada ocorreu a falta de cache.
- O preparo opcional de shaders no Forge verifica a memória disponível antes de iniciar o Vulkan, libera o buffer de importação do cache antes de iniciar os workers e usa até dois workers por padrão no Android. A preparação continua desmarcada por padrão em instalações novas.

### Build e validação

- Corrigida a seleção do DXC para compilar shaders NRI de Android no Linux x86-64. Os scripts de release agora validam o certificado Android esperado antes de empacotar e verificam a assinatura após gerar o APK.
- Build do [código consolidado do PR #10](https://github.com/VictorGNW/MoonfallCurse-Android/pull/10) no commit [`7cf2bd24`](https://github.com/VictorGNW/MoonfallCurse-Android/commit/7cf2bd244dd221dbbfcb1a2aa95bfe8c87969941). O APK passou na auditoria Android e usa o mesmo certificado da versão pública anterior. O pacote Forge também foi assinado.
- A validação desta build em Mali, Adreno e Xclipse permanece pendente. O suporte à ROM USA v1.1 para a receita Forge atual segue no [PR #2](https://github.com/VictorGNW/MoonfallCurse-Android/pull/2).

## 0.5.0-alpha.2

As mudanças anteriores, incluindo o layout de toque adaptativo, o modo Original 30 FPS e o reaproveitamento do pacote de shaders, estão nas [notas da pré-release 0.5.0-alpha.2](https://github.com/VictorGNW/MoonfallCurse-Android/releases/tag/v0.5.0-alpha.2).
