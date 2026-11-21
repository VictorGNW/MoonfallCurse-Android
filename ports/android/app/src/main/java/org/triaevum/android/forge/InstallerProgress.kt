package org.triaevum.android.forge

internal object InstallerProgress {
    fun percent(done: Long, total: Long): Int? {
        if (total <= 0 || done !in 0..total) return null
        if (done == total) return 100
        return (done.toDouble() / total * 100).toInt().coerceAtMost(99)
    }

    fun remainingMs(done: Long, total: Long, completedSinceSample: Long, elapsedMs: Long): Long? {
        if (done <= 0 || done >= total || elapsedMs < 5_000 || completedSinceSample <= 0) return null
        // Ignore the first 5% of a measured stage and keep the estimate within one day.
        if ((percent(done, total) ?: 0) < 5) return null
        return ((total - done).toDouble() / completedSinceSample * elapsedMs)
            .takeIf { it in 1.0..86_400_000.0 }?.toLong()
    }
}
