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
 
package com.luxoft.ivilink.sdk.componentservices;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.IBinder;
import android.util.Log;

import com.luxoft.ivilink.sdk.android.lib.utils.log.BigLog;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;
import com.luxoft.ivilink.sdk.helpers.NotificationHandler;
import com.luxoft.ivilink.sdk.helpers.StartupProgress;

/**
 * All components except for System Controller are run in non-default processes
 * using Services derived from this class. It is made so to separate their
 * address spaces.
 */
public abstract class ProtoComponentService extends Service {

	private static final int NOTIFICATION_MAGIC_NUMBER = 42;

	@Override
	public IBinder onBind(Intent intent) {
		return null;
	}

	protected abstract StartupProgress getProgress();

	protected abstract void startComponent(Intent intent);

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		BigLog.i(this.toString(), Logging.getCurrentMethodName(intent, flags, startId), '!');
		if (startId != 1) {
			BigLog.i(this.toString(), "Trying to start service more than once!", '!');
			return START_NOT_STICKY;
		}
		String password = intent.getStringExtra("password");
		assert (password != null) : "Password was not provided!";
		BigLog.i(this.toString(), "Password: " + password, '!');
		registerReceiver(new FromSystemControllerBroadcastReceiver(), new IntentFilter(password));
		startComponent(intent);
		startForeground(NOTIFICATION_MAGIC_NUMBER, NotificationHandler.createFakeNotification());
		getProgress().updateProgress((Service) this);
		return START_NOT_STICKY;
	}

	class FromSystemControllerBroadcastReceiver extends BroadcastReceiver {
		@Override
		public void onReceive(Context context, Intent intent) {
			Log.v(ProtoComponentService.this.toString(),
					Logging.getCurrentMethodName(context, intent));
			unregisterReceiver(this);
			android.os.Process.killProcess(android.os.Process.myPid());
		}
	}
}
