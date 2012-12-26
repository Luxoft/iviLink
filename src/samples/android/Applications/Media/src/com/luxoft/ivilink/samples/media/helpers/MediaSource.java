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
 
package com.luxoft.ivilink.samples.media.helpers;

import android.content.Context;
import android.os.Environment;
import android.util.Log;

import com.luxoft.ivilink.samples.media.VideoActivity;

public class MediaSource {
	public static final String mediaPath = Environment.getExternalStorageDirectory()
			.getAbsolutePath() + "/iviLinkMedia/";

	private static MediaSource instance = new MediaSource();

	private Context context;

	public static MediaSource getInstance() {
		return instance;
	}

	public void setMediaGui(VideoActivity videoActivity) {
		this.context = videoActivity;
	}

	// invoked from native
	@SuppressWarnings("unused")
	private boolean startVlcServer(int pipeReadDescriptor) {
		Log.v(this.toString(), "startVlcServer(" + pipeReadDescriptor + ")");
		// this call is thread-safe
		return VlcServerLoader.getInstance().launchVLC(context, pipeReadDescriptor);
	}
}
