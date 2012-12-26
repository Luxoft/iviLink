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
 
package com.luxoft.ivilink.samples.basic;

import android.util.Log;

import com.luxoft.ivilink.samples.basic.api.BasicSampleAPI;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;
import com.luxoft.ivilink.sdk.android.nonnative.GenericProfileGetter;
import com.luxoft.ivilink.sdk.android.nonnative.ProfileNotFoundException;

public class BasicSampleProfileProxy implements BasicSampleAPI.ProfileAPI {

	private final String tag = BasicSampleProfileProxy.class.getName();
	private GenericProfileGetter<BasicSampleAPI.ProfileAPI> profileGetter;

	public BasicSampleProfileProxy(String profileApiUid, String serviceUid) {
		Log.v(tag, Logging.getCurrentMethodName(profileApiUid, serviceUid));
		profileGetter = new GenericProfileGetter<BasicSampleAPI.ProfileAPI>(
				profileApiUid, serviceUid, BasicSampleAPI.ProfileAPI.class);
	}

	public void sendOperands(byte a, byte b) {
		Log.v(tag, Logging.getCurrentMethodName(a, b));
		try {
			profileGetter.getProfile().sendOperands(a, b);
			profileGetter.releaseProfile();
		} catch (ProfileNotFoundException e) {
			Log.e(tag, "Could not obtain profile instance: " + e.getMessage());
		}
	}

	public void sendResult(byte res) {
		Log.v(tag, Logging.getCurrentMethodName(res));
		try {
			profileGetter.getProfile().sendResult(res);
			profileGetter.releaseProfile();
		} catch (ProfileNotFoundException e) {
			Log.e(tag, "Could not obtain profile instance: " + e.getMessage());
		}
	}
}
