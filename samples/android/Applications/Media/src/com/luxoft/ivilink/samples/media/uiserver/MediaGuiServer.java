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
package com.luxoft.ivilink.samples.media.uiserver;

import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Set;

import android.util.Log;
import android.view.View;

import com.luxoft.ivilink.samples.media.interfaces.MediaGui;
import com.luxoft.ivilink.samples.media.structs.MediaType;
import com.luxoft.ivilink.samples.media.structs.TrackInformation;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

/**
 * Stores UI state. When the state is changed somehow, notifies registered
 * clients (see {@link #registerClient(MediaGui.Client)}. This class is not
 * thread-safe, and shouldn't be used directly: see
 * {@link LocalMediaGuiProxiesFactory} (for clients running in the same process)
 * and {@link MediaGuiServerIpcProxy} (for clients running in other processes)
 */
class MediaGuiServer implements MediaGui, MediaGui.Server {
    private static final String TAG = MediaGuiServer.class.getName();

    private static MediaGuiServer mInstance = new MediaGuiServer();
    private boolean mIsUiLocked = false;

    private ViewVisibility mPauseVisibility = ViewVisibility.GONE;
    private ViewVisibility mPlayVisibility = ViewVisibility.VISIBLE;

    private ButtonState mSyncState = ButtonState.NA;
    private ButtonState mToggleState = ButtonState.NA;

    private int mPlaylistPosition = -1;
    private PlaylistIcon mPlaylistIcon = PlaylistIcon.NONE;
    private List<TrackInformation> mPlaylist = new LinkedList<TrackInformation>();

    private Set<Client> mClientsSet = new HashSet<MediaGui.Client>();

    static MediaGuiServer getInstance() {
        return mInstance;
    }

    /**
     * Registers a client. Its state is updated immediately
     */
    public void registerClient(MediaGui.Client client) {
        if (mClientsSet.add(client)) {
            onRequestUiState(client);
        }
    }

    /**
     * Unregister a client, so that it will not receive notifications anymore
     */
    public void unregisterClient(MediaGui.Client client) {
        if (mClientsSet.contains(client)) {
            mClientsSet.remove(client);
        }
    }

    enum ViewVisibility {
        VISIBLE(View.VISIBLE), GONE(View.GONE);
        private int mValue;

        private ViewVisibility(int value) {
            mValue = value;
        }

        public int getViewValue() {
            return mValue;
        }

        public static ViewVisibility fromIntValue(int value) {
            for (ViewVisibility vv : ViewVisibility.values()) {
                if (value == vv.getViewValue()) {
                    return vv;
                }
            }
            return null;
        }
    }

    enum ButtonState {
        ENABLED, DISABLED, ACTIVE, NA;
    }

    enum PlaylistIcon {
        PLAY, PAUSE, NONE;
    }

    public void pauseVisible() {
        Log.v(TAG, Logging.getCurrentMethodName());
        mPauseVisibility = ViewVisibility.VISIBLE;
        mPlayVisibility = ViewVisibility.GONE;
        for (Client client : mClientsSet) {
            client.onHidePlayButton();
            client.onShowPauseButton();
        }
    }

    public void playVisible() {
        Log.v(TAG, Logging.getCurrentMethodName());
        mPauseVisibility = ViewVisibility.GONE;
        mPlayVisibility = ViewVisibility.VISIBLE;
        for (Client client : mClientsSet) {
            client.onHidePauseButton();
            client.onShowPlayButton();
        }
    }

    public void setSyncState(boolean isOn) {
        Log.v(TAG, Logging.getCurrentMethodName(isOn));
        if (isOn) {
            mSyncState = ButtonState.ACTIVE;
            for (Client client : mClientsSet) {
                client.onActivateSync();
            }
        } else {
            mSyncState = ButtonState.ENABLED;
            for (Client client : mClientsSet) {
                client.onDeactivateSync();
            }
        }
    }

    public void setToggleState(boolean isEnabled) {
        Log.v(TAG, Logging.getCurrentMethodName(isEnabled));
        if (isEnabled) {
            mToggleState = ButtonState.ENABLED;
            for (Client client : mClientsSet) {
                client.onEnableToggle();
            }
        } else {
            mToggleState = ButtonState.DISABLED;
            for (Client client : mClientsSet) {
                client.onDisableToggle();
            }
        }
    }

