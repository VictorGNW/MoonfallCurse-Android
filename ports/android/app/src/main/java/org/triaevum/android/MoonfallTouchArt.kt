package org.triaevum.android

import android.content.Context
import android.graphics.BitmapFactory
import android.graphics.Rect
import org.triaevum.android.controls.AzaharButtonIds as Button
import org.triaevum.android.controls.TouchControlArt

internal class MoonfallTouchArt(context: Context) {
    val standard = TouchControlArt(BitmapFactory.decodeResource(context.resources, R.drawable.moonfall_touch_standard),
        mapOf(
            Button.TRIGGER_L to Rect(44, 117, 278, 226),
            Button.BUTTON_ZL to Rect(52, 229, 278, 328),
            Button.DPAD_UP to Rect(70, 332, 320, 578),
            Button.STICK_LEFT to Rect(65, 579, 370, 880),
            Button.BUTTON_SELECT to Rect(573, 794, 830, 895),
            Button.BUTTON_START to Rect(830, 794, 1089, 895),
            Button.BUTTON_HOME to Rect(1550, 23, 1660, 131),
            Button.BUTTON_ZR to Rect(1196, 117, 1423, 228),
            Button.TRIGGER_R to Rect(1398, 155, 1631, 382),
            Button.BUTTON_X to Rect(1240, 345, 1418, 510),
            Button.BUTTON_Y to Rect(1094, 469, 1262, 631),
            Button.BUTTON_A to Rect(1393, 470, 1559, 635),
            Button.BUTTON_B to Rect(1235, 594, 1402, 756),
            Button.STICK_C to Rect(1399, 673, 1614, 885),
        ))
    val zora = TouchControlArt(BitmapFactory.decodeResource(context.resources, R.drawable.moonfall_touch_zora),
        mapOf(
            Button.TRIGGER_L to Rect(46, 123, 275, 230),
            Button.BUTTON_ZL to Rect(66, 237, 268, 316),
            Button.DPAD_UP to Rect(54, 321, 296, 561),
            Button.STICK_LEFT to Rect(62, 564, 368, 871),
            Button.BUTTON_SELECT to Rect(529, 760, 786, 866),
            Button.BUTTON_START to Rect(791, 760, 1048, 866),
            Button.BUTTON_HOME to Rect(1553, 25, 1654, 126),
            Button.BUTTON_ZR to Rect(1169, 129, 1380, 232),
            Button.TRIGGER_R to Rect(1388, 161, 1633, 405),
            Button.BUTTON_X to Rect(1228, 343, 1396, 501),
            Button.BUTTON_Y to Rect(1072, 457, 1241, 617),
            Button.BUTTON_A to Rect(1379, 457, 1553, 624),
            Button.BUTTON_B to Rect(1219, 576, 1392, 746),
            Button.STICK_C to Rect(1398, 656, 1617, 873),
        ))
}
