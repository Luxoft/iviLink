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
package com.luxoft.ivilink.sdk.android.lib.utils.applicationmanager;

import android.content.Intent;
import android.util.Log;

import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

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
