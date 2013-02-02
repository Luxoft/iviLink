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


package com.luxoft.ivilink.sdk.helpers;

/*
 * contains String constants
 */
public class Common {
	public static String serviceBR;

	public static final String PASSWORD = "password";

	public static final String misc = "misc";
	public static final String reset = "reset";
	public static final String idle = "ok";
	public static final String shutdown = "shutdown all"; // for internal use

	/* CSystemController's class name */
	public static String serviceClassName = null;

	// protocol between SystemControllerService and LauncherActivity
	// intent extra tags
	public static final String message = "message";
	public static final String progressValue = "progress";
	// intent filter
	public static final String ifProgress = "show progress dialog";
	// messages
	public static final String doneLaunch = "All done! iviLink is ready!";
	public static final String resetDialog = "show dialog";

	// TAG
	public static final String TAG = "iviLink.SystemController.";
}
