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
 
package com.luxoft.ivilink.samples.navi;

import java.util.List;

import com.luxoft.ivilink.samples.navi.api.NaviAPI;
import com.luxoft.ivilink.sdk.android.nonnative.GenericProfileGetter;
import com.luxoft.ivilink.sdk.android.nonnative.ProfileNotFoundException;

public class NaviSenderProfileProxy implements NaviAPI.ProfileAPI {

	GenericProfileGetter<NaviAPI.ProfileAPI> profileGetter;

	public NaviSenderProfileProxy(String profileAPI, String serviceUID) {
		profileGetter = new GenericProfileGetter<NaviAPI.ProfileAPI>(
				profileAPI, serviceUID, NaviAPI.ProfileAPI.class);
	}

	public void sendTips(List<String> tips) {
		try {
			profileGetter.getProfile().sendTips(tips);
			profileGetter.releaseProfile();
		} catch (ProfileNotFoundException e) {
			e.printStackTrace();
		}
	}

}
