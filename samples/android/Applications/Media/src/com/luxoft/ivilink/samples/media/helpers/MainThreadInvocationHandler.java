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

import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;

import android.os.Handler;
import android.os.Looper;

/**
 * If a dynamic proxy is created with this InvocationHandler, all calls to its
 * API will be posted to the UI thread, but because of that null (default)
 * values will always be returned - so it may cause incorrect behavior if used
 * with getter methods.
 */
public class MainThreadInvocationHandler implements InvocationHandler {
    private Handler mHandler = new Handler(Looper.getMainLooper());

    private Object mTarget;

    /**
     * @param target
     *            reference to an object all method calls will be passed to.
     */
    public MainThreadInvocationHandler(Object target) {
        mTarget = target;
    }

    public Object invoke(Object proxy, final Method method, final Object[] args) throws Throwable {
        if (!Looper.getMainLooper().equals(Looper.myLooper())) {
            mHandler.post(new Runnable() {
                public void run() {
                    callMethod(method, args);
                }
            });
        } else {
            callMethod(method, args);
        }
        return null;
    }

    private void callMethod(Method method, Object[] args) {
        try {
            method.invoke(mTarget, args);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
