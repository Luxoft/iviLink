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


package com.luxoft.ivilink.sdk.helpers;

import java.io.File;

import android.os.Environment;
import android.util.Log;

import com.luxoft.ivilink.sdk.android.lib.utils.Paths;

public class DirectoryHelper {
    private static final String TAG = DirectoryHelper.class.getName();

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
        return checkDir(Paths.getAppDatabase()) && checkDir(Paths.getProfileRepository())
                && checkDir(Paths.getServiceRepository());
    }

    /*
     * creates directories and warns user if some of the already existing
     * folders are empty
     */
    public static String createMissingDirs() {
        StringBuilder tResult = new StringBuilder(
                "For normal use of this product these steps must be done:\n");
        fixDirs(Paths.getAppDatabase(), tResult);
        fixDirs(Paths.getProfileRepository(), tResult);
        fixDirs(Paths.getServiceRepository(), tResult);
        Log.v("DirectoryHelper", "result is: " + tResult.toString());
        return tResult.toString();
    }

    /*
     * checks external storage state
     */
    public static boolean checkExternalStorageState() {
        String state = Environment.getExternalStorageState();
        return Environment.MEDIA_MOUNTED.equals(state);
    }
}
