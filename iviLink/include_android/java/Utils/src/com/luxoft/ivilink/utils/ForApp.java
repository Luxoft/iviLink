/* 
 * 
 * iviLINK SDK, version 1.1.2
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
 * 
 */

package com.luxoft.ivilink.utils;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.view.WindowManager;

/**
 * Contains helper functions that are used in sample applications
 */
public final class ForApp {
	/**
	 * Allows to check if the Application has been started by iviLink not waiting for the onInitDone callback.
	 * @param intent
	 * @return
	 */
	public static boolean isIntentFromIviLink(Intent intent) {
		if (intent != null) {
			return intent.getBooleanExtra("ivilink", false);
		}
		return false;
	}
    /**
     * @param asker
     *            activity for which the launch info should be generated
     * @return the launch info (String, from which an Intent to launch the
     *         application can be created) of the asker's application in the
     *         same form it is stored in the Application Manager's database
     */
    public static String getLaunchInfo(Activity activity) {
        Assertion.check(activity != null);
        ComponentName name = new ComponentName(activity, activity.getClass());
        return name.flattenToShortString();
    }

    /**
     * @param asker
     *            Any instance of the application's context (Service, Activity,
     *            etc.)
     * @return path to the internal directory of the asker's package
     */
    public static String getInternalPath(Context asker) {
        Assertion.check(asker != null);
        return asker.getFilesDir().getAbsolutePath();
    }

    /**
     * @return path to service repository on the device
     */
    public static String getServicePath() {
        return Paths.getServiceRepository();
    }

    /**
     * Turns on the device's screen if it is in sleep mode and keeps it on until
     * the provided activity goes off-screen
     * 
     * @param activity
     */
    public static void setWindowFlagsToKeepDeviceOn(Activity activity) {
        Assertion.check(activity != null);
        activity.getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        activity.getWindow().addFlags(WindowManager.LayoutParams.FLAG_DISMISS_KEYGUARD);
        activity.getWindow().addFlags(WindowManager.LayoutParams.FLAG_SHOW_WHEN_LOCKED);
        activity.getWindow().addFlags(WindowManager.LayoutParams.FLAG_TURN_SCREEN_ON);
    }

    /**
     * Registers receiver to receive shutdown command from AntiIviLink and the
     * iviLink application: unless the receiver is unregistered later, the
     * application will be killed when either AntiIviLink is used, or the
     * iviLink application is being shut down or restarted
     * 
     * @param context
     *            used to register created BroadcastReceiver
     * @return registered BroadcastReceiver. You should store this reference and
     *         unregister it in the context's onDestroy method
     */
    public static BroadcastReceiver registerReceiverFromKillerApp(Context context) {
        Assertion.check(context != null);
        return KillerReceiver.registerReceiverFromKillerApp(context);
    }
}
