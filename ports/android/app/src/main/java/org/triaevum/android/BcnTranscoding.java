package org.triaevum.android;

import android.content.Context;

import java.io.File;
import java.io.IOException;

public final class BcnTranscoding {
    static final String ENVIRONMENT_KEY = "TRIAEVUM_DISABLE_BCN_TRANSCODING";

    private BcnTranscoding() {}

    // The Forge and game run in separate Android processes. The presence of
    // the private file is the setting, so neither process caches a value.
    private static File disabledMarker(Context context) {
        return new File(context.getNoBackupFilesDir(), "disable-bcn-transcoding");
    }

    public static boolean isDisabled(Context context) {
        return disabledMarker(context).isFile();
    }

    public static void setDisabled(Context context, boolean disabled) throws IOException {
        File marker = disabledMarker(context);
        if (disabled) {
            marker.createNewFile();
        } else if (!marker.delete() && marker.exists()) {
            throw new IOException("Could not remove BCn opt-out marker");
        }
    }
}
