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

import org.videolan.vlc.EventManager;
import org.videolan.vlc.LibVLC;
import org.videolan.vlc.LibVlcException;

import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import com.luxoft.ivilink.samples.media.players.audio.service.AudioPlayerProtocol.FromServiceToPlayer;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

public class VlcPlayer {

	LibVLC mLibVlc;

	CyclicBarrier mPlayBarrier = new CyclicBarrier(2);

	Context mSender;

	Handler vlcEventHandler = new Handler() {
		@Override
		public void handleMessage(Message msg) {
			Log.v(this.toString(), Logging.getCurrentMethodName(msg));
			int event = msg.getData().getInt("event");
			if (event == EventManager.MediaPlayerPlaying) {
				Log.i(this.toString(), "playing!");
				if (!mPlayBarrier.isBroken()) {
					try {
						mPlayBarrier.await(50, TimeUnit.MICROSECONDS);
					} catch (InterruptedException e) {
						e.printStackTrace();
					} catch (BrokenBarrierException e) {
						e.printStackTrace();
					} catch (TimeoutException e) {
						e.printStackTrace();
					}
				}
			} else if (event == EventManager.MediaPlayerEndReached) {
				Log.i(this.toString(), "media player end reached!");
				VlcPlayer.this.sendMessage(FromServiceToPlayer.COMPLETED);
			}
		}
	};

	public VlcPlayer(Context sender) {
		Log.v(this.toString(), Logging.getCurrentMethodName(sender));
		mSender = sender;
		EventManager.getInstance().addHandler(vlcEventHandler);
		try {
			mLibVlc = LibVLC.getInstance();
		} catch (LibVlcException e) {
			e.printStackTrace();
		}
	}

	public void play(final String url) {
		assert (url != null) : "was asked to play null!";
		Log.v(this.toString(), Logging.getCurrentMethodName(url));
		synchronized (mLibVlc) {
			Log.i(this.toString(), "IN play");
			int tryNum = 0;
			while (tryNum < AudioPlayerProtocol.PLAYBACK_RETRIES_LIMIT) {
				mPlayBarrier.reset();
				mLibVlc.setVolume(1);
				mLibVlc.readMedia(url, true);
				mLibVlc.play();
				try {
					mPlayBarrier.await(AudioPlayerProtocol.PLAYBACK_START_WAIT_TIMEOUT_MILLIS,
							TimeUnit.MILLISECONDS);
					sendMessage(FromServiceToPlayer.PLAYING);
					return;
				} catch (InterruptedException e) {
					e.printStackTrace();
				} catch (BrokenBarrierException e) {
					e.printStackTrace();
				} catch (TimeoutException e) {
					e.printStackTrace();
				}
				tryNum++;
			}
			sendMessage(FromServiceToPlayer.ERROR);
			Log.i(this.toString(), "OUT play");
		}
	}

	public void pause() {
		Log.v(this.toString(), Logging.getCurrentMethodName());
		synchronized (mLibVlc) {
			Log.i(this.toString(), "IN pause");
			mLibVlc.pause();
			Log.i(this.toString(), "OUT pause");
		}
	}

	public void resume() {
		Log.v(this.toString(), Logging.getCurrentMethodName());
		synchronized (mLibVlc) {
			Log.i(this.toString(), "IN resume");
			mLibVlc.play();
			Log.i(this.toString(), "OUT resume");
		}
	}

	synchronized void sendMessage(FromServiceToPlayer what) {
		mSender.sendBroadcast(new Intent(AudioPlayerProtocol.TO_PLAYER).putExtra(
				AudioPlayerProtocol.MESSAGE, what.toString()));
	}
}
