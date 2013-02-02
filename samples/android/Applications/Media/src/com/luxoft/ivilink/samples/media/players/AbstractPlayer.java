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
import android.util.Log;

import com.luxoft.ivilink.samples.media.interfaces.ButtonProcessor;
import com.luxoft.ivilink.samples.media.interfaces.Player;
import com.luxoft.ivilink.samples.media.interfaces.PlayerEventProcessor;
import com.luxoft.ivilink.samples.media.players.ipc.PlayerProxy;
import com.luxoft.ivilink.sdk.android.lib.utils.log.BigLog;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

/**
 * Instances of its subclasses live in the main application process. They don't
 * really play anything, they only communicate (pass commands and handle events)
 * with entities that perform the actual playback (they exist in other
 * processes) via proxies (see {@link PlayerProxy} ).
 */
public abstract class AbstractPlayer implements Player, PlayerEventProcessor {
    protected static final char highlight = '!';

    protected ButtonProcessor mButtonProcessor;
    protected Context mContext;

    private PlayerEventProcessor mPlayerDispatcher;

    // defined by subclasses
    protected PlayerProxy mPlayerProxy;

    AbstractPlayer(Context activity, ButtonProcessor bProcessor, PlayerEventProcessor eProcessor) {
        mContext = activity;
        mButtonProcessor = bProcessor;
        mPlayerDispatcher = eProcessor;
        mPlayerProxy = createPlayerProxy(mContext, mButtonProcessor);
    }

    // from the PlayerEventProcessor interface
    public void onCompletion() {
        BigLog.i(tag(), Logging.getCurrentMethodName(), highlight);
        mPlayerDispatcher.onCompletion();
    }

    public void onError() {
        BigLog.e(tag(), Logging.getCurrentMethodName(), highlight);
        mPlayerProxy.onStopPlayback();
        mPlayerDispatcher.onError();
    }

    public void onConnectedToStream() {
        BigLog.e(tag(), Logging.getCurrentMethodName(), highlight);
        mPlayerDispatcher.onConnectedToStream();
    }

    public void onStartPlayback(String path) {
        Log.v(tag(), Logging.getCurrentMethodName(path));
        mPlayerProxy.onStartPlayback(path);
    }

    public void onResumePlayback() {
        Log.v(tag(), Logging.getCurrentMethodName());
        mPlayerProxy.onResumePlayback();
    }

    public void onPausePlayback() {
        Log.v(tag(), Logging.getCurrentMethodName());
        mPlayerProxy.onPausePlayback();
    }

    public void onStopPlayback() {
        Log.v(tag(), Logging.getCurrentMethodName());
        mPlayerProxy.onStopPlayback();
    }

    protected abstract PlayerProxy createPlayerProxy(Context context, ButtonProcessor processor);

    protected abstract String tag();
}
