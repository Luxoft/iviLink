package com.luxoft.ivilink.sdk.killall;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;

import com.luxoft.ivilink.utils.IntentActions;

public class AntiIviLink extends Activity {

	Dialog alert;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		createAlertDialog();
	}

	void createAlertDialog() {
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		builder.setMessage("Shutdown iviLink's samples?").setCancelable(false)
				.setPositiveButton("Shutdown", new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int id) {
						AntiIviLink.this.sendBroadcast(new Intent(IntentActions.SHUT_DOWN_NOTIFICATION));
						alert.cancel();
						AntiIviLink.this.finish();
					}
				}).setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int id) {
						alert.cancel();
						AntiIviLink.this.finish();
					}
				});
		alert = builder.create();
		alert.show();
	}
}