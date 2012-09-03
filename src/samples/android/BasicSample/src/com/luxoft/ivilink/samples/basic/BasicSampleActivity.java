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

package com.luxoft.ivilink.samples.basic;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.widget.TextView;

import com.luxoft.ivilink.sdk.android.lib.utils.ForApp;

/*
 * To fully understand what is going on, please see samples/linux/BasicSample/
 * and samples/linux/Profiles/BasicSampleProfileImpl
 * This is just a simple wrapper around native core app.
 */

public class BasicSampleActivity extends Activity {
	// will display information about callbacks
	TextView tv;
	// is used to update tv from background threads
	Handler poster;
	// flag that the native app has finished
	boolean isDone;
	/*
	 * Flag that a callback has been invoked from the native code. If it is still
	 * false by the time BasicSample is done then something went wrong.
	 */
	boolean wasThereACallback;
	// logging tag
	final String tag = "BasicSample";

	// loading the native library
	static {
		/*
		 * see the jni folder for jni glue source files 
		 * and samples/linux/BasicSample for core source files
		 */
		System.loadLibrary("appwrapper");
	}

	@Override
	public void onCreate(Bundle icicle) {
		super.onCreate(icicle);
		/*
		 * This is a helper function, it will turn on the device's screen if it
		 * is in sleep mode and it will keep it on until this Activity goes
		 * off-screen. The ForApp class is described in
		 * framework/public/AndroidCommonLib
		 */
		ForApp.setWindowFlagsToKeepDeviceOn(this);
		setContentView(R.layout.sampleapp);
		tv = (TextView) findViewById(R.id.resultsView);
		tv.setSingleLine(false);
		tv.setText("Hello iviLink!\n");
		poster = new Handler();

		// we need to supply the following information to the native code:
		/*
		 * Path to service repository (this is usually
		 * /mnt/sdcard/service_repository)
		 */
		final String serviceRepoPath = ForApp.getServicePath();
		/*
		 * Launch info of the application (in the same form is is written in the
		 * app man's database). Please use the provided method for generating it.
		 * Note! Will probably not work correctly if your Activity has a custom
		 * android:process defined in the AndroidManifest.xml file.
		 */
		final String launchInfo = ForApp.getLaunchInfo(this);
		/*
		 * path to the application's private directory (usually smth like
		 * /data/data/package.name/)
		 */
		final String internalDir = ForApp.getInternalPath(this);
		/*
		 * our "main" function is a blocking call so we have to run it in a
		 * separate thread to avoid ANR errors
		 */
		new Thread(new Runnable() {
			public void run() {
				Log.i(tag, "Starting native app!");
				main(serviceRepoPath, launchInfo, internalDir);
				// will post a note when main has finished
				poster.post(new Runnable() {
					public void run() {
						tv.append("BasicSample has finished!\n");
						if (!wasThereACallback) {
							tv.append("We haven't received any reply from the other side!\n");
							tv.append("Something went wrong, please check logs.\n");
						} else {
							tv.append("Everything went fine.\n");
						}
						Log.v(tag, "Core App is done!");
						isDone = true;
					}
				});
			}
		}).start();
	}

	/*
	 * see jni/src/com_luxoft_samples_basic_BasicSampleActivity.cpp 
	 * for implementation
	 */
	native void main(String pathToServiceRepo, String launchInfo,
			String internalPath);

	/*
	 * invoked from the native code when the other side has sent us operands
	 * (meaning we were started by iviLink as a result of loading a service) 
	 * see samples/linux/BasicSample/CBasicSample.hpp:classBasicSampleProfileCallbacks
	 */
	void operandsReceived(final int a, final int b) {
		Log.i(tag, "operands received: a=" + a + "; b=" + b);
		// we set a flag that a callback has really happened
		wasThereACallback = true;
		// we cannot update the ui from a non-ui thread, so we use a Handler
		poster.post(new Runnable() {
			public void run() {
				tv.append("Got operands: " + a + ", " + b + "\n");
			}
		});
	}

	/*
	 * invoked from the native code when the other side has sent us the result
	 * of the addition (meaning we were started by the user)
	 */
	void resultReceived(final int res) {
		Log.i(tag, "result received: res=" + res);
		wasThereACallback = true;
		poster.post(new Runnable() {
			public void run() {
				tv.append("Got a result: " + res + "\n");
			}
		});
	}

	@Override
	public void onBackPressed() {
		if (isDone) {
			super.onBackPressed();
		}
	}

	@Override
	public void onPause() {
		super.onPause();
		if (isDone) {
			finish();
		}
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
		android.os.Process.killProcess(android.os.Process.myPid());
	}
	
}
