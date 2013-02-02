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
package com.luxoft.ivilink.samples.media.players.ipc;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.wifi.WifiManager;
import android.net.wifi.WifiManager.MulticastLock;
import android.util.Log;

import com.luxoft.ivilink.samples.media.helpers.ProcessesInfo;
import com.luxoft.ivilink.samples.media.interfaces.Player;
import com.luxoft.ivilink.samples.media.interfaces.PlayerEventProcessor;
import com.luxoft.ivilink.samples.media.players.ipc.CommunicationProtocol.FromLogicToPlayer;
import com.luxoft.ivilink.samples.media.players.ipc.CommunicationProtocol.FromPlayerToPlayerLogic;
import com.luxoft.ivilink.sdk.android.lib.utils.Assertion;
import com.luxoft.ivilink.sdk.android.lib.utils.ProcessesHelper;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

/**
 * Communicates with the main process (the one that contains the iviLink
 * application and all native player logic)
 */
public class PlayerLogicProxy extends InvocationSerializator<PlayerEventProcessor> {
    private final static String TAG = PlayerLogicProxy.class.getName();
    @SuppressWarnings("unused")
    private MulticastLock mMulticastLock;
    private final Context mContext;
    private Player mPlayerCallback;
    private final String mIntentAction;
    private final String mIntentActionSuffix;
    private ExecutorService mExecutor = Executors.newCachedThreadPool();
    private Thread mMainAppWatchdogThread;

    private static PlayerLogicProxy mInstance;

    private TerminationHandler mTerminationHandler;

    public interface TerminationHandler {
        public void onTerminationNotification();
    }

    /**
     * Registers a TerminationHandler - its method onTerminationNotification
     * will be called either if the main process commands to this one to stop or
     * if the main process has died. After onTerminationNotification, current
     * process will be killed.
     * 
     * @param handler
     *            can be <code>null</code>
     */
    public void setTerminationHandler(TerminationHandler handler) {
        synchronized (PlayerLogicProxy.class) {
            mTerminationHandler = handler;
        }
    }

    public static PlayerLogicProxy getInstance(Context context, String additionalReceiverTag) {
        synchronized (PlayerLogicProxy.class) {
            if (mInstance == null) {
                mInstance = new PlayerLogicProxy(context, additionalReceiverTag);
            }
            return mInstance;
        }
    }

    public static PlayerLogicProxy getInstance(Context context, Player callback,
            String additionalReceiverTag) {
        synchronized (PlayerLogicProxy.class) {
            if (mInstance == null) {
                mInstance = new PlayerLogicProxy(context, callback, additionalReceiverTag);
            }
            return mInstance;
        }
    }

    public static PlayerLogicProxy getExistingInstance() {
        synchronized (PlayerLogicProxy.class) {
            return mInstance;
        }
    }

    private BroadcastReceiver FromPlayerLogic = new BroadcastReceiver() {

        @Override
        public void onReceive(Context arg0, Intent intent) {
            String message = intent.getStringExtra(CommunicationProtocol.MESSAGE);
            final FromLogicToPlayer fltp = FromLogicToPlayer.fromString(message);
            if (fltp == null) {
                Log.w(TAG, "could not parse command from: " + message);
                return;
            }
            mExecutor.execute(new RemoteTask(fltp));
        }
    };

    private MulticastLock obtainMulticastLock() {
        WifiManager wifiManager = (WifiManager) mContext.getSystemService(Context.WIFI_SERVICE);
        return wifiManager.createMulticastLock(mIntentAction);
    }

    private PlayerLogicProxy(Context context, Player callback, String additionalReceiverTag) {
        this(context, additionalReceiverTag);
        registerCallback(callback);
    }

    /**
     * When this constructor is used, the newly-created proxy instance will not
     * be receiving any commands until a callback is provided with a call to
     * registerCallback().
     * 
     * @param context
     * @param additionalReceiverTag
     */
    private PlayerLogicProxy(Context context, String additionalReceiverTag) {
        super(context, PlayerEventProcessor.class);
        mContext = context;
        mIntentActionSuffix = additionalReceiverTag;
        mIntentAction = CommunicationProtocol.TO_BUTTON_PROCESSOR + mIntentActionSuffix;
        mMulticastLock = obtainMulticastLock();
        mMainAppWatchdogThread = new Thread(mMainApplicationWatchdog);
        mMainAppWatchdogThread.start();
    }

    /**
     * After that the proxy will start receiving remote commands.
     * 
     * @param callback
     */
    public void registerCallback(Player callback) {
        mPlayerCallback = callback;
        mContext.registerReceiver(FromPlayerLogic, new IntentFilter(
                CommunicationProtocol.TO_SERVICE + mIntentActionSuffix));
    }

    /**
     * 
     * @param launchIntent
     * @throws NullPointerException
     *             when a callback was not provided in either the constructor or
     *             with registerCallbac().
     */
    public void decodeInitialMessage(final Intent launchIntent) throws NullPointerException {
        String message = launchIntent.getStringExtra(CommunicationProtocol.MESSAGE);
        FromLogicToPlayer fltp = FromLogicToPlayer.fromString(message);
        Assertion.check(fltp == FromLogicToPlayer.PLAY);
        String path = launchIntent.getStringExtra(CommunicationProtocol.EXTRA_INFO);
        Assertion.check(path != null);
        mPlayerCallback.onStartPlayback(path);
    }

    private class RemoteTask implements Runnable {
        FromLogicToPlayer mTask;

        public RemoteTask(FromLogicToPlayer task) {
            mTask = task;
        }

        public void run() {
            switch (mTask) {
            case PAUSE:
                mPlayerCallback.onPausePlayback();
                break;
            case PLAY:
                Log.w(TAG, "This should never hapen actually!");
                break;
            case RESUME:
                mPlayerCallback.onResumePlayback();
                break;
            case STOP:
                mPlayerCallback.onStopPlayback();
                synchronized (PlayerLogicProxy.class) {
                    if (mTerminationHandler != null) {
                        mTerminationHandler.onTerminationNotification();
                    }
                }
                android.os.Process.killProcess(android.os.Process.myPid());
            default:
                break;

            }
        }
    }

    private Runnable mMainApplicationWatchdog = new Runnable() {
        public void run() {
            String mainProcessName = ProcessesInfo.getMainProcessName(mContext);
            while (true) {
                int pid = ProcessesHelper.findPidOfApp(mContext, mainProcessName);
                if (pid != ProcessesHelper.ERROR_PID) {
                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                } else {
                    synchronized (PlayerLogicProxy.class) {
                        if (mTerminationHandler != null) {
                            mTerminationHandler.onTerminationNotification();
                        }
                    }
                    android.os.Process.killProcess(android.os.Process.myPid());
                }
            }
        }
    };

    @Override
    protected String getIntentAction() {
        return mIntentAction;
    }

}
