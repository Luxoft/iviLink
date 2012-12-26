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
 
package com.luxoft.ivilink.samples.media.structs;

public enum MediaType {
	AUDIO, VIDEO, NA;
	final static private String AUDIO_EXT = "m4a";
	final static private String VIDEO_EXT = "mp4";

	@Override
	public String toString() {
		if (this == AUDIO) {
			return "audio";
		} else if (this == VIDEO) {
			return "video";
		} else {
			return "err";
		}
	}

	public static MediaType fromString(String type) {
		if (AUDIO.toString().equals(type)) {
			return AUDIO;
		} else if (VIDEO.toString().equals(type)) {
			return VIDEO;
		} else {
			return NA;
		}
	}

	public static MediaType fromExtension(String extension) {
		if (extension.equals(AUDIO_EXT)) {
			return AUDIO;
		} else if (extension.equals(VIDEO_EXT)) {
			return VIDEO;
		} else {
			return NA;
		}
	}

	public String getExtension() {
		if (this == AUDIO)
			return AUDIO_EXT;
		if (this == VIDEO)
			return VIDEO_EXT;
		return "err";
	}
}
