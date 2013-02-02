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
package com.luxoft.ivilink.samples.notifications;

import org.json.JSONException;
import org.json.JSONObject;

public class PackingHelper {
	public static String createRequest(String tuPack, String receiverType) {
		if (PermittedReceivers.fromString(receiverType) == PermittedReceivers.NOF) {
			return "";
		}
		JSONObject obj = new JSONObject();
		try {
			obj.put(JsonConst.receiver, receiverType);
			obj.put(JsonConst.message, new JSONObject(tuPack));
		} catch (JSONException e) {
			e.printStackTrace();
		}
		return obj.toString();
	}

	public static PermittedReceivers getReceiver(String unPack) {
		JSONObject obj;
		try {
			obj = new JSONObject(unPack);
		} catch (JSONException e) {
			e.printStackTrace();
			return PermittedReceivers.NOF;
		}
		String receiver;
		try {
			receiver = obj.getString(JsonConst.receiver);
		} catch (JSONException e) {
			e.printStackTrace();
			return PermittedReceivers.NOF;
		}
		return PermittedReceivers.fromString(receiver);
	}

	public static JSONObject extractRequest(String unPack) {
		JSONObject obj;
		try {
			obj = new JSONObject(unPack);
		} catch (JSONException e) {
			e.printStackTrace();
			return null;
		}
		if (getReceiver(unPack) == PermittedReceivers.NOF) {
			return null;
		}
		JSONObject message;
		try {
			message = obj.getJSONObject(JsonConst.message);
		} catch (JSONException e) {
			e.printStackTrace();
			return null;
		}
		return message;
	}
}
