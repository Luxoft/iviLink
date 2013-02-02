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
package com.luxoft.ivilink.samples.media.helpers;

import android.content.Context;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.content.pm.ServiceInfo;
import android.util.Log;

import com.luxoft.ivilink.samples.media.MediaPlayerService;
import com.luxoft.ivilink.samples.media.players.audio.AudioService;
import com.luxoft.ivilink.samples.media.players.video.VideoPlayerService;

public class ProcessesInfo {
    private final static String TAG = ProcessesInfo.class.getName();

    public static String getMusicPlayerProcessName(Context context) {
        return getServiceProcessName(context.getApplicationContext(), AudioService.class.getName());
    }

    public static String getVideoPlayerProcessName(Context context) {
        return getServiceProcessName(context.getApplicationContext(),
                                     VideoPlayerService.class.getName());
    }

    public static String getMainProcessName(Context context) {
        return getServiceProcessName(context.getApplicationContext(),
                                     MediaPlayerService.class.getName());
    }

    public static String getActivityProcessName(Context context, String className) {
        try {
            ActivityInfo[] activities = context.getPackageManager()
                    .getPackageInfo(context.getPackageName(), PackageManager.GET_ACTIVITIES).activities;
            for (ActivityInfo activity : activities) {
                Log.v(TAG, "current class's name: " + activity.name);
                Log.v(TAG, "target class's name: " + className);
                if (activity.name.equals(className)) {
                    Log.i(TAG, "Found! Result is: " + activity.processName);
                    return activity.processName;
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }

    public static String getServiceProcessName(Context context, String className) {
        try {
            ServiceInfo[] services = context.getPackageManager()
                    .getPackageInfo(context.getPackageName(), PackageManager.GET_SERVICES).services;
            for (ServiceInfo service : services) {
                Log.v(TAG, "current class's name: " + service.name);
                Log.v(TAG, "target class's name: " + className);
                if (service.name.equals(className)) {
                    Log.i(TAG, "Found! Result is: " + service.processName);
                    return service.processName;
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }
}
