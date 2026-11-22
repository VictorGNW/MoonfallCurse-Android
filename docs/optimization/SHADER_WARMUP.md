# Shader warmup seguro

- Usar histórico real dos pipelines mais comuns, evitando varrer combinações exponenciais.
- Limitar tempo, memória, número de shaders e threads do warmup.
- Não bloquear primeiro frame indefinidamente nem degradar bateria por compilação agressiva.
- Registrar progresso e erros; invalidação por ABI, shader e driver.
- Garantir shutdown sem tarefas pendentes acessando objetos destruídos.
- Habilitar warmup de forma incremental, medindo benefício em P95/P99.
