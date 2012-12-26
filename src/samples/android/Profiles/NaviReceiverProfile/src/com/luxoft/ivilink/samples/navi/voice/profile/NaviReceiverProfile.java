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
 
package com.luxoft.ivilink.samples.navi.voice.profile;

import java.util.ArrayList;
import java.util.List;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import android.util.Log;

import com.luxoft.ivilink.sdk.android.lib.utils.log.BigLog;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;
import com.luxoft.ivilink.sdk.android.nonnative.AbstractProfile;
import com.luxoft.ivilink.samples.navi.voice.api.NaviVoiceAPI;
import com.luxoft.ivilink.samples.navi.voice.api.NaviVoiceAPI.NaviVoiceApplicationAPI;

public class NaviReceiverProfile extends AbstractProfile {

	NaviVoiceAPI.NaviVoiceApplicationAPI applicationAPI;

	private final static String tag = NaviReceiverProfile.class.getName();

	private int channelID;

	public NaviReceiverProfile(String profileIUID, String serviceUID, byte[] nativeCallbacksPointer) {
		super(profileIUID, serviceUID, nativeCallbacksPointer);
		if (profileCallbacks instanceof NaviVoiceAPI.NaviVoiceApplicationAPI) {
			applicationAPI = (NaviVoiceApplicationAPI) profileCallbacks;
		} else {
			throw new RuntimeException("Provided callbacks are of a wrong type");
		}
	}

	@Override
	protected void onEnable() {
		Log.v(tag, Logging.getCurrentMethodName());
		channelID = allocateChannel("NavigationChannel");
		Log.i(tag, "allocated channel: " + channelID);
		assert (channelID != 0) : "Could not allocate a channel";
	}

	@Override
	protected void onDisable() {
		Log.v(tag, Logging.getCurrentMethodName());
		deallocateChannel(channelID);
	}

	@Override
	protected void onDataReceived(byte[] data, int channelID) {
		Log.v(this.getClass().getName(), Logging.getCurrentMethodName(data, channelID));
		String jsonString = new String(data);
		BigLog.i(tag, "Got message: " + jsonString, '!');
		JSONObject jsonObj;
		try {
			jsonObj = new JSONObject(jsonString);
			JSONArray wordsArray = jsonObj.getJSONArray("play");
			List<String> phrase = new ArrayList<String>();
			for (int i = 0; i < wordsArray.length(); i++) {
				phrase.add(wordsArray.getString(i));
			}
			String translation = Dictionary.getInstance().translatePhrase(phrase);
			BigLog.i(tag, "translated: " + translation, '!');
			applicationAPI.receiveTip(translation);
		} catch (JSONException e) {
			e.printStackTrace();
		}
	}

	@Override
	public String getName() {
		return this.getClass().getSimpleName();
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
		return "NaviReceiverProfile_PAPI_UID";
	}

}
