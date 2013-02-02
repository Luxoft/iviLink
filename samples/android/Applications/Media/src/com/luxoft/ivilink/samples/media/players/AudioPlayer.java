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

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;

import com.luxoft.ivilink.samples.media.MediaPlayerService;
import com.luxoft.ivilink.samples.media.PlaylistActivity;
import com.luxoft.ivilink.samples.media.interfaces.ButtonProcessor;
import com.luxoft.ivilink.samples.media.interfaces.PlayerEventProcessor;
import com.luxoft.ivilink.samples.media.players.ipc.AudioPlayerProxy;
import com.luxoft.ivilink.samples.media.players.ipc.PlayerProxy;
import com.luxoft.ivilink.sdk.android.lib.utils.log.BigLog;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

/**
 * In addition to {@link AbstractPlayer} functionality, displays a
 * {@link ProgressDialog} when onStartPlayback is called if the application is
 * in the foreground mode.
 */
public class AudioPlayer extends AbstractPlayer {
    private final static String TAG = AudioPlayer.class.getName();
    private static ProgressDialog mProgress;
    private static Handler mHandler = new Handler(Looper.getMainLooper());
    private static Activity mProgressDisplayContext;

    static public void registerActivityContext(final Activity activity) {
        runOnUiThread(new Runnable() {
            public void run() {
                mProgressDisplayContext = activity;
            }
        });
    }

    static public void unregisterActivityContext() {
        hideDialog();
        runOnUiThread(new Runnable() {
            public void run() {
                mProgressDisplayContext = null;
            }
        });
    }

    public AudioPlayer(Context context, ButtonProcessor buttonProcessor,
            PlayerEventProcessor eventProcessor) {
        super(context, buttonProcessor, eventProcessor);
    }

    DialogInterface.OnCancelListener mDefaultOnCancelListener = new DialogInterface.OnCancelListener() {
        public void onCancel(DialogInterface dialog) {
            mPlayerProxy.onStopPlayback();
            mButtonProcessor.onStopClicked();
        }
    };

    @Override
    public void onStartPlayback(String path) {
        Log.v(tag(), Logging.getCurrentMethodName(path));
        if (!MediaPlayerService.getExistingInstance().isInBackgroundMode()) {
            mContext.startActivity(new Intent(mContext, PlaylistActivity.class)
                    .addFlags(Intent.FLAG_ACTIVITY_NEW_TASK));
        }
        super.onStartPlayback(path);
        createDialog("Preparing media...", mDefaultOnCancelListener);
    }

    @Override
    public void onStopPlayback() {
        hideDialog();
        super.onStopPlayback();
    }

    @Override
    protected String tag() {
        return TAG;
    }

    @Override
    public void onError() {
        Log.v(tag(), Logging.getCurrentMethodName());
        hideDialog();
        super.onError();
    }

    @Override
    public void onConnectedToStream() {
        Log.v(tag(), Logging.getCurrentMethodName());
        hideDialog();
        super.onConnectedToStream();
    }

    @Override
    protected PlayerProxy createPlayerProxy(Context context, ButtonProcessor processor) {
        return new AudioPlayerProxy(context, processor, (PlayerEventProcessor) this);
    }

    private static void hideDialog() {
        BigLog.e(TAG, Logging.getCurrentMethodName(), highlight);
        runOnUiThread(new Runnable() {
            public void run() {
                if (mProgress != null && mProgress.isShowing()) {
                    mProgress.dismiss();
                    mProgress = null;
                }
            }
        });
    }

    private void createDialog(final String message, final DialogInterface.OnCancelListener listener) {
        BigLog.e(tag(), Logging.getCurrentMethodName(message, listener), highlight);
        runOnUiThread(new Runnable() {
            public void run() {
                if (mProgressDisplayContext != null) {
                    if (mProgress != null && mProgress.isShowing()) {
                        mProgress.dismiss();
                    }
                    mProgress = new ProgressDialog(mProgressDisplayContext);
                    mProgress.setTitle("");
                    mProgress.setMessage(message);
                    mProgress.setCanceledOnTouchOutside(false);
                    mProgress.setCancelable(true);
                    mProgress.setOnCancelListener(listener);
                    mProgress.show();
                }
            }
        });

    }

    static private void runOnUiThread(Runnable runnable) {
        if (Looper.getMainLooper().equals(Looper.myLooper())) {
            runnable.run();
        } else {
            mHandler.post(runnable);
        }
    }
}
