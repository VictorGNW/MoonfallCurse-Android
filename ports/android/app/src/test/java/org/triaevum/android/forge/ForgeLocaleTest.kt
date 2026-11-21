package org.triaevum.android.forge

import org.junit.Assert.assertEquals
import org.junit.Test

class ForgeLocaleTest {
    @Test fun followsFirstSupportedDeviceLanguage() {
        assertEquals("pt", ForgeLocale.choose(null, listOf("pt", "en")))
        assertEquals("fr", ForgeLocale.choose(null, listOf("ja", "fr", "es")))
        assertEquals("en", ForgeLocale.choose(null, listOf("ja", "nl")))
    }

    @Test fun explicitChoiceOverridesDeviceAndAutomaticRestoresIt() {
        assertEquals("es", ForgeLocale.choose("es", listOf("pt")))
        assertEquals("pt", ForgeLocale.choose(null, listOf("pt")))
        assertEquals("it", ForgeLocale.choose("nl", listOf("it")))
    }
}
