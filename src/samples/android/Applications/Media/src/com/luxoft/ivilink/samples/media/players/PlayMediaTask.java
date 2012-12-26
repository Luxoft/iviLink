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
 
package com.luxoft.ivilink.samples.media.players;

import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnErrorListener;
import android.os.AsyncTask;
import android.util.Log;
import android.widget.Toast;

import com.luxoft.ivilink.samples.media.interfaces.ButtonProcessor;
import com.luxoft.ivilink.samples.media.interfaces.Scrollable;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

public abstract class PlayMediaTask extends AsyncTask<Void, Integer, Boolean> implements
		OnErrorListener {
	final int PARTIES = 2;
	final int RETRIES = 10;
	final int RETRIES_SLEEP_MILLIS = 1500;
	final int ERROR_SLEEP_MILLIS = 1000;

	protected boolean cancelledByUser = false;

	protected ProgressDialog progress;
	protected Context context;
	protected Scrollable scrollable;
	protected ButtonProcessor buttonProcessor;
	protected String idTag;
	protected CyclicBarrier errorBarrier;
	protected String mediaPath;

	public PlayMediaTask(Context context, ButtonProcessor processor, String mediaPath,
			Scrollable scrollable) {
		this.mediaPath = mediaPath;
		this.context = context;
		this.buttonProcessor = processor;
		this.scrollable = scrollable;
		errorBarrier = new CyclicBarrier(PARTIES);
		idTag = this.toString();
	}

	@Override
	public void onPreExecute() {
		Log.w(idTag, Logging.getCurrentMethodName());
		registerAsErrorListener();
		prepareBeforeRunning();
		progress = new ProgressDialog(context);
		progress.setTitle("");
		progress.setMessage("Preparing media...");
		progress.setCanceledOnTouchOutside(false);
		progress.setCancelable(true);
		progress.setOnCancelListener(new DialogInterface.OnCancelListener() {
			public void onCancel(DialogInterface dialog) {
				cancelledByUser = true;
				PlayMediaTask.this.cancel(true);
			}
		});
		progress.show();
	}

	@Override
	protected Boolean doInBackground(Void... params) {
		Log.w(idTag, Logging.getCurrentMethodName());
		for (int i = 0; i < RETRIES; i++) {
			errorBarrier.reset();
			if (isCancelled()) {
				Log.w(idTag, "was cancelled!");
				return false;
			}
			if (attemptPlayback(i)) {
				if (isCancelled()) {
					Log.w(idTag, "was cancelled!");
					return false;
				}
				try {
					errorBarrier.await(ERROR_SLEEP_MILLIS, TimeUnit.MILLISECONDS);
				} catch (InterruptedException e1) {
					e1.printStackTrace();
					// the interruption may be the cause
					if (isCancelled()) {
						Log.w(idTag, "was cancelled!");
						return false;
					}
				} catch (BrokenBarrierException e1) {
				} catch (TimeoutException e1) {
					// Successful!
					Log.w(idTag, "Error barrier has timed out! Success!");
					return true;
				}
			}
			if (isCancelled()) {
				Log.w(idTag, "was cancelled!");
				return false;
			}
			Log.w(idTag, "sleeping! i=" + i + ", sleep");
			try {
				Thread.sleep(RETRIES_SLEEP_MILLIS);
			} catch (InterruptedException e) {
				e.printStackTrace();
				// the interruption may be the cause
				if (isCancelled()) {
					Log.w(idTag, "was cancelled!");
					return false;
				}
			}
			Log.w(idTag, "sleeping! i=" + i + ", sleep done!");
		}
		Log.w(idTag, "Player failed completely!");
		return false;
	}

	@Override
	public void onCancelled() {
		Log.v(idTag, Logging.getCurrentMethodName());
		progress.dismiss();
		unregisterAsErrorListener();
		if (cancelledByUser) {
			Log.v(idTag, "cancelled by user");
			buttonProcessor.onStopClicked();
		}
		cleanUpCancelOrFail();
	}

	protected abstract void unregisterAsErrorListener();

	protected abstract void registerAsErrorListener();

	protected abstract void prepareBeforeRunning();

	protected abstract void cleanUpSuccess();

	protected abstract void cleanUpCancelOrFail();

	// true in case of preliminary success - will wait on barrier
	protected abstract boolean attemptPlayback(int tryNum);

	@Override
	public void onPostExecute(Boolean result) {
		Log.v(idTag, Logging.getCurrentMethodName(result));
		progress.dismiss();
		unregisterAsErrorListener();
		if (result.booleanValue()) {
			Log.w(idTag, "Media started successfully!");
			cleanUpSuccess();
			scrollable.scrollToSelectedPosition();
		} else if (!isCancelled()) {
			buttonProcessor.onNextClicked();
			Toast.makeText(context, "Sorry! This media cannot be played!", Toast.LENGTH_SHORT)
					.show();
			cleanUpCancelOrFail();
		} else if (cancelledByUser) {
			Log.v(idTag, "cancelled by user");
			buttonProcessor.onStopClicked();
			cleanUpCancelOrFail();
		}
	}

	public boolean onError(MediaPlayer mp, int what, int extra) {
		Log.w(idTag, Logging.getCurrentMethodName(mp, what, extra));
		try {
			errorBarrier.await(ERROR_SLEEP_MILLIS, TimeUnit.MILLISECONDS);
		} catch (InterruptedException e) {
			e.printStackTrace();
		} catch (BrokenBarrierException e) {
			e.printStackTrace();
		} catch (TimeoutException e) {
			e.printStackTrace();
		}
		return true;
	}

}
