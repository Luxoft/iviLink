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
