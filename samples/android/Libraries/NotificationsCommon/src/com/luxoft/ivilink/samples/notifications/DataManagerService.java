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
package com.luxoft.ivilink.samples.notifications;

import java.util.Map;

import org.json.JSONException;
import org.json.JSONObject;

import com.luxoft.ivilink.sdk.android.lib.utils.ForApp;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;

public abstract class DataManagerService extends Service implements DataSender {
	protected final String tag = "iviLink.samples.Notifications.DataReceiverService";
	private static boolean hasNativeStated;
	BroadcastReceiver killer;

	static {
		System.loadLibrary("appwrapper");
	}

	@Override
	public IBinder onBind(Intent intent) {
		return null;
	}

	protected Map<String, RequestManager> managersMap;

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		killer = ForApp.registerReceiverFromKillerApp(this);
		String launchInfo = intent.getStringExtra(IntentTags.launchInfo);
		Log.v(tag, "launchInfo: " + launchInfo);
		String serviceName = intent.getStringExtra(IntentTags.serviceName);
		Log.v(tag, "serviceName: " + serviceName);
		fillManagersMap();
		if (!hasNativeStated) {
			startApp(ForApp.getServicePath(), launchInfo,
					ForApp.getInternalPath(this), serviceName);
			hasNativeStated = true;
		}
		return onStartCommandImpl();
	}

	abstract protected void fillManagersMap();

	abstract protected int onStartCommandImpl(); // TODO notification and
													// foreground stuff

	// TODO called from native
	@SuppressWarnings("unused")
	protected void handleMessage(String message) {
		JSONObject obj;
		try {
			obj = new JSONObject(message);
		} catch (JSONException e) {
			Log.e(tag, "invalid json format: " + message);
			e.printStackTrace();
			return;
		}
		PermittedReceivers receiver = PackingHelper.getReceiver(message);
		if (receiver == PermittedReceivers.NOF) {
			Log.e(tag, "Receiver is not in the lists of permitted receivers");
			return;
		}
		RequestManager manager;
		if ((manager = managersMap.get(receiver.toString())) != null) {
			JSONObject request = PackingHelper.extractRequest(message);
			if (request == null) {
				Log.e(tag, "Request is null!");
				return;
			}
			manager.manageRequest(request);
		} else {
			Log.e(tag, "could not find manager for: " + receiver.toString());
		}
	}

	// called from native
	protected void onServiceDropped() {
		Log.w(tag, "Counter-side has died! Finishing!");
		android.os.Process.killProcess(android.os.Process.myPid());
	}

	private native void startApp(String servicePath, String launchInfo,
			String internalPath, String serviceName);

	public void sendData(String data) {
		if (hasNativeStated) {
			sendString(data);
		}
	}

	private native void sendString(String string);
}
