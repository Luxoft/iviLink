package com.luxoft.ivilink.sdk.authentication;

import android.content.ComponentName;
import android.content.Context;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.util.Log;

import com.luxoft.ivilink.utils.log.Logging;

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
