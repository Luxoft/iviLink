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
 

package com.luxoft.ivilink.sdk;

import java.io.IOException;
import java.io.InputStream;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.res.Configuration;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.LinearLayout.LayoutParams;
import android.widget.ProgressBar;
import android.widget.Spinner;
import android.widget.TextView;

import com.luxoft.ivilink.samples.splash.Splash;
import com.luxoft.ivilink.samples.splash.Splash.Length;
import com.luxoft.ivilink.sdk.android.lib.utils.ForApp;
import com.luxoft.ivilink.sdk.android.lib.utils.ForSDK;
import com.luxoft.ivilink.sdk.bluetooth.BluetoothHelper;
import com.luxoft.ivilink.sdk.bluetooth.BluetoothStatus;
import com.luxoft.ivilink.sdk.helpers.Common;
import com.luxoft.ivilink.sdk.helpers.DirectoryHelper;

/*
 * Entry point of iviLink's daemon.
 * If SystemControllerService is not running,
 * it checks directories and starts the Service if everything is ok.
 * Displays stack's startup progress.
 */
public class SysCtrlServLauncherActivity extends Activity {
	/*
	 * Contains the following options: shutdown, reset, cancel (do nothing) is
	 * shown when: a) app's icon was clicked and the Service is running already
	 * b) status bar notification was clicked
	 */
	AlertDialog alert;
	/*
	 * Displayed during stack's startup process
	 */
	Dialog progress;
	ProgressBar progressBar;
	TextView progressText;

	com.luxoft.ivilink.sdk.helpers.Spinner mSpinner;
	/*
	 * Receives requests to update progress's text
	 */
	ProgressDialogBroadcastReceiver pdbr = new ProgressDialogBroadcastReceiver();
	// tag for logging
	static final String tag = Common.TAG + "EntryActivity";
	boolean isServiceForHelperSet = false;

	@Override
	public void onCreate(Bundle icicle) {
		super.onCreate(icicle);
		registerReceiver(pdbr, new IntentFilter(Common.ifProgress));

		if (getIntent().getBooleanExtra(Common.resetDialog, false)) {
			// activity was started via a pending intent from a status bar
			// notification
			createAlertDialogThreeButtons();
			return;
		}
		if (!isSysCtrlServiceRunning()) {
			// checks that the external storage is accessible and there are at
			// least some files in directories
			if (DirectoryHelper.checkProjectDirs() && DirectoryHelper.checkExtState()) {
				Splash.displaySplashScreen(this, new Runnable() {
					public void run() {
						Log.i(tag, "All checks are green, starting the service");
						ForApp.setWindowFlagsToKeepDeviceOn(SysCtrlServLauncherActivity.this);
						if (BluetoothStatus.isBluetoothUsable()) {
							Log.i(tag, "Bluetooth is available!");
							BluetoothStatus.requestDiscoverable(SysCtrlServLauncherActivity.this);
						} else {
							Log.e(tag, "Bluetooth is not avaliable!");
							startService(new Intent(SysCtrlServLauncherActivity.this,
									SystemControllerService.class).putExtra("bluetooth", false));
						}
					}
				}, Length.LONG);
			} else {
				if (!DirectoryHelper.checkExtState()) {
					// external storage is not mounted
					createAlertDialogOneButton("Please mount external storage and try again");
				} else {
					// external storage is mounted, but some of needed
					// directories were not there
					// diplay an alert
					createAlertDialogOneButton(DirectoryHelper.createMissingDirs());
				}
			}
		} else {
			Log.i(tag, "Service is running already");
			createAlertDialogThreeButtons();
		}

	}

	@Override
	public void onDestroy() {
		super.onDestroy();
		unregisterReceiver(pdbr);
		if (mSpinner != null) {
			mSpinner.stop();
		}
	}

	@Override
	public void onBackPressed() {
		Log.v(tag, "Overriding back button");
	}

	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		Log.v(tag, "overriding onConfigurationChanged");
		super.onConfigurationChanged(newConfig);
	}

	// checks if SystemControllerService is already launched
	boolean isSysCtrlServiceRunning() {
		if (Common.serviceClassName == null)
			return false;
		return ForSDK.checkIfServiceIsRunning(this, Common.serviceClassName);
	}

	void createAlertDialogOneButton(String message) {
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		builder.setMessage(message).setCancelable(false)
				.setPositiveButton("OK", new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int id) {
						alert.cancel();
						SysCtrlServLauncherActivity.this.finish();
					}
				});
		alert = builder.create();
		alert.show();
	}

	void createAlertDialogThreeButtons() {
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		builder.setMessage("Shutdown/restart IVILink?").setCancelable(false)
				.setPositiveButton("Shutdown", new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int id) {
						alert.cancel();
						SysCtrlServLauncherActivity.this.finish();
						sendBroadcast(new Intent(Common.serviceBR).putExtra(Common.misc,
								Common.shutdown));
					}
				}).setNeutralButton("Restart", new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int id) {
						alert.cancel();
						SysCtrlServLauncherActivity.this.finish();
						sendBroadcast(new Intent(Common.serviceBR).putExtra(Common.misc,
								Common.reset));
					}
				}).setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int id) {
						alert.cancel();
						SysCtrlServLauncherActivity.this.finish();
					}
				});
		alert = builder.create();
		alert.show();
	}

	class ProgressDialogBroadcastReceiver extends BroadcastReceiver {
		@Override
		public void onReceive(Context context, Intent intent) {
			String reqString = intent.getStringExtra(Common.message);
			int progressValue = intent.getIntExtra(Common.progressValue, 0);
			Log.i(this.getClass().getSimpleName(), "got intent with: " + reqString + " "
					+ progressValue);
			if (reqString != null)
				setProgressText(reqString, progressValue);
		}
	}

	/**
	 * Displays message with progress bar value in a Dialog
	 */
	void setProgressText(String msg, int progressValue) {
		if (msg.equals(Common.doneLaunch)) {
			if (progress != null) {
				if (progress.isShowing())
					progress.dismiss();
			}
			this.finish();
			return;
		}
		if (progress == null) {
			progress = createProgressDialog();
			progressBar = (ProgressBar) progress.findViewById(R.id.progressBar);
			progressText = (TextView) progress.findViewById(R.id.progressText);
			progressBar.setProgress(progressValue);
			progressText.setText(msg);
			progress.show();
			progress.setCancelable(false);
		} else {
			progressBar.setProgress(progressValue);
			progressText.setText(msg);
		}
	}

	private Dialog createProgressDialog() {
		Dialog result = new Dialog(this, R.style.custom_dialog);
		result.setContentView(R.layout.progressdisplay);
		ImageView spinner = (ImageView) result.findViewById(R.id.spinnerImage);
		spinner.setScaleType(ImageView.ScaleType.CENTER);
		mSpinner = new com.luxoft.ivilink.sdk.helpers.Spinner(new Handler(), spinner, 100,
				getResources());
		result.setTitle("iviLink stack is starting...");
		return result;
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		// we get here after BluetoothHelper.requestDiscovery
		if (resultCode == RESULT_CANCELED) {
			Log.w(tag, "RFESULT_CANCELED");
			startService(new Intent(this, SystemControllerService.class).
					putExtra("bluetooth", false));
		} else {
			Log.i(tag, "RESULT_OK, will be using bluetooth!");
			startService(new Intent(this, SystemControllerService.class)
					.putExtra("bluetooth", true));
		}
	}
}
