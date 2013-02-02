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


package com.luxoft.ivilink.sdk.helpers;

import android.app.Notification;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;

import com.luxoft.ivilink.sdk.R;
import com.luxoft.ivilink.sdk.SysCtrlServLauncherActivity;

public class NotificationHandler {

	/**
	 * Creates a Notification for the SystemControllerService with a
	 * PendingIntent that will launch SysCtrlServLaunchActivity when activated
	 * 
	 * @param creator
	 *            Used in PendingIntent creation
	 * @return created Notification
	 */
	@SuppressWarnings("deprecation")
	public static Notification createMainNotification(Context creator) {
		int icon = R.drawable.ic_launcher;
		CharSequence tickerText = "IVILinkSDK started";
		long when = System.currentTimeMillis();
		Notification notification = new Notification(icon, tickerText, when);
		Context context = creator.getApplicationContext();
		CharSequence contentTitle = "IVILink SDK is running!";
		CharSequence contentText = "Click for options";
		Intent notificationIntent = new Intent(creator, SysCtrlServLauncherActivity.class);
		notificationIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		notificationIntent.putExtra(Common.shutdown, true);
		PendingIntent contentIntent = PendingIntent.getActivity(creator, 0, notificationIntent, 0);
		notification.setLatestEventInfo(context, contentTitle, contentText, contentIntent);
		return notification;
	}

	/**
	 * Creates a Notification with empty icon and null message. Since all of
	 * iviLink's components are run as foreground services, but it would be too
	 * much to show 6 status bar notification (only SystemControllerService
	 * shows one), fake ones are generated.
	 * 
	 * @return created Notification
	 */
	@SuppressWarnings("deprecation")
	public static Notification createFakeNotification() {
		Notification note = new Notification(0, null, System.currentTimeMillis());
		note.flags |= Notification.FLAG_NO_CLEAR;
		return note;
	}
}
