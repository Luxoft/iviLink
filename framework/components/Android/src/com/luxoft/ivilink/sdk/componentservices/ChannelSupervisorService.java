package com.luxoft.ivilink.sdk.componentservices;

import android.content.Intent;

import com.luxoft.ivilink.sdk.CChannelSupervisorWrapper;
import com.luxoft.ivilink.sdk.helpers.StartupProgress;

/**
 * In this service the Channel Supervisor (Negotiator) process is run
 */
public class ChannelSupervisorService extends ProtoComponentService {

	CChannelSupervisorWrapper wrapper;

	@Override
	protected StartupProgress getProgress() {
		return StartupProgress.CHANNEL_SUPERVISOR;
	}

	@Override
	protected void startComponent(Intent intent) {
		wrapper = new CChannelSupervisorWrapper();
		wrapper.start();
	}
}
