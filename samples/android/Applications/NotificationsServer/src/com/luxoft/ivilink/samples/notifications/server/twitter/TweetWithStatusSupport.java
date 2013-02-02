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

import org.json.JSONObject;

import twitter4j.Status;

import com.luxoft.ivilink.samples.notifications.Tweet;

public class TweetWithStatusSupport extends Tweet {

	public TweetWithStatusSupport(JSONObject description) {
		super(description);
	}

	public TweetWithStatusSupport(String name, String time, String content,
			String userpic, long id) {
		super(name, time, content, userpic, id);
	}

	public TweetWithStatusSupport(Status status) {
		super(status.getUser().getName(), Utility.getDateDifference(status
				.getCreatedAt()), status.getText(), status.getUser()
				.getProfileImageURL().toString(), status.getId());
	}
}
