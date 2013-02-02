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


package com.luxoft.ivilink.sdk;

import android.content.ActivityNotFoundException;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.Gravity;
import android.widget.Toast;

import com.luxoft.ivilink.sdk.android.lib.utils.ProcessesHelper;
import com.luxoft.ivilink.sdk.android.lib.utils.log.BigLog;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;
import com.luxoft.ivilink.sdk.authentication.AuthenticationActivity;
import com.luxoft.ivilink.sdk.authentication.AuthenticationProcessInfo;
import com.luxoft.ivilink.sdk.helpers.WakeUpHelper;

/**
 * Instance of this class is passed to Application Manager's CLauncher class and
 * it invokes the startApplication method when an application request comes
 */
public class AppLauncher {
    private static final String TAG = AppLauncher.class.getName();
    private static final int WAIT_PID_CYCLES_LIMIT = 500;
    private static Context mContext;
    private static Handler mHandler;

    public static void init(Context caller) {
        mContext = caller;
        mHandler = new Handler(Looper.getMainLooper());
        WakeUpHelper.disableKeyguard(mContext);
    }

    /**
     * Is invoked from Application Manager's CLauncher to launch a requested
     * application.
     * 
     * @param launchString
     *            String in the following format:
     *            "applications.package.name/.your.activity.name", where
     *            application.package.name is the Application's package name
     *            (see AndroidManifest.xml manifest's attribute "package"), and
     *            .your.activity.name is the activity's "android:name" attribute
     *            from the AndroidManifest.xml. Applications may use
     *            ForApp.getLaunchInfo(Activity) method to generate it.
     * @return Process ID of a newly-launched application, or ERROR_PID (-1) in
     *         case of error
     */
    @SuppressWarnings("unused")
    private static int startApplication(final String launchString) {
        if (mContext == null) {
            BigLog.e(AppLauncher.class.getName(), "AppLauncher not inited!", '!');
            return ProcessesHelper.ERROR_PID;
        }
        WakeUpHelper.wakeUpIfNeeded(mContext, mHandler);
        if (launchString.equals(AuthenticationProcessInfo.getAuthenticationLaunchInfo(mContext))) {
            return launchAuthentication();
        }
        ComponentName name = ComponentName.unflattenFromString(launchString);
        String processName = ProcessesHelper.getProcessNameForComponent(mContext, name);
        if (processName == null) {
            showErrorToast("Could not resolve process name for component: "
                    + name.flattenToString());
            return ProcessesHelper.ERROR_PID;
        }
        Log.i(TAG, "Process name for component: " + name.flattenToShortString() + " is "
                + processName);
        try {
            launchActivity(name);
        } catch (ActivityNotFoundException e) {
            e.printStackTrace();
            showErrorToast(genErrorMsg(launchString));
            return ProcessesHelper.ERROR_PID;
        }
        return waitPid(processName);
    }

    private static String genErrorMsg(String launchInfo) {
        return "Launch of app " + launchInfo + " failed! Please check if this app is installed."
                + " Please check AppMan's database for typos.";
    }

    private static int launchAuthentication() {
        String authenticationProcessName = AuthenticationProcessInfo
                .getAuthenticationProcessName(mContext);
        Log.i(TAG, "Authentication process name: " + authenticationProcessName);
        mContext.startActivity(new Intent(mContext, AuthenticationActivity.class)
                .addFlags(Intent.FLAG_ACTIVITY_NEW_TASK));
        return waitPid(authenticationProcessName);
    }

    private static int waitPid(String processName) {
        int i = 0;
        while (i <= WAIT_PID_CYCLES_LIMIT) {
            int pid = ProcessesHelper.findPidOfApp(mContext, processName);
            if (pid == ProcessesHelper.ERROR_PID) {
                Log.w(TAG, "App hasn't started yet! process: " + processName + " tries: " + i);
            } else {
                Log.i(TAG, "App has started! process: " + processName + " pid: " + pid
                        + " total tries: " + i);
                return pid;
            }
            i++;
        }
        return ProcessesHelper.ERROR_PID;
    }

    private static void showErrorToast(final String message) {
        BigLog.e(TAG, message, '!');
        mHandler.post(new Runnable() {
            public void run() {
                Toast toast = Toast.makeText(mContext, message, Toast.LENGTH_LONG);
                toast.setGravity(Gravity.CENTER, 0, 0);
                toast.show();
            }
        });
    }

    private static void launchActivity(ComponentName component) throws ActivityNotFoundException {
        Log.v(TAG, Logging.getCurrentMethodName() + " : " + component.toShortString());
        Intent launchIntent = new Intent(Intent.ACTION_MAIN);
        launchIntent.addCategory(Intent.CATEGORY_LAUNCHER);
        launchIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        launchIntent.setComponent(component);
        mContext.startActivity(launchIntent);
    }
}
