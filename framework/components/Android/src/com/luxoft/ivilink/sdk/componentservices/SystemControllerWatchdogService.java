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
package com.luxoft.ivilink.sdk.componentservices;

import android.content.Context;
import android.content.Intent;

import com.luxoft.ivilink.sdk.SystemControllerWatchdog;
import com.luxoft.ivilink.sdk.SystemControllerWatchdog.SystemControllerWatchdogCallback;
import com.luxoft.ivilink.sdk.helpers.AlarmHandler;
import com.luxoft.ivilink.sdk.helpers.Common;
import com.luxoft.ivilink.sdk.helpers.StartupProgress;

public class SystemControllerWatchdogService extends ProtoComponentService implements
		SystemControllerWatchdogCallback {

	private SystemControllerWatchdog mComponent;
	private String mPasswordToShutdown;

	public void resetStack() {
		// scheduling restart
		AlarmHandler.setAlarm((Context) this, AlarmHandler.RESTART_TIMEOUT);
		// killing all components including self
		sendBroadcast(new Intent(mPasswordToShutdown));
	}

	@Override
	protected StartupProgress getProgress() {
		return StartupProgress.SYSTEM_WATCHDOG;
	}

	@Override
	protected void startComponent(Intent intent) {
		mPasswordToShutdown = intent.getStringExtra(Common.PASSWORD);
		mComponent = new SystemControllerWatchdog((SystemControllerWatchdogCallback) this);
		mComponent.start();
	}

}
