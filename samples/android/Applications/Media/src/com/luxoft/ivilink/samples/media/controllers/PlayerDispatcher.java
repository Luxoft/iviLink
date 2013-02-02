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
package com.luxoft.ivilink.samples.media.controllers;

import java.util.HashMap;
import java.util.Map;

import android.util.Log;

import com.luxoft.ivilink.samples.media.Bridge;
import com.luxoft.ivilink.samples.media.interfaces.JniPlayerCallback;
import com.luxoft.ivilink.samples.media.interfaces.Player;
import com.luxoft.ivilink.samples.media.interfaces.PlayerEventProcessor;
import com.luxoft.ivilink.samples.media.players.AbstractPlayer;
import com.luxoft.ivilink.samples.media.structs.MediaType;
import com.luxoft.ivilink.samples.media.structs.PlayingState;
import com.luxoft.ivilink.sdk.android.lib.utils.log.BigLog;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

/**
 * Is connected with native IPlayer's implementation. Passes calls from
 * PlayerLogic to an appropriate AbstractPlayer instance depending on the type
 * of the media that is currently being played.
 */
public class PlayerDispatcher implements JniPlayerCallback, PlayerEventProcessor {
    protected char logHighlight = '!';
    private final static String TAG = PlayerDispatcher.class.getName();

    protected final int OK = 0;
    protected final int ERROR = -1;

    private Map<MediaType, Player> mPlayers = new HashMap<MediaType, Player>();
    static SharedState mSharedState = SharedState.getInstance();

    public void addPlayer(AbstractPlayer player, MediaType type) {
        mPlayers.put(type, player);
    }

    // from the PlayerEventProcessor interface
    public void onCompletion() {
        mSharedState.setPlayingState(PlayingState.STOPPED);
        /* will force playerlogic to check our player's state - isTrackEnded */
        Bridge.getExistingInstance().onCompletion();
    }

    public void onError() {
        Log.v(TAG, Logging.getCurrentMethodName());
        Bridge.getExistingInstance().onNextClicked();
    }

    public void onConnectedToStream() {
        Log.v(TAG, Logging.getCurrentMethodName());
        Bridge.getExistingInstance().onPlaybackStarted();
    }

    // from the JniPlayerWrapper interface
    // invoked from the CAndroidPlayerImpl class
    public int play(String path, String form) {
        Log.v(TAG, Logging.getCurrentMethodName(path, form));
        for (Player player : mPlayers.values()) {
            player.onStopPlayback();
        }
        if (mSharedState.getPlayingState() != PlayingState.STOPPED) {
            BigLog.i(TAG, "play:: is not stopped", logHighlight);
            if (mSharedState.getCurrentMediaType() != MediaType.NA) {
                BigLog.i(TAG, "play:: is not stopped:: is not NA", logHighlight);
                mPlayers.get(mSharedState.getCurrentMediaType()).onStopPlayback();
            }
        }
        mSharedState.setCurrentMediaType(MediaType.NA);
        mSharedState.setPlayingState(PlayingState.STOPPED);
        BigLog.i(TAG, "play(" + path + "," + form + ")", logHighlight);
        if (MediaType.fromString(form) == MediaType.NA) {
            BigLog.e(TAG, "Could not parse form: " + form, logHighlight);
            return ERROR;
        }
        mSharedState.setCurrentMediaType(MediaType.fromString(form));
        mSharedState.setPlayingState(PlayingState.PLAYING);
        mPlayers.get(mSharedState.getCurrentMediaType()).onStartPlayback(path);
        return OK;
    }

    public void pause() {
        BigLog.i(TAG, Logging.getCurrentMethodName(), logHighlight);
        if (mSharedState.getPlayingState() != PlayingState.PLAYING) {
            BigLog.w(TAG, "pause() invoked in some wrong state", logHighlight);
        }
        if (mSharedState.getCurrentMediaType() != MediaType.NA) {
            mPlayers.get(mSharedState.getCurrentMediaType()).onPausePlayback();
        } else {
            BigLog.e(TAG, "pause called in a wrong state", logHighlight);
        }
        mSharedState.setPlayingState(PlayingState.PAUSED);
    }

    public int resume() {
        BigLog.i(TAG, Logging.getCurrentMethodName(), logHighlight);
        if (mSharedState.getPlayingState() != PlayingState.PAUSED) {
            BigLog.w(TAG, "resume() invoked in some wrong state", logHighlight);
        }
        if (mSharedState.getCurrentMediaType() != MediaType.NA) {
            mPlayers.get(mSharedState.getCurrentMediaType()).onResumePlayback();
        }
        return OK;
    }

    public void stop() {
        BigLog.i(TAG, Logging.getCurrentMethodName(), logHighlight);
        for (Player player : mPlayers.values()) {
            player.onStopPlayback();
        }
        mSharedState.setCurrentMediaType(MediaType.NA);
        mSharedState.setPlayingState(PlayingState.STOPPED);
    }

    public boolean isTrackEnded() {
        return mSharedState.getPlayingState() == PlayingState.STOPPED;
    }
}
