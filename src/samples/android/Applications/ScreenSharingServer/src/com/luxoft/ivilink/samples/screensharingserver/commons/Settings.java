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
 
package com.luxoft.ivilink.samples.screensharingserver.commons;

public class Settings {
	public static boolean startedByUser = true;
	
	public static String currentActivity;
	public static boolean isMuted=false;
	
	public static String remoteIp;
	public static String myIp;
	public static final int vncCommPort = 5253;
	public static final int vncServPort = 5905;
	public static final String tVNCLaunch = "VNC client is ready!";
	public static final String tVNCServerLaunch = "ScreenSharing Server launched!";
	public static final String exit = "exit";
	
	public static final String vncMsgExit = "0";
	public static final String vncMsgStart = "1";
	public static final String vncMsgServerAddress = "2";
	public static final String vncMsgStartAck = "3";

	public static final String fVncConfig="/sdcard/demo/vnc.txt";

	public static final String ifTagVnc="Vnc Service";
	//intent filters
	public static final String ifGlobal="Global Signal";
	public static final String ifFromAnyActivity="message from whichever activity";
	public static final String ifShareButton="commands to switch share button states";
	
	//intent tags
	public static final String tStatus="connection status tag";
	public static final String tMessage="packed message";
	public static final String tChannel="channel";
	public static final String tMsgBxId="message id";
	public static final String tVncCommand="vnc command";

	public static final byte cVnc=(byte)4;

	public static final int idKillAll=4;	
}
