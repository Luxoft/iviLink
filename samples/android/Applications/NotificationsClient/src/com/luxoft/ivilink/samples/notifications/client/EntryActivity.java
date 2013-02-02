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
package com.luxoft.ivilink.samples.notifications.client;

import com.luxoft.ivilink.samples.notifications.IntentTags;
import com.luxoft.ivilink.samples.splash.Splash;
import com.luxoft.ivilink.samples.splash.Splash.Length;
import com.luxoft.ivilink.sdk.android.lib.utils.ForApp;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

public class EntryActivity extends Activity {

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		startService(new Intent(this, DataServiceClientImplementation.class)
				.putExtra(IntentTags.launchInfo, ForApp.getLaunchInfo(this))
				.putExtra(IntentTags.serviceName, "NotificationsClientService"));

		Splash.displaySplashScreen(this, new Runnable() {
			public void run() {
				EntryActivity.this.finish();
			}
		}, Length.SHORT);
	}

}
