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

import java.util.List;

import android.util.Log;

import com.luxoft.ivilink.sdk.android.lib.utils.Paths;
import com.luxoft.ivilink.sdk.helpers.Common;

/**
 * Encapsulates Application Manager
 */
public class ApplicationManagerWrapper {

    public interface LaunchInfoProcessor {
        public boolean isLaunchInfoValid(String launchInfo);

        public String trimLaunchInfo(String launchInfo);
    }

    // pointer to native CDatabase
    private long mDatabaseInstance;

    static {
        System.loadLibrary("ApplicationManager");
    }

    private Thread mThread;

    public void start(LaunchInfoProcessor checker) {
        synchronized (this) {
            if (mThread != null && mThread.isAlive()) {
                return;
            }
            mDatabaseInstance = loadDatabase(Paths.getAppDatabase(), checker);
            (mThread = new Thread(new Runnable() {
                public void run() {
                    startApplicationManager(mDatabaseInstance, AppLauncher.class);
                    Log.e(Common.TAG + ".ApplicationManager", "has died!");
                }
            })).start();
        }
    }

    public void addApplicationToDatabase(String launchInfo, List<String> services) {
        if (mDatabaseInstance == 0) {
            return;
        }
        addApplicationToDatabase(mDatabaseInstance, launchInfo, services);
    }

    public void checkDatabase(String partialLaunchInfo) {
        if (mDatabaseInstance == 0) {
            return;
        }
        checkDatabase(mDatabaseInstance, partialLaunchInfo);
    }

    private native void startApplicationManager(long databaseInstance, Class<AppLauncher> launcher);

    private native long loadDatabase(String pathToDatabase, LaunchInfoProcessor checker);

    private native void addApplicationToDatabase(long databaseInstance, String launchInfo,
            List<String> services);

    private native void checkDatabase(long databaseInstance, String partialLaunchInfo);
}
