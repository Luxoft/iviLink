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
package com.luxoft.ivilink.sdk.android.lib.utils.log;

import java.nio.ByteBuffer;
import java.util.Formatter;

import android.util.Log;

/**
 * Provides some helpful functions for logging.
 */
public class Logging {

	private static final int CALLER_METHOD_STACK_INDEX;

	static {
		int i = 0;
		for (StackTraceElement ste : Thread.currentThread().getStackTrace()) {
			i++;
			if (ste.getClassName().equals(Logging.class.getName())) {
				break;
			}
		}
		Log.i(Logging.class.getCanonicalName(), "CALLER_METHOD_STACK_INDEX = " + i);
		CALLER_METHOD_STACK_INDEX = i;
	}

	/**
	 * 
	 * @return Name of the method in which this one was called
	 */
	public static String getCurrentMethodName() {
		StackTraceElement[] ste = Thread.currentThread().getStackTrace();
		return "CURRENT METHOD: " + ste[CALLER_METHOD_STACK_INDEX].getMethodName() + "(...)";
	}

	/**
	 * 
	 * @param args
	 *            List of arguments of the caller method should be placed here
	 * @return Name of the method in which this one was called with list of
	 *         arguments in human-readable format
	 */
	public static String getCurrentMethodName(Object... args) {
		StackTraceElement[] ste = Thread.currentThread().getStackTrace();
		StringBuffer buffer = new StringBuffer();
		if (args == null) {
			buffer.append("null");
		} else {
			for (int i = 0; i < args.length; i++) {
				if (args[i] != null) {
					buffer.append(args[i].toString());
				} else {
					buffer.append("null");
				}
				buffer.append(i == args.length - 1 ? "" : ", ");
			}
		}
		return "CURRENT METHOD: " + ste[CALLER_METHOD_STACK_INDEX].getMethodName() + "("
				+ buffer.toString() + ")";
	}

	/**
	 * Converts byte array into a hex String
	 * 
	 * @param bytes
	 *            Bytes to convert
	 * @return Converted String
	 */
	public static String bytesToHexString(byte[] bytes) {
		StringBuilder sb = new StringBuilder(bytes.length * 3);

		Formatter formatter = new Formatter(sb);
		for (byte b : bytes) {
			formatter.format(" %02x", b);
		}
		formatter.close();
		return sb.toString();
	}

	  /**
	   * Converts contents of a ByteBuffer into a hex String
	   * 
	   * @param bytes
	   *            Bytes to convert
	   * @return Converted String
	   */
	  public static String bytesToHexString(ByteBuffer bytes) {
	      return bytesToHexString(bytes.array());
	  }
}
