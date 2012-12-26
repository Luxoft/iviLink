/* 
 * iviLINK SDK, version 1.1.19
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012, Luxoft Professional Corp., member of IBS group
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

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.util.Log;
import android.widget.Toast;

import com.luxoft.ivilink.sdk.android.lib.utils.Assertion;
import com.luxoft.ivilink.sdk.android.lib.utils.ForApp;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

/**
 * This class represents an application in iviLink.
 * 
 */
public final class CApp {

	public static String internalPath;

	private final String tag = CApp.class.getName();

	private static boolean hasBeenCreated = false;

	static boolean isServiceManagerCreated() {
		return hasBeenCreated;
	}

	ApplicationStateCallbacks appCallbacks;

	static {
		System.loadLibrary("AndroidCommonLibNative");
	}

	private native void createAppNative(String serviceRepoPath, String internalPath,
			String launchInfo, List<String> serviceUIDs, Class<ProfileCreator> profileCreatorClass);

	private native void useServiceNative(String serviceUID, boolean enable);

	private native void registerProfileCallbacksNative(String profileApi, Object callbacks);

	private native void initInIviLinkNative();

	private native boolean getLaunchInfoNative();

	private native boolean loadServiceNative(String serviceUID);

	private native void unloadServiceNative(String serviceUID);

	private native int getActiveServicesListSizeNative();

	private native String getActiveServiceByListPositionNative(int position);

	private native boolean isServiceActiveNative(String serviceUID);

	private native boolean isLinkAliveNative();

	/**
	 * Constructor. Creates native CAppImplementation instance.
	 * 
	 * @param activity
	 *            Should be the entry point of the application
	 */
	public CApp(Activity activity) {
		this(activity, (List<String>) null);
		Log.i(tag, Logging.getCurrentMethodName(activity));
	}

	/**
	 * Constructor. Creates native CAppImplementation instance with the
	 * specified service enabled.
	 * 
	 * @param activity
	 *            Should be the entry point of the application
	 * @param serviceUID
	 *            Service name
	 */
	public CApp(Activity activity, String serviceUID) {
		this(activity, Arrays.asList(serviceUID));
		Log.v(tag, Logging.getCurrentMethodName(serviceUID));
	}

	/**
	 * Constructor. Creates native CAppImplementation instance with the
	 * specified services enabled.
	 * 
	 * @param activity
	 *            Should be the entry point of the application
	 * @param serviceUIDs
	 *            List of service names
	 */
	public CApp(final Activity activity, List<String> serviceUIDs) {
		Log.v(tag, Logging.getCurrentMethodName(serviceUIDs));
		internalPath = ForApp.getInternalPath(activity);
		if (!checkIfActivityIsAnEntryOne(activity)) {
			Log.e(tag, "provided Activity is not among application's entry activities,"
					+ " iviLink may not work correctly");
			activity.runOnUiThread(new Runnable() {
				public void run() {
					Toast.makeText(
							activity,
							"Warning: "
									+ "provided Activity is not among application's entry activities,"
									+ " iviLink may not work correctly!", Toast.LENGTH_LONG).show();
				}
			});
		}

		createAppNative(ForApp.getServicePath(), ForApp.getInternalPath(activity),
				ForApp.getLaunchInfo(activity), serviceUIDs, ProfileCreator.class);
		hasBeenCreated = true;
	}

	/**
	 * Initializes native CAppImplementation in iviLink (after that it will
	 * start receiving callbacks)
	 * 
	 * @param callbacks
	 *            Receiver of iviLink events (such as incoming service and
	 *            others)
	 */
	public void initInIVILink(ApplicationStateCallbacks callbacks) {
		Log.v(tag, Logging.getCurrentMethodName(callbacks));
		appCallbacks = callbacks;
		Assertion.check(appCallbacks != null, "ApplicationStateCallbacks provided are null");
		initInIviLinkNative();
	}

	/**
	 * Enables/disables a service.
	 * 
	 * @param serviceUID
	 *            Service to change enabled state
	 * @param enable
	 *            Enable/Disable
	 */
	public void setEnabled(String serviceUID, boolean enable) {
		Log.v(tag, Logging.getCurrentMethodName(serviceUID, enable));
		useServiceNative(serviceUID, enable);
	}

	/**
	 * Enables/disables list of services.
	 * 
	 * @param serviceUIDs
	 *            Services to change enabled state
	 * @param enable
	 *            Enable/Disable
	 */
	public void setEnabled(List<String> serviceUIDs, boolean enable) {
		Log.v(tag, Logging.getCurrentMethodName(serviceUIDs, enable));
		for (String uid : serviceUIDs) {
			useServiceNative(uid, enable);
		}
	}

