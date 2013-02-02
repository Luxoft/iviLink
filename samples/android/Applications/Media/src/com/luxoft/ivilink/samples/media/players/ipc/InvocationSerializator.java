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

import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;
import java.lang.reflect.Proxy;
import java.util.Arrays;
import java.util.List;

import org.json.JSONArray;
import org.json.JSONObject;

import android.content.Context;
import android.content.Intent;
import android.util.Log;

import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

/**
 * The pair class to {@link InvocationDeserializator}. Creates a stub instance
 * of the generic parameter class (see {@link getMockInstance()}). When a method
 * of the stub is called, the call is serialized into an intent and broadcast
 * using the action provided by the {@link #getIntentAction()} method. Note:
 * calls to the stub's API always return <code>null</code> (or other default
 * values like <code>0</code> and <code>false</code>).
 * 
 * @param <E>
 *            class whose API is to be stubbed
 */
public abstract class InvocationSerializator<E> implements InvocationHandler {

    private static final String TAG = InvocationSerializator.class.getName();
    private Context mContext;
    private List<Method> mApiMethods;
    private E mClientMockInstance;

    @SuppressWarnings("unchecked")
    public InvocationSerializator(Context context, Class<E> targetClass) {
        mContext = context;
        mApiMethods = Arrays.asList(targetClass.getMethods());
        mClientMockInstance = (E) Proxy.newProxyInstance(targetClass.getClassLoader(),
                                                         new Class[] { targetClass }, this);
    }

    /**
     * 
     * @return stub implementation of the generic parameter, all calls to which
     *         will be serialized and broadcast
     */
    public E getMockInstance() {
        return mClientMockInstance;
    }

    protected abstract String getIntentAction();

    public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {
        Log.v(TAG, Logging.getCurrentMethodName());
        if (!mApiMethods.contains(method)) {
            Log.w(TAG, "method " + method.toString() + " named " + method.getName()
                    + " is not from api, invoking mock");
            return method.invoke(this, args);
        }
        Intent intent = new Intent(getIntentAction());
        intent.putExtra(CommunicationProtocol.METHOD, method.getName());
        Log.i(TAG, Logging.getCurrentMethodName() + " method name: " + method.getName());
        if (args != null) {
            JSONArray argsJsonArr = new JSONArray();
            for (int i = 0; i < args.length; i++) {
                argsJsonArr.put(args[i]);
                Log.i(TAG, Logging.getCurrentMethodName() + " args[" + i + "] = " + args[i]);
            }
            JSONObject obj = new JSONObject();
            obj.put(CommunicationProtocol.ARGS, argsJsonArr);
            Log.i(TAG, Logging.getCurrentMethodName() + " obj: " + obj.toString());
            intent.putExtra(CommunicationProtocol.ARGS, obj.toString());
        }
        if (mContext != null) {
            mContext.sendBroadcast(intent);
        }
        return null;
    }

    public void onContextDestroyed() {
        mContext = null;
    }

}
