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
package com.luxoft.ivilink.samples.media;

import java.lang.reflect.Method;
import java.util.List;

import org.videolan.vlc.Logging;

import android.app.ActivityManager;
import android.app.ActivityManager.RunningTaskInfo;
import android.app.Notification;
import android.app.PendingIntent;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.IBinder;

import com.luxoft.ivilink.samples.media.controllers.PlayerDispatcher;
import com.luxoft.ivilink.samples.media.helpers.MediaSource;
import com.luxoft.ivilink.samples.media.interfaces.ButtonProcessor;
import com.luxoft.ivilink.samples.media.interfaces.MediaPlayerServiceStub;
import com.luxoft.ivilink.samples.media.players.AudioPlayer;
import com.luxoft.ivilink.samples.media.players.VideoPlayer;
import com.luxoft.ivilink.samples.media.players.ipc.InvocationDeserializator;
import com.luxoft.ivilink.samples.media.players.ipc.MediaGuiClientIpcProxy;
import com.luxoft.ivilink.samples.media.structs.MediaType;
import com.luxoft.ivilink.samples.media.uiserver.LocalMediaGuiProxiesFactory;
import com.luxoft.ivilink.sdk.android.lib.utils.ForApp;
import com.luxoft.ivilink.sdk.android.lib.utils.log.BigLog;

public class MediaPlayerService extends Service implements MediaPlayerServiceStub {
    public IBinder onBind(Intent arg0) {
        return null;
    }

    public static final String BACKGROUND_MODE = "background mode";

    private boolean mIsInBackgroundMode = false;

    private PlayerDispatcher mPlayerDispatcher;
    @SuppressWarnings("unused")
    private MediaGuiClientIpcProxy mGuiClientIpcProxy;

    private static MediaPlayerService mInstance;

    public static MediaPlayerServiceStub getExistingInstance() {
        return mInstance;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startID) {
        mInstance = this;
        if (startID != 1) {
            startActivity(new Intent(this, PlaylistActivity.class)
                    .addFlags(Intent.FLAG_ACTIVITY_NEW_TASK));
            return START_NOT_STICKY;
        }
        registerReceiver(mBackgroundNotificationReceiver, new IntentFilter(this.getClass()
                .getName()));
        registerReceiver(mButtonProcessorProxyReceiver,
                         new IntentFilter(ButtonProcessor.class.getName()));
        ForApp.registerReceiverFromKillerApp(this);
        startForeground(53, createMainNotification());
        mGuiClientIpcProxy = new MediaGuiClientIpcProxy(this);
        mPlayerDispatcher = new PlayerDispatcher();
        AudioPlayer aPlayer = new AudioPlayer((Context) this, Bridge.getExistingInstance(),
                mPlayerDispatcher);
        VideoPlayer vPlayer = new VideoPlayer((Context) this, Bridge.getExistingInstance(),
                mPlayerDispatcher);
        mPlayerDispatcher.addPlayer(aPlayer, MediaType.AUDIO);
        mPlayerDispatcher.addPlayer(vPlayer, MediaType.VIDEO);
        new Thread(new Runnable() {
            public void run() {
                Bridge.getExistingInstance()
                        .startApp(LocalMediaGuiProxiesFactory.getMediaGuiProxy(),
                                  mPlayerDispatcher, MediaSource.getInstance());
            }
        }).start();
        startActivity(new Intent(this, PlaylistActivity.class)
                .addFlags(Intent.FLAG_ACTIVITY_NEW_TASK));
        return START_NOT_STICKY;
    }

    @SuppressWarnings("deprecation")
    public Notification createMainNotification() {
        int icon = R.drawable.ic_launcher;
        CharSequence tickerText = "iviLink Media Application";
        long when = System.currentTimeMillis();
        Notification notification = new Notification(icon, tickerText, when);
        Context appContext = getApplicationContext();
        CharSequence notiTitle = "iviLink Media";
        CharSequence notiText = "Click to display playlist";
        Intent notiIntent = new Intent(appContext, ActivitiesDispatcher.class);
        notiIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        PendingIntent pendingIntent = PendingIntent.getActivity(appContext, 0, notiIntent, 0);
        notification.setLatestEventInfo(appContext, notiTitle, notiText, pendingIntent);
        return notification;
    }

    private BroadcastReceiver mBackgroundNotificationReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {
            mIsInBackgroundMode = intent.getBooleanExtra(BACKGROUND_MODE, false);
        }

    };

    private BroadcastReceiver mButtonProcessorProxyReceiver = new InvocationDeserializator(
            ButtonProcessor.class.getMethods()) {
        @Override
        protected void onReceivedRemoteMethodCall(Method method, Object[] args) {
            try {
                method.invoke(Bridge.getExistingInstance(), args);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    };

    public boolean isInBackgroundMode() {
        boolean result = mIsInBackgroundMode || checkTopActivity();
        BigLog.i(this.toString(), Logging.getCurrentMethodName() + " = " + result, '!');
        return result;
    }

    private boolean checkTopActivity() {
        ActivityManager am = (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
        List<RunningTaskInfo> tasks = am.getRunningTasks(1);
        if (tasks != null && !tasks.isEmpty()) {
            ComponentName topActivity = tasks.get(0).topActivity;
            if (!topActivity.getPackageName().equals(getPackageName())) {
                return true;
            }
        }
        return false;
    }
}
