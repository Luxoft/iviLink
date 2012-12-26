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
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.MediaPlayer.OnErrorListener;
import android.os.AsyncTask;
import android.util.Log;

import com.luxoft.ivilink.samples.media.interfaces.ButtonProcessor;
import com.luxoft.ivilink.samples.media.interfaces.PlayerEventProcessor;
import com.luxoft.ivilink.samples.media.interfaces.Scrollable;
import com.luxoft.ivilink.sdk.android.lib.utils.log.BigLog;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

public abstract class AbstractPlayer implements OnErrorListener,
		OnCompletionListener {

	ButtonProcessor buttonProcessor;
	PlayerEventProcessor eventProcessor;
	Scrollable scrollable;
	Activity rootActivity;
	protected char highlight;
	protected AsyncTask<Void, Integer, Boolean> currentTask;

	AbstractPlayer(Activity activity, ButtonProcessor bProcessor,
			PlayerEventProcessor eProcessor, Scrollable scroller) {
		rootActivity = activity;
		buttonProcessor = bProcessor;
		eventProcessor = eProcessor;
		scrollable = scroller;
	}

	public void onCompletion(MediaPlayer mp) {
		BigLog.i(tag(), Logging.getCurrentMethodName(mp), highlight);
		eventProcessor.onCompletion();
	}

	public boolean onError(MediaPlayer mp, int what, int extra) {
		BigLog.e(tag(), Logging.getCurrentMethodName(mp, what, extra),
				highlight);
		eventProcessor.onError();
		return true;
	}

	public void cancelTaskIfRunning() {
		Log.v(tag(), Logging.getCurrentMethodName());
		if (currentTask != null) {
			BigLog.i(tag(), "Was probably running a task, cancelling",
					highlight);
			currentTask.cancel(true);
		}
	}

	public abstract void play(String path);

	public abstract void resume();

	public abstract void pause();

	public abstract void stop();

	protected abstract String tag();
}
