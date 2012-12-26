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
 
package com.luxoft.ivilink.samples.media.players.audio.service;

import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.MediaPlayer.OnErrorListener;
import android.os.AsyncTask;
import android.util.Log;

import com.luxoft.ivilink.samples.media.interfaces.ButtonProcessor;
import com.luxoft.ivilink.samples.media.players.AudioPlayer.InternalPlayer;
import com.luxoft.ivilink.samples.media.players.audio.service.AudioPlayerProtocol.FromPlayerToService;
import com.luxoft.ivilink.samples.media.players.audio.service.AudioPlayerProtocol.FromServiceToPlayer;
import com.luxoft.ivilink.sdk.android.lib.utils.log.BigLog;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

public class FromStreamPlayer implements InternalPlayer {

	private Activity mActivity;
	private ButtonProcessor mButtonProcessor;
	AsyncTask<Void, Integer, Boolean> mCurrentTask;
	private boolean isActive = false;
	private OnCompletionListener mOnCompletionListener;
	private OnErrorListener mOnErrorListener;
	private CyclicBarrier taskBarrier = new CyclicBarrier(2);

	private BroadcastReceiver mServiceReceiver = new BroadcastReceiver() {
		@Override
		public void onReceive(Context context, Intent intent) {
			Log.v(this.toString(), Logging.getCurrentMethodName(context, intent));
			FromServiceToPlayer fstp;
			fstp = FromServiceToPlayer.fromString(intent
					.getStringExtra(AudioPlayerProtocol.MESSAGE));
			if (fstp == null) {
				return;
			}
			switch (fstp) {
			case COMPLETED:
				stop();
				mOnCompletionListener.onCompletion(null);
				break;
			case ERROR:
				if (mCurrentTask != null && mCurrentTask.getStatus() == AsyncTask.Status.RUNNING) {
					mCurrentTask.cancel(true);
				} else {
					mOnErrorListener.onError(null, 0, 0);
				}
				break;
			case PLAYING:
				if (taskBarrier != null && !taskBarrier.isBroken()) {
					try {
						taskBarrier.await(50, TimeUnit.MICROSECONDS);
					} catch (InterruptedException e) {
						e.printStackTrace();
					} catch (BrokenBarrierException e) {
						e.printStackTrace();
					} catch (TimeoutException e) {
						e.printStackTrace();
					}
				}
				break;
			default:
				break;

			}
		}
	};

	public FromStreamPlayer(Activity activity, ButtonProcessor buttonProcessor) {
		mActivity = activity;
		mButtonProcessor = buttonProcessor;
	}

	public void init(OnCompletionListener onCompletionListener, OnErrorListener onErrorListener) {
		mOnErrorListener = onErrorListener;
		mOnCompletionListener = onCompletionListener;
		mActivity.registerReceiver(mServiceReceiver,
				new IntentFilter(AudioPlayerProtocol.TO_PLAYER));
	}

	public void play(final String path) {
		Log.i(this.toString(), Logging.getCurrentMethodName(path));
		isActive = true;
		mActivity.runOnUiThread(new Runnable() {
			public void run() {
				mCurrentTask = new PlayMusicTask(path).execute((Void[]) null);
			}
		});
	}

	public void pause() {
		sendMessage(FromPlayerToService.PAUSE);
	}

	public void resume() {
		sendMessage(FromPlayerToService.RESUME);
	}

	public void stop() {
		isActive = false;
		sendMessage(FromPlayerToService.STOP);
	}

	public boolean isActive() {
		return isActive;
	}

	private void sendMessage(FromPlayerToService what) {
		mActivity.sendBroadcast(new Intent(AudioPlayerProtocol.TO_SERVICE).putExtra(
				AudioPlayerProtocol.MESSAGE, what.toString()));
	}

	public void cancel() {
		if (mCurrentTask != null && mCurrentTask.getStatus() == AsyncTask.Status.RUNNING) {
			mCurrentTask.cancel(true);
		}
	}

	class PlayMusicTask extends AsyncTask<Void, Integer, Boolean> {
		ProgressDialog progress;
		String mMediaPath;
		boolean isCancelledByUser = false;

		public PlayMusicTask(String mediaPath) {
			BigLog.i(this.toString(), mediaPath, '!');
			mMediaPath = mediaPath;
		}

		@Override
		public void onPreExecute() {
			Log.i(this.toString(), Logging.getCurrentMethodName());
			progress = new ProgressDialog(mActivity);
			progress.setTitle("");
			progress.setMessage("Preparing media...");
			progress.setCanceledOnTouchOutside(false);
			progress.setCancelable(true);
			progress.setOnCancelListener(new DialogInterface.OnCancelListener() {
				public void onCancel(DialogInterface dialog) {
					isCancelledByUser = true;
					PlayMusicTask.this.cancel(true);
				}
			});
			progress.show();
		}

		@Override
		public Boolean doInBackground(Void... params) {
			Log.i(this.toString(), Logging.getCurrentMethodName((Object[]) params));
			taskBarrier.reset();
			Log.i(this.toString(),
					mActivity.startService(
							new Intent(mActivity, AudioService.class).putExtra(
									AudioPlayerProtocol.EXTRA_INFO, mMediaPath)).toString());
			try {
				taskBarrier.await(AudioPlayerProtocol.RESPONCE_FROM_SERVICE_WAIT_MILLIS,
						TimeUnit.MILLISECONDS);
				return true;
			} catch (InterruptedException e) {
				// cancelled
				e.printStackTrace();
				return false;
			} catch (BrokenBarrierException bbe) {
				bbe.printStackTrace();
				return false;
			} catch (TimeoutException e) {
				e.printStackTrace();
				return false;
			}
		}

		@Override
		public void onPostExecute(Boolean result) {
			Log.v(this.toString(), Logging.getCurrentMethodName(result));
			progress.dismiss();
			if (result == false) {
				mButtonProcessor.onNextClicked();
			}
		}

		@Override
		public void onCancelled() {
			Log.v(this.toString(), Logging.getCurrentMethodName());
			progress.dismiss();
			stop();
			if (isCancelledByUser) {
				mButtonProcessor.onStopClicked();
			}
		}

	}

	public boolean canPlay(String path) {
		Log.v(this.toString(), Logging.getCurrentMethodName(path));
		if (path.startsWith("rtsp://")) {
			return true;
		}
		return false;
	}

}
