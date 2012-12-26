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
 
package com.luxoft.ivilink.samples.screensharingserver.service;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.IBinder;

abstract public class ServiceParent extends Service{
	
	DieBR die=new DieBR();

	class DieBR extends BroadcastReceiver{
		@Override
		public void onReceive(Context context, Intent intent) {
			stopSelf();
			android.os.Process.killProcess(android.os.Process.myPid());
		}
	}
	
	@Override
	public void onCreate(){
		super.onCreate();
		registerReceiver(die, new IntentFilter("die"));
	}
	
	@Override
	public void onDestroy(){
		super.onDestroy();
		unregisterReceiver(die);
	}
	
	@Override
	public int onStartCommand(Intent intent, int startID, int flags){
		return START_NOT_STICKY;
	}

}
