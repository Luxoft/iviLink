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

import android.content.Context;
import android.view.Gravity;
import android.widget.Toast;

import com.luxoft.ivilink.samples.media.interfaces.MediaGui;

class ClientStub implements MediaGui.Client {

    private Context mContext;

    public ClientStub(Context context) {
        mContext = context;
    }

    public void onCurrentlySelectedTrack(String name) {
        Toast toast = Toast.makeText(mContext, "Current song: " + name, Toast.LENGTH_LONG);
        toast.setGravity(Gravity.CENTER, 0, 0);
        toast.show();
    }

    public void onShowPauseButton() {
    }

    public void onHidePauseButton() {
    }

    public void onShowPlayButton() {
    }

    public void onHidePlayButton() {
    }

    public void onActivateSync() {
    }

    public void onDeactivateSync() {
    }

    public void onEnableToggle() {
    }

    public void onHighlightToggle() {
    }

    public void onDisableToggle() {
    }

    public void onSetSelectedPosition(int position) {
    }

    public void onShowPlayIcon() {
    }

    public void onShowPauseIcon() {
    }

    public void onShowNoIcon() {
    }

    public void onAddToPlaylist(String name, boolean isLocal, boolean isAudio) {
    }

    public void onClearPlaylist() {
    }

    public void onLockGUI() {
    }

    public void onUnlockGUI() {
    }

}
