package com.luxoft.ivilink.applicationmanager;

import android.content.Intent;
import android.util.Log;

import com.luxoft.ivilink.utils.log.Logging;

public class AppManProtocol {
    private static final String TAG = AppManProtocol.class.getName();

    private final static String INTENT_EXTRA_TAG = "app info";

    /**
     * Appends the provided {@link ApplicationInfo} in its serialized form to
     * the given Intent's extras.
     * 
     * @param info
     * @param toBePackedInto
     * @return <code>true</code> in case of success, <code>false</code>
     *         otherwise (e.g. when the ApplicationInfo was <code>null</code> or
     *         it could not be serialized (was not initialized correctly), or
     *         when the Intent was <code>null</code>).
     */
    public static boolean packApplicationInfoToIntent(ApplicationInfo info, Intent toBePackedInto) {
        Log.v(TAG, Logging.getCurrentMethodName(info, toBePackedInto));
        if (info != null && info.isFilled() && toBePackedInto != null) {
            toBePackedInto.putExtra(INTENT_EXTRA_TAG, info.toString());
            return true;
        }
        return false;
    }

    public static ApplicationInfo parseFromIntent(Intent source) {
        Log.v(TAG, Logging.getCurrentMethodName(source));
        String jsonHolder = source.getStringExtra(INTENT_EXTRA_TAG);
        if (jsonHolder == null) {
            return null;
        }
        return ApplicationInfo.fromString(jsonHolder);
    }
}
