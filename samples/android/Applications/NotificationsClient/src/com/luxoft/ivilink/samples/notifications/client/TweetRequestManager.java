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
package com.luxoft.ivilink.samples.notifications.client;

import org.json.JSONException;
import org.json.JSONObject;

import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.util.Log;

import com.luxoft.ivilink.samples.notifications.DataSender;
import com.luxoft.ivilink.samples.notifications.JsonConst;
import com.luxoft.ivilink.samples.notifications.PackingHelper;
import com.luxoft.ivilink.samples.notifications.PermittedReceivers;
import com.luxoft.ivilink.samples.notifications.RequestManager;

public class TweetRequestManager extends RequestManager {
	private static String tag;
	Context context;
	Handler poster = new Handler();

	private static TweetRequestManager instance;

	TweetRequestManager(DataSender sender, PermittedReceivers receiver,
			Context context) {
		super(sender, receiver);
		this.context = context;
		tag = super.tag + ".TweetRM";
		instance = this;
	}

	private TweetRequestManager(DataSender sender, PermittedReceivers receiver) {
		super(sender, receiver);
	}

	public static TweetRequestManager getInstance() {
		Log.v(tag, "instance requested!");
		return instance;
	}

	@Override
	public void manageRequest(JSONObject obj) {
		Log.v(tag, "manage request: ");
		try {
			Log.i(tag, obj.toString());
			if (obj.getString(JsonConst.action).equals(
					JsonConst.TwitterActions.display)) {
				Log.i(tag, "was asked to display a tweet!");
				context.startActivity(new Intent(context, TwitterDisplay.class)
						.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
						.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP)
						.putExtra(JsonConst.tweet,
								obj.getJSONObject(JsonConst.tweet).toString()));
			}
		} catch (JSONException e) {
			e.printStackTrace();
		}
	}

	public void requestRetweet(long id) {
		Log.v(tag, "requestRetweet(" + id + ")");
		try {
			JSONObject toSend = new JSONObject();
			toSend.put(JsonConst.action, JsonConst.TwitterActions.retweet);
			toSend.put(JsonConst.tweet, id);
			String packet = PackingHelper.createRequest(toSend.toString(),
					receiver.toString());
			Log.i(tag, "formed a request: " + packet);
			sender.sendData(packet);
		} catch (JSONException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
