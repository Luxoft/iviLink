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


package com.luxoft.ivilink.samples.climate.helpers;

import android.os.Handler;
import android.util.Log;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.luxoft.ivilink.samples.climate.ClimateActivity;

public class TemperatureGuiSet {
	final static String tag = "Climate.TemperatureGuiSet";
	ImageView tempThumb;
	TextView tempLabel;
	RelativeLayout tempBar;
	int tempBarHeight = -1;
	int tempBarWidth = -1;
	private Limits limits;
	ClimateActivity host;

	Handler poster = new Handler();

	private boolean isReady = false;

	public TemperatureGuiSet(ImageView tempThumb, TextView tempLabel, RelativeLayout tempBar, Limits tempLimits, ClimateActivity host) {
		this.tempThumb = tempThumb;
		this.tempLabel = tempLabel;
		this.tempBar = tempBar;
		this.limits = tempLimits;
		this.host = host;
	}

	public void setMeasuresChangedListener() {
		isReady = false;
		poster.postDelayed(new MeasurementsChecker(), 50);
	}

	void setReady() {
		isReady = true;
		host.setTempOnGuiReady(this);
	}

	public boolean isReady() {
		return isReady;
	}

	int delta() {
		return limits.delta();
	}

	public void setTemperature(int temperature) {
		try {
			if (!isReady) {
				Log.e(tag, "is not ready, will set temperature later");
				return;
			}
			Log.i(tag, "setting temperature to " + temperature);
			int yPos = (int) Math.floor((double) (temperature - limits.getLow()) / (double) (limits.delta()) * (double) (tempBarHeight - tempThumb.getHeight()));
			RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(tempThumb.getWidth(), tempThumb.getHeight());
			params.setMargins(0, tempBarHeight - yPos - tempThumb.getHeight(), 0, yPos);
			tempThumb.setLayoutParams(params);
			tempLabel.setText("" + temperature + "C");
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public void cacheTempBarMeasures() {
		tempBarHeight = tempBar.getHeight();
		tempBarWidth = tempBar.getWidth();
	}

	public void setLayoutParamsBar() {
		tempBarHeight = (tempBarHeight / delta()) * delta();
		tempBar.setLayoutParams(new LinearLayout.LayoutParams(tempBar.getWidth(), tempBarHeight));
	}

	class MeasurementsChecker implements Runnable {
		public void run() {
			if (tempBar.getHeight() != tempBarHeight || tempBar.getWidth() != tempBarWidth) {
				cacheTempBarMeasures();
				poster.postDelayed(this, 50);
			} else {
				Log.i(tag, "measurements have stabilized, setting");
				setLayoutParamsBar();
				setReady();
			}
		}
	}
}
