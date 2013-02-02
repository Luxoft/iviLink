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
package com.luxoft.ivilink.sdk.componentservices;

import java.util.List;

import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.util.Log;

import com.luxoft.ivilink.sdk.AppLauncher;
import com.luxoft.ivilink.sdk.ApplicationManagerWrapper;
import com.luxoft.ivilink.sdk.ApplicationManagerWrapper.LaunchInfoProcessor;
import com.luxoft.ivilink.sdk.android.lib.utils.IntentActions;
import com.luxoft.ivilink.sdk.android.lib.utils.applicationmanager.AppManProtocol;
import com.luxoft.ivilink.sdk.android.lib.utils.applicationmanager.ApplicationInfo;
import com.luxoft.ivilink.sdk.android.lib.utils.log.BigLog;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;
import com.luxoft.ivilink.sdk.authentication.AuthenticationProcessInfo;
import com.luxoft.ivilink.sdk.helpers.StartupProgress;

/**
 * In this service the Application Manager process is run
 */
public class ApplicationManagerService extends ProtoComponentService implements LaunchInfoProcessor {
    private static final String TAG = ApplicationManagerService.class.getName();
    ApplicationManagerWrapper mAppManNative;

    @Override
    protected StartupProgress getProgress() {
        return StartupProgress.APPLICATION_MANAGER;
    }

    @Override
    protected void startComponent(Intent intent) {
        mApplicationsInfoReceiver.registerSelf(this);
        AppLauncher.init(this);
        mAppManNative = new ApplicationManagerWrapper();
        mAppManNative.start(this);
        broadcastApplicationInfoRequest();
        mPackagesAddedOrChangedReceiver.registerSelf(this);
        mPackageRemovedReceiver.registerSelf(this);
    }

    public boolean isLaunchInfoValid(String launchInfo) {
        Log.v(TAG, Logging.getCurrentMethodName(launchInfo));
        if (launchInfo == null || launchInfo.length() == 0) {
            return false;
        }
        if (launchInfo.equals(AuthenticationProcessInfo.getAuthenticationLaunchInfo(this))) {
            /*
             * authentication is the only activity that is not "visible" to the
             * user, but still can be launched by AMP, hence an exception is
             * made
             */
            Log.v(TAG, "Was asked about authentication");
            return true;
        }
        ComponentName componentName = ComponentName.unflattenFromString(launchInfo);
        Intent i = new Intent(Intent.ACTION_MAIN);
        i.addCategory(Intent.CATEGORY_LAUNCHER);
        i.setPackage(componentName.getPackageName());
        PackageManager pm = this.getPackageManager();
        List<ResolveInfo> resolveList = pm.queryIntentActivities(i, 0);
        if (resolveList == null || resolveList.isEmpty()) {
            // this should never happen according to the docs, but still occurs
            // on some HTC devices
            Log.w(TAG, "launch info: " + launchInfo + " is not valid, and the package "
                    + componentName.getPackageName() + " is not even installed");
            return false;
        }
        for (ResolveInfo resolveInfo : resolveList) {
            Log.i(TAG, "activity info: " + resolveInfo.activityInfo.name);
            Log.i(TAG, "provided activity name: " + componentName.getClassName());
            if (resolveInfo.activityInfo.name.equals(componentName.getClassName())) {
                Log.v(TAG, "launch info: " + launchInfo + " is valid");
                return true;
            }
        }
        Log.w(TAG, "launch info: " + launchInfo + " is not valid");
        return false;
    }

    public String trimLaunchInfo(String launchInfo) {
        ComponentName name = ComponentName.unflattenFromString(launchInfo);
        String result = name.flattenToShortString();
        Log.v(TAG, Logging.getCurrentMethodName() + ": " + launchInfo + "->" + result);
        return result;
    }

    private String createLaunchInfo(String packageName, String activityName) {
        return new ComponentName(packageName, activityName).flattenToShortString();
    }

    private void broadcastApplicationInfoRequest() {
        Intent appsRequest = new Intent();
        appsRequest.setAction(IntentActions.APPLICATION_MANAGER_INFO_REQUEST);
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.HONEYCOMB_MR1) {
            appsRequest.addFlags(Intent.FLAG_INCLUDE_STOPPED_PACKAGES);
        }
        sendBroadcast(appsRequest);
    }

    abstract class SelfRegisteringReceiver extends BroadcastReceiver {
        public abstract void registerSelf(Context context);
    }

    private SelfRegisteringReceiver mApplicationsInfoReceiver = new SelfRegisteringReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            Log.v(TAG + "|mApplicationsInfoReceiver", Logging.getCurrentMethodName(context, intent));
            ApplicationInfo info = AppManProtocol.parseFromIntent(intent);
            if (info != null && info.isFilled()) {
                Log.i(TAG, "Got valid info: " + info.toString());
                String launchInfo = createLaunchInfo(info.getPackageName(), info.getActivityName());
                mAppManNative.addApplicationToDatabase(launchInfo, info.getSupportedServices());
            } else {
                Log.w(TAG, "Could not parse info from intent or it was not filled correctly!");
            }
        }

        @Override
        public void registerSelf(Context context) {
            context.registerReceiver(this, new IntentFilter(IntentActions.TO_APPLICATION_MANAGER));
        }
    };

    private SelfRegisteringReceiver mPackagesAddedOrChangedReceiver = new SelfRegisteringReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            Log.v(TAG + "|mPackagesAddedOrChangedReceiver",
                  Logging.getCurrentMethodName(context, intent));
            broadcastApplicationInfoRequest();
        }

        @Override
        public void registerSelf(Context context) {
            IntentFilter intentFilter = new IntentFilter();
            intentFilter.addAction(Intent.ACTION_PACKAGE_ADDED);
            intentFilter.addAction(Intent.ACTION_PACKAGE_REPLACED);
            intentFilter.addDataScheme("package");
            context.registerReceiver(this, intentFilter);
        }
    };

    private SelfRegisteringReceiver mPackageRemovedReceiver = new SelfRegisteringReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            Log.v(TAG + "|mPackageRemovedReceiver", Logging.getCurrentMethodName(context, intent));
            String packageRemoved = intent.getData().getSchemeSpecificPart();
            BigLog.i(TAG, "The following package has been removed: " + packageRemoved, '!');
            if (packageRemoved != null && packageRemoved.length() != 0) {
                mAppManNative.checkDatabase(packageRemoved);
            }
        }

        @Override
        public void registerSelf(Context context) {
            IntentFilter intentFilter = new IntentFilter();
            intentFilter.addAction(Intent.ACTION_PACKAGE_REMOVED);
            intentFilter.addDataScheme("package");
            context.registerReceiver(this, intentFilter);
        }
    };
}
