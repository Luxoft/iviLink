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

import java.util.LinkedList;
import java.util.List;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

import android.util.Log;

/**
 * Structure that stores information about an application to be passed to the
 * ApplicationManager on request. Its contents are: Package name, an Activity
 * class name (it must be the entry Activity of an application, i.e. the
 * Activity that is registered in the AndroidManifest.xml as the receiver of the
 * <code>android.intent.action.MAIN</code> with the category
 * <code>android.intent.category.LAUNCHER</code>), and list of services this
 * application supports (so that when on the other side a service is loaded,
 * this application will be loaded in response).
 * 
 */
public class ApplicationInfo {
    private final static String TAG = ApplicationInfo.class.getName() + " intent";
    private final static String PACKAGE_NAME_TAG = "package name";
    private final static String ACTIVITY_NAME_TAG = "activity name";
    private final static String SERVICES_TAG = "services";
    String mPackageName;
    String mActivityName;
    List<String> mServices;

    public String getPackageName() {
        return mPackageName;
    }

    public String getActivityName() {
        return mActivityName;
    }

    public List<String> getSupportedServices() {
        return mServices;
    }

    /**
     * Check if this instance is filled with data.
     * 
     * @return <code>true</code> if all the fields (activity name, package name
     *         and the list of supported services) are not <code>null</code> and
     *         not empty, <code>false</code> otherwise.
     */
    public boolean isFilled() {
        boolean isValid = getPackageName() != null && getSupportedServices() != null
                && getActivityName() != null;
        if (isValid) {
            isValid = getPackageName().length() != 0 && getActivityName().length() != 0
                    && getSupportedServices().size() != 0;
        }
        return isValid;
    }

    public ApplicationInfo(String packageName, String activityName, List<String> services) {
        mPackageName = packageName;
        mActivityName = activityName;
        mServices = services;
        Log.v(TAG, "created an " + this.getClass().getSimpleName() + " :" + this.toString());
    }

    @Override
    public String toString() {
        return serialize();
    }

    public String serialize() {
        if (!isFilled()) {
            return null;
        }
        try {
            JSONObject object = new JSONObject();
            object.put(PACKAGE_NAME_TAG, mPackageName);
            object.put(ACTIVITY_NAME_TAG, mActivityName);
            object.put(SERVICES_TAG, new JSONArray(mServices));
            return object.toString();
        } catch (JSONException e) {
            // this will never happen actually
            e.printStackTrace();
            return null;
        }
    }

    public static ApplicationInfo fromString(String source) {
        Log.v(TAG, Logging.getCurrentMethodName(source));
        try {
            JSONObject object = new JSONObject(source);
            String activityName = object.getString(ACTIVITY_NAME_TAG);
            String packageName = object.getString(PACKAGE_NAME_TAG);
            JSONArray servicesJson = object.getJSONArray(SERVICES_TAG);
            List<String> services = new LinkedList<String>();
            for (int i = 0; i < servicesJson.length(); i++) {
                try {
                    services.add(servicesJson.getString(i));
                } catch (JSONException e) {
                    e.printStackTrace();
                }
            }
            if (services.size() != 0) {
                return new ApplicationInfo(packageName, activityName, services);
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
        return null;
    }
}
