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
package com.luxoft.ivilink.samples.splash;

import android.app.Activity;
import android.app.Dialog;
import android.graphics.Bitmap;
import android.graphics.Matrix;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.os.Handler;
import android.util.DisplayMetrics;
import android.util.Log;
import android.widget.ImageView;
import android.widget.LinearLayout;

public class Splash {
	private final static String tag = "SplashLib";
	final Dialog dialog;
	final Runnable toDoAfter;
	final Activity contextActivity;
	int currentTimeout = -1;
	final static int SPLASH_DEFAULT_TIMEOUT = 2500;
	private static Splash currentInstance;

	public enum Length {
		LONG, SHORT;
		private final int LENGTH_LONG = 5000;
		private final int LENGTH_SHORT = 1500;

		public int getTimeMs() {
			if (this == LONG) {
				return LENGTH_LONG;
			} else if (this == SHORT) {
				return LENGTH_SHORT;
			}
			return SPLASH_DEFAULT_TIMEOUT;
		}
	}

	public static void displaySplashScreen(Activity activity, Runnable runnable) {
		Log.v(tag, "displaySplashScreen()");
		//currentInstance = new Splash(activity, runnable, SPLASH_DEFAULT_TIMEOUT);
		//currentInstance.display();
		runnable.run();// - when splash must be disabled
	}

	public static void displaySplashScreen(Activity activity,
			Runnable runnable, Length length) {
		Log.v(tag, "displaySplashScreen()");
		//currentInstance = new Splash(activity, runnable, length.getTimeMs());
		//currentInstance.display();
		runnable.run();// - when splash must be disabled
	}

	private Splash(Activity activity, Runnable runnable, int timeout) {
		Log.v(tag, "Splash()");
		toDoAfter = runnable;
		contextActivity = activity;
		currentTimeout = timeout;
		dialog = new Dialog(contextActivity,
				android.R.style.Theme_Black_NoTitleBar_Fullscreen);
	}

	private void display() {
		Log.v(tag, "display()");
		dialog.setContentView(R.layout.splash);
		dialog.setCancelable(false);
		DisplayMetrics displaymetrics = new DisplayMetrics();
		contextActivity.getWindowManager().getDefaultDisplay()
				.getMetrics(displaymetrics);
		int height = displaymetrics.heightPixels;
		int width = displaymetrics.widthPixels;
		dialog.getWindow().setLayout(width, height);
		ImageView splashImage = (ImageView) dialog
				.findViewById(R.id.splashImage);

		scaleImage(splashImage);
		dialog.show();
		contextActivity.runOnUiThread(new Runnable() {
			public void run() {
				new Handler().postDelayed(new Runnable() {
					public void run() {
						Log.v(tag, "after timeout");
						dialog.dismiss();
						toDoAfter.run();
					}
				}, currentTimeout);
			}
		});
	}

	private void scaleImage(ImageView imageView) {
		Drawable drawing = imageView.getBackground();
		Bitmap bitmap = ((BitmapDrawable) drawing).getBitmap();
		int width = bitmap.getWidth();
		int height = bitmap.getHeight();
		float xScale = ((float) contextActivity.getApplicationContext()
				.getResources().getDisplayMetrics().widthPixels)
				/ width;
		float yScale = ((float) contextActivity.getApplicationContext()
				.getResources().getDisplayMetrics().heightPixels)
				/ height;
		float scale = (xScale <= yScale) ? xScale : yScale;
		Log.i(tag, "xScale = " + xScale);
		Log.i(tag, "yScale = " + yScale);
		Log.i(tag, "scale = " + scale);
		Matrix matrix = new Matrix();
		matrix.postScale(scale, scale);
		Bitmap scaledBitmap = Bitmap.createBitmap(bitmap, 0, 0, width, height,
				matrix, true);
		width = scaledBitmap.getWidth(); // re-use
		height = scaledBitmap.getHeight(); // re-use
		BitmapDrawable result = new BitmapDrawable(scaledBitmap);
		Log.i(tag, "scaled width = " + width);
		Log.i(tag, "scaled height = " + height);

		imageView.setImageDrawable(result);

		LinearLayout.LayoutParams params = (LinearLayout.LayoutParams) imageView
				.getLayoutParams();
		params.width = width;
		params.height = height;
		imageView.setLayoutParams(params);
	}
}
