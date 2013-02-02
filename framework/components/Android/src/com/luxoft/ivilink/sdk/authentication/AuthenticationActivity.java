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


package com.luxoft.ivilink.sdk.authentication;

import android.app.Activity;
import android.app.Dialog;
import android.content.Intent;
import android.content.res.Configuration;
import android.os.Bundle;
import android.text.Editable;
import android.text.InputFilter;
import android.text.InputType;
import android.text.SpannableStringBuilder;
import android.text.Spanned;
import android.text.TextWatcher;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.view.View.OnClickListener;
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

	private String mEnteredDigits;

	private EditText mEditText;
	private Button mOkButton, mCancelButton;
	private Dialog mPinDialog;

	private ButtonListener mButtonListener = new ButtonListener();

	private native void start(String pathToServiceRepo, String launchInfo,
			String appPrivateDir, String trlistPath);

	private native void cancelClicked();

	private native void sendPin(int firstDigit, int secondDigit,
			int thirdDigit, int fourthDigit);

	private void showNumPad() {
		runOnUiThread(new Runnable() {
			public void run() {
				mPinDialog = createEnterPinDialog();
				mPinDialog.show();
			}
		});
	}

	private void hideNumPad() {
		runOnUiThread(new Runnable() {
			public void run() {
				if (mPinDialog != null && mPinDialog.isShowing()) {
					mPinDialog.dismiss();
				}
				Toast toast = Toast.makeText(AuthenticationActivity.this,
						"Authentication successful!", Toast.LENGTH_LONG);
				toast.setGravity(Gravity.CENTER, 0, 0);
				toast.show();
			}
		});
	}

	private void resetNumPad() {
		runOnUiThread(new Runnable() {
			public void run() {
				mEditText.setText("");
				mEditText.setFocusable(true);
			}
		});
	}

	private void showRetryPopup() {
		runOnUiThread(new Runnable() {
			public void run() {
				Toast toast = Toast.makeText(AuthenticationActivity.this,
						"PINs are not equal! Try again.", Toast.LENGTH_SHORT);
				toast.setGravity(Gravity.CENTER, 0, 0);
				toast.show();
			}
		});
	}

	private void lockInput() {
		runOnUiThread(new Runnable() {
			public void run() {
				mEditText.setFocusable(false);
				mEditText.setEnabled(false);
				mEditText.setInputType(InputType.TYPE_NULL);
				mOkButton.setEnabled(false);
			}
		});
	}

	private void unlockInput() {
		runOnUiThread(new Runnable() {
			public void run() {
				mEditText.setEnabled(true);
				mEditText.setFocusableInTouchMode(true);
				mEditText.setInputType(InputType.TYPE_CLASS_PHONE);
				mEditText.setText("");
			}
		});
	}

	@Override
	public void onCreate(Bundle icicle) {
		super.onCreate(icicle);
        Log.d("profiler.ComponentIsStarted", "AuthenticationApplication");
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

	private Dialog createEnterPinDialog() {
		if (mPinDialog != null)
			return mPinDialog;
		Dialog dialog = new Dialog(this);
		dialog.setContentView(R.layout.enterpin);
		dialog.setTitle("Please enter PIN");
		dialog.getWindow().getAttributes().gravity = Gravity.CENTER;
		dialog.setCanceledOnTouchOutside(false);
		mEditText = (EditText) dialog.findViewById(R.id.pinBox);
		mEditText.setFilters(new InputFilter[] { mInputFilter,
				new InputFilter.LengthFilter(4) });
		mOkButton = (Button) dialog.findViewById(R.id.verify);
		mOkButton.setOnClickListener(mButtonListener);
		mOkButton.setEnabled(false); // is enabled only when pin is entered
		mCancelButton = (Button) dialog.findViewById(R.id.cancel);
		mCancelButton.setOnClickListener(mButtonListener);
		mEditText.addTextChangedListener(new TextLengthWatcher());
		return dialog;
	}

	private class TextLengthWatcher implements TextWatcher {
		public void afterTextChanged(Editable arg0) {
			mEnteredDigits = arg0.toString();
			Log.v(tag, "Pin text is: " + mEnteredDigits);
			mOkButton.setEnabled(mEnteredDigits.length() == 4);
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

	private class ButtonListener implements OnClickListener {
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
						try {
							sendPin(Integer.parseInt(String
									.valueOf(mEnteredDigits.charAt(0))),
									Integer.parseInt(String
											.valueOf(mEnteredDigits.charAt(1))),
									Integer.parseInt(String
											.valueOf(mEnteredDigits.charAt(2))),
									Integer.parseInt(String
											.valueOf(mEnteredDigits.charAt(3))));
						} catch (NumberFormatException ex) {
							Log.d(tag, "Wrong PIN input");
						}

						Log.i(tag, "ConnectClicked thread has finished");
					}
				}).start();
				break;
			}
		}
	}

	InputFilter mInputFilter = new InputFilter() {
		public CharSequence filter(CharSequence source, int start, int end,
				Spanned dest, int dstart, int dend) {
			if (source instanceof SpannableStringBuilder) {
				SpannableStringBuilder sourceAsSpannableBuilder = (SpannableStringBuilder) source;
				for (int i = end - 1; i >= start; i--) {
					char currentChar = source.charAt(i);
					if (!Character.isDigit(currentChar)) {
						sourceAsSpannableBuilder.delete(i, i + 1);
					}
				}
				return source;
			} else {
				StringBuilder filteredStringBuilder = new StringBuilder();
				for (int i = 0; i < end; i++) {
					char currentChar = source.charAt(i);
					if (Character.isDigit(currentChar)) {
						filteredStringBuilder.append(currentChar);
					}
				}
				return filteredStringBuilder.toString();
			}
		}
	};
}
