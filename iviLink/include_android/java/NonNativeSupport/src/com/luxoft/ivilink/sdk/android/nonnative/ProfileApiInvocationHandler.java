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
package com.luxoft.ivilink.sdk.android.nonnative;

import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;
import java.util.Arrays;
import java.util.List;

import android.util.Log;

import com.luxoft.ivilink.sdk.android.lib.utils.log.BigLog;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

class ProfileApiInvocationHandler implements InvocationHandler {
    private static final String TAG = ProfileApiInvocationHandler.class.toString();
    private static final char LOG_HIGHLIGH = '!';
    private final String mProfileApiUid;
    private final String mServiceUid;
    private final List<Method> mProfileApiMethodsList;

    ProfileApiInvocationHandler(String profileAPI, String serviceUID, Method[] profileApiMethods) {
        Log.v(TAG, Logging.getCurrentMethodName(profileAPI, serviceUID, profileApiMethods));
        mProfileApiUid = profileAPI;
        mServiceUid = serviceUID;
        mProfileApiMethodsList = Arrays.asList(profileApiMethods);
    }

    @Override
    public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {
        BigLog.i(TAG, "Invoke: " + method.toString() + " arguments: " + args, LOG_HIGHLIGH);
        if (!mProfileApiMethodsList.contains(method)) {
            BigLog.e(TAG, "The method is not in profile API: " + method.toString(), LOG_HIGHLIGH);
            return null;
        }
        Object result = null;
        Object profileInstance = ProfileGetter.getProfile(mServiceUid, mProfileApiUid);
        if (profileInstance != null) {
            try {
                result = method.invoke(profileInstance, args);
            } catch (Exception e) {
                e.printStackTrace();
                BigLog.e(TAG, "Error invoking method: " + e.getMessage(), LOG_HIGHLIGH);
            }
            ProfileGetter.releaseProfile(mServiceUid, mProfileApiUid);
        }
        return result;
    }
}