    public void highlightToggle() {
        Log.v(TAG, Logging.getCurrentMethodName());
        mToggleState = ButtonState.ACTIVE;
        for (Client client : mClientsSet) {
            client.onHighlightToggle();
        }
    }

    public void setSelectedPosition(int position) {
        Log.v(TAG, Logging.getCurrentMethodName(position));
        mPlaylistPosition = position;
        for (Client client : mClientsSet) {
            client.onSetSelectedPosition(position);
            if (mPlaylistPosition != -1) {
                client.onCurrentlySelectedTrack(mPlaylist.get(mPlaylistPosition).trackName);
            }
        }
    }

    public void showPlayIcon() {
        Log.v(TAG, Logging.getCurrentMethodName());
        mPlaylistIcon = PlaylistIcon.PLAY;
        for (Client client : mClientsSet) {
            client.onShowPlayIcon();
        }
    }

    public void showPauseIcon() {
        Log.v(TAG, Logging.getCurrentMethodName());
        mPlaylistIcon = PlaylistIcon.PAUSE;
        for (Client client : mClientsSet) {
            client.onShowPauseIcon();
        }
    }

    public void showNoIcon() {
        Log.v(TAG, Logging.getCurrentMethodName());
        mPlaylistIcon = PlaylistIcon.NONE;
        for (Client client : mClientsSet) {
            client.onShowNoIcon();
        }
    }

    public void clearPlayList() {
        Log.v(TAG, Logging.getCurrentMethodName());
        mPlaylist.clear();
        for (Client client : mClientsSet) {
            client.onClearPlaylist();
        }
    }

    public void populateList(String name, boolean isLocal, boolean isAudio) {
        Log.v(TAG, Logging.getCurrentMethodName(name, isLocal, isAudio));
        mPlaylist.add(new TrackInformation(name, isLocal, (isAudio ? MediaType.AUDIO
                : MediaType.VIDEO)));
        for (Client client : mClientsSet) {
            client.onAddToPlaylist(name, isLocal, isAudio);
        }
    }

    public void lockGUI() {
        Log.v(TAG, Logging.getCurrentMethodName());
        mIsUiLocked = true;
        for (Client client : mClientsSet) {
            client.onLockGUI();
        }
    }

    public void unlockGUI() {
        Log.v(TAG, Logging.getCurrentMethodName());
        mIsUiLocked = false;
        for (Client client : mClientsSet) {
            client.onUnlockGUI();
        }
    }

    public void onRequestUiState(Client requester) {
        Log.v(TAG, Logging.getCurrentMethodName(requester));
        if (mIsUiLocked) {
            requester.onLockGUI();
        } else {
            requester.onUnlockGUI();
        }
        requester.onClearPlaylist();
        for (TrackInformation info : mPlaylist) {
            requester.onAddToPlaylist(info.trackName, info.isLocal,
                                      info.mediaType == MediaType.AUDIO);
        }
        switch (mPlaylistIcon) {
        case NONE:
            requester.onShowNoIcon();
            break;
        case PAUSE:
            requester.onShowPauseIcon();
            break;
        case PLAY:
            requester.onShowPlayIcon();
            break;
        default:
            break;

        }
        requester.onSetSelectedPosition(mPlaylistPosition);
        if (mPlaylistPosition != -1) {
            requester.onCurrentlySelectedTrack(mPlaylist.get(mPlaylistPosition).trackName);
        }
        switch (mToggleState) {
        case ACTIVE:
            requester.onHighlightToggle();
            break;
        case DISABLED:
            requester.onDisableToggle();
            break;
        case ENABLED:
            requester.onEnableToggle();
            break;
        case NA:
            break;
        default:
            break;

        }
        switch (mSyncState) {
        case ACTIVE:
            requester.onActivateSync();
            break;
        case DISABLED:
            break;
        case ENABLED:
            requester.onDeactivateSync();
            break;
        case NA:
            break;
        default:
            break;
        }
        switch (mPauseVisibility) {
        case GONE:
            requester.onHidePauseButton();
            break;
        case VISIBLE:
            requester.onShowPauseButton();
            break;
        default:
            break;
        }
        switch (mPlayVisibility) {
        case GONE:
            requester.onHidePlayButton();
            break;
        case VISIBLE:
            requester.onShowPlayButton();
            break;
        default:
            break;
        }
    }
}
