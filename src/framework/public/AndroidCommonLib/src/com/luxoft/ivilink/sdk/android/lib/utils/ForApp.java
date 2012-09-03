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

package com.luxoft.ivilink.sdk.android.lib.utils;

import android.app.Activity;
import android.content.Context;
import android.os.Environment;
import android.view.WindowManager;


/*
 * Contains helper functions
 * that are used in sample apps
 */
public class ForApp {
	
	/*
	 * returns the launch info of the asker's application 
	 * in the same form it is used in the AppMan's database
	 */
	public static String getLaunchInfo(Context asker) {
		if (asker == null) return null;
		return asker.getPackageName()
				+ "/"
				+ asker.getClass().getName()
						.replace(asker.getPackageName(), "");
	}

	/*
	 * returns path to the internal directory of the asker's package
	 */
	public static String getInternalPath(Context asker) {
		if (asker == null) return null;
		return asker.getFilesDir().getAbsolutePath();
	}

	/*
	 * returns path to service repository on the device
	 */
	public static String getServicePath() {
		return Environment.getExternalStorageDirectory().getAbsolutePath() + Directories.service_repository;
	}

	/*
	 * Will turn on the device's screen if it is in sleep mode 
	 * and keep it on until the provided activity goes off-screen
	 */
	public static void setWindowFlagsToKeepDeviceOn(Activity activity) {
		activity.getWindow().addFlags(
				WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		activity.getWindow().addFlags(
				WindowManager.LayoutParams.FLAG_DISMISS_KEYGUARD);
		activity.getWindow().addFlags(
				WindowManager.LayoutParams.FLAG_SHOW_WHEN_LOCKED);
		activity.getWindow().addFlags(
				WindowManager.LayoutParams.FLAG_TURN_SCREEN_ON);
	}

}