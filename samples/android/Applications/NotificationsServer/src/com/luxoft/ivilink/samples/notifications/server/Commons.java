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
package com.luxoft.ivilink.samples.notifications.server;

import java.util.ArrayList;
import java.util.List;

import android.content.Context;
import android.content.SharedPreferences;
import android.util.Log;
import oauth.signpost.OAuthProvider;
import oauth.signpost.commonshttp.CommonsHttpOAuthConsumer;
import twitter4j.Status;
import twitter4j.Twitter;
import twitter4j.http.AccessToken;

// a singleton to store twitter instance and some other objects
public class Commons {
	public static boolean hasAuthActivityStarted;
	public static final String CALLBACK_URL = "callback://tweeter";
	public final static String tag = "iviLink.samples.Notifications.Server.";
	private Twitter twitter;
	private OAuthProvider provider;
	private CommonsHttpOAuthConsumer consumer;
	private Context context;
	private static final Commons instance = new Commons();

	// dev.twitter.com/app
	public static final String CONSUMER_KEY = "NF0aTixBnXHE6o7cubCYew";
	public static final String CONSUMER_SECRET = "oDPmvbQA6ZQdZLloZaGY6UxBhgdaLV8uTbUPJAXlI";

	private List<Status> twitterTimeline = new ArrayList<Status>();

	// shared preferences to store and access the Twitter AccessToken
	public static final String PREFS_NAME = "iviLink TwitterLogin";

	public static Commons getInstance() {
		return instance;
	}

	private Commons() {

	}

	public void setContext(Context context) {
		this.context = context;
	}

	public Twitter getTwitter() {
		return twitter;
	}

	public void setTwitter(Twitter twitter) {
		this.twitter = twitter;
	}

	public void setProvider(OAuthProvider provider) {
		this.provider = provider;
	}

	public OAuthProvider getProvider() {
		return provider;
	}

	public void setConsumer(CommonsHttpOAuthConsumer consumer) {
		this.consumer = consumer;
	}

	public CommonsHttpOAuthConsumer getConsumer() {
		return consumer;
	}

	public void storeAccessToken(AccessToken a) {
		SharedPreferences settings = context.getSharedPreferences(
				Commons.PREFS_NAME, Context.MODE_PRIVATE);
		SharedPreferences.Editor editor = settings.edit();
		editor.putString("accessTokenToken", a.getToken());
		editor.putString("accessTokenSecret", a.getTokenSecret());
		editor.commit();
	}

	public AccessToken getAccessToken() {
		SharedPreferences settings = context.getSharedPreferences(
				Commons.PREFS_NAME, Context.MODE_PRIVATE);
		String token = settings.getString("accessTokenToken", "");
		Log.i(tag, "token: " + token);
		String tokenSecret = settings.getString("accessTokenSecret", "");
		Log.i(tag, "tokenSecret: " + tokenSecret);
		if (token != null && tokenSecret != null && !"".equals(tokenSecret)
				&& !"".equals(token)) {
			return new AccessToken(token, tokenSecret);
		}
		return null;
	}

	public List<Status> getTwitterTimeline() {
		return twitterTimeline;
	}

	public void setTwitterTimeline(List<Status> newTimeline) {
		twitterTimeline.clear();
		twitterTimeline.addAll(newTimeline);
	}

}
