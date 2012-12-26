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

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;
import java.nio.ByteBuffer;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import android.app.Instrumentation;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Handler;
import android.os.IBinder;
import android.util.Log;
import android.widget.Toast;

import com.luxoft.ivilink.samples.screensharingserver.R;
import com.luxoft.ivilink.samples.screensharingserver.ScreenSharingServer;
import com.luxoft.ivilink.samples.screensharingserver.commons.JsonStrings;
import com.luxoft.ivilink.samples.screensharingserver.commons.Settings;
import com.luxoft.ivilink.samples.screensharingserver.NotificationHandler;


public class VncServerService extends ServiceParent {
	Instrumentation instr;
	String device=null;
	
	CommandsBR cbr=new CommandsBR();
	
	Handler notificationHandler;
	
    public enum VncStatus{
    	NO,
    	SERVER
    }
    
    int maxXcoords, minXcoords, maxYcoords, minYcoords;
    int xResolution, yResolution;
    
    public static int servCounter=0;
    
    public static VncStatus isVnc=VncStatus.NO;
	
	boolean hasServerStarted=false;
	
	volatile Socket sock=null;
	volatile ServerSocket sockHost=null;
	
	boolean threadStop=false;
	
	Process startShell;
	final int portno = Settings.vncCommPort;
	
	Handler poster=new Handler();
	
	void getVncParams(){
		File f=new File(Settings.fVncConfig);
		Log.i("VncService", "trying to parce config from: "+f.getAbsolutePath());
		try {
			BufferedReader buf=new BufferedReader(new InputStreamReader(new FileInputStream(f)));
			String json=buf.readLine();
			JSONObject obj=new JSONObject(json);
			maxXcoords=obj.getInt("max x");
			minXcoords=obj.getInt("min x");
			maxYcoords=obj.getInt("max y");
			minYcoords=obj.getInt("min y");
			xResolution=obj.getInt("res x");
			yResolution=obj.getInt("res y");
			try{
				device=obj.getString("device");
			}catch(JSONException jex){
				device=null;
			}
		} catch (Exception e) {
			e.printStackTrace();
			maxXcoords=480;
			minXcoords=0;
			maxYcoords=272;
			minYcoords=0;
			xResolution=800;
			yResolution=480;
			device=null;
		}
	}
	
	public boolean hasRootPermission() {
		boolean rooted = true;
		try {
			File su = new File("/system/bin/su");
			if (su.exists() == false) {
				su = new File("/system/xbin/su");
				if (su.exists() == false) {
					rooted = false;
				}
			}
		} catch (Exception e) {
			Log.e("VNC", "Can't obtain root - Here is what I know: "+e.getMessage());
			rooted = false;
		}
		return rooted;
	}
	
	public boolean isAndroidServerRunning() {
		return sock!=null;
	}
	
	class StreamCleaner implements Runnable{
		InputStream in;
		String tag;
		BufferedReader br;
		public StreamCleaner(InputStream in, String tag) {
			this.in=in;
			this.tag=tag;
			br=new BufferedReader(new InputStreamReader(in));
		}
		@Override
		public void run() {
			String readLine=null;
			try {
				while((readLine=br.readLine())!=null){
					Log.i(tag, readLine);
				}
			} catch (IOException e) {
				Log.i(tag, "pipe closed");
				return;
			}
		}
	}
	
