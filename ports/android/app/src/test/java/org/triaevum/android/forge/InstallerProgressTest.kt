package org.triaevum.android.forge

import org.junit.Assert.assertEquals
import org.junit.Assert.assertNull
import org.junit.Test

class InstallerProgressTest {
    @Test fun displaysOnlyMeasuredProgress() {
        assertNull(InstallerProgress.percent(42, -1))
        assertNull(InstallerProgress.percent(11, 10))
        assertEquals(0, InstallerProgress.percent(0, 10))
        assertEquals(100, InstallerProgress.percent(Long.MAX_VALUE, Long.MAX_VALUE))
    }

    @Test fun estimatesOnlyAnActiveStageWithEnoughSamples() {
        assertNull(InstallerProgress.remainingMs(4, 100, 4, 10_000))
        assertNull(InstallerProgress.remainingMs(25, 100, 0, 10_000))
        assertNull(InstallerProgress.remainingMs(25, 100, 20, 4_999))
        assertNull(InstallerProgress.remainingMs(100, 100, 95, 10_000))
        assertEquals(37_500L, InstallerProgress.remainingMs(25, 100, 20, 10_000))
    }
}
