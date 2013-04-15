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
