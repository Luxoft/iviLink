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

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;

public class NotificationHandler {

	public static final int ID = 1;

	public static void setNotification(Context creator) {
		NotificationManager mNotificationManager = (NotificationManager) creator
				.getSystemService(Context.NOTIFICATION_SERVICE);
		Notification notification = createNotification(creator);
		mNotificationManager.notify(ID, notification); // id is needed for cancel
	}
	

	public static Notification createNotification(Context creator) {
		int icon = R.drawable.ic_launcher;
		CharSequence tickerText = "ScreenSharing Server started";
		long when = System.currentTimeMillis();
		Notification notification = new Notification(icon, tickerText, when);

		Context context = creator.getApplicationContext();
		CharSequence contentTitle = "ScreenSharingServer is running!";
		CharSequence contentText = "Click for exit";
		Intent notificationIntent = new Intent(Settings.EXIT_INTENT_TAG);
		notificationIntent.putExtra(Settings.EXIT, true);
		PendingIntent contentIntent = PendingIntent.getBroadcast(creator, 0,
				notificationIntent, 0);

		notification.setLatestEventInfo(context, contentTitle, contentText,
				contentIntent);
		return notification;
	}
}
