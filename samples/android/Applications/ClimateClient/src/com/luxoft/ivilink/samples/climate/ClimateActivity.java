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


package com.luxoft.ivilink.samples.climate;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.HashMap;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.res.Configuration;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.os.Handler;

import com.luxoft.ivilink.samples.climate.helpers.ButtonStateSetter;
import com.luxoft.ivilink.samples.climate.helpers.JsonStrings;
import com.luxoft.ivilink.samples.climate.helpers.Limits;
import com.luxoft.ivilink.samples.climate.helpers.TemperatureGuiSet;
import com.luxoft.ivilink.samples.splash.Splash;
import com.luxoft.ivilink.samples.splash.Splash.Length;
import com.luxoft.ivilink.sdk.android.lib.utils.ForApp;

public class ClimateActivity extends Activity implements OnClickListener {

	ProgressDialog mLoading;

	BroadcastReceiver killer;

	Map<String, ButtonStateSetter> buttonsMap = new HashMap<String, ButtonStateSetter>();

	final Limits tempLimits = new Limits(16, 27);
	final Limits fanLimits = new Limits(0, 7);

	TemperatureGuiSet tempDSet, tempPSet;

	Bridge bridge;

	final static String tag = "ClimateClientSample";
	String launchInfo;

	@Override
	public void onCreate(Bundle icicle) {
		super.onCreate(icicle);
		killer = ForApp.registerReceiverFromKillerApp(this);
		ForApp.setWindowFlagsToKeepDeviceOn(this);
		Log.i(tag, "is created");
		bridge = Bridge.getInstance(this, new Handler());
		uiInitialization();
	}

	@Override
	public void onResume() {
		super.onResume();
		bridge.startNativeApp();
		Splash.displaySplashScreen(this, new Runnable() {
			public void run() {
				Log.i(tag, "empty stub");
			}
		}, Length.SHORT);
	}

	@Override
	public void onPostResume() {
		super.onPostResume();
		getTempBarParameters();
		bridge.reloadState();
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
		Log.v(tag, "killing process on back press");
		android.os.Process.killProcess(android.os.Process.myPid());
	}

	void uiInitialization() {
		if (getResources().getConfiguration().orientation == Configuration.ORIENTATION_LANDSCAPE) {
			setContentView(R.layout.ac_land);
		} else if (getResources().getConfiguration().orientation == Configuration.ORIENTATION_PORTRAIT) {
			setContentView(R.layout.ac_port);
		}
		initButtons();
		initSliders();
	}

	void getTempBarParameters() {
		Log.v(tag, "setting pre-draw listeners to temp bars");
		tempDSet.setMeasuresChangedListener();
		tempPSet.setMeasuresChangedListener();
	}

	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		Log.v(tag, "configuration changed");
		uiInitialization();
		getTempBarParameters();
		// TODO handle situation when
		bridge.reloadState();

