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
 
package com.luxoft.ivilink.sdk.authentication;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.res.Configuration;
import android.os.Bundle;
import android.util.Log;

import com.luxoft.ivilink.sdk.android.lib.utils.ForApp;
import com.luxoft.ivilink.sdk.helpers.Common;

public class Deauthenticate extends Activity {

	AlertDialog alert;

	static final String tag = Common.TAG + "Deauthenticate";

	@Override
	public void onCreate(Bundle icicle) {
		super.onCreate(icicle);
		Log.v(tag, "onCreate()");
		createAlertDialog();
	}

	@Override
	public void onPause() {
		super.onPause();
		if (alert != null && alert.isShowing()) {
			alert.dismiss();
		}
		finish();
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

	void createAlertDialog() {
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		builder.setMessage("Clear iviLink's trustlist?").setCancelable(false)
				.setPositiveButton("Clear", new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int id) {
						Log.i(tag, "Clearing trustlist");
						findAndDeleteTrustList();
						alert.cancel();
						Deauthenticate.this.finish();
					}
				}).setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int id) {
						Log.i(tag, "Exiting");
						alert.cancel();
						Deauthenticate.this.finish();
					}
				});
		alert = builder.create();
		alert.show();
	}

	private void findAndDeleteTrustList() {
		File dir = new File(ForApp.getInternalPath(Deauthenticate.this));
		String[] fileList = dir.list();
		for (String fileName : fileList) {
			Log.i(tag, "checking file: " + fileName);
			if (!fileName.endsWith(".so") && !fileName.endsWith(".k")) {
				Log.i(tag, "found potential trust list, printing contents: ");
				FileReader fileReader;
				try {
					fileReader = new FileReader(dir.getAbsolutePath() + "/" + fileName);
					BufferedReader br = new BufferedReader(fileReader);
					String line;
					while ((line = br.readLine()) != null) {
						Log.i(tag, fileName + ": " + line);
					}
					br.close();
					fileReader.close();
				} catch (FileNotFoundException e) {
					e.printStackTrace();
				} catch (IOException e) {
					e.printStackTrace();
				}
				File file = new File(dir.getAbsolutePath() + "/" + fileName);
				file.delete();
			}
		}
	}
}
