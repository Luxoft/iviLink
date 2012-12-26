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

public class AudioPlayerProtocol {
	public final static String MESSAGE = "MESSAGE";
	public final static String EXTRA_INFO = "ADDINFO";

	public final static String TO_PLAYER = FromServiceToPlayer.class.getName();
	public final static String TO_SERVICE = FromPlayerToService.class.getName();

	public final static int PLAYBACK_START_WAIT_TIMEOUT_MILLIS = 1000;
	public final static int PLAYBACK_RETRIES_LIMIT = 5;

	public final static int RESPONCE_FROM_SERVICE_WAIT_MILLIS = PLAYBACK_RETRIES_LIMIT
			* PLAYBACK_START_WAIT_TIMEOUT_MILLIS;

	public enum FromPlayerToService {
		PLAY, PAUSE, RESUME, STOP;

		public static FromPlayerToService fromString(String text) {
			return AudioPlayerProtocol.fromString(FromPlayerToService.class, text);
		}
	}

	public enum FromServiceToPlayer {
		COMPLETED, ERROR, PLAYING;

		public static FromServiceToPlayer fromString(String text) {
			return AudioPlayerProtocol.fromString(FromServiceToPlayer.class, text);
		}
	}

	private static <T extends Enum<T>> T fromString(Class<T> c, String string) {
		if (c != null && string != null) {
			try {
				return Enum.valueOf(c, string.trim().toUpperCase());
			} catch (IllegalArgumentException ex) {
				ex.printStackTrace();
			}
		}
		return null;
	}
}
