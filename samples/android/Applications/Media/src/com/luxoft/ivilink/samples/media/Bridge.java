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
import android.util.Log;

import com.luxoft.ivilink.samples.media.helpers.MediaSource;
import com.luxoft.ivilink.samples.media.interfaces.ButtonProcessor;
import com.luxoft.ivilink.samples.media.interfaces.JniPlayerCallback;
import com.luxoft.ivilink.samples.media.interfaces.MediaGui;
import com.luxoft.ivilink.sdk.android.lib.utils.Assertion;
import com.luxoft.ivilink.sdk.android.lib.utils.ForApp;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

/**
 * Communicates with the native application core.
 */
public class Bridge implements ButtonProcessor {
    private boolean hasNativeStarted;
    private static Bridge mInstance;
    private String mInternalPath;
    private String mLaunchInfo;
    private final static String TAG = Bridge.class.getName();

    static {
        System.loadLibrary("iviLinkApplicationLibraryShared");
        System.loadLibrary("application");
    }

    public static Bridge createInstance(Activity activity) {
        Log.v(TAG, Logging.getCurrentMethodName(activity));
        synchronized (Bridge.class) {
            if (mInstance == null) {
                mInstance = new Bridge(activity);
            }
        }
        return mInstance;
    }

    private Bridge(Activity activity) {
        Log.v(TAG, Logging.getCurrentMethodName(activity));
        mInternalPath = ForApp.getInternalPath(activity);
        mLaunchInfo = ForApp.getLaunchInfo(activity);
    }

    private native void storeObjects(MediaGui gui, JniPlayerCallback player, MediaSource source);

    public static Bridge getExistingInstance() {
        Assertion.check(mInstance != null, "Bridge has not been created!");
        return mInstance;
    }

    public synchronized void startApp(final MediaGui gui, final JniPlayerCallback player,
            MediaSource source) {
        Log.v(TAG, Logging.getCurrentMethodName());
        if (!hasNativeStarted) {
            new Thread(new Runnable() {
                public void run() {
                    try {
                        storeObjects(gui, player, MediaSource.getInstance());
                        startApp(ForApp.getServicePath(), mLaunchInfo, mInternalPath,
                                 MediaSource.MEDIA_PATH);
                        hasNativeStarted = true;
                    } catch (UnsatisfiedLinkError e) {
                        e.printStackTrace();
                    } catch (NoSuchMethodError e) {
                        e.printStackTrace();
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
            }).start();
        }
    }

    private native void startApp(String serviceRepo, String launchInfo, String internalPath,
            String pathToMedia);

    private native void playClicked();

    private native void pauseClicked();

    private native void stopClicked();

    private native void nextClicked();

    private native void prevClicked();

    private native void posClicked(int position, int oldPosition);

    private native void completed();

    private native void syncClicked();

    private native void toggleClicked();

    private native void playbackStarted();

    // from the ButtonProcessor interface
    public void onPlayClicked() {
        Log.v(TAG, Logging.getCurrentMethodName());
        if (hasNativeStarted)
            playClicked();
    }

    public void onPauseClicked() {
        Log.v(TAG, Logging.getCurrentMethodName());
        if (hasNativeStarted)
            pauseClicked();
    }

    public void onStopClicked() {
        Log.v(TAG, Logging.getCurrentMethodName());
        if (hasNativeStarted)
            stopClicked();
    }

    public void onNextClicked() {
        Log.v(TAG, Logging.getCurrentMethodName());
        if (hasNativeStarted)
            nextClicked();
    }

    public void onPrevClicked() {
        Log.v(TAG, Logging.getCurrentMethodName());
        if (hasNativeStarted)
            prevClicked();
    }

    public void onPositionClicked(int position, int oldPosition) {
        Log.v(TAG, Logging.getCurrentMethodName(position, oldPosition));
        if (hasNativeStarted)
            posClicked(position, oldPosition);
    }

    public void onCompletion() {
        Log.v(TAG, Logging.getCurrentMethodName());
        if (hasNativeStarted)
            completed();
    }

    public void onSyncClicked() {
        Log.v(TAG, Logging.getCurrentMethodName());
        if (hasNativeStarted)
            syncClicked();
    }

    public void onToggleClicked() {
        Log.v(TAG, Logging.getCurrentMethodName());
        if (hasNativeStarted)
            toggleClicked();
    }

    public void onPlaybackStarted() {
        Log.v(TAG, Logging.getCurrentMethodName());
        if (hasNativeStarted)
            playbackStarted();
    }

}
