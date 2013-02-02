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
package com.luxoft.ivilink.samples.screensharingviewer;

import org.json.JSONException;
import org.json.JSONObject;

import android.app.Activity;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.res.Configuration;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.luxoft.ivilink.samples.screensharingviewer.R;
import com.luxoft.ivilink.samples.screensharingviewer.android.androidVNC.VncCanvasActivity;
import com.luxoft.ivilink.samples.screensharingviewer.service.JsonStrings;
import com.luxoft.ivilink.samples.screensharingviewer.service.Settings;
import com.luxoft.ivilink.samples.splash.Splash;
import com.luxoft.ivilink.samples.splash.Splash.Length;
import com.luxoft.ivilink.sdk.android.lib.utils.ForApp;

public class ScreenSharingViewer extends Activity implements OnClickListener {

	// TabHost th;
	FromServiceBR fsbr;
	ShowMsgBoxBR smbbr = new ShowMsgBoxBR();
	BridgeC bridge;

	ProgressDialog progress;

	ProgressDialogBroadcastReceiver pdbr = new ProgressDialogBroadcastReceiver();

	VncButtonListener mvbl = new VncButtonListener();

	Dialog vncDialog;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		ForApp.setWindowFlagsToKeepDeviceOn(this);
		Splash.displaySplashScreen(this, new Runnable() {
			public void run() {

				progress = new ProgressDialog(ScreenSharingViewer.this,
						ProgressDialog.STYLE_SPINNER);
				progress.show();
				progress.setCancelable(false);

				progress.setMessage("Launching ScreenSharingViewer");

				fsbr = new FromServiceBR();
				IntentFilter filter = new IntentFilter(Settings.ifFromService);
				IntentFilter filter2 = new IntentFilter(
						Settings.ifFromAnyActivity);
				registerReceiver(fsbr, filter);
				registerReceiver(fsbr, filter2);

				smbbr = new ShowMsgBoxBR();
				filter = new IntentFilter(Settings.ifToShow);
				registerReceiver(smbbr, filter);

				vncDialog = new Dialog(ScreenSharingViewer.this);

				bridge = BridgeC.getInstance(ScreenSharingViewer.this,
						new Handler());
				bridge.startApp();

			}
		}, Length.SHORT);
	}

	@Override
	public void onResume() {
		super.onResume();
		Configuration config = this.getResources().getConfiguration();
		Log.v("CreenSharingViewer", "my current configuration: "
				+ config.keyboard);
		// TODO smth
	}

	public void startVncClient() {
		Intent intent = new Intent(this, VncCanvasActivity.class);
		intent.putExtra("vnc ip", Settings.remoteIp);
		intent.putExtra("vnc port", 5901);
		startActivityForResult(intent, 5352);
	}

	@Override
	public void onPause() {
		super.onPause();
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		Log.v("CreenSharingViewer", "Returned from vnc viewer");
		android.os.Process.killProcess(android.os.Process.myPid());
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
		Log.v("CreenSharingViewer", "onDestroy()");
		unregisterReceiver(this.fsbr);
		unregisterReceiver(this.smbbr);
	}

	class FromServiceBR extends BroadcastReceiver {
		@Override
		public void onReceive(Context arg0, Intent intent) {
			byte[] message = intent.getByteArrayExtra(Settings.tMessage);
			if (message != null) {
				byte channel = intent.getByteExtra(Settings.tChannel, (byte) 0);
				switch (channel) {
				case Settings.cVnc:
					try {
						if (new JSONObject(new String(message)).getString(
								JsonStrings.vnc).equals(JsonStrings.client)) {
							ScreenSharingViewer.this.startVncClient();
						} else if (new JSONObject(new String(message))
								.getString(JsonStrings.vnc).equals(
										JsonStrings.disconnectAttempt)) {
							bridge.sendData(Settings.vncMsgExit);
							new Handler().postDelayed(new Runnable() {
								@Override
								public void run() {
									android.os.Process
											.killProcess(android.os.Process
													.myPid());
								}
							}, 2000);
						}
					} catch (JSONException e) {
						e.printStackTrace();
					}
					break;
				default:
					Log.v("CreenSharingViewer", "unknown channel=" + channel
							+ " , ignoring message");
				}
			} else {
				boolean status = intent
						.getBooleanExtra(Settings.tStatus, false);
				Toast.makeText(
						ScreenSharingViewer.this,
						status ? "Connection is online"
								: "Connection is offline", Toast.LENGTH_SHORT)
						.show();
			}
		}
	}

	public Dialog createVncDialog() {
		Dialog dialog = new Dialog(ScreenSharingViewer.this);
		dialog.setContentView(R.layout.vncdialog);
		dialog.setTitle("Please confirm");
		TextView txt = (TextView) dialog.findViewById(R.id.vncDialogText);
		txt.setText("Start screen sharing session as a client?\n");
		Button yes = (Button) dialog.findViewById(R.id.vncOk);
		yes.setOnClickListener(mvbl);
		Button no = (Button) dialog.findViewById(R.id.vncCancel);
		no.setOnClickListener(this.mvbl);
		return dialog;
	}

	public void startVncDialog() {
		if (vncDialog != null) {
			if (vncDialog.isShowing()) {
				Log.v("CreenSharingViewer", "Duplicate vnc requests!");
				return;
			}
		}
		vncDialog = createVncDialog();
		vncDialog.show();
	}

	class VncButtonListener implements OnClickListener {
		@Override
		public void onClick(View v) {
			switch (v.getId()) {
			case R.id.vncOk:
				Log.v("CreenSharingViewer", "VNC dialog : OK");
				bridge.sendData(Settings.vncMsgStartAck);

				progress.dismiss();
				vncDialog.dismiss();

				new Handler().postDelayed(new Runnable() {
					@Override
					public void run() {
						startVncClient();

					}
				}, 4000);

				break;
			case R.id.vncCancel:
				Log.v("CreenSharingViewer", "VNC dialog : CANCEL");
				bridge.sendData(Settings.vncMsgExit);
				vncDialog.dismiss();
				android.os.Process.killProcess(android.os.Process.myPid());
				break;
			}
		}
	}

	class ShowMsgBoxBR extends BroadcastReceiver {
		@Override
		public void onReceive(Context arg0, Intent intent) {
			Log.v("CreenSharingViewer activity", "preparing a message box");
			int id = intent.getIntExtra(Settings.tMsgBxId, -1);
			switch (id) {
			case Settings.idKillAll:
				if (vncDialog.isShowing())
					vncDialog.dismiss();
				new Handler().postDelayed(new Runnable() {
					@Override
					public void run() {
						android.os.Process.killProcess(android.os.Process
								.myPid());
					}
				}, 5000);
				break;
			case -1:
				if (vncDialog.isShowing())
					vncDialog.dismiss();
				break;
			case Settings.idVncConfirmation:
				if (vncDialog != null) {
					if (vncDialog.isShowing()) {
						Log.v("CreenSharingViewer", "Duplicate vnc requests!");
						return;
					}
				}
				vncDialog = createVncDialog();
				vncDialog.show();
				break;
			}
		}

	}

	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);
		// to avoid destruction when vnc is launched
		Log.v("CreenSharingViewer", "config changed: " + newConfig.keyboard);
	}

	@Override
	public void onClick(View arg0) {
	}

	class ProgressDialogBroadcastReceiver extends BroadcastReceiver {
		@Override
		public void onReceive(Context context, Intent intent) {
			String reqString = intent.getStringExtra(Settings.tMessage);
			Log.v("SCreenSharingViewer.ProgressDialogBR", "got intent with: "
					+ reqString);
			if (reqString != null)
				setProgressText(reqString);
		}
	}

	void setProgressText(String msg) {
		if (msg.equals(Settings.tVNCLaunch)) {
			if (progress != null) {
				if (progress.isShowing())
					progress.dismiss();
			}
			this.finish();
			return;
		}
		if (progress == null) {
			progress = new ProgressDialog(this, ProgressDialog.STYLE_SPINNER);
			progress.show();
			progress.setCancelable(false);
		}
		progress.setMessage(msg);
	}
}
