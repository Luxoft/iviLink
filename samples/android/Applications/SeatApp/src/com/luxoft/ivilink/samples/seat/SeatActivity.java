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


package com.luxoft.ivilink.samples.seat;

import java.util.Arrays;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.res.Configuration;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.util.SparseIntArray;
import android.view.Display;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;

import com.luxoft.ivilink.samples.seat.helpers.ChairSelection;
import com.luxoft.ivilink.samples.seat.helpers.Limits;
import com.luxoft.ivilink.samples.seat.helpers.SeatPosition;
import com.luxoft.ivilink.samples.splash.Splash;
import com.luxoft.ivilink.samples.splash.Splash.Length;
import com.luxoft.ivilink.sdk.android.lib.utils.ForApp;

public class SeatActivity extends Activity implements OnClickListener {
	ProgressDialog mLoading;

	BroadcastReceiver killer;

	final static String tag = "iviLink.Samples.SeatApp";

	BridgeC bridge;

	Button heaterB;
	static Button driverB, passengerB;
	int heaterDriverValue, heaterPassengerValue;

	ImageView seatBottom;
	ImageView seatBack;

	BottomTouchListener touchBottom = new BottomTouchListener();
	SeatTouchListener touchSeat = new SeatTouchListener();

	SparseIntArray seatAngleDrawables = new SparseIntArray();

	Limits weightLimit = new Limits(0, 200);
	static double coefX = 20;
	static double coefY = 20;

	SeatPosition position = new SeatPosition(new Limits(0, 200), new Limits(0,
			200), new Limits(-40, 80));

	@Override
	public void onCreate(Bundle icicle) {
		super.onCreate(icicle);
		killer = ForApp.registerReceiverFromKillerApp(this);
		ForApp.setWindowFlagsToKeepDeviceOn(this);
		bridge = BridgeC.getInstance(this, new Handler());
		bridge.startApp();
		guiInitialization();

		Display display = getWindowManager().getDefaultDisplay();
		@SuppressWarnings("deprecation")
		// using deprecated methods for api<13 compatibility
		int res = display.getWidth() * display.getHeight();
		Log.i(tag, "onCreate(): total screen resolution is: " + res + "px");
		// this "magic" value is chosen because tested on 1280x720 device:
		// and we chose coef=20 there and it worked fine
		coefX = res / 46080d;
		Log.i(tag, "onCreate(): coefX =" + coefX);
		coefY = coefX; // for now
	}

	@Override
	public void onResume() {
		super.onResume();
		Splash.displaySplashScreen(this, new Runnable() {
			public void run() {
				Log.v(tag, "empty stub");
			}
		}, Length.SHORT);
	}

	@Override
	public void onPostResume() {
		super.onPostResume();
		bridge.initRequest();
	}

	@Override
	public void onPause() {
		super.onPause();
		bridge.saveState();
		android.os.Process.killProcess(android.os.Process.myPid());
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
		unregisterReceiver(killer);
	}

	@Override
	public void onBackPressed() {
		Log.i(tag, "dying on back press");
		android.os.Process.killProcess(android.os.Process.myPid());
	}

	void guiInitialization() {
		if (getResources().getConfiguration().orientation == Configuration.ORIENTATION_PORTRAIT) {
			setContentView(R.layout.seat_portrait);
		} else {
			this.setContentView(R.layout.seat_landscape);
		}
		initButtons();
		initSeatAngles();

		seatBottom = (ImageView) findViewById(R.id.seatBottomDnD);
		seatBack = (ImageView) findViewById(R.id.seatBackDnD);

		seatBottom.setOnTouchListener(touchBottom);
		seatBack.setOnTouchListener(touchSeat);

		seatBack.setBackgroundResource(seatAngleDrawables.get(position
				.getBackAngleDef()));
	}

	enum Direction {
		VERTICAL, HORIZONTAL, NOTDECIDED
	}

	public void initButtons() {
		heaterB = (Button) findViewById(R.id.heater);
		heaterB.setOnClickListener(this);

		driverB = (Button) findViewById(R.id.driver);
		driverB.setOnClickListener(this);

		passengerB = (Button) findViewById(R.id.passenger);
		passengerB.setOnClickListener(this);

		ChairSelection.setButtons(driverB, passengerB);
	}

