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
package com.luxoft.ivilink.samples.notifications.server;

import java.util.HashMap;

import android.app.Notification;
import android.app.PendingIntent;
import android.content.Intent;
import android.util.Log;

import com.luxoft.ivilink.samples.notifications.DataManagerService;
import com.luxoft.ivilink.samples.notifications.PermittedReceivers;
import com.luxoft.ivilink.samples.notifications.RequestManager;

public class DataServiceServerImplementation extends DataManagerService {

	@Override
	protected void fillManagersMap() {
		managersMap = new HashMap<String, RequestManager>();
		Log.i("service", "about to create twitter manager");
		managersMap.put(PermittedReceivers.TWITTER.toString(),
				new TwitterServerRequestManager(this,
						PermittedReceivers.TWITTER, this));
		Log.i("service", "created twitter manager");
	}

	@SuppressWarnings("deprecation")
	@Override
	protected int onStartCommandImpl() {
		int icon = R.drawable.icon;
		CharSequence tickerText = "Notifications Server started!";
		long when = System.currentTimeMillis();
		Notification notification = new Notification(icon, tickerText, when);
		CharSequence contentTitle = "Notifications Server is running!";
		CharSequence contentText = "Click for options";
		Intent notificationIntent = new Intent(this, Options.class);
		notificationIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		PendingIntent contentIntent = PendingIntent.getActivity(this, 0,
				notificationIntent, 0);
		notification.setLatestEventInfo(this, contentTitle, contentText,
				contentIntent);
		startForeground(52, notification);
		return START_NOT_STICKY;
	}

}
