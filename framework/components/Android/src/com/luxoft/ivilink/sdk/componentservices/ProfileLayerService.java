package com.luxoft.ivilink.sdk.componentservices;

import android.content.Intent;

import com.luxoft.ivilink.sdk.CProfileManagerWrapper;
import com.luxoft.ivilink.sdk.helpers.StartupProgress;

/**
 * In this service the Profile Manager Process and Profile Repository are run
 */
public class ProfileLayerService extends ProtoComponentService {
	CProfileManagerWrapper wrapper;

	@Override
	protected StartupProgress getProgress() {
		return StartupProgress.PROFILE_LAYER;
	}

	@Override
	protected void startComponent(Intent intent) {
		wrapper = new CProfileManagerWrapper();
		wrapper.start();
	}
}
