package com.luxoft.ivilink.applicationmanager;

import com.luxoft.ivilink.utils.IntentActions;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

/**
 * A <code>BroadcastReceiver</code> that sends {@link ApplicationInfo} to the
 * ApplicationManager when triggered. Its subclasses should be registered
 * statically in the AndroidManifest.xml to receive the action
 * <code>com.luxoft.ivilink.intent.action.APPLICATION_MANAGER_INFO_REQUEST</code>
 * (see {@link IntentActions})
 */
public abstract class ApplicationManagerRequestsReceiver extends BroadcastReceiver {

    protected abstract ApplicationInfo getApplicationInfo(Context context);

    @Override
    public final void onReceive(Context context, Intent intent) {
        Log.v(this.toString(), "Got an intent: action: " + intent.getAction() + " extras: "
                + intent.getExtras());
        if (!intent.getAction().equals(IntentActions.APPLICATION_MANAGER_INFO_REQUEST)) {
            Log.w(this.toString(), "Unexpected action: " + intent.getAction());
            return;
        }
        Intent response = new Intent();
        response.setAction(IntentActions.TO_APPLICATION_MANAGER);
        if (AppManProtocol.packApplicationInfoToIntent(getApplicationInfo(context), response)) {
            context.sendBroadcast(response);
        }
    }
}
