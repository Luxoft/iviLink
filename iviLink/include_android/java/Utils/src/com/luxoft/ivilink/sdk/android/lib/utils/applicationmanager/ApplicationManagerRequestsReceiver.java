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
package com.luxoft.ivilink.sdk.android.lib.utils.applicationmanager;

import com.luxoft.ivilink.sdk.android.lib.utils.IntentActions;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

/**
 * A <code>BroadcastReceiver</code> that sends {@link ApplicationInfo} to the
 * ApplicationManager when triggered. Its subclasses should be registered
 * statically in the AndroidManifest.xml to receive the action
 * <code>com.luxoft.ivilink.intent.action.APPLICATION_MANAGER_INFO_REQUEST</code>
 * (see {@link IntentActions})
 */
public abstract class ApplicationManagerRequestsReceiver extends BroadcastReceiver {

    protected abstract ApplicationInfo getApplicationInfo(Context context);

    @Override
    public final void onReceive(Context context, Intent intent) {
        Log.v(this.toString(), "Got an intent: action: " + intent.getAction() + " extras: "
                + intent.getExtras());
        if (!intent.getAction().equals(IntentActions.APPLICATION_MANAGER_INFO_REQUEST)) {
            Log.w(this.toString(), "Unexpected action: " + intent.getAction());
            return;
        }
        Intent response = new Intent();
        response.setAction(IntentActions.TO_APPLICATION_MANAGER);
        if (AppManProtocol.packApplicationInfoToIntent(getApplicationInfo(context), response)) {
            context.sendBroadcast(response);
        }
    }
}
