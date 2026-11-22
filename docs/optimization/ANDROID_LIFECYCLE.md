# Ciclo de vida Android

- Mapear pause, resume, stop, surfaceDestroyed e recriação da swapchain.
- Garantir que nenhuma render thread acesse superfície ou recurso destruído.
- Suspender renderização e trabalho GPU dispensável em background.
- Preservar áudio/entrada e restaurar o estado após retomada.
- Evitar persistir arquivos durante pausa bloqueando a thread gráfica; writes atômicos de saves/config quando viáveis.
- Verificar diferentes versões Android; logs de referência não comprovam todos os caminhos.
Fonte: https://developer.android.com/guide/components/activities/activity-lifecycle
