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



package com.luxoft.ivilink.sdk;
import android.util.Log;
import com.luxoft.ivilink.sdk.helpers.Common;
/*
 * Encapsulates Connectivity Agent
 */
public class CConnectivityAgentWrapper {
	
	Thread mThread;
	
	public void start(){
		(mThread = new Thread(new Runnable(){
			public void run(){
				startCA();
				Log.e(Common.TAG+".ConnectivityAgent", "has died!");
			}
		})).start();
	}

	//code is taken from ConnectivityAgent/src/linux/main.cpp
	private native void startCA();
}
