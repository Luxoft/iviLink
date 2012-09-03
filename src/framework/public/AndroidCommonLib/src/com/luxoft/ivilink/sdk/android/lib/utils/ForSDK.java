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

import java.util.List;

import android.app.ActivityManager;
import android.app.ActivityManager.RunningAppProcessInfo;
import android.app.ActivityManager.RunningServiceInfo;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.os.Environment;
import android.util.Log;

/*
 * Contains helper functions
 * that are used in the SDK process
 * (they are mostly of no use for Applications)
 */

public class ForSDK {

	final static String tag = "iviLink.Utils.ForSDK";

	public static boolean checkIfServiceIsRunning(Context checker,
			String serviceName) {
		ActivityManager manager = (ActivityManager) checker
				.getSystemService(Context.ACTIVITY_SERVICE);
		for (RunningServiceInfo rsi : manager
				.getRunningServices(Integer.MAX_VALUE)) {
			if (serviceName.equals(rsi.service.getClassName())) {
				return true;
			}
		}
		return false;
	}

	public static int findPidOfRunningAppOneShot(Context asker,
			String packageName) {
		ActivityManager am = (ActivityManager) asker
				.getSystemService(Context.ACTIVITY_SERVICE);
		List<RunningAppProcessInfo> apps = am.getRunningAppProcesses();
		for (RunningAppProcessInfo rapi : apps) {
			if (rapi.processName.equals(packageName)) {
				Log.i(tag, "found package"+packageName+" with pid: " + rapi.pid);
				return rapi.pid;
			}
		}
		return -1;
	}

	// TODO optimize
	public static int findPidOfRunningApp(Context asker, String packageName) {
		Log.v(tag, "looking for " + packageName + " among running apps");
		int res;
		while (true) {
			if ((res = findPidOfRunningAppOneShot(asker, packageName)) != -1) {
				return res;
			}
		}
	}

	public static void launchActivity(Context starter, String targetActivity) {
		Log.v(tag, "starting activity " + targetActivity);
		Intent launchIntent = generateLaunchIntent(targetActivity);
		starter.startActivity(launchIntent);
	}

	public static Intent generateLaunchIntent(String targetActivity) {
		Intent launchIntent = new Intent(Intent.ACTION_MAIN);
		launchIntent.addCategory(Intent.CATEGORY_LAUNCHER);
		launchIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		launchIntent.setComponent(ComponentName
				.unflattenFromString(targetActivity));
		return launchIntent;
	}
	
	public static String getAppManDirectory(){
		return Environment.getExternalStorageDirectory().getAbsolutePath()+Directories.app_database;
	}
	
	public static String getProfRepoDirectory(){
		return Environment.getExternalStorageDirectory().getAbsolutePath()+Directories.profile_repository;
	}
}