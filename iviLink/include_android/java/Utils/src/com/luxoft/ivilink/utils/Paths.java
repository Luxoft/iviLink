/* 
 * 
 * iviLINK SDK, version 1.1.2
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
 * 
 */

package com.luxoft.ivilink.utils;

import android.os.Environment;

public final class Paths {

	private static final String externalPath = Environment.getExternalStorageDirectory()
			.getAbsolutePath();

	private static final String app_database = "/application_manager_database/";
	private static final String profile_repository = "/profile_repository/";
	private static final String service_repository = "/service_repository/";

	public static String getAppDatabase() {
		return externalPath + app_database;
	}

	public static String getProfileRepository() {
		return externalPath + profile_repository;
	}

	public static String getServiceRepository() {
		return externalPath + service_repository;
	}
}
