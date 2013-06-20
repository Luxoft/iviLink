package com.luxoft.ivilink.sdk.helpers;

import android.content.res.Resources;
import android.graphics.drawable.Drawable;
import android.os.Handler;
import android.widget.ImageView;

import com.luxoft.ivilink.sdk.R;

public class Spinner implements Runnable {
	private boolean stopFlag = false;
	private ImageView imageToUpdate;
	private Handler poster;
	private int updateTimeoutMillis;

	private int internalCounter = 0;
	private final int internalCounterLimit = 12;
	// for memory usage optimization - otherwise gc will be called very often
	Drawable[] drawablesCache = new Drawable[internalCounterLimit];

	public Spinner(Handler poster, ImageView imageToUpdate, int updateTimeoutMillis,
			Resources resources) {
		this.imageToUpdate = imageToUpdate;
		this.poster = poster;
		this.updateTimeoutMillis = updateTimeoutMillis;
		for (int i = 0; i < internalCounterLimit; i++) {
			drawablesCache[i] = resources.getDrawable(R.drawable.loading_00 + i);
		}
		this.poster.postDelayed((Runnable) this, this.updateTimeoutMillis);
	}

	@SuppressWarnings("deprecation")
	public void run() {
		if (stopFlag) {
			return;
		}
		internalCounter++;
		if (internalCounter == internalCounterLimit) {
			internalCounter = 0;
		}
		try {
			imageToUpdate.setBackgroundDrawable(drawablesCache[internalCounter]);
			imageToUpdate.invalidate();
		} catch (Exception e) {
			e.printStackTrace();
		}
		if (!stopFlag) {
			poster.postDelayed(this, updateTimeoutMillis);
		}
	}

	public void stop() {
		stopFlag = true;
	}
}
