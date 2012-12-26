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
 
package com.luxoft.ivilink.samples.navi.voice;

import android.app.Notification;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.IBinder;
import android.util.Log;

import com.luxoft.ivilink.samples.navi.voice.api.NaviVoiceAPI;
import com.luxoft.ivilink.samples.navi.voice.api.NaviVoiceAPI.NaviVoiceApplicationAPI;
import com.luxoft.ivilink.sdk.android.lib.utils.ForApp;
import com.luxoft.ivilink.sdk.android.lib.utils.log.BigLog;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;
import com.luxoft.ivilink.sdk.android.nonnative.ApplicationStateCallbacks;

public class NaviVoiceService extends Service implements ApplicationStateCallbacks {

	private final String tag = NaviVoiceService.class.getSimpleName();

	TipsPlayer mPlayer;

	@Override
	public IBinder onBind(Intent intent) {
		return null;
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		Log.v(tag, Logging.getCurrentMethodName(intent, flags, startId));
		ForApp.registerReceiverFromKillerApp(this);
		setNotification();
		mPlayer = TipsPlayer.getInstance(this, new Handler());
		Shared.cappInstance.initInIVILink((ApplicationStateCallbacks) this);
		return START_NOT_STICKY;
	}

	@SuppressWarnings("deprecation")
	private void setNotification() {
		Log.v(tag, Logging.getCurrentMethodName());
		int icon = R.drawable.ic_launcher;
		CharSequence tickerText = "Navi Voice Service started!";
		long when = System.currentTimeMillis();
		Notification notification = new Notification(icon, tickerText, when);

		Context context = this.getApplicationContext();
		CharSequence contentTitle = "Navi Voice App";
		CharSequence contentText = "Click to shutdown";
		Intent notificationIntent = new Intent(this, NotificationCallback.class);
		notificationIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		PendingIntent contentIntent = PendingIntent.getActivity(this, 0, notificationIntent, 0);
		notification.setLatestEventInfo(context, contentTitle, contentText, contentIntent);
		startForeground(1, notification);
	}

	public void onInitDone(boolean isStartedByUser) {
		Log.v(tag, Logging.getCurrentMethodName(isStartedByUser));
		if (isStartedByUser) {
			Shared.cappInstance.registerProfileCallbacks(NaviVoiceAPI.profileApiUid,
					(NaviVoiceApplicationAPI) mPlayer);
			if (!Shared.cappInstance.loadService(Shared.SERVICE_NAME)) {
				BigLog.e(tag, "Could not load service " + Shared.SERVICE_NAME, '!');
				android.os.Process.killProcess(android.os.Process.myPid());
			}
		}
	}

	public void onIncomingServiceBeforeLoading(String serviceUID) {
		Log.v(tag, Logging.getCurrentMethodName(serviceUID));
		assert (serviceUID.equals(Shared.SERVICE_NAME));
		Shared.cappInstance.registerProfileCallbacks(NaviVoiceAPI.profileApiUid, mPlayer);
	}

	public void onIncomingServiceAfterLoading(String serviceUID) {
		Log.v(tag, Logging.getCurrentMethodName(serviceUID));
		assert (serviceUID.equals(Shared.SERVICE_NAME));
	}

	public void onServiceLoadError(String serviceUID) {
		Log.v(tag, Logging.getCurrentMethodName(serviceUID));
		android.os.Process.killProcess(android.os.Process.myPid());
	}

	public void onServiceDropped(String serviceUID) {
		Log.v(tag, Logging.getCurrentMethodName(serviceUID));
		android.os.Process.killProcess(android.os.Process.myPid());
	}

	public void onPhysicalLinkDown() {
		Log.v(tag, Logging.getCurrentMethodName());
		android.os.Process.killProcess(android.os.Process.myPid());
	}

	public void onLinkEstablished() {
		Log.v(tag, Logging.getCurrentMethodName());
	}
}
