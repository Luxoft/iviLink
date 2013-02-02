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

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import org.videolan.vlc.EventManager;
import org.videolan.vlc.LibVLC;
import org.videolan.vlc.LibVlcException;
import org.videolan.vlc.Logging;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.Surface;

import com.luxoft.ivilink.samples.media.MediaPlayerService;
import com.luxoft.ivilink.samples.media.interfaces.Player;
import com.luxoft.ivilink.samples.media.interfaces.PlayerEventProcessor;
import com.luxoft.ivilink.samples.media.players.ipc.CommunicationProtocol.FromLogicToPlayer;

/**
 * A Singleton that plays media using the LibVLC. It should not be created and
 * used in the main process (the one that has the {@link MediaPlayerService}
 * running), because there is a hidden native libvlc instance there (that is
 * used as a streaming server) and they will conflict.
 */
public class VlcMediaPlayer implements Player {
    private static final String TAG = VlcMediaPlayer.class.getName();
    private PlayerEventProcessor mEventsProcessor;
    private static VlcMediaPlayer mInstance;
    private LibVLC mLibVlcInstance;
    private String mCurrentPath;
    private ExecutorService mExecutor = Executors.newCachedThreadPool();

    private FromLogicToPlayer mLatestCommand;

    public interface SurfaceScaleCallback {
        public void setSurfaceSize(int width, int height, int sar_num, int sar_den);
    }

    private VlcMediaPlayer(PlayerEventProcessor processor) {
        Log.v(TAG, Logging.getCurrentMethodName(processor));
        mEventsProcessor = processor;
        try {
            mLibVlcInstance = LibVLC.getInstance();
        } catch (LibVlcException e) {
            e.printStackTrace();
            mEventsProcessor.onError();
        }
        EventManager.getInstance().addHandler(mVlcEventHandler);
    }

    public static VlcMediaPlayer getInstance(PlayerEventProcessor processor) {
        Log.v(TAG, Logging.getCurrentMethodName(processor));
        synchronized (VlcMediaPlayer.class) {
            if (mInstance == null) {
                mInstance = new VlcMediaPlayer(processor);
            }
            return mInstance;
        }
    }

    /**
     * @return an instance or <code>null</code> if
     *         {@link #getInstance(PlayerEventProcessor processor)} has not been
     *         called beforehand.
     */
    public static VlcMediaPlayer getExistingInstance() {
        Log.v(TAG, Logging.getCurrentMethodName());
        synchronized (VlcMediaPlayer.class) {
            return mInstance;
        }
    }

    /**
     * Starts asynchronously attempting to open the path. If it cannot be opened
     * in {@link PlayerConsts.RETRIES_LIMIT} attempts, an error is reported to
     * the PlayerEventProcessor (the onError() call). If the path has been
     * opened successfully, then PlayerEventProcessor's onConnectedToStream() is
     * called.
     * 
     * @param path
     *            url to a media stream or a file
     */
    public void onStartPlayback(String path) {
        Log.v(TAG, Logging.getCurrentMethodName(path));
        mCurrentPath = path;
        mLatestCommand = FromLogicToPlayer.PLAY;
        mExecutor.execute(new ReadMediaTask(mLibVlcInstance, mCurrentPath));
    }

    public void onPausePlayback() {
        Log.v(TAG, Logging.getCurrentMethodName());
        mLatestCommand = FromLogicToPlayer.PAUSE;
        mLibVlcInstance.pause();
    }

    public void onResumePlayback() {
        Log.v(TAG, Logging.getCurrentMethodName());
        mLatestCommand = FromLogicToPlayer.RESUME;
        mLibVlcInstance.play();
    }

    public void onStopPlayback() {
        Log.v(TAG, Logging.getCurrentMethodName());
        mLatestCommand = FromLogicToPlayer.STOP;
        mLibVlcInstance.stop();
    }

    /**
     * Played video will be rendered to the provided surface
     * 
     * @param surface
     * @param scaleCallback
     * @param width
     * @param height
     */
    public void attachSurface(Surface surface, SurfaceScaleCallback scaleCallback, int width,
            int height) {
        Log.v(TAG, Logging.getCurrentMethodName());
        mLibVlcInstance.attachSurface(surface, scaleCallback, width, height);
    }

    public void detachSurface() {
        Log.v(TAG, Logging.getCurrentMethodName());
        mLibVlcInstance.detachSurface();
    }

    public boolean isPlaying() {
        return mLibVlcInstance.isPlaying();
    }

    private Handler mVlcEventHandler = new Handler(Looper.getMainLooper()) {
        public int mErrorsCounter = 0;

        @Override
        public void handleMessage(Message msg) {
            Log.v(TAG, Logging.getCurrentMethodName(msg));
            int event = msg.getData().getInt("event");
            if (event == EventManager.MediaPlayerPlaying) {
                Log.i(TAG, "connected to the stream " + mCurrentPath);
                Log.i(TAG, "latest command: " + mLatestCommand);
                // checking, since this event might also come when resuming
                if (mLatestCommand == FromLogicToPlayer.PLAY) {
                    mEventsProcessor.onConnectedToStream();
                }
                mErrorsCounter = 0;
            } else if (event == EventManager.MediaPlayerEndReached) {
                Log.i(TAG, "media player end reached, reporting completion");
                mEventsProcessor.onCompletion();
            } else if (event == EventManager.MediaPlayerEncounteredError) {
                // occurs when trying to connect to a stream that is not ready
                mErrorsCounter++;
                Log.i(TAG, "media player error number " + mErrorsCounter);
                if (mErrorsCounter > PlayerConsts.RETRIES_LIMIT) {
                    Log.i(TAG, "error number has exceeded the limit, reporting to player logic");
                    mEventsProcessor.onError();
                    mErrorsCounter = 0;
                } else {
                    Log.i(TAG, "media player error number is still below the limit, retrying");
                    mExecutor.execute(new ReadMediaTask(mLibVlcInstance, mCurrentPath,
                            PlayerConsts.SLEEP_BEFORE_RETRY_MILLIS));
                }
            }
        }
    };

}
