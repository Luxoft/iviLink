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

import java.io.File;
import java.io.FilenameFilter;
import java.io.IOException;
import java.io.OutputStream;
import java.util.Timer;
import java.util.TimerTask;

import android.app.NotificationManager;
import android.app.Service;

import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.ContextWrapper;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.content.res.Configuration;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.util.Log;
import android.widget.Toast;

import com.luxoft.ivilink.samples.screensharingserver.ScreenSharingServer;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

import org.onaips.vnc.*;

public class ScreenSharingServer extends Service {
    private static final String TAG = "ScreenSharingServer";

    Handler poster = new Handler();

    ExitBroadcastReceiver exitBR = new ExitBroadcastReceiver();
    ServerActivityBroadcastReceiver serverActivityBR = new ServerActivityBroadcastReceiver();

    boolean needToStopServer = false;
    BridgeC bridge;
    private ServerManager serverManager = null;

    ScreenSharingServerActivity parentActivity;

    @Override
    public void onCreate() {
        super.onCreate();
        Log.i(TAG, Logging.getCurrentMethodName());
        
        if (hasRootPermission()) {
            try {
                copyVncLibs();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.i(TAG, Logging.getCurrentMethodName());
                
        doBindService();

        registerReceiver(exitBR, new IntentFilter(Settings.EXIT_INTENT_TAG));
        registerReceiver(serverActivityBR, new IntentFilter(Settings.INTENT_TAG));

        sendBroadcast(new Intent(Settings.INTENT_TAG).putExtra(Settings.SHOW_START_PROGRESS, true));

        bridge = BridgeC.getInstance(ScreenSharingServer.this, new Handler());
        bridge.startApp(intent.getStringExtra(Settings.LAUNCH_INFO),
                intent.getStringExtra(Settings.INTERNAL_PATH));

        return START_NOT_STICKY;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.i(TAG, Logging.getCurrentMethodName());
        try {
            unregisterReceiver(exitBR);
            unregisterReceiver(serverActivityBR);
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        }
    }

    void doBindService() {
        Log.i(TAG, Logging.getCurrentMethodName());
        if (!bindService(new Intent(this, ServerManager.class), mConnection,
                Context.BIND_AUTO_CREATE)) {
            Log.e(TAG, "Error binding service!");
        }
    }

    private ServiceConnection mConnection = new ServiceConnection() {
        public void onServiceConnected(ComponentName className, IBinder binder) {
            Log.i(TAG, "mConnection::onServiceConnected()");
            serverManager = ((ServerManager.MyBinder) binder).getService();
        }

        public void onServiceDisconnected(ComponentName className) {
            Log.i(TAG, "mConnection::onServiceDisconnected()");
            serverManager = null;
        }
    };

    public void showTextOnScreen(final String t) {
        Log.i(TAG, Logging.getCurrentMethodName());
        new Handler(Looper.getMainLooper()).post(new Runnable() {
            public void run() {
                Toast.makeText(ScreenSharingServer.this, t, Toast.LENGTH_LONG).show();
            }
        });
    }

    public void startServer() {
        Log.i(TAG, Logging.getCurrentMethodName());
        if (serverManager != null) {
            serverManager.startServer();
            Timer t = new Timer();
            t.schedule(new TimerTask() {

                @Override
                public void run() {
                    if (!ServerManager.isServerRunning()) {
                        showTextOnScreen("Could not start server");
                        Log.i(TAG, "Could not start server");
                    } else {
                        poster.post(new Runnable() {
                            public void run() {
                                Toast.makeText(ScreenSharingServer.this,
                                        Settings.VNC_SERVER_LAUNCHED, Toast.LENGTH_LONG).show();
                                ((NotificationManager) getSystemService(NOTIFICATION_SERVICE))
                                        .notify(NotificationHandler.ID, NotificationHandler
                                                .createNotification(ScreenSharingServer.this));
                                sendBroadcast(new Intent(Settings.INTENT_TAG).putExtra(
                                        Settings.DISMISS_PROGRESS, true));
                            }
                        });
                    }
                }
            }, 4000);
        } else {
            Log.e(TAG, "!!! Server == null !!!");
        }
    }

    public void stopServer() {
        Log.i(TAG, Logging.getCurrentMethodName());
        if (serverManager != null) {
            serverManager.killServer();
            Timer t = new Timer();
            t.schedule(new TimerTask() {

                @Override
                public void run() {
                    if (ServerManager.isServerRunning()) {
                        showTextOnScreen("Could not stop server");
                        Log.i(TAG, "Could not stop server");
                    }
                }
            }, 4000);
        } else {
            Log.e(TAG, "!!! Server == null !!!");
        }

    }

    public void launchServices() {
        if (!ServerManager.isServerRunning()) {
            Log.i(TAG, "VncService not started, starting");
            startServer();
        }
    }

    class ExitBroadcastReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            Log.i(TAG, "ExitBroadcastReceiver " + Logging.getCurrentMethodName());
            if (intent.getBooleanExtra(Settings.EXIT, false)) {
                Log.i(TAG, "Broadcast received : stopServer() and exit");

                ((NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE))
                        .cancel(NotificationHandler.ID);
                stopServer();
                new Handler().postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        Log.i(TAG, "Broadcast received : " + "unbind, unregister, killProcess()");
                        unbindService(mConnection);
                        try {
                            unregisterReceiver(exitBR);
                        } catch (IllegalArgumentException e) {
                            e.printStackTrace();
                        }
                        android.os.Process.killProcess(android.os.Process.myPid());
                    }
                }, 5000);
            }
        }
    }

    class ServerActivityBroadcastReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            if (intent.getBooleanExtra(Settings.VNC_DIALOG_OK, false)) {
                launchServices();

                new Handler().postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        boolean serverCantStart = false;
                        final int timeout = 10000;
                        final int sleepTime = 500;
                        int waitTime = 0;

                        while (!ServerManager.isServerRunning() && !serverCantStart) {
                            if (waitTime >= timeout) {
                                serverCantStart = true;
                            }
                            try {
                                Thread.sleep(sleepTime);
                                waitTime += sleepTime;
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                        }
                        Log.i(TAG, "Server started, sending vncMsgStartAck, waitTime was : "
                                + waitTime);
                        bridge.sendData(Settings.VNC_MSG_START_ACK);
                    }
                }, 1000);
            } else if (intent.getBooleanExtra(Settings.VNC_DIALOG_CANCEL, false)) {
                bridge.sendData(Settings.VNC_MSG_EXIT);
                android.os.Process.killProcess(android.os.Process.myPid());
            }

        }
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
    }

    public void startVncDialog() {
        sendBroadcast(new Intent(Settings.INTENT_TAG).putExtra(Settings.SHOW_VNC_DIALOG, true));
    }

    public static boolean hasRootPermission() {
        Log.i(TAG, Logging.getCurrentMethodName());
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
            Log.e(TAG, "Can't obtain root. Error message: " + e.getMessage());
            e.printStackTrace();
            rooted = false;
        }

        return rooted;
    }

    private void copyVncLibs() throws IOException {
        Log.i(TAG, Logging.getCurrentMethodName());
        ContextWrapper contextWrapper = new ContextWrapper(this);
        String oldLibPath = contextWrapper.getFilesDir().getPath().replace("/files", "/lib");

        File libDir = new File(oldLibPath);
        FilenameFilter filter = new FilenameFilter() {
            public boolean accept(File libDir, String fileName) {
                return (fileName.endsWith(".so") && fileName.contains("libdvnc"));
            }
        };

        File[] libsToCopy = libDir.listFiles(filter);
        final String onaipsPackageName = "org.onaips.vnc";
        String onaipsFilesDirPath = libDir.getPath()
                .replace(contextWrapper.getPackageName(), onaipsPackageName)
                .replace("/lib", "/files");
        String onaipsRootDirPath = onaipsFilesDirPath.replace("/files", "");

        Process p = Runtime.getRuntime().exec("su");
        OutputStream os = p.getOutputStream();

        if (!(new File(onaipsRootDirPath).exists())) {
            String commandMkDir = "mkdir " + onaipsRootDirPath;
            Log.i(TAG, "org.onaips.vnc folder not found, creating : " + commandMkDir);
            os.write((commandMkDir + "\n").getBytes("ASCII"));
        }
        if (!(new File(onaipsFilesDirPath).exists())) {
            String commandMkSubDir = "mkdir " + onaipsFilesDirPath;
            Log.i(TAG, "org.onaips.vnc/files folder not found, creating : " + commandMkSubDir);
            os.write((commandMkSubDir + "\n").getBytes("ASCII"));
        }

        for (int i = 0; i < libsToCopy.length; i++) {
            Log.i(TAG, " Lib to copy : " + libsToCopy[i].getPath());

            String command = "cat "
                    + libsToCopy[i]
                    + " > "
                    + libsToCopy[i].getPath()
                            .replace(contextWrapper.getPackageName(), onaipsPackageName)
                            .replace("/lib/", "/files/");
            Log.i(TAG, "command : " + command);
            os.write((command + "\n").getBytes("ASCII"));
        }
    }

    @Override
    public IBinder onBind(Intent arg0) {
        return null;
    }
}
