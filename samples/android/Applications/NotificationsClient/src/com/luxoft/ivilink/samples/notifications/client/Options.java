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
package com.luxoft.ivilink.samples.notifications.client;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.os.Bundle;

public class Options extends Activity {

	Dialog alert;

	@Override
	public void onCreate(Bundle icicle) {
		super.onCreate(icicle);
		createAlertDialogTwoButtons();
	}

	@Override
	public void onBackPressed() {

	}

	@Override
	public void onPause() {
		super.onPause();
		if (alert != null && alert.isShowing()) {
			alert.dismiss();
		}
		finish();
	}

	void createAlertDialogTwoButtons() {
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		builder.setMessage("Shutdown the Notification apps?")
				.setCancelable(false)
				.setPositiveButton("Shutdown",
						new DialogInterface.OnClickListener() {
							public void onClick(DialogInterface dialog, int id) {
								alert.cancel();
								android.os.Process
										.killProcess(android.os.Process.myPid());
							}
						})
				.setNegativeButton("Cancel",
						new DialogInterface.OnClickListener() {
							public void onClick(DialogInterface dialog, int id) {
								alert.cancel();
								Options.this.finish();
							}
						});
		alert = builder.create();
		alert.show();
	}
}
