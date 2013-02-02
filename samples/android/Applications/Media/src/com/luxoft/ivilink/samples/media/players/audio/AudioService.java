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
package com.luxoft.ivilink.samples.media.players.audio;

import android.app.Notification;
import android.app.Service;
import android.content.Intent;
import android.os.IBinder;

import com.luxoft.ivilink.samples.media.MediaPlayerService;
import com.luxoft.ivilink.samples.media.players.VlcMediaPlayer;
import com.luxoft.ivilink.samples.media.players.ipc.CommunicationProtocol;
import com.luxoft.ivilink.samples.media.players.ipc.MediaGuiServerIpcProxy;
import com.luxoft.ivilink.samples.media.players.ipc.PlayerLogicProxy;
import com.luxoft.ivilink.sdk.android.lib.utils.log.BigLog;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

public class AudioService extends Service {

    private VlcMediaPlayer mPlayerImpl;
    private PlayerLogicProxy mLogicProxy;

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startID) {
        BigLog.i(this.toString(), Logging.getCurrentMethodName(intent, flags, startID), '!');
        if (startID != 1) {
            /*
             * this may be caused only by an error somewhere in the main
             * process, since this instance is supposed to receive the
             * onStartCommand only once, and then be killed some time later
             */
            mLogicProxy.getMockInstance().onError();
            return START_NOT_STICKY;
        }

        if (intent.getBooleanExtra(MediaPlayerService.BACKGROUND_MODE, false)) {
            new MediaGuiServerIpcProxy(this).registerClient(new ClientStub(this));
        }
        mLogicProxy = PlayerLogicProxy.getInstance(this, CommunicationProtocol.AUDIO);
        mPlayerImpl = VlcMediaPlayer.getInstance(mLogicProxy.getMockInstance());
        mLogicProxy.registerCallback(mPlayerImpl);
        @SuppressWarnings("deprecation")
        Notification note = new Notification(0, null, System.currentTimeMillis());
        note.flags |= Notification.FLAG_NO_CLEAR;
        startForeground(42, note);
        mLogicProxy.decodeInitialMessage(intent);
        return START_NOT_STICKY;
    }
}
