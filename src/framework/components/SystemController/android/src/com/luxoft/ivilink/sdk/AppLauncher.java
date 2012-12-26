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
 

package com.luxoft.ivilink.sdk;

import android.content.Context;
import android.os.Handler;
import android.util.Log;
import android.widget.Toast;

import com.luxoft.ivilink.sdk.android.lib.utils.ForSDK;
import com.luxoft.ivilink.sdk.helpers.Common;
import com.luxoft.ivilink.sdk.helpers.WakeUpHelper;

/**
 * Instance of this class is passed to Application Manager's CLauncher class and
 * it invokes the startApplication method when an application request comes
 */
public class AppLauncher {
	private static Context context;
	private static AppLauncher instance;
	private static Handler poster;

	static final String tag = Common.TAG + "AppLauncher";

	private AppLauncher(Context context, Handler poster) {
		AppLauncher.context = context;
		AppLauncher.poster = poster;
		WakeUpHelper.disableKeyguard(context);
	}

	public static AppLauncher getInstance(Context caller, Handler handler) {
		synchronized (AppLauncher.class) {
			if (instance == null) {
				instance = new AppLauncher(caller, handler);
			}
		}
		return instance;
	}

	/**
	 * Is invoked from Application Manager's CLauncher to launch a requested
	 * application.
	 * 
	 * @param launchString
	 *            String in the following format:
	 *            "package.name/.your.activity.name", where package.name is the
	 *            application's package name (see AndroidManifest.xml manifest's
	 *            attribute "package"), and .your.activity.name is the
	 *            activity's "android:name" attribute from the
	 *            AndroidManifest.xml. Applications may use
	 *            ForApp.getLaunchInfo(Activity) method to generate it.
	 *            Currently only activities running in the default application's
	 *            process (they have no attribute "process" specified in the
	 *            AndroidManifest.xml) are supported.
	 * @return Process ID of a newly-launched application, or -1 in case of
	 *         error
	 */
	public static int startApplication(final String launchString) {
		WakeUpHelper.wakeUpIfNeeded(context, poster);
		try {
			ForSDK.launchActivity(context, launchString);
		} catch (android.content.ActivityNotFoundException e) {
			poster.post(new Runnable() {
				public void run() {
					Toast.makeText(context, genErrorMsg(launchString), Toast.LENGTH_LONG).show();
				}
			});
			Log.e(tag, "launch failed! Launch Info: " + launchString);
			e.printStackTrace();
			return -1;
		}
		String packageName = launchString.substring(0, launchString.indexOf("/"));
		Log.i(tag, "Package name of context: " + context.getPackageName());
		if (packageName.equals(context.getPackageName())) {
			Log.w(tag, "this is AUTH we launching! Package name is not default!");
			// it is run in a custom process
			packageName = packageName + ".authentication";
		}
		Log.i(tag, "looking for package: " + packageName);
		return ForSDK.findPidOfRunningApp(context, packageName);
	}

	private static String genErrorMsg(String launchInfo) {
		return "Launch of app " + launchInfo + " failed! Please check if this app is installed."
				+ " Please check AppMan's database for typos.";
	}
}
