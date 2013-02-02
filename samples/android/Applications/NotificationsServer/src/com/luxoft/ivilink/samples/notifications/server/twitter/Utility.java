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
/**
 * 
 */
package com.luxoft.ivilink.samples.notifications.server.twitter;

import java.util.Calendar;
import java.util.Date;

/**
 * @author robert.hinds
 * 
 *         Utility class for helper methods
 */
public class Utility {

	public static String getDateDifference(Date thenDate) {
		Calendar now = Calendar.getInstance();
		Calendar then = Calendar.getInstance();
		now.setTime(new Date());
		then.setTime(thenDate);
		// Get the represented date in milliseconds
		long nowMs = now.getTimeInMillis();
		long thenMs = then.getTimeInMillis();
		// Calculate difference in milliseconds
		long diff = nowMs - thenMs;
		// Calculate difference in seconds
		long diffMinutes = diff / (60 * 1000);
		long diffHours = diff / (60 * 60 * 1000);
		long diffDays = diff / (24 * 60 * 60 * 1000);

		if (diffMinutes < 60) {
			if (diffMinutes == 1)
				return diffMinutes + " minute ago";
			else
				return diffMinutes + " minutes ago";
		} else if (diffHours < 24) {
			if (diffHours == 1)
				return diffHours + " hour ago";
			else
				return diffHours + " hours ago";
		} else if (diffDays < 30) {
			if (diffDays == 1)
				return diffDays + " day ago";
			else
				return diffDays + " days ago";
		} else {
			return "a long time ago..";
		}
	}
}
