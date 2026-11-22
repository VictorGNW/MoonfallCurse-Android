# Frame pacing Android

- Determinar quem já controla o ritmo: SDL, runtime, NRI, apresentação Vulkan ou Swappy.
- Nunca adicionar pacers simultâneos sem remover/conciliar o responsável atual.
- Separar simulação, renderização, frame generation e apresentação para não alterar velocidade de jogo.
- Medir tempo entre apresentações e latência de input, não apenas FPS médio.
- Avaliar extensões e Swappy só se integração e testes mostrarem benefício.
Fonte: https://developer.android.com/games/sdk/frame-pacing/
