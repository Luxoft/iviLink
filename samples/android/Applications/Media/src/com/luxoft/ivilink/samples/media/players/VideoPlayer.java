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
package com.luxoft.ivilink.samples.media.players;

import android.content.Context;

import com.luxoft.ivilink.samples.media.interfaces.ButtonProcessor;
import com.luxoft.ivilink.samples.media.interfaces.PlayerEventProcessor;
import com.luxoft.ivilink.samples.media.players.ipc.PlayerProxy;
import com.luxoft.ivilink.samples.media.players.ipc.VideoPlayerProxy;

public class VideoPlayer extends AbstractPlayer {
    private final static String TAG = VideoPlayer.class.getName();

    public VideoPlayer(Context context, ButtonProcessor buttonProcessor,
            PlayerEventProcessor eventProcessor) {
        super(context, buttonProcessor, eventProcessor);
    }

    @Override
    protected String tag() {
        return TAG;
    }

    @Override
    protected PlayerProxy createPlayerProxy(Context context, ButtonProcessor processor) {
        return new VideoPlayerProxy(context, processor, this);
    }
}
