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
package com.luxoft.ivilink.sdk.authentication;

import android.content.ComponentName;
import android.content.Context;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.util.Log;

import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

public class AuthenticationProcessInfo {
    private final static String TAG = AuthenticationProcessInfo.class.getName();
    // a failsafe
    private static final String AUTH_PROCESS_DEFAULT_NAME = "com.luxoft.ivilink.sdk.authentication";

    public static String getAuthenticationLaunchInfo(Context context) {
        ComponentName name = new ComponentName(context, AuthenticationActivity.class);
        return name.flattenToShortString();
    }

    public static String getAuthenticationProcessName(Context context) {
        try {
            String activityClassName = AuthenticationActivity.class.getName();
            ActivityInfo[] list = context.getPackageManager()
                    .getPackageInfo(context.getPackageName(), PackageManager.GET_ACTIVITIES).activities;
            for (ActivityInfo activity : list) {
                Log.v(TAG, "current activity's name: " + activity.name);
                Log.v(TAG, "target activity's name: " + activityClassName);
                if (activity.name.equals(activityClassName)) {
                    Log.i(TAG, "Found! Result is: " + activity.processName);
                    return activity.processName;
                }
            }
        } catch (NameNotFoundException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }
        /*
         * this will never happen unless the AuthenticationActivity is deleted
         * and/or the AndroidManifest is somehow tampered with
         */
        Log.e(TAG, Logging.getCurrentMethodName() + " returning default value");
        return AUTH_PROCESS_DEFAULT_NAME;
    }
}
