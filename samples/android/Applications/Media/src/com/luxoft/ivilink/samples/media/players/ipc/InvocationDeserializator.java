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
package com.luxoft.ivilink.samples.media.players.ipc;

import java.lang.reflect.Method;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

/**
 * The pair class to the {@link InvocationSerializator}. When the receiver is
 * triggered, it deserializes the remote method and arguments and calls
 * {@link #onReceivedRemoteMethodCall(Method method, Object[] args)}
 */
public abstract class InvocationDeserializator extends BroadcastReceiver {
    private static final String TAG = InvocationDeserializator.class.getName();

    private Method[] mAvaliableApiMethods;

    public InvocationDeserializator(Method[] apiMethods) {
        mAvaliableApiMethods = apiMethods;
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        Log.v(TAG, Logging.getCurrentMethodName(context, intent));
        String methodName = intent.getStringExtra(CommunicationProtocol.METHOD);
        if (methodName == null || methodName.length() == 0) {
            Log.e(TAG, "Could not extract method's name from the intent");
            return;
        }
        Log.i(TAG, "Extracted method name: " + methodName);
        String argsJsonHolder = intent.getStringExtra(CommunicationProtocol.ARGS);
        Log.i(TAG, "args: " + argsJsonHolder);
        Object[] args = null;
        if (argsJsonHolder != null && argsJsonHolder.length() != 0) {
            try {
                JSONObject obj = new JSONObject(argsJsonHolder);
                JSONArray argsJsonArr = obj.getJSONArray(CommunicationProtocol.ARGS);
                args = new Object[argsJsonArr.length()];
                for (int i = 0; i < argsJsonArr.length(); i++) {
                    try {
                        args[i] = argsJsonArr.get(i);
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }
                }
            } catch (JSONException e) {
                e.printStackTrace();
            }
        }
        for (Method method : mAvaliableApiMethods) {
            if (method.getName().equals(methodName)) {
                onReceivedRemoteMethodCall(method, args);
                break;
            }
        }
    }

    protected abstract void onReceivedRemoteMethodCall(Method method, Object[] args);
}
