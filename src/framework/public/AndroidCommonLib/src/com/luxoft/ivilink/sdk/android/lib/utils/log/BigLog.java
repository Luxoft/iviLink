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
 
package com.luxoft.ivilink.sdk.android.lib.utils.log;

import android.util.Log;

public class BigLog {
	public static void w(String tag, String message, char highlight) {
		String highlightString = createHighlightString(highlight,
				message.length());
		Log.w(tag, highlightString);
		Log.w(tag, message);
		Log.w(tag, highlightString);
	}

	public static void i(String tag, String message, char highlight) {
		String highlightString = createHighlightString(highlight,
				message.length());
		Log.i(tag, highlightString);
		Log.i(tag, message);
		Log.i(tag, highlightString);
	}

	public static void e(String tag, String message, char highlight) {
		String highlightString = createHighlightString(highlight,
				message.length());
		Log.e(tag, highlightString);
		Log.e(tag, message);
		Log.e(tag, highlightString);
	}

	private static String createHighlightString(char highlight, int length) {
		return new String(new char[length]).replace('\0', highlight);
	}
}