	class SuLauncher implements Runnable{
		Process proc=null;
		public void run(){
			try {
				proc=Runtime.getRuntime().exec("su");
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}


	@Override
	public IBinder onBind(Intent arg0) {
		return null;
	}
	
	private void installServer(){
		uninstallServer();
		InputStream ins = getResources().openRawResource(R.raw.androidvncserver);
		int size;
		Log.i("VncServer", "trying to install server");
		try {
			size = ins.available();
			byte[] buffer = new byte[size];
			ins.read(buffer);
			ins.close();
			FileOutputStream fos = new FileOutputStream(getFilesDir().getAbsolutePath() + "/androidvncserver");
			fos.write(buffer);
			fos.close();
			Log.i("VncServer", "install successful");
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	private void uninstallServer(){
		File f=new File(getFilesDir().getAbsolutePath() + "/androidvncserver");
		Log.i("VncServer", "attempting to uninstall server");
		if(f.exists()){
			f.delete();
			Log.i("VncServer", "uninstall successful");
		}
	}
	
	@Override
	public int onStartCommand(Intent i, int arg1, int arg2){
		notificationHandler = new Handler();
		getVncParams();
		IntentFilter ifilter=new IntentFilter(Settings.ifTagVnc);
		registerReceiver(cbr, ifilter);
		installServer();
		
		JSONObject obj = new JSONObject();
		try {
			obj.put(JsonStrings.vnc, JsonStrings.server);
		} catch (JSONException e2) {}
		
		sendBroadcast(new Intent(Settings.ifTagVnc).putExtra(Settings.tMessage, obj.toString().getBytes()));
		
		return super.onStartCommand(i, arg1, arg2);
	}
	
	//TODO test
	private int injectRelease(int x, int y, OutputStream os){
		//Log.v("VncTest","injecting press:" +x+ " "+y);
		//int[] coords=transformCoordinates(x,y);
		Log.i("VncTest", "injecting release event");
		/*
		String com1="sendevent "+device+" 3 48 0"; //some press event?
		String com2="sendevent "+device+" 3 50 4"; //???
		String com3="sendevent "+device+" 3 53 "+(maxYcoords-coords[1]); //y coordinate dafuk
		String com4="sendevent "+device+" 3 54 "+coords[0]; //x coordinate
		String com5="sendevent "+device+" 0 2 0"; //end marker
		String com6="sendevent "+device+" 0 0 0"; //end marker
		*/
		String com1="sendevent "+device+" 3 24 0";
		String com2="sendevent "+device+" 1 330 0";
		String com3="sendevent "+device+" 3 58 0";
		String com4="sendevent "+device+" 3 48 0";
		String com5="sendevent "+device+" 0 2 0";
		String com6="sendevent "+device+" 0 0 0";
		try{
			writeCommand(os, com1);
			writeCommand(os, com2);
			writeCommand(os, com3);
			writeCommand(os, com4);
			writeCommand(os, com5);
			writeCommand(os, com6);
			os.flush();	
			return 1;
		}catch(Exception e){
			Log.e("VncTest", "injecting press failed!");
			return -1;
		}
	}
	
	/*
	private int[] transformCoordinates(int x, int y){
		int[] newCoords=new int[2];
		double coefX=(double)xResolution/(double)(maxXcoords-minXcoords);
		double coefY=(double)yResolution/(double)(maxYcoords-minYcoords);
		newCoords[0]=(int)Math.round((double)x/coefX);
		newCoords[1]=(int)Math.round((double)y/coefY);
		return newCoords;
	}*/

	private int[] transformCoordinates(int x, int y, int maxX, int maxY, int resX, int resY){
		int[] newCoords=new int[2];
		double coefX=(double)resX/(double)maxX;
		double coefY=(double)resY/(double)maxY;
		newCoords[0]=(int)Math.round((double)x/coefX);
		newCoords[1]=(int)Math.round((double)y/coefY);
		return newCoords;
	}
	
	//TODO test
	private int injectPress(int x, int y, OutputStream os){
		Log.i("VncTest","injecting press:" +x+ " "+y);
		/*int[] coords=transformCoordinates(x,y);
		
		Log.v("VncTest", "real x="+coords[0]+ " real y="+coords[1]);
		String com1="sendevent "+device+" 3 48 50"; //some press event?
		String com2="sendevent "+device+" 3 50 4"; //???
		String com3="sendevent "+device+" 3 53 "+(maxYcoords-coords[1]); //y coordinate
		String com4="sendevent "+device+" 3 54 "+coords[0]; //x coordinate
		String com5="sendevent "+device+" 0 2 0"; //end marker
		String com6="sendevent "+device+" 0 0 0"; //end marker
		try{
			writeCommand(os, com1);
			writeCommand(os, com2);
			writeCommand(os, com3);
			writeCommand(os, com4);
			writeCommand(os, com5);
			writeCommand(os, com6);
			return 1;
		}catch(Exception e){
			Log.v("VncTest", "injecting press failed!");
			return -1;
		}
		*/

		int[] coords=transformCoordinates(x,y, 480, 272, 800,480);		
		Log.i("VncTest", "real x="+coords[0]+ " real y="+coords[1]);

		String com1="sendevent "+device+" 3 0 "+coords[0];
		String com2="sendevent "+device+" 3 1 "+coords[1];
		String com3="sendevent "+device+" 3 24 12"; //12 is some random pressure value
		String com4="sendevent "+device+" 1 330 1";
		String com5="sendevent "+device+" 3 53 "+coords[0];
		String com6="sendevent "+device+" 3 54 "+coords[1];
		String com7="sendevent "+device+" 3 58 12"; //must be equal to com3's value
		String com8="sendevent "+device+" 3 48 12";
		String com9="sendevent "+device+" 0 2 0";
		String com10="sendevent "+device+" 0 0 0";
		try {
			writeCommand(os, com1);
			writeCommand(os, com2);
			writeCommand(os, com3);
			writeCommand(os, com4);
			writeCommand(os, com5);
			writeCommand(os, com6);
			writeCommand(os, com7);
			writeCommand(os, com8);
			writeCommand(os, com9);
			writeCommand(os, com10);

			os.flush();
			return 1;

		} catch (Exception e) {
			Log.e("VncTest", "injecting press failed!");
			return -1;
		}
	}

	private int injectClick(int x, int y, OutputStream os){
		Log.i("VncServerService", "injecting click");
		int i=injectPress(x, y, os);
		if(i==-1) return -1;
		int j=injectRelease(x, y, os);
		if(j==-1) return -1;
		return 1;
	}
	
	
	//TODO test
	public int injectMove(int x, int y, int oldX, int oldY, OutputStream os){
		Log.i("VncTest", "injecting move event");
		
		
		//return injectPress(x, y, os);
		
		
		//int[] coords=transformCoordinates(x, y);
		int[] coords=transformCoordinates(x, y, 480, 272, 800, 480);
		try {
			String com1=null;
			String com2=null;
			if(x!=oldX){
				com1="sendevent "+device+" 3 0 "+coords[0];
			}
			if(y!=oldY){
				com2="sendevent "+device+" 3 1 "+coords[1];
			}
			String com5="sendevent "+device+" 3 53 "+coords[0];
			String com6="sendevent "+device+" 3 54 "+coords[1];
			String com7="sendevent "+device+" 3 58 12";
			String com8="sendevent "+device+" 3 48 12";
			String com9="sendevent "+device+" 0 2 0";
			String com10="sendevent "+device+" 0 0 0";
			
			if(com1!=null) writeCommand(os, com1);
			if(com2!=null) writeCommand(os, com2);
			writeCommand(os, com5);
			writeCommand(os, com6);
			writeCommand(os, com7);
			writeCommand(os, com8);
			writeCommand(os, com9);
			writeCommand(os, com10);
			
			os.flush();
			return 1;
		} catch (IOException e) {
			Log.e("VncTest", "inject move failed!");
			return -1;
		} catch (Exception e) {
			Log.e("VncTest", "inject move failed!");
			return -1;
		}
	}
	
	public void startServer() throws Exception{
		startShell=null;
		int count=1;
		while(startShell==null){
			SuLauncher runner=new SuLauncher();
			Thread thr=new Thread(runner);
			thr.start();
			Thread.sleep(100);
			count++;
			if(runner.proc!=null){
				startShell=runner.proc;
				Log.i("VncTest", "started su process successfully");
			}else{
				Log.e("VncTest", "failed to start su process to start server, retrying");
				thr.interrupt();
				if(count>100) throw new Exception();
			}
		}
		OutputStream os=startShell.getOutputStream();
		new Thread(new StreamCleaner(startShell.getErrorStream(), "ErrorStream")).start();
		new Thread(new StreamCleaner(startShell.getInputStream(), "InputStream")).start();
		Log.i("VncTest", "got output stream: "+os);
		String permission_string="chmod 777 " + getFilesDir().getAbsolutePath()+ "/androidvncserver";
		Log.i("VncTest", "will be communicating with port "+portno);
		String server_string=getFilesDir().getAbsolutePath()+ "/androidvncserver "+portno+" "+Settings.vncServPort;
		writeCommand(os, permission_string);
		writeCommand(os, server_string);
		os.flush();
		
		notificationHandler.post(new Runnable() {
			public void run() {
				Toast.makeText(VncServerService.this, Settings.tVNCServerLaunch,
						Toast.LENGTH_LONG).show();
				startForeground(NotificationHandler.ID, NotificationHandler
						.createNotification(VncServerService.this) );
			}
		});

	}
	
	OutputStream restartSuProcess() throws Exception{
		Process sh = null;
		int count=1;
		while(sh==null){
			SuLauncher runner=new SuLauncher();
			Thread thr=new Thread(runner);
			thr.start();
			Thread.sleep(100);
			count++;
			if(runner.proc!=null){
				sh=runner.proc;
			}else{
				thr.interrupt();
				if(count>100) throw new Exception();
			}
		}
		new Thread(new StreamCleaner(startShell.getErrorStream(), "ErrorStream")).start();
		new Thread(new StreamCleaner(startShell.getInputStream(), "InputStream")).start();
		return sh.getOutputStream();
	}
	
	static void writeCommand(OutputStream os, String command) throws Exception {
		os.write((command + "\n").getBytes("ASCII"));
	}
	
	
	@Override
	public void onDestroy(){
		super.onDestroy();
		Log.i("VncTest", "is destroyed!");
		threadStop=true;
		if(sock!=null){
			try {
				sock.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		if(startShell!=null) startShell.destroy();
		unregisterReceiver(this.cbr);
	}
	
	
	class CommandsBR extends BroadcastReceiver{
		@Override
		public void onReceive(Context arg0, Intent arg1) {
			String command=new String(arg1.getByteArrayExtra(Settings.tMessage));
			Log.i("VncServerService", "got info: "+command);
			JSONObject obj;
			Intent i;
			try {
				obj = new JSONObject(command);
				String what=obj.getString(JsonStrings.vnc);
				if (what.equals(JsonStrings.stopVnc)){
					switch(isVnc){
					case NO:
						Log.i("VncService", "for some reason was asked to stop when not vncing at all");
						break;
					case SERVER:
						
						Toast.makeText(VncServerService.this, "Screen sharing server has stopped", Toast.LENGTH_LONG).show();
						isVnc=VncStatus.NO;
						sendBroadcast(new Intent(Settings.ifShareButton).putExtra("state", "unlit"));
						break;
						
					}
				} else if (what.equals(JsonStrings.server)){
					servCounter++;
					isVnc=VncStatus.SERVER;
					sendBroadcast(new Intent(Settings.ifShareButton).putExtra("state", "lit"));
					if(!isAndroidServerRunning()){
						Log.i("VncTest", "server not started");
						if(!serverRoutine()) {
							Toast.makeText(VncServerService.this, "Sorry! The device is not rooted, screen sharing is unable to start", Toast.LENGTH_LONG);
							sendBroadcast(new Intent(Settings.ifShareButton).putExtra("state", "unlit"));
							i=new Intent(Settings.ifFromAnyActivity);
							i.putExtra(Settings.tChannel, Settings.cVnc);
							obj=new JSONObject();
							obj.put(JsonStrings.vnc, JsonStrings.disconnectAttempt);
							i.putExtra(Settings.tMessage, obj.toString().getBytes());
							sendBroadcast(i);
							return; 
						}
						sendBroadcast(new Intent(Settings.ifShareButton).putExtra("state", "in progress"));
					} else{
						i=new Intent(Settings.ifFromAnyActivity);
						i.putExtra(Settings.tChannel, Settings.cVnc);
						obj=new JSONObject();
						obj.put(JsonStrings.vnc, "nya");
						i.putExtra(Settings.tMessage, obj.toString().getBytes());
						sendBroadcast(i);
					}
					sendBroadcast(new Intent("cease and desist")); //sent to share activity to kill it
				} else if(what.equals("cancel")){
					sendBroadcast(new Intent(Settings.ifShareButton).putExtra("state", "dismiss"));
				}
			} catch (JSONException e) {
				Log.i("VncService", "got something unparseable: "+command);
			}
		}
	}    
	
	void stopOnError(){
		if(sock!=null){
			try {
				sock.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
			sock=null;
		}
		Toast.makeText(VncServerService.this, 
			"Sorry! Su has encountered some problems, was unable to start process. " +
			"To avoid this problem rebooting device is advised.", Toast.LENGTH_LONG).show();
	
		android.os.Process.killProcess(android.os.Process.myPid() );
	}
	
	class Stopper implements Runnable{
		@Override
		public void run(){
			stopOnError();
		}
	}
	
	void setDevice(){
		File f=new File("/proc/bus/input/devices");
		try {
			BufferedReader buf = new BufferedReader(new InputStreamReader(new FileInputStream(f)));
			String t="a";
			while(!t.contains("touch")){
				t=buf.readLine().toLowerCase().trim();
			}
			while(!t.contains("Handlers")){
				t=buf.readLine();
			}
			device="/dev/input/"+t.substring(t.indexOf("=")+1);
			Log.i("VncTest", "got device: "+device);
		} catch (Exception e) {
			device="/dev/input/event2";
			e.printStackTrace();
		}
	}
	
	public boolean serverRoutine(){
		if(hasRootPermission()){
			threadStop=false;
			new Thread(new Runnable(){
				@Override
				public void run() {
					if(device==null){
						setDevice();
					}
					Log.i("VncTest", "has root permission: "+hasRootPermission());
					try {
						startServer();
					} catch (Exception e) {
						Log.v("VncTest", "failed to start server");
						e.printStackTrace();
						poster.post(new Stopper());
						return;
					}
					
					int oldX=0, oldY=0;
					
					Log.i("VncTest", "entering connection cycle!");
					while(sock==null){
						try {
							Log.i("VncTest", "trying to connect to "+portno);
							sock=new Socket("localhost", portno);
							Log.i("VncTest", "connected!");
						} catch (UnknownHostException e) {
							try {
								Thread.sleep(200);								
							} catch (InterruptedException e1) {}
						} catch (IOException e) {
							try {
								Thread.sleep(200);								
							} catch (InterruptedException e1) {}
						}
					}
					poster.post(new Runnable(){
						@Override
						public void run(){
							Intent i=new Intent(Settings.ifFromAnyActivity);
							i.putExtra(Settings.tChannel, Settings.cVnc);
							JSONObject obj=new JSONObject();
							try {
								obj.put(JsonStrings.vnc, "nya");
							} catch (JSONException e) {}
							i.putExtra(Settings.tMessage, obj.toString().getBytes());
							sendBroadcast(i);
							//sendBroadcast(new Intent("cease and desist"));
						}
					});
					JSONObject obj=new JSONObject();
					InputStream sockIn = null;
					try {
						sockIn=sock.getInputStream();
						Log.i("VncTest", "got input stream!");
					} catch (IOException e) {
						Log.e("VncTest", "could not get input stream");
						e.printStackTrace();
						poster.post(new Stopper());
						return;
					}
					boolean connected=true;
					boolean justPressed=false;
					OutputStream osTemp = null;
					try{
						osTemp=restartSuProcess();
					}catch(Exception e){
						poster.post(new Stopper());
						return;
					}
					while(connected){
						if(threadStop) return;
						byte[] len=new byte[2];
						int read;
						try {
							read=sockIn.read(len);
							if(read==-1){
								poster.post(new Stopper());
								return;
							}
						} catch (IOException e) {
							e.printStackTrace();
						}
						int length=ByteBuffer.wrap(len).getShort();
						Log.i("VncTest", "got length: "+length);
						byte[] temp=new byte[length];
						try {
							if(threadStop) return;
							read=sockIn.read(temp);
							if(read==-1){
								poster.post(new Stopper());
								return;
							}
							while(read<length){
								int tRead=sockIn.read(temp, read, length-read);
								if(tRead==-1){
									poster.post(new Stopper());
									return;
								}else{
									read+=tRead;
								}
							}
						} catch (IOException e) {
							e.printStackTrace();
						}
						if(temp!=null){
							try {
								obj=new JSONObject(new String(temp));
								String todo=obj.getString("vncServer");
								if(todo.equals("left pressed")){
									justPressed=true;
									JSONArray arr=obj.getJSONArray("coords");
									Log.i("VncTest", "got press event");
									oldX=arr.getInt(0);
									oldY=arr.getInt(1);
								} else if (todo.equals("left released")){
									if(justPressed){
										while(injectPress(oldX, oldY, osTemp)==-1){
											Log.w("VnvServerRoutine", "broken pipe");
											try{
												osTemp=restartSuProcess();
											}catch(Exception e){
												poster.post(new Stopper());
												return;
											}
										}
										justPressed=false;
									}
									while(injectRelease(oldX, oldY,osTemp)==-1){
										Log.w("VnvServerRoutine", "broken pipe");
										try{
											osTemp=restartSuProcess();
										}catch(Exception e){
											poster.post(new Stopper());
											return;
										}
									}
								} else if (todo.equals("drag")){
									if(justPressed){
										Log.i("VncService", "starting drag with a press");
										justPressed=false;
										while(injectPress(oldX,oldY, osTemp)==-1){
											Log.w("VnvServerRoutine", "broken pipe");
											try{
												osTemp=restartSuProcess();
											}catch(Exception e){
												poster.post(new Stopper());
												return;
											}
										}
									}
									JSONArray arr=obj.getJSONArray("coords");
									while(injectMove(arr.getInt(0), arr.getInt(1), oldX, oldY, osTemp)==-1){
										Log.w("VnvServerRoutine", "broken pipe");
										try{
											osTemp=restartSuProcess();
										}catch(Exception e){
											poster.post(new Stopper());
											return;
										}
									}
									oldX=arr.getInt(0);
									oldY=arr.getInt(1);
								} else if (todo.equals("left clicked")){
									JSONArray arr=obj.getJSONArray("coords");
									while(injectClick(arr.getInt(0), arr.getInt(1), osTemp)==-1){
										Log.w("VnvServerRoutine", "broken pipe");
										try {
											/*sh = Runtime.getRuntime().exec("su");
											osTemp=sh.getOutputStream();*/
											osTemp=restartSuProcess();
										} catch (Exception e) {}
									}
								}
							} catch (JSONException e) {
								Log.e("VncTest", "could not parse a valid json");
							}
						}
					}
					try {
						sock.close();
						threadStop=true;
					} catch (IOException e) {}
				}
			}).start();
			return true;
		} else{
			return false;
		}
	}
}
