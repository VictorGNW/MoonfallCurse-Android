# Instruções para GPT — MoonfallCurse-Android

Escopo: porte ARM64 do MM3D/TriAevum para Android, Forge, SDL e Vulkan/NRI.

## Regras permanentes
1. Otimizar para diversas GPUs (Mali, Adreno e Xclipse); o Edge 60 é referência de teste, não alvo exclusivo. Nunca inferir comportamento universal a partir de um celular.
2. Consultar recursos Vulkan reais, extensões, formatos, limites e drivers. Isolar workarounds por capacidades ou bugs comprovados; preservar otimizações e fallbacks de outras GPUs.
3. Distinguir confirmado no código, observado em log, proposto, hipótese e não testado. Evidência e código atuais prevalecem sobre docs antigos.
4. Economizar tokens: consultar docs/INDEX.md e somente os arquivos envolvidos; localizar símbolos antes de ler arquivos extensos; reutilizar investigações existentes.
5. **Não executar gameplay autônomo:** não jogar, avançar fases ou simular jogadores, a menos que o usuário solicite explicitamente. Preferir logs, revisão estática, testes unitários e builds incrementais.
6. Não executar builds completas repetidas nem benchmarks longos sem hipótese clara. Validar proporcionalmente à alteração.
7. Não sacrificar P95/P99, latência, estabilidade ou bateria em troca de FPS médio. Preferir alterações pequenas e reversíveis.
8. Preservar Gradle, Forge, package ID, assinatura de APK, instalação, configuração e saves; não publicar ROMs, segredos ou chaves.
9. Legacy Vulkan 1.1 é um caminho planejado e separado, sem comprometer o renderizador moderno. Vulkan 1.1 não significa GPU necessariamente lenta.
10. SGSR v1 com EdgeDir é **planejado, não confirmado como implementado**. Medir custo e qualidade por GPU e manter escalonamento convencional como fallback.
11. Verificar upstream/forks e licenças antes de portar correções. Não importar automaticamente commits mais recentes.
12. Relatar o que mudou, o que foi testado ou não, limitações e riscos. Não inventar resultados.

**Objetivo:** desempenho sustentável e compatibilidade ampla, não ganhos exclusivos do aparelho do testador.
