# Índice de documentação do MoonfallCurse-Android

Leia primeiro AGENTS.md na raiz e docs/DEVELOPMENT_STATUS.md. Consulte apenas os guias relacionados à tarefa.

| Tema | Documento |
|---|---|
| Estado e evidências | [DEVELOPMENT_STATUS.md](DEVELOPMENT_STATUS.md) |
| Compatibilidade de GPU | [GPU_COMPATIBILITY.md](GPU_COMPATIBILITY.md) |
| Metas e validação | [PERFORMANCE_BUDGET.md](PERFORMANCE_BUDGET.md) |
| GSR EdgeDir | [optimization/GSR_EDGEDIR.md](optimization/GSR_EDGEDIR.md) |
| Diagnóstico de stutters | [optimization/FRAME_TIME_TRIAGE.md](optimization/FRAME_TIME_TRIAGE.md) |
| Renderizador moderno | [renderer/MODERN_VULKAN.md](renderer/MODERN_VULKAN.md) |
| Legacy Vulkan 1.1 | [renderer/LEGACY_VULKAN_1_1.md](renderer/LEGACY_VULKAN_1_1.md) |
| Enhanced for Mobile | [presets/ENHANCED_MOBILE.md](presets/ENHANCED_MOBILE.md) |
| Forks e upstream | [upstream/FORK_REGISTRY.md](upstream/FORK_REGISTRY.md) |
| Relatos upstream | [upstream/UPSTREAM_BUGS.md](upstream/UPSTREAM_BUGS.md) |
| Fontes | [references/SOURCES.md](references/SOURCES.md) |

Os guias são hipóteses e propostas até validação em código/hardware. Não confundir planejamento com implementação. Outros documentos do pacote consolidado podem ser incorporados sem sobrescrever documentação específica do repositório.

## Guias adicionais
- [Arquitetura](ARCHITECTURE.md), [build](BUILD_ANDROID.md), [testes](TESTING.md), [matriz de testes](TEST_MATRIX.md), [roadmap](ROADMAP.md), [decisões](DECISIONS.md) e [problemas conhecidos](KNOWN_ISSUES.md).
- Otimização: [memória](optimization/MEMORY.md), [alocações Vulkan](optimization/GPU_RESOURCE_LIFETIME.md), [warmup](optimization/SHADER_WARMUP.md), [ciclo de vida Android](optimization/ANDROID_LIFECYCLE.md), [energia](optimization/ENERGY_TEST_PROTOCOL.md), [pacing](optimization/ANDROID_FRAME_PACING.md) e [validação do EdgeDir](optimization/GSR_VALIDATION.md).
- Confiabilidade: [saves](reliability/SAVE_INTEGRITY.md), [configuração](reliability/CONFIG_MIGRATION.md), [triagem de crashes](reliability/CRASH_TRIAGE.md) e [checklist de regressão](reliability/REGRESSION_CHECKLIST.md).
- Upstream: [fila de revisão](upstream/REVIEW_QUEUE.md) e [política de importação](upstream/REVIEW_POLICY.md).
- [Modelo de relatório](templates/CHANGE_REPORT.md) e [histórico documental](CHANGELOG_DOCS.md).

Todos os demais guias técnicos encontram-se organizados em optimization/, renderer/, reliability/ e upstream/. As propostas não representam recursos implementados.
- [Compilação APK em GitHub Actions (artifact interno)](ANDROID_ACTIONS_BUILD.md).
