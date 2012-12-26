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
 
package com.luxoft.ivilink.samples.media.players.audio.local;

import java.io.File;

import android.media.MediaPlayer;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.MediaPlayer.OnErrorListener;
import android.util.Log;

import com.luxoft.ivilink.samples.media.players.AudioPlayer.InternalPlayer;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

public class FromFilePlayer implements InternalPlayer {
	OnCompletionListener mOnCompletionListener;
	OnErrorListener mOnErrorListener;
	MediaPlayer mMediaPlayer;

	private boolean isActive = false;

	public void init(OnCompletionListener onCompletionListener, OnErrorListener onErrorListener) {
		mOnCompletionListener = onCompletionListener;
		mOnErrorListener = onErrorListener;
		mMediaPlayer = new MediaPlayer();
		mMediaPlayer.setOnCompletionListener(onCompletionListener);
	}

	public void play(String path) {
		try {
			mMediaPlayer.reset();
		} catch (IllegalStateException e) {
		}
		try {
			mMediaPlayer.setDataSource(path);
			mMediaPlayer.prepare();
		} catch (Exception e) {
			e.printStackTrace();
			mOnErrorListener.onError(mMediaPlayer, 0, 0);
			return;
		}
		mMediaPlayer.start();
		isActive = true;
	}

	public void pause() {
		mMediaPlayer.pause();
	}

	public void resume() {
		mMediaPlayer.start();
	}

	public void stop() {
		isActive = false;
		mMediaPlayer.stop();
		mMediaPlayer.reset();
	}

	public boolean isActive() {
		return isActive;
	}

	public void cancel() {
		try {
			stop();
		} catch (IllegalStateException e) {
			e.printStackTrace();
		}
	}

	public boolean canPlay(String path) {
		Log.v(this.toString(), Logging.getCurrentMethodName(path));
		File file = new File(path);
		if (file.exists() && file.isFile()) {
			return true;
		}
		return false;
	}
}
