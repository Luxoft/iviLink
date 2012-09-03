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



package com.luxoft.ivilink.sdk.helpers;
/*
 * contains String constants
 */
public class Common {
	public static String serviceBR; // = "launch"; //TODO generate this on start
	/* constants for broadcast receiver - see CSystemController.java */
	public static final String component = "component";
	public static final String componentNameProfMan = "profilemanager";
	public static final String componentNameAppMan = "appman";
	public static final String componentNameProfRepo = "profilerepo";
	public static final String componentNameConnAgnt = "connectivityagent";
	public static final String componentNameChanSuperv = "channelsupervisor";
	
	public static final String misc = "misc";
	public static final String reset = "reset";
	public static final String idle = "ok";
	public static final String shutdown = "shutdown all"; // for internal use
	
	
	/*CSystemController's class name*/
	public static String serviceClassName = null;
	
	// protocol between SystemControllerService and LauncherActivity
	public static final String ifProgress = "show progress dialog";
	public static final String message = "message";
	public static final String conAgntLaunch = "Started ConnectivityAgent...";
	public static final String negLaunch = "Started Negotiator...";
	public static final String profmanLaunch = "Started Profile Manager...";
	public static final String profrepoLaunch = "Started Profile Repository...";
	public static final String appmanLaunch = "Started Application Manager...";
	public static final String authLaunch = "Started Authentication...";
	public static final String doneLaunch = "All done! iviLink is ready!";	
	public static final String resetDialog = "show dialog";
	
	//TAG
	public static final String TAG = "iviLink.SystemController.";
}
