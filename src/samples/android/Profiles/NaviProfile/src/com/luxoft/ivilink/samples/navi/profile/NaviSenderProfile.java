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
 
package com.luxoft.ivilink.samples.navi.profile;

import java.util.List;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import android.util.Log;

import com.luxoft.ivilink.samples.navi.api.NaviAPI;
import com.luxoft.ivilink.sdk.android.lib.utils.Assertion;
import com.luxoft.ivilink.sdk.android.nonnative.AbstractProfile;

public class NaviSenderProfile extends AbstractProfile implements NaviAPI.ProfileAPI {

	int channelID;

	public NaviSenderProfile(String profileIUID, String serviceUID, byte[] nativeCallbacksPointer) {
		super(profileIUID, serviceUID, nativeCallbacksPointer);
	}

	@Override
	public String getName() {
		return this.getClass().getName();
	}

	@Override
	public int getVersion() {
		return 1;
	}

	@Override
	public String getUid() {
		return "NaviSenderProfileImpl_UID";
	}

	@Override
	public String getProfileApiUid() {
		return NaviAPI.profileApi;
	}

	@Override
	protected void onEnable() {
		channelID = allocateChannel("NavigationChannel");
		Assertion.check(channelID != 0, "could not allocate a channel");
	}

	@Override
	protected void onDisable() {
		deallocateChannel(channelID);
	}

	public void sendTips(List<String> tips) {
		JSONObject obj = new JSONObject();
		JSONArray arr = new JSONArray();
		for (String str : tips) {
			if (!str.equals(" "))
				arr.put(str);
		}
		try {
			obj.put("play", arr);
			Log.i(getName(), obj.toString());
			sendData(obj.toString().getBytes(), channelID);
		} catch (JSONException e) {
			e.printStackTrace();
		}
	}
}
