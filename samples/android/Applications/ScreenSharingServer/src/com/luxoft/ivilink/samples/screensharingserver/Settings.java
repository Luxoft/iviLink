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
package com.luxoft.ivilink.samples.screensharingserver;

public class Settings {
	public static boolean startedByUser = true;
	
	public static final String VNC_SERVER_LAUNCHED = "ScreenSharing Server launched!";
	public static final String EXIT = "exit";
	
	public static final String VNC_MSG_EXIT = "0";
	public static final String VNC_MSG_START = "1";
	public static final String VNC_MSG_SERVER_ADDRESS = "2";
	public static final String VNC_MSG_START_ACK = "3";

	public static final String EXIT_INTENT_TAG = "exit intent";
	
	public static final String LAUNCH_INFO = "launchInfo";
	public static final String INTERNAL_PATH = "internalPath";
	public static final String SHOW_START_PROGRESS = "showStartProgress";
	public static final String SHOW_VNC_DIALOG = "showVncDialog";
	public static final String VNC_DIALOG_OK = "vncDialogOk";
	public static final String VNC_DIALOG_CANCEL = "vncDialogCancel";
	public static final String DISMISS_PROGRESS = "dismissProgress";
	
	public static final String INTENT_TAG = "com.luxoft.ivilink.samples.screensharingserver";
}
