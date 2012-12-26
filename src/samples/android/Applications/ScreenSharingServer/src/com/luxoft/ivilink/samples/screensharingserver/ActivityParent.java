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
 
package com.luxoft.ivilink.samples.screensharingserver;

import org.json.JSONException;
import org.json.JSONObject;

import android.app.Activity;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;

import com.luxoft.ivilink.samples.screensharingserver.R;
import com.luxoft.ivilink.samples.screensharingserver.commons.JsonStrings;
import com.luxoft.ivilink.samples.screensharingserver.commons.Settings;
import com.luxoft.ivilink.samples.screensharingserver.service.VncServerService;

public abstract class ActivityParent extends Activity {
	public String tag;
	public byte channel;
	public boolean isVisible = false;

	public Context mCntx;

	public GlobalBroadcastReceiver gbr = new GlobalBroadcastReceiver();

	@Override
	public void onCreate(Bundle icicle) {
		super.onCreate(icicle);
		mCntx = this;
		registerReceiver(gbr, new IntentFilter("global"));
		
		//if(Settings.authFailed) finish();
	}

	@Override
	public void onResume() {
		super.onResume();
		Log.v(tag, "on resume");
		Settings.currentActivity = tag;
		notifyRemoteOfMyActivity();
		isVisible = true;
	}

	@Override
	public void onPause() {
		Log.v(tag, "on pause");
		super.onPause();
		isVisible = false;
	}

	@Override
	public void onDestroy() {
		Log.v(tag, "on destroy");
		super.onDestroy();
		unregisterReceiver(gbr);
	}

	@Override
	public void onStop() {
		Log.v(tag, "on stop");
		super.onStop();
	}

	class GlobalBroadcastReceiver extends BroadcastReceiver {
		@Override
		public void onReceive(Context arg0, Intent arg1) {
			if (arg1.getBooleanExtra("finish", false)) {
				ActivityParent.this.finish();
				return;
			}
		}
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if ((keyCode == KeyEvent.KEYCODE_VOLUME_DOWN || keyCode == KeyEvent.KEYCODE_VOLUME_UP)
				&& Settings.isMuted) {
			return true;
		} else {
			return super.onKeyDown(keyCode, event);
		}
	}

	@Override
	public boolean onKeyUp(int keyCode, KeyEvent event) {
		if ((keyCode == KeyEvent.KEYCODE_VOLUME_DOWN || keyCode == KeyEvent.KEYCODE_VOLUME_UP)
				&& Settings.isMuted) {
			return true;
		} else {
			return super.onKeyUp(keyCode, event);
		}
	}

	public void notifyRemoteOfMyActivity() {
		Intent intent = new Intent(Settings.ifFromAnyActivity);
		intent.putExtra(Settings.tChannel, (byte) 0x00);
		JSONObject obj = new JSONObject();
		try {
			obj.put(JsonStrings.activity, tag);
		} catch (JSONException e) {
			e.printStackTrace();
		}
		intent.putExtra(Settings.tMessage, obj.toString().getBytes());
		sendBroadcast(intent);
	}

	@Override
	public void onBackPressed() {
	} // to avoid pausing activities
}