	/**
	 * Checks if this Application was started by user or by iviLink's
	 * Application Manager
	 * 
	 * @return true if the application was started by user; false otherwise
	 */
	public boolean wasStartedByUser() {
		Log.v(tag, Logging.getCurrentMethodName());
		return getLaunchInfoNative();
	}

	/**
	 * Registers an object to receive profile callbacks from a profile
	 * implementation of the specified API. Type-check is not performed. It is
	 * advised to call this method either in initDone or in
	 * incomingServiceBeforeLoading. If no profile callbacks have been
	 * registered, profile creation (part of loading a service) will fail.
	 * 
	 * @param profileApiUid
	 *            Profile API UID
	 * @param callbacks
	 */
	public void registerProfileCallbacks(String profileApiUid, Object callbacks) {
		Log.v(tag, Logging.getCurrentMethodName(profileApiUid, callbacks));
		registerProfileCallbacksNative(profileApiUid, callbacks);
	}

	/**
	 * Loads the service specified.
	 * 
	 * @param serviceUID
	 *            Name of the service to load
	 * @return true in case of success (after that profiles for this service are
	 *         accessible and can be used)
	 */
	public boolean loadService(String serviceUID) {
		Log.v(tag, Logging.getCurrentMethodName(serviceUID));
		return loadServiceNative(serviceUID);
	}

	/**
	 * Unloads the service specified.
	 * 
	 * @param serviceUID
	 *            Name of the service to unload
	 */
	public void unloadService(String serviceUID) {
		Log.v(tag, Logging.getCurrentMethodName(serviceUID));
		unloadServiceNative(serviceUID);
	}

	/**
	 * Gets list of active services
	 * 
	 * @return List of active services
	 */
	public List<String> getActiveServices() {
		Log.v(tag, Logging.getCurrentMethodName());
		List<String> result = new ArrayList<String>();
		int size = getActiveServicesListSizeNative();
		Log.i(tag, "size = " + size);
		for (int i = 0; i < size; i++) {
			result.add(getActiveServiceByListPositionNative(i));
			Log.i(tag, "active service [" + i + "] = " + result.get(i));
		}
		return result;
	}

	/**
	 * Checks if a service is active (loaded and its profiles can be used)
	 * 
	 * @return List of active services
	 */
	public boolean isActive(String serviceUID) {
		Log.v(tag, Logging.getCurrentMethodName(serviceUID));
		return isServiceActiveNative(serviceUID);
	}

	/**
	 * Check if this side's iviLink is connected
	 * 
	 * @return <code>true</code> if a connection is established,
	 *         <code>false</code> otherwise
	 */
	public boolean isLinkAlive() {
		return isLinkAliveNative();
	}

	private boolean checkIfActivityIsAnEntryOne(Activity activity) {
		Log.v(tag, Logging.getCurrentMethodName(activity));
		Intent i = new Intent(Intent.ACTION_MAIN);
		i.addCategory(Intent.CATEGORY_LAUNCHER);
		i.setPackage(activity.getApplicationContext().getPackageName());
		PackageManager pm = activity.getPackageManager();
		List<ResolveInfo> resolveList = pm.queryIntentActivities(i, 0);
		for (ResolveInfo resolveInfo : resolveList) {
			Log.i(tag, "activity info: " + resolveInfo.activityInfo.name);
			Log.i(tag, "provided activity name: " + activity.getClass().getName());
			if (resolveInfo.activityInfo.name.equals(activity.getClass().getName())) {
				return true;
			}
		}
		return false;
	}

	private void initDone(final boolean isStartedByUser) {
		Log.v(tag, Logging.getCurrentMethodName(isStartedByUser));
		appCallbacks.onInitDone(isStartedByUser);
	}

	private void incomingServiceBeforeLoading(final String serviceUID) {
		Log.v(tag, Logging.getCurrentMethodName(serviceUID));
		appCallbacks.onIncomingServiceBeforeLoading(serviceUID);
	}

	private void incomingServiceAfterLoading(final String serviceUID) {
		Log.v(tag, Logging.getCurrentMethodName(serviceUID));
		appCallbacks.onIncomingServiceAfterLoading(serviceUID);
	}

	private void serviceLoadError(final String serviceUID) {
		Log.v(tag, Logging.getCurrentMethodName(serviceUID));
		appCallbacks.onServiceLoadError(serviceUID);
	}

	private void serviceDropped(final String serviceUID) {
		Log.v(tag, Logging.getCurrentMethodName(serviceUID));
		appCallbacks.onServiceDropped(serviceUID);
	}

	private void connectionLost() {
		Log.v(tag, Logging.getCurrentMethodName());
		appCallbacks.onPhysicalLinkDown();
	}

	private void connectionEstablished() {
		Log.v(tag, Logging.getCurrentMethodName());
		appCallbacks.onLinkEstablished();
	}

}
