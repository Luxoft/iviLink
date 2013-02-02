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


package com.luxoft.ivilink.sdk;

import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;
import com.luxoft.ivilink.sdk.helpers.Common;

import android.content.Intent;
import android.util.Log;

/**
 * An instance of this class is passed to System Controller's main,
 * and from there - to utils/android/MakeRequest. Whenever a component 
 * must be launched or reset/shutdown is needed, the answerRequest method is invoked
 */
public class ComponentLauncher {
	enum RequestType {
		eLaunchWatchdog,
		eLaunchAgent, 
		eLaunchNegotiator, 
		eLaunchProfileManager, 
		eLaunchApplicationManager, 
		eStartAuth, 
		eIdle, // ready
		eShutdown,
		eReset
	}

	SystemControllerService mService;

	ComponentLauncher(SystemControllerService service) {
		mService = service;
	}

	final static RequestType[] reqVals = RequestType.values();

	/**
	 * Invoked from the native code, see utils/android/private/MakeRequest.cpp
	 * @param someRequest
	 */
	void answerRequest(int someRequest) {
		Log.v(this.toString(), Logging.getCurrentMethodName(someRequest));
		switch (reqVals[someRequest]) {
		case eLaunchWatchdog:
			mService.launchSystemControllerWatchdog();
			break;
		case eLaunchAgent:
            Log.d("profiler.ComponentIsStarted", "IVILinkConnectivityAgent");
			mService.launchConnectivityAgent();
			break;
		case eLaunchNegotiator:
            Log.d("profiler.ComponentIsStarted", "IVILinkNegotiator");
			mService.launchChannelSupervisor();
			break;
		case eLaunchProfileManager:
            Log.d("profiler.ComponentIsStarted", "IVILinkProfileManager");
			mService.launchProfileManager();
			break;
		case eLaunchApplicationManager:
            Log.d("profiler.ComponentIsStarted", "IVILinkApplicationManager");
			mService.launchApplicationManager();
			break;
		case eStartAuth:
			mService.launchAuthentication();
			break;
		case eIdle:
			mService.sendBroadcast(new Intent(Common.serviceBR).putExtra(Common.misc,
					Common.doneLaunch));
			break;
		case eShutdown:
			mService.sendBroadcast(new Intent(Common.serviceBR).putExtra(Common.misc,
					Common.shutdown));
			break;
		case eReset:
			mService.sendBroadcast(new Intent(Common.serviceBR).putExtra(Common.misc, Common.reset));
			break;
		}
	}
}