	public void initSeatAngles() {
		for (int i = 0; i < 8; i++) {
			seatAngleDrawables.put(-5 - 5 * i, R.drawable.seat_back_m05 + i);
		}
		for (int i = 0; i < 17; i++) {
			seatAngleDrawables.put(5 * i, R.drawable.seat_back_00 + i);
		}
	}

	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.heater:
			if (ChairSelection.isDriverSelected()) {
				bridge.driverHeaterRequest();
			}
			if (ChairSelection.isPassengerSelected()) {
				bridge.passHeaterRequest();
			}
			return;
		case R.id.driver:
			bridge.driverSeatRequest();
			return;
		case R.id.passenger:
			bridge.passSeatRequest();
			return;
		}
	}

	/*
	 * transforms finger movements into state-changing requests (we can move
	 * seat's bottom horizontally and vertically)
	 */
	class BottomTouchListener implements OnTouchListener {
		private float[] touchOld = new float[2];
		float[] accum = new float[2];
		Direction direction = Direction.NOTDECIDED;

		private final int accumSizeY = (int) coefY;
		private final int accumSizeX = (int) coefX;

		public boolean onTouch(View v, MotionEvent event) {
			switch (event.getAction()) {
			case MotionEvent.ACTION_DOWN:
				touchOld[0] = event.getRawX();
				touchOld[1] = event.getRawY();
				direction = Direction.NOTDECIDED;
				accum[0] = .0f;
				accum[1] = .0f;
				return true;
			case MotionEvent.ACTION_MOVE:
				float dX = event.getRawX() - touchOld[0];
				float dY = event.getRawY() - touchOld[1];
				if ((dX == .0) && (dY == .0)) {
					return true;
				}
				touchOld[0] = event.getRawX();
				touchOld[1] = event.getRawY();
				accum[0] += dX;
				accum[1] += dY;
				if (direction == Direction.NOTDECIDED) {
					if (Math.abs(accum[0]) >= Math.abs(accum[1])
							&& Math.abs(accum[0]) >= accumSizeX) {
						direction = Direction.HORIZONTAL;
					} else if ((Math.abs(accum[1]) >= Math.abs(accum[0]))
							&& (Math.abs(accum[1]) >= accumSizeY)) {
						direction = Direction.VERTICAL;
					}
				}
				switch (direction) {
				case NOTDECIDED:
					break;
				case VERTICAL:
					while (Math.abs(accum[1]) >= accumSizeY) {
						if (Math.signum(accum[1]) > 0) {
							bridge.bottomDownRequest();
						} else {
							bridge.bottomUpRequest();
						}
						accum[1] = (accum[1] > 0) ? (float) (accum[1] - accumSizeY)
								: (float) (accum[1] + accumSizeY);
					}
					break;
				case HORIZONTAL:
					while (Math.abs(accum[0]) >= accumSizeX) {
						if (Math.signum(accum[0]) > 0) {
							bridge.bottomRightRequest();
						} else {
							bridge.bottomLeftRequest();
						}
						accum[0] = (accum[0] > 0) ? (float) (accum[0] - accumSizeX)
								: (float) (accum[0] + accumSizeX);
					}
					break;
				}
				return true;
			case MotionEvent.ACTION_UP:
				// TODO probably check values left in accum
				return true;
			}
			return false;
		}
	}

	/*
	 * transforms finger movements into state-changing requests (we can rotate
	 * seat's back)
	 */
	class SeatTouchListener implements OnTouchListener {
		private float tAngle;
		private float[] touchOld = new float[2];
		private float[] pivotPoint = new float[2];

		public boolean onTouch(View v, MotionEvent event) {
			switch (event.getAction()) {
			case MotionEvent.ACTION_DOWN:
				touchOld[0] = event.getRawX();
				touchOld[1] = event.getRawY();

				int[] locationOnScreen = new int[2];
				seatBack.getLocationOnScreen(locationOnScreen);
				pivotPoint[0] = locationOnScreen[0] + seatBack.getWidth() / 2;
				pivotPoint[1] = locationOnScreen[1] + seatBack.getHeight() + 10;
				tAngle = 0;
				return true;
			case MotionEvent.ACTION_MOVE:
				double[] v1 = { event.getRawX() - pivotPoint[0],
						event.getRawY() - pivotPoint[1] }; // new
															// vector
				double[] v2 = { touchOld[0] - pivotPoint[0],
						touchOld[1] - pivotPoint[1] };
				if (Arrays.equals(v1, v2)) {
					return true;
				}
				double cosAngle = (v1[0] * v2[0] + v1[1] * v2[1])
						/ (Math.sqrt(Math.abs(v1[0] * v1[0] + v1[1] * v1[1])) * Math
								.sqrt(Math.abs(v2[0] * v2[0] + v2[1] * v2[1])));
				if (v1[0] > v2[0]) {
					tAngle += (Math.acos(cosAngle) * 180. / Math.PI);
				} else {
					tAngle -= (Math.acos(cosAngle) * 180. / Math.PI);
				}
				touchOld[0] = event.getRawX();
				touchOld[1] = event.getRawY();
				while (Math.abs(Math.round(tAngle)) >= 5) {
					if (Math.signum(tAngle) > 0) {
						bridge.backRightRequest();
					} else {
						bridge.backLeftRequest();
					}
					tAngle = (tAngle > 0) ? tAngle - 5 : tAngle + 5;
				}
				return true;
			case MotionEvent.ACTION_UP:
				// TODO probably check values left in accum
				return true;
			}
			return false;
		}
	}

	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);

		if (getResources().getConfiguration().orientation == Configuration.ORIENTATION_PORTRAIT) {
			setContentView(R.layout.seat_portrait);
		} else {
			this.setContentView(R.layout.seat_landscape);
		}

		initButtons();

		seatBottom = (ImageView) findViewById(R.id.seatBottomDnD);
		seatBack = (ImageView) findViewById(R.id.seatBackDnD);
		seatBottom.setOnTouchListener(touchBottom);
		seatBack.setOnTouchListener(touchSeat);

		moveHorizontally();
		moveVertically();
		moveSeatBack();

		if (ChairSelection.isDriverSelected()) {
			ChairSelection
					.setDriverBtnActive(getResources().getConfiguration().orientation);
			onHeaterDriverState(heaterDriverValue);
		} else if (ChairSelection.isPassengerSelected()) {
			ChairSelection.setPassengerBtnActive(getResources()
					.getConfiguration().orientation);
			onHeaterPassState(heaterPassengerValue);
		}
	}

	/*
	 * we "move" seat by changing weights of surrounding elements
	 */
	@SuppressWarnings("deprecation")
	private void moveHorizontally() {
		((LinearLayout) findViewById(R.id.seatLeftOffset))
				.setLayoutParams(new LinearLayout.LayoutParams(0,
						LinearLayout.LayoutParams.FILL_PARENT, weightLimit
								.fullCheck(100 - position.getXOffset())));
		((LinearLayout) findViewById(R.id.seatRightOffset))
				.setLayoutParams(new LinearLayout.LayoutParams(0,
						LinearLayout.LayoutParams.FILL_PARENT, weightLimit
								.fullCheck(100 + position.getXOffset())));
	}

	@SuppressWarnings("deprecation")
	private void moveVertically() {
		((LinearLayout) findViewById(R.id.seatTopOffset))
				.setLayoutParams(new LinearLayout.LayoutParams(
						LinearLayout.LayoutParams.FILL_PARENT, 0, weightLimit
								.fullCheck(100 - position.getYOffset())));
		((LinearLayout) findViewById(R.id.seatBottomOffset))
				.setLayoutParams(new LinearLayout.LayoutParams(
						LinearLayout.LayoutParams.FILL_PARENT, 0, weightLimit
								.fullCheck(100 + position.getYOffset())));
	}

	/*
	 * we "rotate" seat by changing its drawable
	 */
	private void moveSeatBack() {
		seatBack.setBackgroundResource(seatAngleDrawables.get(position
				.getBackAngle()));
	}

	void onHeaterDriverState(int heaterVal) {
		if (ChairSelection.isDriverSelected()) {
			heaterB.setBackgroundResource(R.drawable.heater_selector_0
					+ heaterVal);
			heaterDriverValue = heaterVal;
		}
	}

	void onHeaterPassState(int heaterVal) {
		if (ChairSelection.isPassengerSelected()) {
			heaterB.setBackgroundResource(R.drawable.heater_selector_0
					+ heaterVal);
			heaterPassengerValue = heaterVal;
		}
	}

	void onSeatBottomX(int x) {
		position.setBottomX(x);
		moveHorizontally();
	}

	void onSeatBottomY(int y) {
		position.setBottomY(y);
		moveVertically();
	}

	void onSeatBackAngle(int angle) {
		position.setBackAngle(angle);
		moveSeatBack();
	}

	void onSwitchToDriver() {
		ChairSelection
				.setDriverBtnActive(getResources().getConfiguration().orientation);
	}

	void onSwitchToPassenger() {
		ChairSelection
				.setPassengerBtnActive(getResources().getConfiguration().orientation);
	}

	void onShowLoadingScreen() {
		if (mLoading != null && mLoading.isShowing()) {
			return;
		}
		mLoading = new ProgressDialog(this);
		mLoading.setMessage("Loading...");
		mLoading.setCancelable(false);
		mLoading.show();
	}

	void onHideLoadingScreen() {
		if (mLoading != null && mLoading.isShowing()) {
			mLoading.dismiss();
			mLoading = null;
		}
	}
}
