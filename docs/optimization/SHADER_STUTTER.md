# Stutters causados por shaders — hipótese

- Instrumentar misses e duração de criação de pipeline em relação ao frame time.
- Diferenciar compile, cache miss, I/O e GPU occupancy.
- Preparar antecipadamente apenas shaders comprovadamente necessários.
- Não criar mais threads sem revisar race conditions e ciclo de vida.
- Limitar warmup para evitar RAM/energia e demora excessiva na inicialização.
- Registrar taxa de hit em sessão fria e aquecida; não chamar correlação de causa sem timestamps.
