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

import android.app.Activity;
import android.app.ActivityManager;
import android.app.ActivityManager.RunningServiceInfo;
import android.content.Context;
import android.content.Intent;

import com.luxoft.ivilink.samples.media.controllers.SharedState;
import com.luxoft.ivilink.samples.media.players.video.VideoPlayerActivity;
import com.luxoft.ivilink.samples.media.structs.MediaType;

public class ActivitiesDispatcher extends Activity {
    @Override
    public void onResume() {
        super.onResume();
        sendBroadcast(new Intent(MediaPlayerService.class.getName())
                .putExtra(MediaPlayerService.BACKGROUND_MODE, false));
        if (!isPlayerServiceRunning()) {
            Bridge.createInstance(this);
            startService(new Intent(this, MediaPlayerService.class));
        } else {
            startActivity(new Intent(this, PlaylistActivity.class));
            if (SharedState.getInstance().getCurrentMediaType() == MediaType.VIDEO) {
                startActivity(new Intent(this, VideoPlayerActivity.class));
            }
        }
        finish();
    }

    private boolean isPlayerServiceRunning() {
        ActivityManager manager = (ActivityManager) this.getSystemService(Context.ACTIVITY_SERVICE);
        for (RunningServiceInfo rsi : manager.getRunningServices(Integer.MAX_VALUE)) {
            if (MediaPlayerService.class.getName().equals(rsi.service.getClassName())) {
                return true;
            }
        }
        return false;
    }
}
