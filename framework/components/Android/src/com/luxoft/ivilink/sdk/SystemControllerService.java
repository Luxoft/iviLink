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


package com.luxoft.ivilink.sdk;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.util.Log;
import android.widget.Toast;

import com.luxoft.ivilink.sdk.android.lib.utils.IntentActions;
import com.luxoft.ivilink.sdk.android.lib.utils.ProcessesHelper;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;
import com.luxoft.ivilink.sdk.authentication.AuthenticationActivity;
import com.luxoft.ivilink.sdk.authentication.AuthenticationProcessInfo;
import com.luxoft.ivilink.sdk.componentservices.ApplicationManagerService;
import com.luxoft.ivilink.sdk.componentservices.ChannelSupervisorService;
import com.luxoft.ivilink.sdk.componentservices.ConnectivityAgentService;
import com.luxoft.ivilink.sdk.componentservices.ProfileLayerService;
import com.luxoft.ivilink.sdk.componentservices.ShutdownBroadcastReceiver;
import com.luxoft.ivilink.sdk.componentservices.SystemControllerWatchdogService;
import com.luxoft.ivilink.sdk.config.Config;
import com.luxoft.ivilink.sdk.helpers.AlarmHandler;
import com.luxoft.ivilink.sdk.helpers.BigIntegerGenerator;
import com.luxoft.ivilink.sdk.helpers.Common;
import com.luxoft.ivilink.sdk.helpers.NotificationHandler;
import com.luxoft.ivilink.sdk.loggingdaemon.LogsRecorder;

public class SystemControllerService extends Service {
	// empty method stub
	@Override
	public IBinder onBind(Intent arg0) {
		// cannot be bound
		return null;
	}

	private static final String tag = SystemControllerService.class.getName();

	private final String componentsPassword = BigIntegerGenerator.nextSessionId();

	// the value is set from the SysCtrlLauncherActivity's intent
	private boolean bluetoothPermitted = false;

	/*
	 * The SysCtrlServLauncherActivity may ask us to shutdown or reset
	 */
	private ActivityIntentReceiver air;

	// contains ssm functionality
	private CWrapper wrapper = new CWrapper();

	@Override
	public void onCreate() {
		/*
		 * Activity may ask us to shutdown or restart, we don't want this to
		 * happen on accident (and we don't want to bind the service, that's why
		 * intents are used), that's why we generate a random long String to use
		 * as an intent filter
		 */
		Common.serviceBR = BigIntegerGenerator.nextSessionId();
		Log.i(tag, "password is: " + Common.serviceBR);
		air = new ActivityIntentReceiver();
		registerReceiver(air, new IntentFilter(Common.serviceBR));
		Common.serviceClassName = this.getClass().getName();
		// since it may be killed by the wd
		registerReceiver(new ShutdownBroadcastReceiver(), new IntentFilter(componentsPassword));
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		if (startId != 1) {
			Log.e(tag, "Trying to start " + this.getClass().getSimpleName() + " more than once!");
			return START_NOT_STICKY;
		}
		bluetoothPermitted = intent.getBooleanExtra("bluetooth", false);
		Log.i(tag, "starting SSM  thread");
		if (Config.sendMails) {
			LogsRecorder.startRecording();
		}
		wrapper.start(new ComponentLauncher(this), Config.useAuthentication);
		startForeground(1, NotificationHandler.createMainNotification(this));
		return START_NOT_STICKY; // is set as a foreground service anyway
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
		unregisterReceiver(air);
	}

	/* launcher functions */

	void launchService(Class<?> serviceClass) {
		Log.v(tag, Logging.getCurrentMethodName(serviceClass));
		startService(new Intent(this, serviceClass).putExtra(Common.PASSWORD, componentsPassword));
	}

	void launchConnectivityAgent() {
		Log.v(tag, Logging.getCurrentMethodName());
		startService(new Intent(this, ConnectivityAgentService.class).putExtra(Common.PASSWORD,
				componentsPassword).putExtra("bluetooth", bluetoothPermitted));
	}

