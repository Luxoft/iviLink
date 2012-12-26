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

import android.app.Notification;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.IBinder;

import com.luxoft.ivilink.samples.media.players.audio.service.AudioPlayerProtocol.FromPlayerToService;
import com.luxoft.ivilink.sdk.android.lib.utils.log.BigLog;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

public class AudioService extends Service {

	private VlcPlayer mPlayer;

	@Override
	public IBinder onBind(Intent intent) {
		return null;
	}

	private BroadcastReceiver mCommandsReceiver = new BroadcastReceiver() {

		@Override
		public void onReceive(Context context, Intent intent) {
			String message = intent.getStringExtra(AudioPlayerProtocol.MESSAGE);
			FromPlayerToService fpts = FromPlayerToService.fromString(message);
			if (fpts == null) {
				BigLog.i(this.toString(), "Could not parse command from string: " + message, '!');
				return;
			}
			switch (fpts) {
			case PAUSE:
				mPlayer.pause();
				break;
			case RESUME:
				mPlayer.resume();
				break;
			case STOP:
				BigLog.w(this.toString(), "Got command to STOP, terminating", '!');
				context.unregisterReceiver(this);
				android.os.Process.killProcess(android.os.Process.myPid());
				break;
			default:
				BigLog.w(this.toString(), "Unexpected command: " + fpts.toString(), '!');
				break;

			}
		}
	};

	@Override
	public int onStartCommand(final Intent intent, int flags, int startID) {
		BigLog.i(this.toString(), Logging.getCurrentMethodName(intent, flags, startID), '!');
		if (startID != 1) {
			return START_NOT_STICKY;
		}
		AppWatchdog.start(this);
		mPlayer = new VlcPlayer(this);
		registerReceiver(mCommandsReceiver, new IntentFilter(AudioPlayerProtocol.TO_SERVICE));
		@SuppressWarnings("deprecation")
		Notification note = new Notification(0, null, System.currentTimeMillis());
		note.flags |= Notification.FLAG_NO_CLEAR;
		startForeground(42, note);
		new Thread(new Runnable() {
			public void run() {
				// otherwise will conflict will callbacks receiver (it is run on
				// the UI thread) and won't work
				mPlayer.play(intent.getStringExtra(AudioPlayerProtocol.EXTRA_INFO));
			}
		}).start();
		return START_NOT_STICKY;
	}

}
