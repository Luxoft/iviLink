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
 
package com.luxoft.ivilink.samples.media.players.audio.service;

import android.content.Context;

import com.luxoft.ivilink.sdk.android.lib.utils.ForSDK;
import com.luxoft.ivilink.sdk.android.lib.utils.log.BigLog;

public class AppWatchdog {
	private final static int CHECK_TIMEOUT_MILLIS = 1000;
	private static Thread checkerThread;
	private static Context context;
	private static Runnable checker = new Runnable() {
		public void run() {
			while (true) {
				int pid = ForSDK.findPidOfRunningAppOneShot(context, context
						.getApplicationContext().getPackageName());
				if (pid == -1) {
					BigLog.i(this.toString(), "Main Application has died!", '!');
					android.os.Process.killProcess(android.os.Process.myPid());
				} else {
					try {
						Thread.sleep(CHECK_TIMEOUT_MILLIS);
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
				}
			}
		}
	};

	public static void start(Context context) {
		if (checkerThread == null || !checkerThread.isAlive()) {
			AppWatchdog.context = context;
			checkerThread = new Thread(checker);
			checkerThread.start();
		}
	}
}
