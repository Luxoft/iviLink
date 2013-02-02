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
package com.luxoft.ivilink.samples.media.helpers;

import android.content.Context;
import android.util.Log;

import com.luxoft.ivilink.sdk.android.lib.utils.log.BigLog;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

public class VlcServerLoader {
    private Thread mVlcThread;
    private static final String TAG = VlcServerLoader.class.getName();
    private static final char logHighlight = 'L';
    private static VlcServerLoader instance = new VlcServerLoader();

    static {
        System.loadLibrary("vlcjni");
    }

    private VlcServerLoader() {
    }

    public static VlcServerLoader getInstance() {
        return instance;
    }

    public synchronized boolean launchVlcServer(Context context, final int pipeReadDescriptor) {
        BigLog.w(TAG, Logging.getCurrentMethodName(context, pipeReadDescriptor), logHighlight);
        if (mVlcThread != null && mVlcThread.isAlive()) {
            BigLog.w(TAG, "vlc server has already started", logHighlight);
            return false;
        }
        Log.v(TAG, "pipe descriptor is: " + pipeReadDescriptor);
        mVlcThread = new Thread(new Runnable() {
            public void run() {
                BigLog.w(TAG, "Starting VLC server...", logHighlight);
                launchVlc(pipeReadDescriptor);
                BigLog.w(TAG, "VLC server has stopped!", logHighlight);
            }
        });
        mVlcThread.start();
        return true;
    }

    private native void launchVlc(int pipeFds);
}
