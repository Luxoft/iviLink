package com.luxoft.ivilink.utils;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.util.Log;

/**
 * BroadcastRecever that is used to receive shutdown commands from AntiIviLink
 * and the iviLink application
 */
final class KillerReceiver extends BroadcastReceiver {
    final private static String TAG = KillerReceiver.class.getName();

    @Override
    public void onReceive(Context context, Intent intent) {
        Log.i(TAG, "got intent with " + intent.getAction());
        if (intent.getAction().equals(IntentActions.SHUT_DOWN_NOTIFICATION)) {
            context.unregisterReceiver(this);
            android.os.Process.killProcess(android.os.Process.myPid());
        } else {
            Log.w(TAG, "Unexpected action came: " + intent.getAction());
        }
    }

    /**
     * @return registered BroadcastReceiver. The instance should be stored in a
     *         variable and unregistered in the context's onDestroy() callback
     */
    static BroadcastReceiver registerReceiverFromKillerApp(Context context) {
        KillerReceiver receiver = new KillerReceiver();
        context.registerReceiver(receiver, new IntentFilter(IntentActions.SHUT_DOWN_NOTIFICATION));
        return receiver;
    }

}
