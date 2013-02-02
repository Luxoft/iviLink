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
package com.luxoft.ivilink.samples.notifications.server.twitter;

import java.util.ArrayList;
import java.util.List;

import twitter4j.Paging;
import twitter4j.Status;
import twitter4j.Twitter;
import twitter4j.TwitterException;
import android.util.Log;

public class TweetReader {

	public static synchronized List<Status> retrieveSpecificUsersTweets(
			Twitter twitter, int page) throws TwitterException {
		List<Status> statuses = new ArrayList<Status>();
		Paging p = new Paging(page);
		statuses = (ArrayList<Status>) twitter.getFriendsTimeline(p);
		return statuses;
	}

	public static Status getLatestStatus(Twitter twitter) {
		Status result = null;
		try {
			Paging p = new Paging(1);
			result = (Status) (twitter.getFriendsTimeline(p).get(0));
		} catch (TwitterException e) {
			Log.e("Twitter", e.getMessage());
		}
		return result;
	}
}
