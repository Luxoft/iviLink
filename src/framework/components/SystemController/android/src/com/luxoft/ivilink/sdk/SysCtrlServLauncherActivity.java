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

import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.util.Log;

import com.luxoft.ivilink.sdk.android.lib.utils.ForApp;
import com.luxoft.ivilink.sdk.android.lib.utils.ForSDK;
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
	 * Contains the following options: shutdown, reset, cancel (do nothing)
	 * is shown when:
	 * a) app's icon was clicked and the Service is running already
	 * b) status bar notification was clicked
	 */
	AlertDialog alert;
	/*
	 * Displayed during stack's startup process
	 */
	ProgressDialog progress;
	/*
	 * Receives requests to update progress's text
	 */
	ProgressDialogBroadcastReceiver pdbr = new ProgressDialogBroadcastReceiver();
	// tag for logging
	static final String tag = Common.TAG+"EntryActivity";

	@Override
	public void onCreate(Bundle icicle) {
		super.onCreate(icicle);
		registerReceiver(pdbr, new IntentFilter(Common.ifProgress));
		
		if(getIntent().getBooleanExtra(Common.resetDialog, false)){
			// activity was started via a pending intent from a status bar notification
			createAlertDialogThreeButtons();
			return;
		}
		if (!isSysCtrlServiceRunning()) {
			// checks that the external storage is accessible and there are at least some files in directories
			if (DirectoryHelper.checkProjectDirs() && DirectoryHelper.checkExtState()) {
				Log.v(tag, "All checks are green, starting the service");
				ForApp.setWindowFlagsToKeepDeviceOn(this);
				startService(new Intent(this, SystemControllerService.class));
			} else {
				if (!DirectoryHelper.checkExtState()) {
					// external storage is not mounted
					createAlertDialogOneButton("Please mount external storage and try again");
				} else {
					// external storage is mounted, but some of needed directories were not there
					// diplay an alert
					createAlertDialogOneButton(DirectoryHelper.createMissingDirs());
				}
			}
		} else {
			Log.v(tag, "Service is running already");
			createAlertDialogThreeButtons();
		}
		
	}
	
	@Override
	public void onDestroy(){
		super.onDestroy();
		unregisterReceiver(pdbr);
	}
	
	@Override
	public void onBackPressed(){
		Log.v(tag, "Overriding back button");
	}

	// checks if SystemControllerService is already launched
	boolean isSysCtrlServiceRunning() {
		if (Common.serviceClassName == null) return false;
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
						sendBroadcast(new Intent(Common.serviceBR).putExtra(Common.misc, Common.shutdown));
					}
				})
				.setNeutralButton("Restart", new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int id) {
						alert.cancel();
						SysCtrlServLauncherActivity.this.finish();
						sendBroadcast(new Intent(Common.serviceBR).putExtra(Common.misc, Common.reset));
					}
				})
				.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int id) {
						alert.cancel();
						SysCtrlServLauncherActivity.this.finish();
					}
				});
		alert = builder.create();
		alert.show();
	}
	
	class ProgressDialogBroadcastReceiver extends BroadcastReceiver{
		@Override
		public void onReceive(Context context, Intent intent) {
			String reqString = intent.getStringExtra(Common.message);
			Log.v("SysCtrlAndr.ProgressDialogBR", "got intent with: "+reqString);
			if(reqString!= null) setProgressText(reqString);
		}
	}
	
	/*
	 * displays msg in a ProgressDialog
	 */
	void setProgressText(String msg){
		if(msg.equals(Common.doneLaunch)){
			if(progress!=null) {
				if(progress.isShowing()) progress.dismiss();
			}
			this.finish();
			return;
		}
		if(progress == null){
			progress = new ProgressDialog(this, ProgressDialog.STYLE_SPINNER);
			progress.show();
			progress.setCancelable(false);
		}
		progress.setMessage(msg);
	}
}
