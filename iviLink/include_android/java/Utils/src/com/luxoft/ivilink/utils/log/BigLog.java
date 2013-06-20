package com.luxoft.ivilink.utils.log;

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
