package org.triaevum.android;

import android.content.Context;
import android.os.Build;
import android.view.Surface;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.ViewGroup;
import org.libsdl.app.SDLSurface;
import org.triaevum.android.diagnostics.StartupDiagnostics;

/** SDL retains lifecycle/input ownership; only the Android buffer extent is constrained. */
final class TriAevumSurface extends SDLSurface {
    private final int maximumShortEdge;
    private int presentationRate;
    private boolean frameRateHintActive;
    private boolean surfaceReady;

    void setPresentationRate(int hz) {
        if (presentationRate == hz) return;
        presentationRate = hz;
        applyFrameRateHint();
    }

    void setFrameRateHintActive(boolean active) {
        if (frameRateHintActive == active) return;
        frameRateHintActive = active;
        applyFrameRateHint();
    }

    private void applyFrameRateHint() {
        if (Build.VERSION.SDK_INT < 30 || !surfaceReady) return;
        Surface surface = getHolder().getSurface();
        if (!surface.isValid()) return;
        float rate = frameRateHintActive ? presentationRate : 0;
        try {
            if (Build.VERSION.SDK_INT >= 31) {
                surface.setFrameRate(rate, Surface.FRAME_RATE_COMPATIBILITY_DEFAULT,
                    Surface.CHANGE_FRAME_RATE_ONLY_IF_SEAMLESS);
            } else {
                surface.setFrameRate(rate, Surface.FRAME_RATE_COMPATIBILITY_DEFAULT);
            }
        } catch (IllegalArgumentException | IllegalStateException error) {
            Log.w("TriAevum", "Android declined the presentation frame-rate hint", error);
        }
    }

    TriAevumSurface(Context context, int maximumShortEdge) {
        super(context);
        this.maximumShortEdge = maximumShortEdge;
        // A fixed buffer must not become the view's desired layout size on rotation.
        setLayoutParams(new ViewGroup.LayoutParams(
            ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
    }

    @Override public void surfaceCreated(SurfaceHolder holder) {
        StartupDiagnostics.stage(getContext(), "surface_created");
        super.surfaceCreated(holder);
    }

    @Override public void surfaceDestroyed(SurfaceHolder holder) {
        surfaceReady = false;
        StartupDiagnostics.stage(getContext(), "surface_destroyed");
        super.surfaceDestroyed(holder);
    }

    @Override protected void onSizeChanged(int width, int height, int oldWidth, int oldHeight) {
        super.onSizeChanged(width, height, oldWidth, oldHeight);
        int[] extent = SurfaceExtentPolicy.fit(width, height, maximumShortEdge);
        getHolder().setFixedSize(extent[0], extent[1]);
    }

    @Override public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        int viewWidth = getWidth() > 0 ? getWidth() : width;
        int viewHeight = getHeight() > 0 ? getHeight() : height;
        int[] extent = SurfaceExtentPolicy.fit(viewWidth, viewHeight, maximumShortEdge);
        if (width != extent[0] || height != extent[1]) {
            holder.setFixedSize(extent[0], extent[1]);
            return; // Do not start Vulkan against the transient full-resolution buffer.
        }
        Log.i("TriAevum", "Render surface " + width + "x" + height
            + ", view " + viewWidth + "x" + viewHeight);
        StartupDiagnostics.stage(getContext(), "surface_ready " + width + "x" + height);
        super.surfaceChanged(holder, format, width, height);
        // SDL normalizes touch positions with these fields. Touch uses view pixels,
        // not the independently sized SurfaceHolder buffer sent to Vulkan above.
        mWidth = viewWidth;
        mHeight = viewHeight;
        surfaceReady = true;
        applyFrameRateHint();
    }
}
