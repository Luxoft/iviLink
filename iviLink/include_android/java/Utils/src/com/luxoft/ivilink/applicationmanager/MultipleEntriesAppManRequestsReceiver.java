package com.luxoft.ivilink.applicationmanager;

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
