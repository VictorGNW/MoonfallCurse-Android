package org.triaevum.android.controls

enum class TouchShortcut(val label: String, val glyph: String, internal val hidMask: Int) {
    OFF("Off", "", 0),
    OCARINA("Ocarina", "♪", (1 shl 15) or (1 shl 0)),
    ITEM_I("Item I", "I", (1 shl 15) or (1 shl 10)),
    ITEM_II("Item II", "II", (1 shl 15) or (1 shl 11)),
    PICTOGRAPH("Pictograph Box", "CAM", 1 shl 14);

    companion object {
        const val FIRST_ID = 820
        const val SLOT_COUNT = 3
        val slots = FIRST_ID until (FIRST_ID + SLOT_COUNT)
    }
}
