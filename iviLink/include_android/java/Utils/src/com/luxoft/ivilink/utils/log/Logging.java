package com.luxoft.ivilink.utils.log;

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
