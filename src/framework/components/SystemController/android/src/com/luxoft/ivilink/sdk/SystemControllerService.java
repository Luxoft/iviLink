/* 
 * 
 * iviLINK SDK, version 1.1.2
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
 * 
 */



package com.luxoft.ivilink.sdk;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Handler;
import android.os.IBinder;
import android.util.Log;
import android.widget.Toast;

import com.luxoft.ivilink.sdk.android.lib.utils.ForSDK;
import com.luxoft.ivilink.sdk.authentication.AuthenticationActivity;
import com.luxoft.ivilink.sdk.helpers.AlarmHandler;
import com.luxoft.ivilink.sdk.helpers.Common;
import com.luxoft.ivilink.sdk.helpers.NotificationHandler;
import com.luxoft.ivilink.sdk.helpers.SecureRandomIntentFilterGenerator;

public class SystemControllerService extends Service {

	static {
		System.loadLibrary("ivi");
	}

	Handler launcherHandler = new Handler();

	static final String tag = Common.TAG+"Service";

	/*
	 * The SysCtrlServLauncherActivity may ask us
	 * to shutdown or reset
	 */
	ActivityIntentReceiver air;

	// contains ssm functionality
	CWrapper wrapper = new CWrapper();
	// contains connectivity agent functionality
	CConnectivityAgentWrapper agent = new CConnectivityAgentWrapper();
	// contains negotiator functionality
	CChannelSupervisorWrapper supervisor = new CChannelSupervisorWrapper();
	// contains application manager functionality
	CApplicationManagerWrapper appman = new CApplicationManagerWrapper();
	// contains profile manager functionality
	CProfileManagerWrapper profman = new CProfileManagerWrapper();
	// contains profile repository functionality
	CProfileRepositoryWrapper profrepo = new CProfileRepositoryWrapper();

	// empty method stub
	@Override
	public IBinder onBind(Intent arg0) {
		return null;
	}

	@Override
	public void onCreate() {
		/*
		 * Activity may ask us to shutdown or restart, 
		 * we don't want this to happen on accident 
		 * (and we don't want to bind the service, that's why intents are used),
		 * that's why we generate a random long String to use as an intent filter 
		 */
		Common.serviceBR = SecureRandomIntentFilterGenerator.nextSessionId();
		Log.v(tag, "password is: " + Common.serviceBR);
		air = new ActivityIntentReceiver();
		registerReceiver(air, new IntentFilter(Common.serviceBR));
		Common.serviceClassName = this.getClass().getName();
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		Log.v(tag, "starting SSM  thread");
		wrapper.start(new ComponentLauncher(this), false);
		return START_NOT_STICKY; // is set as a foreground service later
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
		unregisterReceiver(air);
	}

	/* launcher functions */

	void launchConnectivityAgent() {
		Log.v(tag, "starting Connectivity Agent thread");
		agent.start();
		sendBroadcast(new Intent(Common.ifProgress).putExtra(Common.message,
				Common.conAgntLaunch));
	}

	public void launchChannelSupervisor() {
		Log.v(tag, "starting Channel Supervisor thread");
		supervisor.start();
		sendBroadcast(new Intent(Common.ifProgress).putExtra(Common.message,
				Common.negLaunch));
	}

	public void launchProfileManager() {
		Log.v(tag, "starting Profile Manager thread");
		profman.start();
		sendBroadcast(new Intent(Common.ifProgress).putExtra(Common.message,
				Common.profmanLaunch));
	}

	public void launchApplicationManager() {
		Log.v(tag, "starting Application Manager thread");
		appman.start(ForSDK.getAppManDirectory(), AppLauncher
				.getInstance(SystemControllerService.this, launcherHandler));
		sendBroadcast(new Intent(Common.ifProgress).putExtra(Common.message,
				Common.appmanLaunch));
	}

	public void launchProfileRepository() {
		Log.v(tag, "starting Profile Repository thread");
		profrepo.start(ForSDK.getProfRepoDirectory());
		sendBroadcast(new Intent(Common.ifProgress).putExtra(Common.message,
				Common.profrepoLaunch));
	}

	public void launchAuthentication() {
		startActivity(new Intent(this, AuthenticationActivity.class).addFlags(Intent.FLAG_ACTIVITY_NEW_TASK));
	}

	void doReset() {
		Log.v(tag, "doReset()");
		AlarmHandler.setWakeApp(SystemControllerService.this, 2000);
		doShutdown();
	}

	void doShutdown() {
		Log.v(tag, "doShutdown()");
		// searching for running authentication in case the shutdown request came unexpected when it is still running
		int pid = ForSDK.findPidOfRunningAppOneShot(this, this.getPackageName()+".authentication");
		if(pid == -1){
			Log.i(tag, "Authentication process is not running");
		}else{
			android.os.Process.killProcess(pid);
		}
		android.os.Process.killProcess(android.os.Process.myPid());
	}

	void allOk() {
		Log.v(tag, "allOk() - switching to the foreground state");
		sendBroadcast(new Intent(Common.ifProgress).putExtra(Common.message,
				Common.doneLaunch));
		launcherHandler.post(new Runnable() {
			public void run() {
				Toast.makeText(SystemControllerService.this, Common.doneLaunch,
						Toast.LENGTH_LONG).show();
				startForeground(NotificationHandler.ID, NotificationHandler
						.createNotification(SystemControllerService.this));
			}
		});
	}

	class ActivityIntentReceiver extends BroadcastReceiver {
		@Override
		public void onReceive(Context context, Intent intent) {
			Log.v(tag, "intent came from the Launcher Activity");
			String misc = intent.getStringExtra(Common.misc);
			if (misc.equals(Common.reset)) {
				doReset();
			} else if (misc.equals(Common.shutdown)) {
				doShutdown();
			}
		}
	}
}
