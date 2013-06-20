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
