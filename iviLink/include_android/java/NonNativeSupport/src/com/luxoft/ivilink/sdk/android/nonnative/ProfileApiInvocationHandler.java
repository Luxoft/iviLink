package com.luxoft.ivilink.sdk.android.nonnative;

import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;
import java.util.Arrays;
import java.util.List;

import android.util.Log;

import com.luxoft.ivilink.utils.log.BigLog;
import com.luxoft.ivilink.utils.log.Logging;

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