	void launchSystemControllerWatchdog() {
		Log.v(tag, Logging.getCurrentMethodName());
		launchService(SystemControllerWatchdogService.class);
	}

	void launchChannelSupervisor() {
		Log.v(tag, Logging.getCurrentMethodName());
		launchService(ChannelSupervisorService.class);
	}

	void launchProfileManager() {
		Log.v(tag, Logging.getCurrentMethodName());
		launchService(ProfileLayerService.class);
	}

	void launchApplicationManager() {
		Log.v(tag, Logging.getCurrentMethodName());
		launchService(ApplicationManagerService.class);
	}

	public void launchAuthentication() {
		Log.v(tag, Logging.getCurrentMethodName());
		startActivity(new Intent(this, AuthenticationActivity.class)
				.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK));
	}

	private void doShutdown(final boolean reset) {
		Log.v(tag, Logging.getCurrentMethodName(reset));
		// antiIviLink
		sendBroadcast(new Intent(IntentActions.SHUT_DOWN_NOTIFICATION));
		if (Config.sendMails) {
			Toast.makeText(this, "Sending mail with logs now, will shutdown shortly",
					Toast.LENGTH_LONG).show();
		}
		/*
		 * to avoid network on main thread exception if mails are sent, and wait
		 * for authentication's shutting down if it is on
		 */
		new Thread(new Runnable() {
			public void run() {
			    String authProcessName = AuthenticationProcessInfo.getAuthenticationProcessName(SystemControllerService.this);
				int pid = ProcessesHelper.findPidOfApp(SystemControllerService.this, authProcessName);
				if (pid != ProcessesHelper.ERROR_PID) {
					Log.i(tag, "Authentication is on-screen, killing");
					android.os.Process.killProcess(pid);
					int antiEndlessLoopCounter = 0;
					// this is to prevent a restart instead of a shutdown
					while (!SysCtrlServLauncherActivity.isActive()) {
						antiEndlessLoopCounter++;
						if (antiEndlessLoopCounter > 5) {
							Log.w(tag, "After auth kill, waited too long for entry activity to resume, breaking");
							// trying to shut it down at least 
							sendBroadcast(new Intent(Common.ifProgress).putExtra(Common.message, Common.doneLaunch));
							break;
						}
						Log.i(tag, "After auth kill, waiting for our entry activity to resume");
						try {
							Thread.sleep(50);
						} catch (InterruptedException e) {
							e.printStackTrace();
						}
					}
					Log.i(tag, "After auth kill, our entry activity has resumed");
				}
				if (Config.sendMails) {
					LogsRecorder.mailLog();
					Log.i(tag, "mailing done!");
				} else {
					// we are giving time for anti-ivilink to kick in
					try {
						Thread.sleep(1000);
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
				}
				if (reset) {
					AlarmHandler.setAlarm(SystemControllerService.this);
				}
				// kills all components including self
				sendBroadcast(new Intent(componentsPassword));
			}
		}).start();
	}

	private void allOk() {
		Log.v(tag, "allOk() - switching to the foreground state");
		sendBroadcast(new Intent(Common.ifProgress).putExtra(Common.message, Common.doneLaunch));
		new Handler(Looper.getMainLooper()).post(new Runnable() {
			public void run() {
				Toast.makeText(SystemControllerService.this, Common.doneLaunch, Toast.LENGTH_LONG)
						.show();
			}
		});
	}

	class ActivityIntentReceiver extends BroadcastReceiver {
		@Override
		public void onReceive(Context context, Intent intent) {
			Log.v(tag, "intent came from the Launcher Activity");
			String misc = intent.getStringExtra(Common.misc);
			if (misc.equals(Common.reset)) {
				doShutdown(true);
			} else if (misc.equals(Common.shutdown)) {
				doShutdown(false);
			} else if (misc.equals(Common.doneLaunch)) {
				allOk();
			}
		}
	}
}
