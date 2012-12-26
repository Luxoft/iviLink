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

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.CyclicBarrier;

import android.app.Activity;
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.MediaPlayer.OnErrorListener;
import android.util.Log;

import com.luxoft.ivilink.samples.media.interfaces.ButtonProcessor;
import com.luxoft.ivilink.samples.media.interfaces.PlayerEventProcessor;
import com.luxoft.ivilink.samples.media.interfaces.Scrollable;
import com.luxoft.ivilink.samples.media.players.audio.local.FromFilePlayer;
import com.luxoft.ivilink.samples.media.players.audio.service.FromStreamPlayer;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

public class AudioPlayer extends AbstractPlayer {
	CyclicBarrier taskBarrier = new CyclicBarrier(2);

	public interface InternalPlayer {
		public void init(MediaPlayer.OnCompletionListener onCompletionListener,
				MediaPlayer.OnErrorListener onErrorListener);

		public void play(String path);

		public void cancel();

		public void pause();

		public void resume();

		public void stop();

		public boolean isActive();

		public boolean canPlay(String path);
	}

	List<InternalPlayer> mPlayersList = new ArrayList<InternalPlayer>();

	public AudioPlayer(Activity activity, ButtonProcessor buttonProcessor,
			PlayerEventProcessor eventProcessor, Scrollable scrollable) {
		super(activity, buttonProcessor, eventProcessor, scrollable);
		highlight = 'A';
		mPlayersList.add(new FromFilePlayer());
		mPlayersList.add(new FromStreamPlayer(rootActivity, buttonProcessor));
		for (InternalPlayer player : mPlayersList) {
			player.init((OnCompletionListener) this, (OnErrorListener) this);
		}
	}

	public void play(String path) {
		Log.v(tag(), Logging.getCurrentMethodName(path));
		for (InternalPlayer player : mPlayersList) {
			if (player.canPlay(path)) {
				player.play(path);
				return;
			}
		}
		onError(null, 0, 0);
	}

	public void resume() {
		Log.v(tag(), Logging.getCurrentMethodName());
		for (InternalPlayer player : mPlayersList) {
			if (player.isActive()) {
				player.resume();
			}
		}
	}

	public void pause() {
		Log.v(tag(), Logging.getCurrentMethodName());
		for (InternalPlayer player : mPlayersList) {
			if (player.isActive()) {
				player.pause();
			}
		}
	}

	public void stop() {
		Log.v(tag(), Logging.getCurrentMethodName());
		for (InternalPlayer player : mPlayersList) {
			if (player.isActive()) {
				player.stop();
			}
		}
	}

	@Override
	protected String tag() {
		return this.getClass().getSimpleName();
	}

	@Override
	public void cancelTaskIfRunning() {
		for (InternalPlayer player : mPlayersList) {
			if (player.isActive()) {
				player.cancel();
			}
		}
	}
}
