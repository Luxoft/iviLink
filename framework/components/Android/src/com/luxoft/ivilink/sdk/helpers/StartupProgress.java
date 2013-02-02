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
package com.luxoft.ivilink.sdk.helpers;

import android.content.Context;
import android.content.Intent;

public enum StartupProgress {
	SYSTEM_WATCHDOG, CONNECTIVITY_AGENT, CHANNEL_SUPERVISOR, PROFILE_LAYER, APPLICATION_MANAGER, AUTHENTICATION;

	public int getProgress() {
		switch (this) {
		case SYSTEM_WATCHDOG:
			return 5;
		case APPLICATION_MANAGER:
			return 15;
		case CONNECTIVITY_AGENT:
			return 20;
		case CHANNEL_SUPERVISOR:
			return 50;
		case PROFILE_LAYER:
			return 70;
		case AUTHENTICATION:
			return 90;
		default:
			break;
		}
		return 0;
	}

	public String getMessage() {
		switch (this) {
		case CONNECTIVITY_AGENT:
			return "Establishing physical connection";
		case CHANNEL_SUPERVISOR:
			return "Physical connection established, initializing channel layer";
		case PROFILE_LAYER:
			return "Channel layer initialized, initializing profile layer";
		case APPLICATION_MANAGER:
			return "Application layer initialized, starting connectivity layer";
		case AUTHENTICATION:
			return "Authentifying";
		case SYSTEM_WATCHDOG:
			return "System Watchdog started, initializing application layer";
		default:
			break;
		}
		return "";
	}

	public void updateProgress(Context context) {
		context.sendBroadcast(new Intent(Common.ifProgress).putExtra(
				Common.message, this.getMessage()).putExtra(
				Common.progressValue, this.getProgress()));
	}
}
