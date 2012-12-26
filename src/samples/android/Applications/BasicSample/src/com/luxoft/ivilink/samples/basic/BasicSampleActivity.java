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
 

package com.luxoft.ivilink.samples.basic;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.widget.TextView;

import com.luxoft.ivilink.samples.basic.api.BasicSampleAPI;
import com.luxoft.ivilink.sdk.android.lib.utils.ForApp;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;
import com.luxoft.ivilink.sdk.android.nonnative.ApplicationStateCallbacks;
import com.luxoft.ivilink.sdk.android.nonnative.CApp;

public class BasicSampleActivity extends Activity implements
		ApplicationStateCallbacks, BasicSampleAPI.ApplicationAPI {

	final static String SERVICE_TO_LOAD_NAME = "BasicSampleServiceHUI";

	private String activeService;

	CApp app;
	BasicSampleProfileProxy profileProxy;
	TextView textView;
	final String tag = BasicSampleActivity.class.getCanonicalName();
	BroadcastReceiver killer;

	@Override
	public void onCreate(Bundle icicle) {
		Log.v(tag, Logging.getCurrentMethodName(icicle));
		super.onCreate(icicle);

		killer = ForApp.registerReceiverFromKillerApp(this);

		ForApp.setWindowFlagsToKeepDeviceOn(this);

		setContentView(R.layout.sampleapp);
		textView = (TextView) findViewById(R.id.resultsView);
		textView.setSingleLine(false);
		textView.setText("Hello iviLink!\n");

		app = new CApp((Activity) this, SERVICE_TO_LOAD_NAME);
	}

	@Override
	public void onResume() {
		Log.v(tag, Logging.getCurrentMethodName());
		super.onResume();
		app.initInIVILink((ApplicationStateCallbacks) this);
		if(!app.isLinkAlive()) {
			throw new RuntimeException();
		}
	}

	@Override
	public void onPause() {
		Log.v(tag, Logging.getCurrentMethodName());
		super.onPause();
		unregisterReceiver(killer);
		android.os.Process.killProcess(android.os.Process.myPid());
	}

	public void onInitDone(boolean isStartedByUser) {
		Log.v(tag, Logging.getCurrentMethodName(isStartedByUser));
		if (isStartedByUser) {
			app.registerProfileCallbacks(BasicSampleAPI.apiName,
					(BasicSampleAPI.ApplicationAPI) this);
			activeService = SERVICE_TO_LOAD_NAME;
			profileProxy = new BasicSampleProfileProxy(BasicSampleAPI.apiName,
					activeService);
			if (app.loadService(SERVICE_TO_LOAD_NAME)) {
				profileProxy.sendOperands((byte) 0x4, (byte) 0x10);
			} else {
				runOnUiThread(new Runnable() {
					public void run() {
						textView.append("Failed to load service: "
								+ SERVICE_TO_LOAD_NAME + "\n");
					}
				});
				Log.e(tag, "Failed to load service");
			}
		}
	}

	public void onIncomingServiceBeforeLoading(final String serviceUID) {
		Log.v(tag, Logging.getCurrentMethodName(serviceUID));
		activeService = serviceUID;
		profileProxy = new BasicSampleProfileProxy(BasicSampleAPI.apiName,
				activeService);
		app.registerProfileCallbacks(BasicSampleAPI.apiName,
				(BasicSampleAPI.ApplicationAPI) this);
		runOnUiThread(new Runnable() {
			public void run() {
				textView.append("Incoming service: " + serviceUID + "\n");
			}
		});
	}

	public void onIncomingServiceAfterLoading(String serviceUID) {
		Log.v(tag, Logging.getCurrentMethodName(serviceUID));
	}

	public void onServiceLoadError(final String serviceUID) {
		Log.v(tag, Logging.getCurrentMethodName(serviceUID));
		runOnUiThread(new Runnable() {
			public void run() {
				textView.append("Service load error: " + serviceUID + "\n");
			}
		});
	}

	public void onServiceDropped(final String serviceUID) {
		Log.v(tag, Logging.getCurrentMethodName(serviceUID));
		runOnUiThread(new Runnable() {
			public void run() {
				textView.append("Service dropped: " + serviceUID + "\n");
			}
		});
	}

	public void onPhysicalLinkDown() {
		Log.v(tag, Logging.getCurrentMethodName());
	}

	public void onLinkEstablished() {
		Log.v(tag, Logging.getCurrentMethodName());
	}

	/**
	 * Application callback. Invoked by profile from a background thread.
	 */
	public void receiveOperands(final byte a, final byte b) {
		Log.v(tag, Logging.getCurrentMethodName(a, b));
		profileProxy.sendResult((byte) (a + b));
		runOnUiThread(new Runnable() {
			public void run() {
				textView.append("Got operands: " + a + ", " + b + "\n");
			}
		});
	}

	/**
	 * Application callback. Invoked by profile from a background thread.
	 */
	public void receiveResult(final byte res) {
		Log.v(tag, Logging.getCurrentMethodName(res));
		runOnUiThread(new Runnable() {
			public void run() {
				textView.append("Got a result: " + res + "\n");
			}
		});
	}
}
