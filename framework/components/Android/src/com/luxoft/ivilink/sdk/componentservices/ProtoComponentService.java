package com.luxoft.ivilink.sdk.componentservices;

import android.app.Service;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.IBinder;

import com.luxoft.ivilink.utils.Assertion;
import com.luxoft.ivilink.utils.log.BigLog;
import com.luxoft.ivilink.utils.log.Logging;
import com.luxoft.ivilink.sdk.helpers.Common;
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
		String password = intent.getStringExtra(Common.PASSWORD);
		Assertion.check(password != null, "Password was not provided!");
		BigLog.i(this.toString(), "Password: " + password, '!');
		registerReceiver(new ShutdownBroadcastReceiver(), new IntentFilter(password));
		startComponent(intent);
		startForeground(NOTIFICATION_MAGIC_NUMBER, NotificationHandler.createFakeNotification());
		try {
			getProgress().updateProgress((Service) this);
		} catch (NullPointerException e) {
			e.printStackTrace();
		}
		return START_NOT_STICKY;
	}

}
