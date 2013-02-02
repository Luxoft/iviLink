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

import android.content.Context;
import android.content.Intent;

import com.luxoft.ivilink.samples.media.MediaPlayerService;
import com.luxoft.ivilink.samples.media.helpers.ProcessesInfo;
import com.luxoft.ivilink.samples.media.interfaces.ButtonProcessor;
import com.luxoft.ivilink.samples.media.interfaces.PlayerEventProcessor;
import com.luxoft.ivilink.samples.media.players.audio.AudioService;
import com.luxoft.ivilink.samples.media.players.ipc.CommunicationProtocol.FromLogicToPlayer;


/**
 * Communicates with the {@link AudioService}
 */
public class AudioPlayerProxy extends PlayerProxy {
    private static String TAG = AudioPlayerProxy.class.getName();

    public AudioPlayerProxy(Context context, ButtonProcessor buttonProcessor,
            PlayerEventProcessor playerEventProcessor) {
        super(context, buttonProcessor, playerEventProcessor);
    }

    @Override
    protected String getIntentActionSuffix() {
        return CommunicationProtocol.AUDIO;
    }

    @Override
    protected void launchProcess(String pathToPlay) {
        mContext.startService(new Intent(mContext, AudioService.class)
                .putExtra(CommunicationProtocol.EXTRA_INFO, pathToPlay)
                .putExtra(CommunicationProtocol.MESSAGE, FromLogicToPlayer.PLAY.toString())
                .putExtra(MediaPlayerService.BACKGROUND_MODE,
                          MediaPlayerService.getExistingInstance().isInBackgroundMode()));
    }

    @Override
    protected String getLogTag() {
        return TAG;
    }

    @Override
    protected String getProcessName() {
        return ProcessesInfo.getMusicPlayerProcessName(mContext);
    }
}
