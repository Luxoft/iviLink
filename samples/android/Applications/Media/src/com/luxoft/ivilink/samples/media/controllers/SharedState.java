/* 
 * iviLINK SDK, version 1.2
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012-2013, Luxoft Professional Corp., member of IBS group
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

import com.luxoft.ivilink.samples.media.structs.MediaType;
import com.luxoft.ivilink.samples.media.structs.PlayingState;

public class SharedState {
	private static final String tag =  SharedState.class.getName();
	private PlayingState playingState = PlayingState.STOPPED;
	private MediaType mediaType = MediaType.NA;
	private static SharedState instance = new SharedState();

	private SharedState() {

	}

	public static SharedState getInstance() {
		return instance;
	}

	public PlayingState getPlayingState() {
		Log.v(tag, "getPlayingState(): " + playingState.toString());
		return playingState;
	}

	public MediaType getCurrentMediaType() {
		Log.v(tag, "getCurrentMediaType(): " + mediaType.toString());
		return mediaType;
	}

	public PlayingState getPlayingState(boolean silent) {
		if (!silent)
			return getPlayingState();
		return playingState;
	}

	public MediaType getCurrentMediaType(boolean silent) {
		if (!silent)
			return getCurrentMediaType();
		return mediaType;
	}

	public void setPlayingState(PlayingState state) {
		Log.v(tag, "setPlayingState(): " + playingState.toString() + "->"
				+ state.toString());
		this.playingState = state;
	}

	public void setCurrentMediaType(MediaType type) {
		Log.v(tag, "setCurrentMediaType(): " + mediaType.toString() + "->"
				+ type.toString());
		this.mediaType = type;
	}
}
