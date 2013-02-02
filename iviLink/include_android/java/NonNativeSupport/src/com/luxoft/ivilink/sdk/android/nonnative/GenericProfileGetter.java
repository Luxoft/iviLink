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

import com.luxoft.ivilink.sdk.android.lib.utils.Assertion;

import android.util.Log;

/**
 * Generic class used to obtain an instance of profile implementation from the
 * Service Manager using specified profile API UID and service UID. Performs
 * type-checking of the retrieved profile instance. Retrieved reference to a
 * profile implementation instance should not be stored. The profile
 * implementation instance must be released after using (call releaseProfile()
 * after successful getProfile())
 * 
 * @param <ProfileAPI>
 *            interface the profile should implement.
 */
public final class GenericProfileGetter<ProfileAPI> {
	String profileAPI;
	String serviceUID;
	Class<?> profileApiInterfaceClass;

	/**
	 * Constructor.
	 * 
	 * @param profileAPI
	 *            Profile API UID
	 * @param serviceUID
	 *            Service UID
	 * @param profileApiClass
	 *            Instance of the ProfileAPI Class object (because type erasure
	 *            obstructs run-time generic type checking)
	 */
	public GenericProfileGetter(String profileAPI, String serviceUID,
			Class<ProfileAPI> profileApiClass) {
		Assertion.check(profileApiClass.isInterface());
		this.profileApiInterfaceClass = profileApiClass;
		this.profileAPI = profileAPI;
		this.serviceUID = serviceUID;
	}

	/**
	 * Gets an implementation of the ProfileAPI from the Service Manager.
	 * 
	 * @return Instance of an object implementing ProfileAPI
	 * @throws ProfileNotFoundException
	 *             When instance could not be retrieved.
	 */
	@SuppressWarnings({ "unchecked", "rawtypes" })
	public ProfileAPI getProfile() throws ProfileNotFoundException {
		Object obj = ProfileGetter.getProfile(serviceUID, profileAPI);
		if (obj == null) {
			Log.e(GenericProfileGetter.class.getName(), "no profile instance");
			throw new ProfileNotFoundException("could not obtain profile instance");
		}
		if (!profileApiInterfaceClass.isAssignableFrom(obj.getClass())) {
			Log.e(GenericProfileGetter.class.getName(), obj.getClass().getName()
					+ " does not implement the required interface, list of interfaces: ");
			Class[] interfaces = obj.getClass().getInterfaces();
			for (Class claz : interfaces) {
				Log.v(GenericProfileGetter.class.getName(), "interface: " + claz.getName());
			}
			throw new ProfileNotFoundException(obj.getClass().getName()
					+ " does not implement the required interface");
		} else {
			return ((ProfileAPI) obj);
		}
	}

	/**
	 * Releases an implementation of the ProfileAPI (should be called after
	 * successful getProfile())
	 */
	public void releaseProfile() {
		ProfileGetter.releaseProfile(serviceUID, profileAPI);
	}
}
