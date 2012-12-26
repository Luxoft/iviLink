/* 
 * iviLINK SDK, version 1.1.19
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012, Luxoft Professional Corp., member of IBS group
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
public class KillerReceiver extends BroadcastReceiver {
	final private static String tag = KillerReceiver.class.getName();

	@Override
	public void onReceive(Context context, Intent intent) {
		Log.i(tag, "got intent with " + intent.getAction());
		assert (intent.getAction().equals(IntentActions.KILLER_APP));
		Log.i(tag, "my pid: " + android.os.Process.myPid());
		Log.i(tag, "my package: " + context.getPackageName());
		context.unregisterReceiver(this);
		Log.i(tag, "unregistered receiver");
		android.os.Process.killProcess(android.os.Process.myPid());
	}

	/**
	 * @return registered BroadcastReceiver. The instance should be stored in a
	 *         variable and unregistered in the context's onDestroy() callback
	 */
	static BroadcastReceiver registerReceiverFromKillerApp(Context context) {
		KillerReceiver receiver = new KillerReceiver();
		context.registerReceiver(receiver, new IntentFilter(
				IntentActions.KILLER_APP));
		return receiver;
	}

}
