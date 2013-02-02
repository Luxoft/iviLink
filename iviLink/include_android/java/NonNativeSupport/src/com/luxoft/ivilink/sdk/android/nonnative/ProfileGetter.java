/* 
 * iviLINK SDK, version 1.2
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012-2013, Luxoft Professional Corp., member of IBS group
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; version 2.1.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * 
 */ 
package com.luxoft.ivilink.sdk.android.nonnative;

import android.util.Log;

import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

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
        if (IviLinkApplication.isServiceManagerCreated()) {
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
        if (IviLinkApplication.isServiceManagerCreated()) {
            releaseProfileNative(serviceName, profileAPIName);
        } else {
            Log.w(ProfileGetter.class.getName(), "Service Manager has not been created");
        }
    }

    static private native void releaseProfileNative(String serviceName, String profileAPI);
}
