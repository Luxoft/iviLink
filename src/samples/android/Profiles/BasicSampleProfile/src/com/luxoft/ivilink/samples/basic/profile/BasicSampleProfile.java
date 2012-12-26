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
 
package com.luxoft.ivilink.samples.basic.profile;

import android.util.Log;

import com.luxoft.ivilink.samples.basic.api.BasicSampleAPI;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;
import com.luxoft.ivilink.sdk.android.nonnative.AbstractProfile;

public class BasicSampleProfile extends AbstractProfile implements
		BasicSampleAPI.ProfileAPI {

	int channelID;

	BasicSampleAPI.ApplicationAPI castProfileCallbacks;

	private final static byte SEND_OPERANDS = 1;
	private final static byte SEND_RESULT = 2;

	public BasicSampleProfile(String profileIUID, String serviceUID,
			byte[] nativeCallbacksPointer) {
		super(profileIUID, serviceUID, nativeCallbacksPointer);
		if (profileCallbacks instanceof BasicSampleAPI.ApplicationAPI) {
			castProfileCallbacks = (BasicSampleAPI.ApplicationAPI) profileCallbacks;
		} else {
			throw new RuntimeException(
					"wrong profile callbacks were supplied: "
							+ profileCallbacks.getClass().getName());
		}
	}

	@Override
	protected void onEnable() {
		Log.v(this.getClass().getName(), Logging.getCurrentMethodName());
		channelID = allocateChannel("CBasicSampleProfileImpl");
		Log.i(this.getClass().getName(), "allocated channel: " + channelID);
	}

	@Override
	protected void onDataReceived(byte[] data, int channelID) {
		Log.v(this.getClass().getName(),
				Logging.getCurrentMethodName(data, channelID));
		Log.i(this.getClass().getName(),
				"got data: " + Logging.bytesToHexString(data));
		if (data[0] == SEND_RESULT) {
			castProfileCallbacks.receiveResult(data[1]);
		} else if (data[0] == SEND_OPERANDS) {
			castProfileCallbacks.receiveOperands(data[1], data[2]);
		}
	}

	// from BasicSampleAPI.ProfileAPI
	public void sendOperands(byte a, byte b) {
		Log.v(this.getClass().getName(), Logging.getCurrentMethodName(a, b));
		byte[] data = new byte[3];
		data[0] = SEND_OPERANDS;
		data[1] = a;
		data[2] = b;
		sendData(data, channelID);
	}

	public void sendResult(byte res) {
		Log.v(this.getClass().getName(), Logging.getCurrentMethodName(res));
		byte[] data = new byte[2];
		data[0] = SEND_RESULT;
		data[1] = res;
		sendData(data, channelID);
	}

	// from AbstractProfile - the following methods must be implemented
	@Override
	public String getName() {
		Log.v(this.getClass().getName(), Logging.getCurrentMethodName());
		return "BSP";
	}

	@Override
	public int getVersion() {
		Log.v(this.getClass().getName(), Logging.getCurrentMethodName());
		return 1;
	}

	@Override
	public String getUid() {
		Log.v(this.getClass().getName(), Logging.getCurrentMethodName());
		return "BasicSampleProfileImpl_UID";
	}

	@Override
	public String getProfileApiUid() {
		Log.v(this.getClass().getName(), Logging.getCurrentMethodName());
		return BasicSampleAPI.apiName;
	}

}
