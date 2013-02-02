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
package com.luxoft.ivilink.sdk.loggingdaemon;

import java.io.File;
import java.io.IOException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;

import android.os.Environment;
import android.util.Log;

import com.luxoft.ivilink.sdk.android.lib.utils.log.BigLog;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;
import com.luxoft.ivilink.sdk.loggingdaemon.mail.GMailSender;

public class LogsRecorder {
	private static final String FILE_NAME_PREFIX = "iviLink_log_" + android.os.Build.DEVICE + "_";
	private static final String LOGCAT_COMMAND = "logcat -vthreadtime -f";
	private static final String LOGCAT_CLEAN_COMMAND = "logcat -c";

	private static final String tag = LogsRecorder.class.getName();

	private static String recordedFileName;
	private static Process logcatProcess;

	private static String getDateTime() {
		DateFormat dateFormat = new SimpleDateFormat("yyyy_MM_dd_HH_mm");
		Date date = new Date();
		return dateFormat.format(date);
	}

	public static void startRecording() {
		synchronized (LogsRecorder.class) {
			recordedFileName = Environment.getExternalStorageDirectory().getAbsolutePath() + "/"
					+ FILE_NAME_PREFIX + getDateTime() + ".txt";
			try {
				Runtime.getRuntime().exec(LOGCAT_CLEAN_COMMAND);
				logcatProcess = Runtime.getRuntime().exec(LOGCAT_COMMAND + recordedFileName);
			} catch (IOException e) {
				e.printStackTrace();
				logcatProcess = null;
			}
		}
	}

	public static void destroyLogcatProcess() {
		synchronized (LogsRecorder.class) {
			Log.v(tag, Logging.getCurrentMethodName());
			if (logcatProcess == null) {
				Log.e(tag, "LogcatProcess is already destroyed");
				return;
			}
			logcatProcess.destroy();
			logcatProcess = null;
		}
	}

	public static void mailLog() {
		synchronized (LogsRecorder.class) {
			Log.v(tag, Logging.getCurrentMethodName());
			destroyLogcatProcess();
			GMailSender sender = new GMailSender("knilivi", "Gogland95!");
			File mailAttachment = new File(recordedFileName);
			if (mailAttachment.exists()) {
				try {
					sender.sendMail(
							android.os.Build.BRAND + " " + android.os.Build.MODEL + " logs",
							"Some logs FYI.\nDevice info:\n" + getDeviceInfo(),
							"knilivi@gmail.com", "knilivi@gmail.com", mailAttachment);
					mailAttachment.delete();
				} catch (Exception e) {
					e.printStackTrace();
					// for anti-ivilink's sake
					try {
						Thread.sleep(500);
					} catch (InterruptedException e1) {
						e1.printStackTrace();
					}
				}
			}
		}
	}

	private static String getDeviceInfo() {
		StringBuilder sb = new StringBuilder();
		sb.append("Android OS version code: " + android.os.Build.VERSION.SDK_INT + "\n");
		sb.append("Device: " + android.os.Build.DEVICE + "\n");
		sb.append("Display: " + android.os.Build.DISPLAY + "\n");
		sb.append("Hardware: " + android.os.Build.HARDWARE + "\n");
		sb.append("Manufacturer: " + android.os.Build.MANUFACTURER + "\n");
		if (android.os.Build.VERSION.SDK_INT >= 9) {
			sb.append("Serial: " + android.os.Build.SERIAL + "\n");
		} else {
			sb.append("Serial: " + "Version 8 or less, can not get SERIAL" + "\n");
		}
		BigLog.i(tag, sb.toString(), '!');
		return sb.toString();
	}
}
