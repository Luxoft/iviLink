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

public class JsonConst {
	// /public static final String type = "type";
	public static final String receiver = "receiver";

	public class Types {
		public static final String twitter = "twitter";
	}

	public static final String responce = "responce";
	public static final String requestStatus = "request status";
	public static final String requestError = "request error";
	public static final String requestSuccess = "request success";

	public static final String message = "message";

	// TODO serialization/deserialization
	public static final String tweet = "tweet";

	public static final String action = "action";

	public class TwitterActions {
		public static final String retweet = "retweet";
		public static final String fetch = "fetch";
		public static final String display = "display";
		// TODO more
	}
}
