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

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.util.Log;

/**
 * A subclass of {@link ApplicationManagerRequestsReceiver}. Can be used in
 * applications where there is more than one entry Activity.
 */
public abstract class MultipleEntriesAppManRequestsReceiver<E extends Activity> extends
        ApplicationManagerRequestsReceiver {

    /**
     * @return the class of the Activity subclass in your application that
     *         should be started when the other side loads one of the supported
     *         services.
     */
    protected abstract Class<E> getActivityClass();

    /**
     * @return list of services that this application supports
     */
    protected abstract List<String> getSupportedServices();

    @Override
    protected ApplicationInfo getApplicationInfo(Context context) {
        if (getActivityClass() == null) {
            Log.w(this.toString(), "getActivityClass returned null, aborting");
            return null;
        }
        String packageName = context.getApplicationContext().getPackageName();
        Intent i = new Intent(Intent.ACTION_MAIN);
        i.addCategory(Intent.CATEGORY_LAUNCHER);
        i.setPackage(packageName);
        PackageManager pm = context.getPackageManager();
        List<ResolveInfo> resolveList = pm.queryIntentActivities(i, 0);
        String className = getActivityClass().getName();
        for (ResolveInfo resolveInfo : resolveList) {
            if (resolveInfo.activityInfo.name.equals(className)) {
                return new ApplicationInfo(packageName, className, getSupportedServices());
            }
        }
        return null;
    }
}
