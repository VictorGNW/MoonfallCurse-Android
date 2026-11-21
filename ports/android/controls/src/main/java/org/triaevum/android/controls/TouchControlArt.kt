package org.triaevum.android.controls

import android.graphics.Bitmap
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.graphics.Path
import android.graphics.Rect
import android.graphics.RectF
import android.graphics.Typeface

/** Draws a sprite within the existing control's hit box, preserving its aspect ratio. */
class TouchControlArt(private val bitmap: Bitmap, private val sprites: Map<Int, Rect>) {
    private val destination = RectF()
    private val outline = Path()
    private val paint = Paint(Paint.ANTI_ALIAS_FLAG or Paint.FILTER_BITMAP_FLAG)
    private val thumb = Paint(Paint.ANTI_ALIAS_FLAG).apply {
        color = Color.rgb(109, 241, 250)
        style = Paint.Style.STROKE
    }

    private val label = Paint(Paint.ANTI_ALIAS_FLAG).apply {
        color = Color.rgb(255, 220, 252)
        textAlign = Paint.Align.CENTER
        typeface = Typeface.create(Typeface.SERIF, Typeface.BOLD)
    }

    internal fun drawShortcut(canvas: Canvas, button: InputOverlayDrawableButton): Boolean {
        val glyph = EmulationMenuSettings.shortcut(button.id).glyph
        if (glyph.isEmpty() || !draw(canvas, button.id, button.bounds, button.opacity,
                button.status == AzaharButtonState.PRESSED)) return false
        val bounds = button.bounds
        label.alpha = paint.alpha
        label.textSize = bounds.height() * if (glyph.length > 2) 0.25f else 0.43f
        canvas.drawText(glyph, bounds.exactCenterX(),
            bounds.exactCenterY() - (label.ascent() + label.descent()) / 2f, label)
        return true
    }

    internal fun drawStick(canvas: Canvas, joystick: InputOverlayDrawableJoystick): Boolean {
        val pressed = joystick.trackId != -1
        if (!draw(canvas, joystick.joystickId, joystick.visualBounds, joystick.opacity, pressed)) return false
        if (pressed) {
            val bounds = joystick.thumbBounds
            val radius = minOf(joystick.bounds.width(), joystick.bounds.height()) * 0.10f
            thumb.strokeWidth = radius * 0.13f
            thumb.alpha = joystick.opacity
            canvas.drawCircle(bounds.exactCenterX(), bounds.exactCenterY(), radius, thumb)
        }
        return true
    }

    internal fun draw(canvas: Canvas, id: Int, bounds: Rect, opacity: Int, pressed: Boolean): Boolean {
        val source = sprites[id] ?: return false
        val scale = minOf(bounds.width().toFloat() / source.width(), bounds.height().toFloat() / source.height())
        val halfWidth = source.width() * scale / 2f
        val halfHeight = source.height() * scale / 2f
        destination.set(bounds.exactCenterX() - halfWidth, bounds.exactCenterY() - halfHeight,
            bounds.exactCenterX() + halfWidth, bounds.exactCenterY() + halfHeight)
        paint.alpha = if (pressed) opacity + (255 - opacity) / 4 else opacity
        // Clip samples from the sprite sheet so nearby controls and ambient
        // glow cannot leak into an individual control's corners.
        outline.rewind()
        when (id) {
            AzaharButtonIds.DPAD_UP -> outline.addRect(destination, Path.Direction.CW)
            AzaharButtonIds.TRIGGER_L, AzaharButtonIds.BUTTON_ZL, AzaharButtonIds.BUTTON_ZR,
            AzaharButtonIds.BUTTON_SELECT, AzaharButtonIds.BUTTON_START -> {
                val radius = destination.height() / 2f
                outline.addRoundRect(destination, radius, radius, Path.Direction.CW)
            }
            else -> outline.addOval(destination, Path.Direction.CW)
        }
        val save = canvas.save()
        canvas.clipPath(outline)
        canvas.drawBitmap(bitmap, source, destination, paint)
        canvas.restoreToCount(save)
        return true
    }
}
