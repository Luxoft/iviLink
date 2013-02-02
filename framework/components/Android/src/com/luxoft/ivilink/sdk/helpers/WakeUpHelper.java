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


package com.luxoft.ivilink.sdk.helpers;

import android.app.KeyguardManager;
import android.app.KeyguardManager.KeyguardLock;
import android.content.Context;
import android.os.Handler;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.util.Log;

public class WakeUpHelper {
	static final String tag = Common.TAG + "WakeUpHelper";
	
	// just acquiring the WakeLock is not enough when a keyguard is on
	public static void disableKeyguard(Context context){
		Log.v(tag, "Disabling keyduard - will be reenabled later when this process has died");
		KeyguardManager km = (KeyguardManager)context.getSystemService(Context.KEYGUARD_SERVICE);
		KeyguardLock klock = km.newKeyguardLock(tag);
		klock.disableKeyguard(); // will be re-enabled when this process has died
	}

	/*
	 * wake up the device when starting an app after an application request from the other side
	 */
	public static void wakeUpIfNeeded(Context context, Handler poster) {
		PowerManager pm = (PowerManager) context
				.getSystemService(Context.POWER_SERVICE);
		if (pm.isScreenOn()) {
			Log.i(tag, "Screen is on, nothing needs to be done");
			return;
		} else {
			Log.i(tag, "Screen is off, will wake the device up now!");
		}
		WakeLock wl = pm.newWakeLock(PowerManager.FULL_WAKE_LOCK
				| PowerManager.ACQUIRE_CAUSES_WAKEUP
				| PowerManager.ON_AFTER_RELEASE, tag);
		wl.acquire();
		wl.release(); // ON_AFTER_RELEASE flag keeps screen on for some time
	}
}
