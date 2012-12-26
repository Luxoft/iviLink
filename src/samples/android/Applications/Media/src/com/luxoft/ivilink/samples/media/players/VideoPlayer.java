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

import android.app.Activity;
import android.content.Context;
import android.media.MediaPlayer;
import android.util.Log;
import android.widget.VideoView;

import com.luxoft.ivilink.samples.media.interfaces.ButtonProcessor;
import com.luxoft.ivilink.samples.media.interfaces.PlayerEventProcessor;
import com.luxoft.ivilink.samples.media.interfaces.Scrollable;
import com.luxoft.ivilink.samples.media.interfaces.VideoVisibilityController;
import com.luxoft.ivilink.sdk.android.lib.utils.log.BigLog;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

public class VideoPlayer extends AbstractPlayer {
	VideoView player;
	VideoVisibilityController videoVisibilityController;

	public VideoPlayer(Activity activity, ButtonProcessor buttonProcessor,
			PlayerEventProcessor eventProcessor, Scrollable scrollable, VideoView videoView,
			VideoVisibilityController vvc) {
		super(activity, buttonProcessor, eventProcessor, scrollable);
		highlight = 'V';
		this.player = videoView;
		player.setOnErrorListener(this);
		player.setOnCompletionListener(this);
		this.videoVisibilityController = vvc;
	}

	public void play(final String path) {
		Log.v(tag(), Logging.getCurrentMethodName(path));
		BigLog.i(tag(), "Was asked to play: " + path, highlight);
		rootActivity.runOnUiThread(new Runnable() {
			public void run() {
				currentTask = new PlayVideoTask(rootActivity, buttonProcessor, path, scrollable)
						.execute((Void[]) null);
			}
		});
	}

	public void resume() {
		Log.v(tag(), Logging.getCurrentMethodName());
		rootActivity.runOnUiThread(new Runnable() {
			public void run() {
				player.start();
			}
		});
	}

	public void pause() {
		Log.v(tag(), Logging.getCurrentMethodName());
		rootActivity.runOnUiThread(new Runnable() {
			public void run() {
				player.pause();
			}
		});
	}

	public void stop() {
		Log.v(tag(), Logging.getCurrentMethodName());
		rootActivity.runOnUiThread(new Runnable() {
			public void run() {
				player.stopPlayback();
				player.suspend();
				videoVisibilityController.listVisible();
			}
		});
	}

	@Override
	public boolean onError(MediaPlayer mp, int what, int extra) {
		videoVisibilityController.listVisible();
		return super.onError(mp, what, extra);
	}

	@Override
	public void onCompletion(MediaPlayer mp) {
		videoVisibilityController.listVisible();
		super.onCompletion(mp);
	}

	class PlayVideoTask extends PlayMediaTask {

		public PlayVideoTask(Context context, ButtonProcessor processor, String mediaPath,
				Scrollable scrollable) {
			super(context, processor, mediaPath, scrollable);
		}

		@Override
		protected void unregisterAsErrorListener() {
			player.setOnErrorListener(VideoPlayer.this);
			player.setOnCompletionListener(VideoPlayer.this);
		}

		@Override
		protected void registerAsErrorListener() {
			player.setOnErrorListener(this);
			player.setOnCompletionListener(null);
		}

		@Override
		protected boolean attemptPlayback(int tryNum) {
			publishProgress(tryNum);
			return true;
		}

		@Override
		public void onProgressUpdate(Integer... integers) {
			Log.v(idTag, Logging.getCurrentMethodName(integers[0]));
			player.setVideoPath(mediaPath);
			player.start();
		}

		@Override
		protected void prepareBeforeRunning() {
			Log.v(idTag, Logging.getCurrentMethodName());
		}

		@Override
		protected void cleanUpSuccess() {
			if (currentTask == this) {
				currentTask = null;
			}
			videoVisibilityController.videoVisible();
		}

		@Override
		protected void cleanUpCancelOrFail() {
			Log.v(idTag, Logging.getCurrentMethodName());
			if (currentTask == this) {
				Log.w(idTag, "currentTask is this, setting to null");
				currentTask = null;
				videoVisibilityController.listVisible();
			}
		}
	}

	@Override
	protected String tag() {
		return this.getClass().getSimpleName();
	}
}
