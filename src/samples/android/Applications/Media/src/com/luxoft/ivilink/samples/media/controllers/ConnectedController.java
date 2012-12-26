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
 
package com.luxoft.ivilink.samples.media.controllers;

import android.util.Log;

import com.luxoft.ivilink.samples.media.Bridge;
import com.luxoft.ivilink.samples.media.interfaces.JniPlayerWrapper;
import com.luxoft.ivilink.samples.media.interfaces.Scalable;
import com.luxoft.ivilink.samples.media.players.AbstractPlayer;
import com.luxoft.ivilink.samples.media.structs.MediaType;
import com.luxoft.ivilink.samples.media.structs.PlayingState;
import com.luxoft.ivilink.sdk.android.lib.utils.log.BigLog;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

/* implements the MediaGui interface to store state before passing commands to the stateless VideoActivity */
public class ConnectedController extends Controller implements JniPlayerWrapper {

	public ConnectedController(Scalable scalable) {
		super(scalable);
	}

	public void onCompletion() {
		Log.v(getTag(), Logging.getCurrentMethodName());
		sharedState.setPlayingState(PlayingState.STOPPED);
		/* will force playerlogic to check our player's state - isTrackEnded */
		Bridge.getInstance().onCompletion();
	}

	public void onError() {
		Log.v(getTag(), Logging.getCurrentMethodName());
		onCompletion(); // does the same logic
	}

	// from the JniPlayerWrapper interface
	// invoked from the CAndroidPlayerImpl class
	public int play(String path, String form) {
		Log.v(getTag(), Logging.getCurrentMethodName(path, form));
		for (AbstractPlayer player : players.values()) {
			player.cancelTaskIfRunning();
		}
		if (sharedState.getPlayingState() != PlayingState.STOPPED) {
			BigLog.i(getTag(), "play:: is not stopped", logHighlight);
			if (sharedState.getCurrentMediaType() != MediaType.NA) {
				BigLog.i(getTag(), "play:: is not stopped:: is not NA",
						logHighlight);
				players.get(sharedState.getCurrentMediaType()).stop();
			}
		}
		sharedState.setCurrentMediaType(MediaType.NA);
		sharedState.setPlayingState(PlayingState.STOPPED);
		BigLog.i(getTag(), "play(" + path + "," + form + ")", logHighlight);
		if (MediaType.fromString(form) == MediaType.NA) {
			BigLog.e(getTag(), "Could not parse form: " + form, logHighlight);
			return ERROR;
		}
		sharedState.setCurrentMediaType(MediaType.fromString(form));
		sharedState.setPlayingState(PlayingState.PLAYING);
		players.get(sharedState.getCurrentMediaType()).play(path);
		return OK;
	}

	public void pause() {
		BigLog.i(getTag(), Logging.getCurrentMethodName(), logHighlight);
		if (sharedState.getPlayingState() != PlayingState.PLAYING) {
			BigLog.w(getTag(), "pause() invoked in some wrong state",
					logHighlight);
			return;
		}
		if (sharedState.getCurrentMediaType() != MediaType.NA) {
			players.get(sharedState.getCurrentMediaType()).pause();
		} else {
			BigLog.e(getTag(), "pause called in a wrong state", logHighlight);
		}
		sharedState.setPlayingState(PlayingState.PAUSED);
	}

	public int resume() {
		BigLog.i(getTag(), Logging.getCurrentMethodName(), logHighlight);
		if (sharedState.getPlayingState() != PlayingState.PAUSED) {
			BigLog.w(getTag(), "resume() invoked in some wrong state",
					logHighlight);
			return ERROR;
		}
		if (sharedState.getCurrentMediaType() != MediaType.NA) {
			players.get(sharedState.getCurrentMediaType()).resume();
			sharedState.setPlayingState(PlayingState.PLAYING);
		}
		return OK;
	}

	public void stop() {
		for (AbstractPlayer player : players.values()) {
			player.cancelTaskIfRunning();
		}
		BigLog.i(getTag(), Logging.getCurrentMethodName(), logHighlight);
		if (sharedState.getCurrentMediaType() != MediaType.NA) {
			players.get(sharedState.getCurrentMediaType()).stop();
		}
		sharedState.setCurrentMediaType(MediaType.NA);
		sharedState.setPlayingState(PlayingState.STOPPED);
	}

	public boolean isTrackEnded() {
		return sharedState.getPlayingState() == PlayingState.STOPPED;
	}

	@Override
	protected String getTag() {
		return this.getClass().getSimpleName();
	}
}
