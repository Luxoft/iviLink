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

public class Tweet {
	private class TweetInternal {
		final String name;
		final String time;
		final String content;
		final long id;
		final String userpic;

		TweetInternal(String name, String time, String content, String userpic,
				long id) {
			this.name = name;
			this.time = time;
			this.content = content;
			this.userpic = userpic;
			this.id = id;
		}
	}

	private class JsonConst {
		static final String avatar = "avatar";
		static final String name = "name";
		static final String time = "time";
		static final String id = "id";
		static final String content = "content";
	}

	private TweetInternal tweetInternal;

	public Tweet(JSONObject description) {
		try {
			String name = description.getString(JsonConst.name);
			String time = description.getString(JsonConst.time);
			long id = description.getLong(JsonConst.id);
			String content = description.getString(JsonConst.content);
			String userpic = description.getString(JsonConst.avatar);
			tweetInternal = new TweetInternal(name, time, content, userpic, id);
		} catch (JSONException e) {
			tweetInternal = new TweetInternal("", "", "", "", 0);
			e.printStackTrace();
		}
	}

	public Tweet(String name, String time, String content, String userpic,
			long id) {
		tweetInternal = new TweetInternal(name, time, content, userpic, id);
	}

	public String getUserName() {
		return tweetInternal.name;
	}

	public String getAvatarUrl() {
		return tweetInternal.userpic;
	}

	public String getTimeOfCreation() {
		return tweetInternal.time;
	}

	public long getTweetID() {
		return tweetInternal.id;
	}

	public String getStatusContent() {
		return tweetInternal.content;
	}

	@Override
	public String toString() {
		JSONObject obj = new JSONObject();
		try {
			obj.put(JsonConst.name, tweetInternal.name);
			obj.put(JsonConst.time, tweetInternal.time);
			obj.put(JsonConst.id, tweetInternal.id);
			obj.put(JsonConst.content, tweetInternal.content);
			obj.put(JsonConst.avatar, tweetInternal.userpic);
		} catch (JSONException e) {
			e.printStackTrace();
		}
		return obj.toString();
	}

}
