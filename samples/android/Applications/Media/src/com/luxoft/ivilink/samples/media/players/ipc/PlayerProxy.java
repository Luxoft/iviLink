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

import java.lang.reflect.Method;

import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.util.Log;

import com.luxoft.ivilink.samples.media.interfaces.ButtonProcessor;
import com.luxoft.ivilink.samples.media.interfaces.Player;
import com.luxoft.ivilink.samples.media.interfaces.PlayerEventProcessor;
import com.luxoft.ivilink.samples.media.players.ipc.CommunicationProtocol.FromLogicToPlayer;
import com.luxoft.ivilink.sdk.android.lib.utils.ProcessesHelper;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

/**
 * Communicates with an entity in another process that performs actual playback.
 * The other process is launched in onStartPlayback and killed in onStopPlayback
 */
public abstract class PlayerProxy implements Player {

    protected Context mContext;
    private PlayerEventProcessor mEventProcessor;
    private String mIntentAction;

    public PlayerProxy(Context context, ButtonProcessor buttonProcessor,
            PlayerEventProcessor playerEventProcessor) {
        mContext = context;
        mEventProcessor = playerEventProcessor;
        mIntentAction = CommunicationProtocol.TO_SERVICE + getIntentActionSuffix();
        mContext.registerReceiver(new PlayerEventsReceiver(mEventProcessor), new IntentFilter(
                CommunicationProtocol.TO_BUTTON_PROCESSOR + getIntentActionSuffix()));
    }

    public void onStartPlayback(String path) {
        onStopPlayback();
        launchProcess(path);
        waitForStart();
    }

    public void onPausePlayback() {
        sendMessage(FromLogicToPlayer.PAUSE);
    }

    public void onResumePlayback() {
        sendMessage(FromLogicToPlayer.RESUME);
    }

    public void onStopPlayback() {
        sendMessage(FromLogicToPlayer.STOP);
        verifyDeath();
    }

    protected final void sendMessage(FromLogicToPlayer what) {
        Log.v(getLogTag(), Logging.getCurrentMethodName(what));
        mContext.sendBroadcast(new Intent(mIntentAction).putExtra(CommunicationProtocol.MESSAGE,
                                                                  what.toString()));
    }

    protected void verifyDeath() {
        Log.v(getLogTag(), Logging.getCurrentMethodName());
        while (isAlive()) {
            Log.v(getLogTag(), Logging.getCurrentMethodName() + " in cycle");
            sendMessage(FromLogicToPlayer.STOP);
            try {
                Thread.sleep(50);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        Log.v(getLogTag(), Logging.getCurrentMethodName() + " done");
    }

    protected void waitForStart() {
        Log.v(getLogTag(), Logging.getCurrentMethodName());
        while (isDead()) {
            Log.v(getLogTag(), Logging.getCurrentMethodName() + " in cycle");
            try {
                Thread.sleep(50);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        Log.v(getLogTag(), Logging.getCurrentMethodName() + " done");
    }

    protected abstract String getIntentActionSuffix();

    protected abstract String getProcessName();

    protected abstract void launchProcess(String pathToPlay);

    protected abstract String getLogTag();

    protected boolean isDead() {
        return !isAlive();
    }

    protected boolean isAlive() {
        return ProcessesHelper.findPidOfApp(mContext, getProcessName()) != ProcessesHelper.ERROR_PID;
    }

    private class PlayerEventsReceiver extends InvocationDeserializator {
        private PlayerEventProcessor mEventProcessor;

        public PlayerEventsReceiver(PlayerEventProcessor processor) {
            super(processor.getClass().getMethods());
            mEventProcessor = processor;
        }

        @Override
        protected void onReceivedRemoteMethodCall(Method method, Object[] args) {
            try {
                method.invoke(mEventProcessor, args);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

    }
}
