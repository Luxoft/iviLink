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

import java.util.List;

import org.json.JSONException;
import org.json.JSONObject;

import twitter4j.Status;
import twitter4j.TwitterException;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.util.Log;
import android.widget.Toast;

import com.luxoft.ivilink.samples.notifications.DataSender;
import com.luxoft.ivilink.samples.notifications.JsonConst;
import com.luxoft.ivilink.samples.notifications.PackingHelper;
import com.luxoft.ivilink.samples.notifications.PermittedReceivers;
import com.luxoft.ivilink.samples.notifications.RequestManager;
import com.luxoft.ivilink.samples.notifications.Tweet;
import com.luxoft.ivilink.samples.notifications.server.twitter.TweetReader;
import com.luxoft.ivilink.samples.notifications.server.twitter.TweetWithStatusSupport;

public class TwitterServerRequestManager extends RequestManager {
	private TwitterServerRequestManager(DataSender sender, PermittedReceivers receiver) {
		super(sender, receiver);
		context = null;
	}

	public static final String INTENT_ACTION = "twitter timeline updated";

	public static final int CHECK_TIMEOUT = 5 * 60 * 1000; // 5 minutes refresh

	private static String tag;
	final Context context;
	Handler poster = new Handler();

	Status latestStatus;

	private static TwitterServerRequestManager instance;

	TwitterServerRequestManager(DataSender sender, PermittedReceivers receiver, Context context) {
		super(sender, receiver);
		this.context = context;
		instance = this;
		tag = super.tag + ".TwitterServerRM";
		Log.i(tag, "created!");
		new Thread(new Runnable() {
			public void run() {
				while (Commons.getInstance().getTwitter() == null) {
					try {
						Thread.sleep(100);
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
				}
				Log.i(tag, "Auth done!");
				while (true) {
					List<Status> newStatuses;
					try {
						newStatuses = TweetReader.retrieveSpecificUsersTweets(Commons.getInstance()
								.getTwitter(), 1);
						if (latestStatus == null) {
							latestStatus = newStatuses.get(0);
							sendStatus(latestStatus);
						} else {
							if (newStatuses.get(0).getCreatedAt()
									.compareTo(latestStatus.getCreatedAt()) > 0) {
								latestStatus = newStatuses.get(0);
								sendStatus(latestStatus);
							}
						}
						synchronized (Commons.class) {
							Commons.getInstance().setTwitterTimeline(newStatuses);
						}
						TwitterServerRequestManager.this.context.sendBroadcast(new Intent(
								INTENT_ACTION));
					} catch (TwitterException e1) {
						Log.e(tag, "Error retrieving tweets:");
						e1.printStackTrace();
					}

					try {
						Thread.sleep(CHECK_TIMEOUT);
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
				}
			}
		}).start();
	}

	public void sendStatus(Status status) {
		Log.v(tag, "sendStatus");
		Tweet tweet = new TweetWithStatusSupport(status);
		Log.i(tag, "sendStatus: tweet " + tweet.toString());
		JSONObject toSend = new JSONObject();
		try {
			toSend.put(JsonConst.action, JsonConst.TwitterActions.display);
			toSend.put(JsonConst.tweet, new JSONObject(tweet.toString()));
			String request = PackingHelper.createRequest(toSend.toString(), receiver.toString());
			Log.i(tag, "sendStatus: formed request " + request);
			sender.sendData(request);
		} catch (JSONException e) {
			e.printStackTrace();
		}
	}

	public static TwitterServerRequestManager getInstance() {
		Log.v(tag, "instance requested!");
		return instance;
	}

	@Override
	public void manageRequest(JSONObject obj) {
		Log.i(tag, "manage request: ");
		try {
			Log.i(tag, obj.toString());
			if (obj.getString(JsonConst.action).equals(JsonConst.TwitterActions.retweet)) {
				Log.i(tag, "was asked to retweet!");
				final long id = obj.getLong(JsonConst.tweet);
				try {
					Commons.getInstance().getTwitter().retweetStatus(id);
					poster.post(new Runnable() {
						public void run() {
							Toast.makeText(context, "Retweeted status " + id + " successfully!",
									Toast.LENGTH_LONG).show();
						}
					});
				} catch (final TwitterException e) {
					e.printStackTrace();
					poster.post(new Runnable() {
						public void run() {
							Toast.makeText(context,
									"Retweet of the status " + id + " failed: " + e.getMessage(),
									Toast.LENGTH_LONG).show();
						}
					});
				}
			}
		} catch (JSONException e) {
			e.printStackTrace();
		}
	}
}
