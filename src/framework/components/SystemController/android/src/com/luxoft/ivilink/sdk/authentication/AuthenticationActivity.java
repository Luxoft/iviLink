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

import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.content.Intent;
import android.content.res.Configuration;
import android.os.Bundle;
import android.os.Handler;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import com.luxoft.ivilink.sdk.R;
import com.luxoft.ivilink.sdk.android.lib.utils.ForApp;
import com.luxoft.ivilink.sdk.helpers.Common;
import com.luxoft.ivilink.sdk.helpers.StartupProgress;

public class AuthenticationActivity extends Activity {

	final static String tag = Common.TAG + "AuthenticationActivity";

	static {
		System.loadLibrary("authenticationApp");
	}

	EditText eText;
	Button connect, cancel;
	Dialog pinDialog;

	Handler poster = new Handler();

	ButtonListener bl = new ButtonListener();

	native void start(String pathToServiceRepo, String launchInfo,
			String appPrivateDir, String trlistPath);

	native void cancelClicked();

	native void setPin(String pin);

	native void connectClicked();

	public void showPinDialog() {
		poster.post(new Runnable() {
			public void run() {
				pinDialog = createEnterPinDialog();
				pinDialog.show();
			}
		});
	}

	public void hideDialog() {
		poster.post(new Runnable() {
			public void run() {
				if (pinDialog != null && pinDialog.isShowing())
					pinDialog.dismiss();
			}
		});
	}

	public void resetPinDialog() {
		poster.post(new Runnable() {
			public void run() {
				eText.setText("");
				Toast.makeText(AuthenticationActivity.this,
						"PINs are not equal! Try again.", Toast.LENGTH_LONG)
						.show();
				connect.setEnabled(true);
			}
		});
	}

	public void lockText() {
		Log.v(tag, "locking EditText");
		poster.post(new Runnable() {
			public void run() {
				// locking text for editing
				eText.setFocusable(false);
				eText.setKeyListener(null);
				eText.setFocusableInTouchMode(false);
				// hiding keyboard
				connect.setEnabled(false);
				//
				InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
				imm.hideSoftInputFromWindow(eText.getWindowToken(), 0);
				Toast.makeText(AuthenticationActivity.this,
						"Waiting for PIN from other side!", Toast.LENGTH_LONG)
						.show();
			}
		});
	}

	@Override
	public void onCreate(Bundle icicle) {
		super.onCreate(icicle);
		sendBroadcast(new Intent(Common.ifProgress).putExtra(Common.message,
				StartupProgress.AUTHENTICATION.getMessage()).putExtra(
				Common.progressValue,
				StartupProgress.AUTHENTICATION.getProgress()));
		ForApp.setWindowFlagsToKeepDeviceOn(this);

		new Thread(new Runnable() {
			public void run() {
				Log.i(tag, "auth thread has started");
				start(ForApp.getServicePath(),
						ForApp.getLaunchInfo(AuthenticationActivity.this),
						ForApp.getInternalPath(AuthenticationActivity.this),
						ForApp.getInternalPath(AuthenticationActivity.this));
				Log.i(tag, "auth thread has finished");
			}
		}).start();
	}

	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		Log.v(tag, "overriding onConfigurationChanged");
		super.onConfigurationChanged(newConfig);
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
		android.os.Process.killProcess(android.os.Process.myPid());
	}

	public Dialog createEnterPinDialog() {
		if (pinDialog != null)
			return pinDialog;
		Dialog dialog = new Dialog(this);
		dialog.setContentView(R.layout.enterpin);
		dialog.setTitle("Please enter PIN");
		dialog.getWindow().getAttributes().gravity = Gravity.CENTER;
		eText = (EditText) dialog.findViewById(R.id.pinBox);
		connect = (Button) dialog.findViewById(R.id.verify);
		connect.setOnClickListener(bl);
		connect.setEnabled(false); // is enabled only when pin is entered
		cancel = (Button) dialog.findViewById(R.id.cancel);
		cancel.setOnClickListener(bl);
		eText.addTextChangedListener(new TextLengthWatcher());
		return dialog;
	}

	class TextLengthWatcher implements TextWatcher {
		String textBackup;

		public void afterTextChanged(Editable arg0) {
			setPin(arg0.toString());
			Log.v(tag, "Pin text is: " + arg0.toString());
			connect.setEnabled(eText.getText().length() == 4);
		}

		public void beforeTextChanged(CharSequence arg0, int arg1, int arg2,
				int arg3) {
			// / empty stub
		}

		public void onTextChanged(CharSequence arg0, int arg1, int arg2,
				int arg3) {
			// empty stub
		}
	}

	class ButtonListener implements OnClickListener {
		public void onClick(View v) {
			switch (v.getId()) {
			case R.id.cancel:
				Log.v(tag, "Cancel clicked!");
				// may take some time to process
				new Thread(new Runnable() {
					public void run() {
						cancelClicked();
						Log.i(tag, "CancelClicked thread has finished");
					}
				}).start();
				break;
			case R.id.verify:
				Log.v(tag, "OK clicked!");
				// may take some time to process
				new Thread(new Runnable() {
					public void run() {
						connectClicked();
						Log.i(tag, "ConnectClicked thread has finished");
					}
				}).start();
				break;
			}
		}
	}
}
