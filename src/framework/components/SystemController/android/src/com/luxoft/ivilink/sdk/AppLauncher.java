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

package com.luxoft.ivilink.sdk;

import android.content.Context;
import android.os.Handler;
import android.util.Log;
import android.widget.Toast;

import com.luxoft.ivilink.sdk.android.lib.utils.ForSDK;
import com.luxoft.ivilink.sdk.helpers.Common;
import com.luxoft.ivilink.sdk.helpers.WakeUpHelper;

/*
 * Instance of this class is passed to Application Manager's CLauncher class
 * and it invokes the startApplication method when an application request comes 
 */
public class AppLauncher {
	static private Context context;
	static private AppLauncher instance;
	static Handler poster;

	static final String tag = Common.TAG + "AppLauncher";

	private AppLauncher(Context context) {
		AppLauncher.context = context;
		WakeUpHelper.disableKeyguard(context);
	}

	public static AppLauncher getInstance(Context caller, Handler handler) {
		synchronized (AppLauncher.class) {
			if (instance == null) {
				instance = new AppLauncher(caller);
				poster = handler;
			}
		}
		return instance;
	}

	// if name or signature changes don't forget to update
	// CApplicationManagerWrapper!
	// called from appman's launcher
	public static int startApplication(final String launchString) {
		WakeUpHelper.wakeUpIfNeeded(context, poster);
		try {
			ForSDK.launchActivity(context, launchString);
		} catch (android.content.ActivityNotFoundException e) {
			poster.post(new Runnable() {
				public void run() {
					Toast.makeText(context, genErrorMsg(launchString),
							Toast.LENGTH_LONG).show();
				}
			});
			Log.e(tag, "launch failed! Launch Info: " + launchString);
			e.printStackTrace();
			return -1;
		}
		String packageName = launchString.substring(0,
				launchString.indexOf("/"));
		Log.v(tag, "Package name of context: " + context.getPackageName());
		if (packageName.equals(context.getPackageName())) {
			Log.w(tag, "this is AUTH we launching! Package name is not default!");
			packageName = packageName + ".authentication"; // because we are
															// running it in a
															// custom process
		}
		Log.v(tag, "looking for package: " + packageName);
		return ForSDK.findPidOfRunningApp(context, packageName);
	}

	private static String genErrorMsg(String launchInfo) {
		return "Launch of app " + launchInfo
				+ " failed! Please check if this app is installed."
				+ " Please check AppMan's database for typos.";
	}
}
