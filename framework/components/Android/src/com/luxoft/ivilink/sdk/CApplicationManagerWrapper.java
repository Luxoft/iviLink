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


package com.luxoft.ivilink.sdk;

import android.util.Log;
import com.luxoft.ivilink.sdk.helpers.Common;

/**
 * Encapsulates Application Manager
 */
public class CApplicationManagerWrapper {
	static {
		System.loadLibrary("ApplicationManager");
	}

	Thread mThread;
	String mPathToDatabase;
	AppLauncher mLauncher;

	/**
	 * Start Application Manager
	 * 
	 * @param pathToDatabase
	 *            path on device where the application manager database xml is
	 *            located
	 * @param launcher
	 *            Instance of AppLauncher, used whenever an application is
	 *            requested
	 */
	public void start(String pathToDatabase, AppLauncher launcher) {
		mLauncher = launcher;
		mPathToDatabase = pathToDatabase;
		(mThread = new Thread(new Runnable() {
			public void run() {
				startAM(mPathToDatabase, mLauncher);
				Log.e(Common.TAG + ".ApplicationManager", "has died!");
			}
		})).start();
	}

	// code taken from AppMan/process/main.cpp
	private native void startAM(String pathToDatabase, AppLauncher launcher);
}
