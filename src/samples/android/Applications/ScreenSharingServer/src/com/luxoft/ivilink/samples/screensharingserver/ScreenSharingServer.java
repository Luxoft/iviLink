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
 
package com.luxoft.ivilink.samples.screensharingserver;

import android.app.ActivityManager;
import android.app.ActivityManager.RunningServiceInfo;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.res.Configuration;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;

import com.luxoft.ivilink.samples.screensharingserver.commons.Settings;
import com.luxoft.ivilink.samples.screensharingserver.service.VncServerService;
import com.luxoft.ivilink.samples.screensharingserver.R;
import com.luxoft.ivilink.samples.screensharingserver.ScreenSharingServer;
import com.luxoft.ivilink.samples.screensharingserver.ScreenSharingServer.VncButtonListener;
import com.luxoft.ivilink.samples.splash.Splash;
import com.luxoft.ivilink.samples.splash.Splash.Length;
import com.luxoft.ivilink.sdk.android.lib.utils.ForApp;

public class ScreenSharingServer extends ActivityParent {
	Handler poster = new Handler();
	
	MsgBoxBroadcastReceiver msgBR=new MsgBoxBroadcastReceiver();

    VncButtonListener mvbl = new VncButtonListener();
    
    Dialog vncDialog;
    ProgressDialog progress;
	
	BridgeC bridge;
	
	@Override
	public void onCreate(Bundle icicle){
		super.onCreate(icicle);
		ForApp.setWindowFlagsToKeepDeviceOn(this);
		
		if(getIntent().getBooleanExtra(Settings.exit, false) ) {
			Log.i("ScreenSharingServer", "ScreenSharingServer::onCreate : Settings.exit = TRUE, EXIT");
			android.os.Process.killProcess(android.os.Process.myPid() );
			return;
		}
		
		
		Splash.displaySplashScreen(this, new Runnable(){
			public void run(){
				tag = Settings.ifFromAnyActivity;
				registerReceiver(msgBR, new IntentFilter(tag));
				progress = new ProgressDialog(ScreenSharingServer.this, ProgressDialog.STYLE_SPINNER);
				progress.show();
				progress.setCancelable(false);
		
				progress.setMessage("Launching ScreenSharingServer");
			
				vncDialog = new Dialog(ScreenSharingServer.this);
			
				bridge = BridgeC.getInstance(ScreenSharingServer.this, new Handler());
				bridge.startApp();
			}
		}, Length.SHORT);
	}
	
	@Override
	public void onPause(){
		super.onPause();
		finish();
	}
	
	@Override
	public void onDestroy(){
		super.onDestroy();
		unregisterReceiver(msgBR);
	}
	
	public void launchServices(){
		if(!isMyServiceRunning("com.luxoft.ivilink.samples.screensharingserver.service.VncServerService")) {
			Log.i(tag, "VncService not started, starting");
			
			startService(new Intent(mCntx, VncServerService.class));
			
		}
	}
	
	private boolean isMyServiceRunning(String serviceName) {
        ActivityManager manager = (ActivityManager) getSystemService(ACTIVITY_SERVICE);
        for (RunningServiceInfo rsi : manager.getRunningServices(Integer.MAX_VALUE)) {
            if (serviceName.equals(rsi.service.getClassName())) {
                return true;
            }
        }
        return false;
    }
	
	class MsgBoxBroadcastReceiver extends BroadcastReceiver{
		@Override
		public void onReceive(Context arg0, Intent arg1) {
			if(arg1.getBooleanExtra("start smth", false)){
				return;
			}
			int msgId;
			if((msgId=arg1.getIntExtra(Settings.tMsgBxId, Integer.MAX_VALUE))!=Integer.MAX_VALUE){
				switch(msgId){
				case Settings.idKillAll:
					new Handler().postDelayed(new Runnable(){
						@Override
						public void run() {
							sendBroadcast(new Intent("global").putExtra("finish", true));
							sendBroadcast(new Intent("die"));
						}
					}, 5000);
					break;
				}
			}
		}
	}
	
	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);
		//TODO ???
	}	
	
	public Dialog createVncDialog(){
		Dialog dialog = new Dialog(ScreenSharingServer.this);
		dialog.setContentView(R.layout.vncdialog);
		dialog.setTitle("Please confirm");
		TextView txt = (TextView)dialog.findViewById(R.id.vncDialogText);
		txt.setText("Start screen sharing session as a server?\n");
		Button yes = (Button)dialog.findViewById(R.id.vncOk);
		yes.setOnClickListener(mvbl);
		Button no = (Button)dialog.findViewById(R.id.vncCancel);
		no.setOnClickListener(this.mvbl);
		return dialog;
	}
	
	public void startVncDialog() {
		if(vncDialog!=null) {
			if(vncDialog.isShowing() ) {
				Log.i("ScreenSharingServer", "Duplicate vnc requests!");
				return;
			}
		}
		vncDialog = createVncDialog();
		vncDialog.show();
	}
	
	class VncButtonListener implements OnClickListener {
		@Override
		public void onClick(View v) {
			switch(v.getId()){
			case R.id.vncOk:
				Log.i("ScreenSharingServer", "VNC dialog : OK");
				launchServices();
				progress.dismiss();
				vncDialog.dismiss();
				
				new Handler().postDelayed(new Runnable() {
					@Override
					public void run() {
						bridge.sendData(Settings.vncMsgStartAck);
					}
        		}, 4000);
				
				break;
			case R.id.vncCancel:
				Log.i("ScreenSharingServer", "VNC dialog : CANCEL");
				bridge.sendData(Settings.vncMsgExit);
				vncDialog.dismiss();
				android.os.Process.killProcess(android.os.Process.myPid() );
				break;
			}
		}		
	}
}
