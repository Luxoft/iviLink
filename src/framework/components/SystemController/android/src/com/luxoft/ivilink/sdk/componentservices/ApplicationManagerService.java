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
 
package com.luxoft.ivilink.sdk.componentservices;

import android.content.Intent;
import android.os.Handler;

import com.luxoft.ivilink.sdk.AppLauncher;
import com.luxoft.ivilink.sdk.CApplicationManagerWrapper;
import com.luxoft.ivilink.sdk.android.lib.utils.ForSDK;
import com.luxoft.ivilink.sdk.helpers.StartupProgress;

/**
 * In this service the Application Manager process is run
 */
public class ApplicationManagerService extends ProtoComponentService {

	CApplicationManagerWrapper wrapper;

	@Override
	protected StartupProgress getProgress() {
		return StartupProgress.APPLICATION_MANAGER;
	}

	@Override
	protected void startComponent(Intent intent) {
		wrapper = new CApplicationManagerWrapper();
		wrapper.start(ForSDK.getAppManDirectory(), AppLauncher.getInstance(this, new Handler()));
	}
}
