# Arquitetura — mapa conceitual

Android Activity/SDL → runtime ARM64 recompilado → PICA/renderer 3DS → NRI/Vulkan → driver Vulkan (sistema ou customizado quando suportado).

## Caminhos para conferir no código
- ports/android/app: Activity, UI, instalador e configurações Android.
- ports/android/runtime: inicialização nativa e carregamento de Vulkan.
- runtime/three_ds_recomp: PICA, shaders, backends e gerenciamento de recursos.
- tools/android: staging e build.

## Contratos a verificar
SDL/NRI/Vulkan devem compartilhar o driver selecionado; selecionar antes de carregar bibliotecas. Caches de driver e shader precisam de identificação de compatibilidade. Configuração inválida não deve causar loop de crashes. Documentar owners de dispositivos, threads e sincronização quando confirmados.
