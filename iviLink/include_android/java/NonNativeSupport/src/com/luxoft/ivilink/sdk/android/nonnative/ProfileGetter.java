package com.luxoft.ivilink.sdk.android.nonnative;

import android.util.Log;

import com.luxoft.ivilink.utils.log.Logging;

/**
 * Contains methods to obtain and to release a profile implementation instance
 * from the Service Manager. It is strongly advised that this class and profile
 * instances should not be used directly. Please use profile proxies created
 * with {@link ProfileProxyFactory} instead.
 */
public final class ProfileGetter {
    /**
     * Get a profile implementation of the specified profile API from Service
     * Manager
     * 
     * @param serviceName
     *            UID of an active service
     * @param profileAPIName
     *            Profile API UID
     * @return reference to an instance, or <code>null</code> if nothing could
     *         be obtained
     */
    public static Object getProfile(String serviceName, String profileAPIName) {
        Log.v(ProfileGetter.class.getName(),
              Logging.getCurrentMethodName(serviceName, profileAPIName));
        if (IviLinkApplication.getInstance() != null && IviLinkApplication.getInstance().isServiceManagerCreated()) {
            return getProfileNative(serviceName, profileAPIName);
        } else {
            Log.w(ProfileGetter.class.getName(), "Service Manager has not been created");
            return null;
        }
    }

    static private native Object getProfileNative(String serviceName, String profileAPI);

    /**
     * Release a profile implementation of the specified profile API
     * 
     * @param serviceName
     *            UID of an active service
     * @param profileAPIName
     *            Profile API UID
     */
    public static void releaseProfile(String serviceName, String profileAPIName) {
        Log.v(ProfileGetter.class.getName(),
              Logging.getCurrentMethodName(serviceName, profileAPIName));
        if (IviLinkApplication.getInstance() != null && IviLinkApplication.getInstance().isServiceManagerCreated()) {
            releaseProfileNative(serviceName, profileAPIName);
        } else {
            Log.w(ProfileGetter.class.getName(), "Service Manager has not been created");
        }
    }

    static private native void releaseProfileNative(String serviceName, String profileAPI);
}
