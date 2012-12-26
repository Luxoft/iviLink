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
 

package com.luxoft.ivilink.sdk.helpers;

import java.io.File;

import com.luxoft.ivilink.sdk.android.lib.utils.ForApp;
import com.luxoft.ivilink.sdk.android.lib.utils.ForSDK;

import android.os.Environment;
import android.util.Log;

public class DirectoryHelper {

	static String extStorage = Environment.getExternalStorageDirectory().getAbsolutePath();

	/*
	 * returns true if path is a non-empty directory
	 */
	private static boolean checkDir(String path) {
		if (checkDirExists(path)) {
			return checkDirEmpty(path);
		}
		return false;
	}

	private static boolean checkDirEmpty(String path) {
		return new File(path).list().length > 0;
	}

	private static boolean checkDirExists(String path) {
		File f = new File(path);
		return f.exists() && f.isDirectory();
	}

	/*
	 * creates a directory @ path
	 */
	private static boolean createDir(String path) {
		File f = new File(path);
		if (f.exists()) {
			f.delete();
		}
		return f.mkdir();
	}

	/*
	 * creates placeholders for missing directories and checks state of other
	 * directories
	 */
	private static void fixDirs(String path, StringBuilder sb) {

		if (!checkDirExists(path)) {
			if (createDir(path)) {
				sb.append("The following directory was created for you: " + path
						+ ", please put files there\n");
			} else {
				sb.append("Creation of the following directory " + path
						+ " failed for some reason!\n");
			}
		} else {
			if (!checkDirEmpty(path)) {
				sb.append("Please put necessary files into folder " + path + "\n");
			}
		}

	}

	// public methods

	/*
	 * returns true if all needed folders pass check
	 */
	public static boolean checkProjectDirs() {
		return checkDir(ForSDK.getAppManDirectory()) && checkDir(ForSDK.getProfRepoDirectory())
				&& checkDir(ForApp.getServicePath());
	}

	/*
	 * creates directories and warns user if some of the already existing
	 * folders are empty
	 */
	public static String createMissingDirs() {
		/*StringBuilder tResult = new StringBuilder(
				"For normal use of this product these steps must be done:\n");
		fixDirs(ForSDK.getAppManDirectory(), tResult);
		fixDirs(ForSDK.getProfRepoDirectory(), tResult);
		fixDirs(ForApp.getServicePath(), tResult);
		Log.v("DirectoryHelper", "result is: " + tResult.toString());
		return tResult.toString(); */
		return "No data found! Please run the Installer first!";
	}

	/*
	 * checks external storage state
	 */
	public static boolean checkExtState() {
		String state = Environment.getExternalStorageState();
		if (Environment.MEDIA_MOUNTED.equals(state)) {
			return true; // readable and writable
		} else if (Environment.MEDIA_MOUNTED_READ_ONLY.equals(state)) {
			// not writable, may cause problems for sdk
			return false;
		} else {
			return false;
		}
	}

}
