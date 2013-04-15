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

import java.util.List;

import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.util.Log;

import com.luxoft.ivilink.sdk.android.lib.utils.log.BigLog;

/**
 * A subclass of {@link ApplicationManagerRequestsReceiver}. Should be used only
 * in applications where there is only one entry Activity.
 */
public abstract class SingleEntryAppManRequestsReceiver extends ApplicationManagerRequestsReceiver {
    private static final String TAG = SingleEntryAppManRequestsReceiver.class.getName();

    @Override
    protected ApplicationInfo getApplicationInfo(Context context) {
        String packageName = context.getApplicationContext().getPackageName();
        Log.v(TAG, "package name: " + packageName);
        Intent i = new Intent(Intent.ACTION_MAIN);
        i.addCategory(Intent.CATEGORY_LAUNCHER);
        i.setPackage(packageName);
        PackageManager pm = context.getPackageManager();
        List<ResolveInfo> resolveList = pm.queryIntentActivities(i, 0);
        if (resolveList.size() > 1) {
            BigLog.w(TAG,
                     "This class is not supposed to be used with apps with more than one entry activity!",
                     '!');
            return null;
        }
        if (resolveList.size() < 1) {
            return null;
        }
        String activityName = resolveList.get(0).activityInfo.name;
        return new ApplicationInfo(packageName, activityName, getSupportedServices());
    }

    /**
     * @return list of services that this application supports
     */
    protected abstract List<String> getSupportedServices();
}
