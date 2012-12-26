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
 
package com.luxoft.ivilink.samples.navi.voice;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.os.Bundle;

public class NotificationCallback extends Activity {
	Dialog alert;

	@Override
	public void onCreate(Bundle icicle) {
		super.onCreate(icicle);
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		builder.setMessage("Shutdown Navi Voice app? (this will kill paired app as well)")
				.setCancelable(false)
				.setPositiveButton("Shutdown", new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int id) {
						alert.cancel();
						android.os.Process.killProcess(android.os.Process.myPid());
					}
				}).setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int id) {
						alert.cancel();
						NotificationCallback.this.finish();
					}
				});
		alert = builder.create();
		alert.show();
	}
}
