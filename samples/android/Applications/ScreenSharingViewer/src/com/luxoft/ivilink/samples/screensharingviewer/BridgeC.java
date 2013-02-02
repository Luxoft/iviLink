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


package com.luxoft.ivilink.samples.screensharingviewer;

import android.content.Intent;
import android.os.Handler;
import android.util.Log;

import com.luxoft.ivilink.samples.screensharingviewer.android.androidVNC.Utils;
import com.luxoft.ivilink.samples.screensharingviewer.service.Settings;
import com.luxoft.ivilink.sdk.android.lib.utils.ForApp;

//  glues java and c code together and takes care of ui interaction thread safety
class BridgeC {

	private ScreenSharingViewer activity;
	private Handler poster;
	private boolean isNativeStarted;// = false;
	Thread mThread;
	
	final String tag = "iviLink.Samples.ScreenSharingViewer.Bridge";

	static {
		System.loadLibrary("appwrapper");
	}
	
	private BridgeC(ScreenSharingViewer activity, Handler poster) {
		setActivity(activity);
		setHandler(poster);
	}
	
	private void setHandler(Handler poster){
		this.poster=poster;
	}
	
	private void setActivity(ScreenSharingViewer activity){
		this.activity = activity;
	}
	
	private static BridgeC instance;
	
	static BridgeC getInstance(ScreenSharingViewer activity, Handler poster){
		synchronized (BridgeC.class) {
			if(instance == null){
				instance = new BridgeC(activity, poster);
			} else {
				instance.setActivity(activity);
				instance.setHandler(poster);
			}
		}
		return instance;
	}
	
	void startApp() {
		Log.v(tag, "ENTER JAVA startApp()");
		if(!isNativeStarted) {
			(mThread = new Thread(new Runnable() {
				public void run() {
					startApp(ForApp.getServicePath(), ForApp.getLaunchInfo(activity),
							ForApp.getInternalPath(activity) );
					Log.v(tag, "JAVA: native startApp() finished");
				}
			})).start();
			Log.v(tag, "JAVA startApp(): native app started!");
			isNativeStarted = true;
		} else {
			Log.v(tag, "JAVA startApp(): native app is already started!");
		}
	}
	
	void dataReceived(String data) {
		Log.v(tag, ">>>>>>>>>>>>>>>>>>>>>>>>>>> ScreenSharingViewer dataReceived >>>>>>>>>>>>>>>>>>>>>>> : " + data);

		if (data.startsWith(Settings.vncMsgStart) ) {
			Settings.startedByUser = false;
		} else if (data.startsWith(Settings.vncMsgServerAddress) ) {
			Settings.remoteIp = data.substring(1);
			Log.v(tag, "remote IP = " + Settings.remoteIp);
			
			if (!Settings.startedByUser) {
				poster.post(new Runnable() {
					public void run() {
						Log.v(tag, "startVncDialog()");
						activity.startVncDialog();										
					}
				});
			}
		} else if (data.startsWith(Settings.vncMsgStartAck) ) {
			activity.startVncClient();
		} else if (data.startsWith(Settings.vncMsgExit) ) {
			android.os.Process.killProcess(android.os.Process.myPid() );
		}
	}

	private native void startApp(String serviceRepo, String launchInfo, String internalPath); 
	
	native void sendData(String data);
}
