package com.luxoft.ivilink.applicationmanager;

import java.util.List;

import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.util.Log;

import com.luxoft.ivilink.utils.log.BigLog;

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
