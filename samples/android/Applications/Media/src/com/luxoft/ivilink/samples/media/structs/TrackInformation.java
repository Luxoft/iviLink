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
package com.luxoft.ivilink.samples.media.structs;

import android.util.Log;

import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

public class TrackInformation {
	public String trackName;
	public boolean isLocal;
	public MediaType mediaType;

	public TrackInformation(String name, boolean isLocal, MediaType mediaType) {
		Log.v(this.toString(), Logging.getCurrentMethodName(name, isLocal));
		this.trackName = name;
		this.isLocal = isLocal;
		this.mediaType = mediaType;
	}

	@Override
	public String toString() {
		return trackName + " is local: " + isLocal + " type: " + mediaType;
	}
}
