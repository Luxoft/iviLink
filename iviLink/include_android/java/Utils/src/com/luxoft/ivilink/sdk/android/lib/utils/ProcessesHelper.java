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
package com.luxoft.ivilink.sdk.android.lib.utils;

import java.util.List;

import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

import android.app.ActivityManager;
import android.app.ActivityManager.RunningAppProcessInfo;
import android.app.ActivityManager.RunningServiceInfo;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.util.Log;

public class ProcessesHelper {

    private static final String TAG = ProcessesHelper.class.getName();

    public static final int ERROR_PID = -1;

    public static int findPidOfApp(Context context, String processName) {
        Log.v(TAG, Logging.getCurrentMethodName(context, processName));
        if (processName == null || processName.length() == 0) {
            Log.w(TAG, Logging.getCurrentMethodName(context, processName) + " invalid parameters!");
            return ERROR_PID;
        }
        ActivityManager am = (ActivityManager) context.getSystemService(Context.ACTIVITY_SERVICE);
        List<RunningAppProcessInfo> apps = am.getRunningAppProcesses();
        for (RunningAppProcessInfo rapi : apps) {
            if (rapi.processName.equals(processName)) {
                Log.i(TAG, "result: " + rapi.pid);
                return rapi.pid;
            }
        }
        Log.w(TAG, "Process " + processName + " not found!");
        return ERROR_PID;
    }

    public static String getProcessNameForComponent(Context context, ComponentName name) {
        Log.v(TAG, Logging.getCurrentMethodName(context, name));
        Intent i = new Intent(Intent.ACTION_MAIN);
        i.addCategory(Intent.CATEGORY_LAUNCHER);
        i.setPackage(name.getPackageName());
        PackageManager pm = context.getPackageManager();
        List<ResolveInfo> resolveList = pm.queryIntentActivities(i, 0);
        if (resolveList == null || resolveList.isEmpty()) {
            Log.i(TAG, "Package " + name.getPackageName() + " is not installed!");
            return null;
        }
        for (ResolveInfo resolveInfo : resolveList) {
            Log.v(TAG, "current activity info: " + resolveInfo.activityInfo.name);
            Log.v(TAG, "provided activity name: " + name.getClassName());
            if (resolveInfo.activityInfo.name.equals(name.getClassName())) {
                Log.i(TAG, "result: " + resolveInfo.activityInfo.processName);
                return resolveInfo.activityInfo.processName;
            }
        }
        Log.w(TAG, "Component not found among the package's components: " + name.toShortString());
        return null;
    }

    public static boolean isServiceRunning(Context context, String className) {
        Log.v(TAG, Logging.getCurrentMethodName(context, className));
        ActivityManager manager = (ActivityManager) context
                .getSystemService(Context.ACTIVITY_SERVICE);
        for (RunningServiceInfo rsi : manager.getRunningServices(Integer.MAX_VALUE)) {
            if (className.equals(rsi.service.getClassName())) {
                Log.i(TAG, "Service " + className + " is running");
                return true;
            }
        }
        Log.i(TAG, "Service " + className + " is not running");
        return false;
    }

}
