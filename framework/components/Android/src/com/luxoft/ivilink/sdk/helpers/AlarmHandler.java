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

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;

import com.luxoft.ivilink.sdk.SysCtrlServLauncherActivity;

public class AlarmHandler {

    public static final int RESTART_TIMEOUT = 1000;

    public static void setAlarm(Context asker, int timeout) {
        Intent launchIntent = generateLaunchIntent(new ComponentName(asker.getPackageName(),
                SysCtrlServLauncherActivity.class.getName()));
        AlarmManager alarmManager = (AlarmManager) asker.getSystemService(Context.ALARM_SERVICE);
        PendingIntent pendingLaunchIntent = PendingIntent.getActivity(asker, 0, launchIntent, 0);
        alarmManager.set(AlarmManager.RTC, System.currentTimeMillis() + timeout,
                         pendingLaunchIntent);
    }

    public static void setAlarm(Context asker) {
        setAlarm(asker, RESTART_TIMEOUT);
    }

    private static Intent generateLaunchIntent(ComponentName name) {
        Intent launchIntent = new Intent(Intent.ACTION_MAIN);
        launchIntent.addCategory(Intent.CATEGORY_LAUNCHER);
        launchIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        launchIntent.setComponent(name);
        return launchIntent;
    }
}
