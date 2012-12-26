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

import java.util.HashMap;
import java.util.Map;

import com.luxoft.ivilink.samples.media.interfaces.GuiController;
import com.luxoft.ivilink.samples.media.interfaces.PlayerEventProcessor;
import com.luxoft.ivilink.samples.media.interfaces.Scalable;
import com.luxoft.ivilink.samples.media.players.AbstractPlayer;
import com.luxoft.ivilink.samples.media.players.AudioPlayer;
import com.luxoft.ivilink.samples.media.players.VideoPlayer;
import com.luxoft.ivilink.samples.media.structs.MediaType;
import com.luxoft.ivilink.samples.media.structs.PlayingState;
import com.luxoft.ivilink.samples.media.structs.VideoScale;

public abstract class Controller implements PlayerEventProcessor, GuiController {
	protected Scalable scalable;
	protected char logHighlight = '!';

	protected final int OK = 0;
	protected final int ERROR = -1;

	Map<MediaType, AbstractPlayer> players;
	static SharedState sharedState = SharedState.getInstance();

	protected abstract String getTag();

	public Controller(Scalable scalable) {
		assert (scalable != null);
		this.scalable = scalable;
	}

	public void setPlayers(AudioPlayer audio, VideoPlayer video) {
		players = new HashMap<MediaType, AbstractPlayer>();
		players.put(MediaType.AUDIO, audio);
		players.put(MediaType.VIDEO, video);
	}

	public boolean isVideoPlayingOrPaused() {
		return sharedState.getPlayingState(true) != PlayingState.STOPPED
				&& sharedState.getCurrentMediaType(true) == MediaType.VIDEO;
	}

	public boolean canProcessPopupButtons() {
		// they can be processed only when playing fullscreen video
		return !canProcessButtons();
	}

	public boolean canProcessButtons() {
		if ((sharedState.getPlayingState() != PlayingState.STOPPED)
				&& sharedState.getScale() == VideoScale.FULLSCREEN
				&& sharedState.getCurrentMediaType() == MediaType.VIDEO) {
			// when playing fullscreen video we don't process user input
			return false;
		}
		return true;
	}

	public boolean isFullScreen() {
		return sharedState.getScale(true) == VideoScale.FULLSCREEN;
	}

	public void onScaleButtonClicked() {
		if ((sharedState.getPlayingState() != PlayingState.STOPPED)
				&& sharedState.getCurrentMediaType() == MediaType.VIDEO) {
			if (sharedState.getScale() == VideoScale.FULLSCREEN) {
				sharedState.setScale(VideoScale.WINDOWED);
				scalable.setWindowMode();
			} else if (sharedState.getScale() == VideoScale.WINDOWED) {
				sharedState.setScale(VideoScale.FULLSCREEN);
				scalable.setFullScreenMode();
			}
		}
	}
}
