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

import org.videolan.vlc.LibVLC;

public class ReadMediaTask implements Runnable {

    private LibVLC mInstance;
    private String mPath;
    private int mSleepTimeoutMillis;
    private boolean mIsNoVideo;

    private static final int NO_SLEEP_TIMEOUT = 0;

    public ReadMediaTask(LibVLC instance, String path, boolean isNoVideo,
            int timeoutBeforeTryingMillis) {
        mInstance = instance;
        mPath = path;
        mSleepTimeoutMillis = timeoutBeforeTryingMillis;
        mIsNoVideo = isNoVideo;
    }

    public ReadMediaTask(LibVLC instance, String path) {
        this(instance, path, false, NO_SLEEP_TIMEOUT);
    }

    public ReadMediaTask(LibVLC instance, String path, boolean isNoVideo) {
        this(instance, path, isNoVideo, NO_SLEEP_TIMEOUT);
    }

    public ReadMediaTask(LibVLC instance, String path, int timeoutBeforeTryingMillis) {
        this(instance, path, false, timeoutBeforeTryingMillis);
    }

    public void run() {
        if (mSleepTimeoutMillis != NO_SLEEP_TIMEOUT) {
            try {
                Thread.sleep(mSleepTimeoutMillis);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        mInstance.readMedia(mPath, mIsNoVideo);
    }

}
