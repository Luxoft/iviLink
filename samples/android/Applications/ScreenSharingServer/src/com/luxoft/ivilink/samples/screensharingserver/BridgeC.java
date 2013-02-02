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


package com.luxoft.ivilink.samples.screensharingserver;

import android.content.Intent;
import android.os.Handler;
import android.util.Log;

import com.luxoft.ivilink.sdk.android.lib.utils.ForApp;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

//  glues java and c code together and takes care of ui interaction thread safety
class BridgeC {

    private ScreenSharingServer activity;
    private Handler poster;
    private boolean isNativeStarted;
    Thread mThread;

    final String tag = "iviLink.Samples.ScreenSharingServer.Bridge";

    static {
        System.loadLibrary("appwrapper");
    }

    private BridgeC(ScreenSharingServer activity, Handler poster) {
        setActivity(activity);
        setHandler(poster);
    }

    private void setHandler(Handler poster) {
        this.poster = poster;
    }

    private void setActivity(ScreenSharingServer activity) {
        this.activity = activity;
    }

    private static BridgeC instance;

    static BridgeC getInstance(ScreenSharingServer activity, Handler poster) {
        synchronized (BridgeC.class) {
            if (instance == null) {
                instance = new BridgeC(activity, poster);
            } else {
                instance.setActivity(activity);
                instance.setHandler(poster);
            }
        }
        return instance;
    }

    void startApp(final String launchInfo, final String internalPath) {
        Log.v(tag, "ENTER JAVA startApp()");
        if (!isNativeStarted) {
            (mThread = new Thread(new Runnable() {
                public void run() {
                    startApp(ForApp.getServicePath(), launchInfo, internalPath);
                    Log.i(tag, "JAVA: native startApp() finished");
                }
            })).start();
            Log.i(tag, "JAVA startApp(): native app started!");
            isNativeStarted = true;
        } else {
            Log.i(tag, "JAVA startApp(): native app is already started!");
        }
    }

    void dataReceived(String data) {
        Log.i(tag, ">>>>>>>>>>>>>>> ScreenSharingServer dataReceived >>>>>>>>>>>>> : " + data);

        if (data.startsWith(Settings.VNC_MSG_START)) {
            Settings.startedByUser = false;
            sendServerAddress();
            poster.post(new Runnable() {
                public void run() {
                    Log.v(tag, "startVncDialog()");
                    activity.startVncDialog();
                }
            });
        } else if (data.startsWith(Settings.VNC_MSG_START_ACK)) {
            activity.launchServices();
        } else if (data.startsWith(Settings.VNC_MSG_EXIT)) {
            finishApp();
        }
    }

    void finishApp() {
        Log.v(tag, Logging.getCurrentMethodName());

        Intent finishIntent = new Intent(Settings.EXIT_INTENT_TAG);
        finishIntent.putExtra(Settings.EXIT, true);
        activity.sendBroadcast(finishIntent);
    }

    native void startApp(String serviceRepo, String launchInfo, String internalPath);

    native void sendData(String data);

    native void sendServerAddress();
}