		super.onConfigurationChanged(newConfig);
	}

	private void initButtons() {
		List<ButtonStateSetter> buttonsList = new ArrayList<ButtonStateSetter>();
		buttonsList.clear();
		buttonsList.add(new ButtonStateSetter((Button) findViewById(R.id.off),
				R.drawable.off_true_selector, R.drawable.off_false_selector,
				JsonStrings.off));
		buttonsList.add(new ButtonStateSetter((Button) findViewById(R.id.auto),
				R.drawable.auto_true_selector, R.drawable.auto_false_selector,
				JsonStrings.auto));
		buttonsList.add(new ButtonStateSetter((Button) findViewById(R.id.ac),
				R.drawable.ac_true_selector, R.drawable.ac_false_selector,
				JsonStrings.ac));
		buttonsList.add(new ButtonStateSetter((Button) findViewById(R.id.dual),
				R.drawable.dual_true_selector, R.drawable.dual_false_selector,
				JsonStrings.dual));
		buttonsList.add(new ButtonStateSetter(
				(Button) findViewById(R.id.recirc),
				R.drawable.rec_true_selector, R.drawable.rec_false_selector,
				JsonStrings.recirc));
		buttonsList.add(new ButtonStateSetter(
				(Button) findViewById(R.id.front),
				R.drawable.front_true_selector,
				R.drawable.front_false_selector, JsonStrings.front));
		buttonsList.add(new ButtonStateSetter((Button) findViewById(R.id.rear),
				R.drawable.rear_true_selector, R.drawable.rear_false_selector,
				JsonStrings.rear));
		buttonsList.add(new ButtonStateSetter(
				(Button) findViewById(R.id.fanDown),
				R.drawable.fan_down_true_selector,
				R.drawable.fan_down_false_selector, JsonStrings.fans[0]));
		buttonsList.add(new ButtonStateSetter(
				(Button) findViewById(R.id.fanMiddle),
				R.drawable.fan_middle_true_selector,
				R.drawable.fan_middle_false_selector, JsonStrings.fans[1]));
		buttonsList.add(new ButtonStateSetter(
				(Button) findViewById(R.id.fanTop),
				R.drawable.fan_top_true_selector,
				R.drawable.fan_top_false_selector, JsonStrings.fans[2]));

		buttonsList.add(new ButtonStateSetter(
				(Button) findViewById(R.id.drvTempUp), -1, -1, null));
		buttonsList.add(new ButtonStateSetter(
				(Button) findViewById(R.id.drvTempDown), -1, -1, null));
		buttonsList.add(new ButtonStateSetter(
				(Button) findViewById(R.id.passTempUp), -1, -1, null));
		buttonsList.add(new ButtonStateSetter(
				(Button) findViewById(R.id.passTempDown), -1, -1, null));
		buttonsList.add(new ButtonStateSetter(
				(Button) findViewById(R.id.fanIncrease), -1, -1, null));
		buttonsList.add(new ButtonStateSetter(
				(Button) findViewById(R.id.fanDecrease), -1, -1, null));

		buttonsMap.clear();
		for (ButtonStateSetter bss : buttonsList) {
			bss.setOnClickListener(this);
			buttonsMap.put(bss.getTag(), bss);
		}
	}

	void initSliders() {
		tempPSet = new TemperatureGuiSet(
				(ImageView) findViewById(R.id.tempPThumb),
				(TextView) findViewById(R.id.tempPVal),
				(RelativeLayout) findViewById(R.id.tempPBar), tempLimits, this);
		tempDSet = new TemperatureGuiSet(
				(ImageView) findViewById(R.id.tempDThumb),
				(TextView) findViewById(R.id.tempDVal),
				(RelativeLayout) findViewById(R.id.tempDBar), tempLimits, this);
	}

	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.off:
			Log.v(tag, "Off btn pressed");
			bridge.offPressed();
			break;
		case R.id.auto:
			bridge.autoPressed();
			Log.v(tag, "Auto btn pressed");
			break;
		case R.id.ac:
			bridge.acPressed();
			Log.v(tag, "A/C btn pressed");
			break;
		case R.id.dual:
			bridge.dualPressed();
			Log.v(tag, "Dual btn pressed");
			break;
		case R.id.recirc:
			bridge.recircPressed();
			Log.v(tag, "Recirc btn pressed");
			break;
		case R.id.front:
			bridge.frontPressed();
			Log.v(tag, "Front btn pressed");
			break;
		case R.id.rear:
			bridge.rearPressed();
			Log.v(tag, "Rear btn pressed");
			break;
		case R.id.fanDown:
			bridge.fanDownPressed();
			Log.v(tag, "FanDown btn pressed");
			break;
		case R.id.fanMiddle:
			bridge.fanMiddlePressed();
			Log.v(tag, "FanMiddle btn pressed");
			break;
		case R.id.fanTop:
			bridge.fanTopPressed();
			Log.v(tag, "FanTop btn pressed");
			break;
		case R.id.fanIncrease:
			bridge.fanIncrease();
			Log.v(tag, "FanIncrease btn pressed");
			break;
		case R.id.fanDecrease:
			bridge.fanDecrease();
			Log.v(tag, "FanDecrease btn pressed");
			break;
		case R.id.drvTempUp:
			bridge.drvTempUp();
			Log.v(tag, "DriverTemp+ btn pressed");
			break;
		case R.id.drvTempDown:
			bridge.drvTempDown();
			Log.v(tag, "DriverTemp- btn pressed");
			break;
		case R.id.passTempUp:
			bridge.passTempUp();
			Log.v(tag, "PassengerTemp+ btn pressed");
			break;
		case R.id.passTempDown:
			bridge.passTempDown();
			Log.v(tag, "PassengerTemp- btn pressed");
			break;
		default:
			// this should never happen actually
			Log.e(tag, "Some input crab happened");
		}
	}

	void setFanSpeedIndicator(int i) {
		int resource = R.id.fan1;
		for (int j = fanLimits.getLow(); j < fanLimits.getHigh(); j++) {
			if (j < i) {
				((ImageView) findViewById(resource + j))
						.setImageResource(getResources().getConfiguration().orientation == Configuration.ORIENTATION_LANDSCAPE ? R.drawable.fan_point_active
								: R.drawable.port_fan_active);
			} else {
				((ImageView) findViewById(resource + j))
						.setImageResource(getResources().getConfiguration().orientation == Configuration.ORIENTATION_LANDSCAPE ? R.drawable.fan_point_inactive
								: R.drawable.port_fan_inactive);
			}
		}
	}

	void moveSlider(TemperatureGuiSet tgs, int temperature) {
		tgs.setTemperature(temperature);
	}

	// called from TemperatureGuiSets
	public void setTempOnGuiReady(TemperatureGuiSet tgs) {
		bridge.reloadState();
	}

	void autoState(boolean status) {
		buttonsMap.get(JsonStrings.auto).setState(status);
	}

	void recircState(boolean status) {

		buttonsMap.get(JsonStrings.recirc).setState(status);
	}

	void acState(boolean status) {
		buttonsMap.get(JsonStrings.ac).setState(status);
	}

	void frontState(boolean status) {
		buttonsMap.get(JsonStrings.front).setState(status);
	}

	void rearState(boolean status) {
		buttonsMap.get(JsonStrings.rear).setState(status);
	}

	void fanLowState(boolean status) {
		buttonsMap.get(JsonStrings.fans[0]).setState(status);
	}

	void fanMidState(boolean status) {
		buttonsMap.get(JsonStrings.fans[1]).setState(status);
	}

	void fanHighState(boolean status) {
		buttonsMap.get(JsonStrings.fans[2]).setState(status);
	}

	void passTemp(int temp) {
		moveSlider(tempPSet, temp);
	}

	void driverTemp(int temp) {
		moveSlider(tempDSet, temp);
	}

	void syncState(boolean status) {
		buttonsMap.get(JsonStrings.dual).setState(status);
	}

	void fanSpeed(int speed) {
		setFanSpeedIndicator(speed);
	}

	void fanOffState(boolean status) {
		buttonsMap.get(JsonStrings.off).setState(status);
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
