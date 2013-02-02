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
package com.luxoft.ivilink.samples.media.players.video;

import org.videolan.vlc.WeakHandler;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.res.Configuration;
import android.graphics.PixelFormat;
import android.media.AudioManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.Toast;

import com.luxoft.ivilink.samples.media.MediaPlayerService;
import com.luxoft.ivilink.samples.media.PlaylistActivity;
import com.luxoft.ivilink.samples.media.R;
import com.luxoft.ivilink.samples.media.interfaces.ButtonProcessor;
import com.luxoft.ivilink.samples.media.interfaces.MediaGui;
import com.luxoft.ivilink.samples.media.players.VlcMediaPlayer;
import com.luxoft.ivilink.samples.media.players.VlcMediaPlayer.SurfaceScaleCallback;
import com.luxoft.ivilink.samples.media.players.ipc.ButtonProcessorProxy;
import com.luxoft.ivilink.samples.media.players.ipc.MediaGuiServerIpcProxy;
import com.luxoft.ivilink.samples.media.players.ipc.PlayerLogicProxy;
import com.luxoft.ivilink.sdk.android.lib.utils.ForApp;
import com.luxoft.ivilink.sdk.android.lib.utils.log.BigLog;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

public class VideoPlayerActivity extends Activity implements MediaGui.Client,
        PlayerLogicProxy.TerminationHandler, SurfaceScaleCallback {

    private final static String TAG = VideoPlayerActivity.class.getName();
    private final static int POP_UP_HIDE_MILLIS = 3000;
    private Handler mHandler = new Handler(Looper.getMainLooper());

    private long mPopupTouchedTimestamp;
    private boolean mIsMuted;
    private View mPopUp;
    private View mPauseOverlay;
    private View mVideoCover;
    private Button mPlayBtn, mPauseBtn, mStopBtn, mPrevBtn, mNextBtn, mSyncBtn, mToggleBtn;
    private FrameLayout mSurfaceFrame;
    private SurfaceView mSurfaceView;
    private SurfaceHolder mSurfaceHolder;

    private static final int SURFACE_SIZE = 3;
    private int mVideoWidth, mVideoHeight, mSarNum, mSarDen;
    private int mSurfaceAlign;

    private VlcMediaPlayer mPlayerImpl;
    private ButtonProcessorProxy mButtonProcessorProxy;
    private ButtonProcessor mButtonProcessor;
    private MediaGui.Server mUiProxy;

    @Override
    public void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        setVolumeControlStream(AudioManager.STREAM_MUSIC);
        Log.v(TAG, Logging.getCurrentMethodName(icicle));
        mUiProxy = new MediaGuiServerIpcProxy(this);
        mPlayerImpl = VlcMediaPlayer.getExistingInstance();
        initUi();
        coverVideoSurface();
        showVoutDialog();
        setupSurface();
        mButtonProcessorProxy = new ButtonProcessorProxy(this);
        mButtonProcessor = mButtonProcessorProxy.getMockInstance();
    }

    private void initUi() {
        Log.v(TAG, Logging.getCurrentMethodName());
        ForApp.setWindowFlagsToKeepDeviceOn(this);
        setContentView(R.layout.video);
        mPauseOverlay = findViewById(R.id.pauseOverlay);
        mVideoCover = findViewById(R.id.videoCoverBlack);
        mSurfaceView = (SurfaceView) findViewById(R.id.videoView);
        mSurfaceView.setOnTouchListener(new VideoOnClickListener());
        mPopUp = findViewById(R.id.videoAdditionalButtons);
        mPopUp.setVisibility(View.GONE);
        mSurfaceFrame = (FrameLayout) findViewById(R.id.videoHostFrame);
        initButtons(new ButtonsPopupListener());
    }

    private void setupSurface() {
        Log.v(TAG, Logging.getCurrentMethodName());
        mSurfaceHolder = mSurfaceView.getHolder();
        mSurfaceHolder.setFormat(PixelFormat.RGBX_8888);
        PixelFormat info = new PixelFormat();
        PixelFormat.getPixelFormatInfo(PixelFormat.RGBX_8888, info);
        int pitch = info.bytesPerPixel;
        mSurfaceAlign = 16 / pitch - 1;
        mSurfaceHolder.addCallback(mSurfaceCallback);
    }

    @Override
    public void onResume() {
        super.onResume();
        Log.v(TAG, Logging.getCurrentMethodName());
        mUiProxy.registerClient(this);
        sendBroadcast(new Intent(MediaPlayerService.class.getName())
                .putExtra(MediaPlayerService.BACKGROUND_MODE, false));
    }

    @Override
    public void onPause() {
        super.onPause();
        Log.v(TAG, Logging.getCurrentMethodName());
        mUiProxy.unregisterClient(this);
        finish();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.v(TAG, Logging.getCurrentMethodName());
        hideProgressDialog();
        ((MediaGuiServerIpcProxy) mUiProxy).onContextDestroyed();
        mButtonProcessorProxy.onContextDestroyed();
    }

    @Override
    public void onBackPressed() {
        mButtonProcessor.onStopClicked();
        startActivity(new Intent(this, PlaylistActivity.class));
    }

    @Override
    public void onUserLeaveHint() {
        super.onUserLeaveHint();
        BigLog.i(TAG, Logging.getCurrentMethodName(), '~');
        sendBroadcast(new Intent(MediaPlayerService.class.getName())
                .putExtra(MediaPlayerService.BACKGROUND_MODE, true));
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        Log.v(TAG, Logging.getCurrentMethodName(newConfig));
        super.onConfigurationChanged(newConfig);
    }

    private void initButtons(View.OnClickListener ocl) {
        Log.v(TAG, Logging.getCurrentMethodName());
        mNextBtn = ((Button) findViewById(R.id.videoAddNext));
        mNextBtn.setOnClickListener(ocl);
        mPrevBtn = (Button) findViewById(R.id.videoAddPrev);
        mPrevBtn.setOnClickListener(ocl);
        mStopBtn = (Button) findViewById(R.id.videoAddStop);
        mStopBtn.setOnClickListener(ocl);
        mPlayBtn = (Button) findViewById(R.id.videoAddPlay);
        mPlayBtn.setOnClickListener(ocl);
        mPauseBtn = (Button) findViewById(R.id.videoAddPause);
        mPauseBtn.setOnClickListener(ocl);
        mSyncBtn = (Button) findViewById(R.id.syncPort);
        mSyncBtn.setOnClickListener(ocl);
        mToggleBtn = (Button) findViewById(R.id.togglePort);
        mToggleBtn.setOnClickListener(ocl);
    }

    private class ButtonsPopupListener implements android.view.View.OnClickListener {
        public void onClick(View v) {
            Log.v(TAG, Logging.getCurrentMethodName(v));
            mPopupTouchedTimestamp = System.currentTimeMillis();
            mHandler.postDelayed(new AddButtonsHider(mPopupTouchedTimestamp), POP_UP_HIDE_MILLIS);
            switch (v.getId()) {
            case R.id.videoAddNext:
                mButtonProcessor.onNextClicked();
                break;
            case R.id.videoAddPause:
                mButtonProcessor.onPauseClicked();
                break;
            case R.id.videoAddPlay:
                mButtonProcessor.onPlayClicked();
                break;
            case R.id.videoAddPrev:
                mButtonProcessor.onPrevClicked();
                break;
            case R.id.videoAddStop:
                mButtonProcessor.onStopClicked();
                break;
            case R.id.syncPort:
                mButtonProcessor.onSyncClicked();
                break;
            case R.id.togglePort:
                mButtonProcessor.onToggleClicked();
                break;
            }
        }
    }

    private void setButtonsPopupVisibility(int visibility) {
        Log.v(TAG, Logging.getCurrentMethodName(visibility));
        mPopupTouchedTimestamp = System.currentTimeMillis();
        mPopUp.setVisibility(visibility);
        if (visibility == View.VISIBLE) {
            mHandler.postDelayed(new AddButtonsHider(mPopupTouchedTimestamp), POP_UP_HIDE_MILLIS);
        }
    }

    private class VideoOnClickListener implements View.OnTouchListener {
        public boolean onTouch(View v, MotionEvent event) {
            Log.v(TAG, Logging.getCurrentMethodName(v, event));
            if (event.getAction() == MotionEvent.ACTION_UP) {
                if (mPopUp.getVisibility() == View.VISIBLE) {
                    setButtonsPopupVisibility(View.GONE);
                } else {
                    setButtonsPopupVisibility(View.VISIBLE);
                }
            }
            return true;
        }
    }

    private class AddButtonsHider implements Runnable {
        private long mTimestamp;

        public AddButtonsHider(long timestamp) {
            this.mTimestamp = timestamp;
        }

        public void run() {
            if (mPopupTouchedTimestamp == mTimestamp) {
                if (mPopUp.getVisibility() == View.VISIBLE) {
                    mPopUp.setVisibility(View.GONE);
                }
            }
        }
    }

    protected final void coverVideoSurface() {
        Log.v(TAG, Logging.getCurrentMethodName());
        mVideoCover.setVisibility(View.VISIBLE);
        mute();
    }

    protected final void uncoverVideoSurface() {
        Log.v(TAG, Logging.getCurrentMethodName());
        mVideoCover.setVisibility(View.GONE);
        unmute();
    }

    private void mute() {
        Log.v(TAG, Logging.getCurrentMethodName());
        if (!mIsMuted) {
            Log.v(TAG, "Was not muted, muting");
            mIsMuted = true;
            ((AudioManager) getSystemService(Context.AUDIO_SERVICE))
                    .setStreamMute(AudioManager.STREAM_MUSIC, mIsMuted);
        }
    }

    private void unmute() {
        Log.v(TAG, Logging.getCurrentMethodName());
        if (mIsMuted) {
            Log.v(TAG, "Was muted, unmuting");
            mIsMuted = false;
            ((AudioManager) getSystemService(Context.AUDIO_SERVICE))
                    .setStreamMute(AudioManager.STREAM_MUSIC, mIsMuted);
        }

    }

    public void onShowPauseButton() {
        Log.v(TAG, Logging.getCurrentMethodName());
        mPauseBtn.setVisibility(View.VISIBLE);
    }

    public void onHidePauseButton() {
        Log.v(TAG, Logging.getCurrentMethodName());
        mPauseBtn.setVisibility(View.GONE);
    }

    public void onShowPlayButton() {
        Log.v(TAG, Logging.getCurrentMethodName());
        mPlayBtn.setVisibility(View.VISIBLE);
    }

    public void onHidePlayButton() {
        Log.v(TAG, Logging.getCurrentMethodName());
        mPlayBtn.setVisibility(View.GONE);
    }

    public void onActivateSync() {
        Log.v(TAG, Logging.getCurrentMethodName());
        mSyncBtn.setBackgroundResource(R.drawable.port_sync_active);
    }

    public void onDeactivateSync() {
        Log.v(TAG, Logging.getCurrentMethodName());
        mSyncBtn.setBackgroundResource(R.drawable.port_sync_inactive);
    }

    public void onEnableToggle() {
        Log.v(TAG, Logging.getCurrentMethodName());
        mToggleBtn.setEnabled(true);
        mToggleBtn.setBackgroundResource(R.drawable.toggle_tab_inactive_port);
    }

    public void onHighlightToggle() {
        Log.v(TAG, Logging.getCurrentMethodName());
        mToggleBtn.setEnabled(true);
        mToggleBtn.setBackgroundResource(R.drawable.toggle_tab_active_port);
    }

    public void onDisableToggle() {
        Log.v(TAG, Logging.getCurrentMethodName());
        mToggleBtn.setEnabled(false);
        mToggleBtn.setBackgroundResource(R.drawable.toggle_tab_disabled_port);

    }

    public void onSetSelectedPosition(int position) {
        Log.v(TAG, Logging.getCurrentMethodName(position));
    }

    public void onShowPlayIcon() {
        Log.v(TAG, Logging.getCurrentMethodName());
        mPauseOverlay.setVisibility(View.GONE);
    }

    public void onShowPauseIcon() {
        Log.v(TAG, Logging.getCurrentMethodName());
        mPauseOverlay.setVisibility(View.VISIBLE);
    }

    public void onShowNoIcon() {
        Log.v(TAG, Logging.getCurrentMethodName());
        mPauseOverlay.setVisibility(View.GONE);
    }

    public void onAddToPlaylist(String name, boolean isLocal, boolean isAudio) {
        Log.v(TAG, Logging.getCurrentMethodName(name, isLocal, isAudio));
    }

    public void onClearPlaylist() {
        Log.v(TAG, Logging.getCurrentMethodName());
    }

    public void onLockGUI() {
        Log.v(TAG, Logging.getCurrentMethodName());
    }

    public void onUnlockGUI() {
        Log.v(TAG, Logging.getCurrentMethodName());
    }

    public void onCurrentlySelectedTrack(String name) {
        Toast toast = Toast.makeText(this, "Current video: " + name, Toast.LENGTH_LONG);
        toast.setGravity(Gravity.CENTER, 0, 0);
        toast.show();
    }

    public void onTerminationNotification() {
        Log.v(TAG, Logging.getCurrentMethodName());
        unmute();
    }

    /**
     * Invoked by the VlcMediaPlayer.
     */
    public void setSurfaceSize(int width, int height, int sar_num, int sar_den) {
        Log.v(TAG, Logging.getCurrentMethodName(width, height, sar_num, sar_den));
        if (width * height == 0)
            return;
        // store video size
        mVideoHeight = height;
        mVideoWidth = width;
        mSarNum = sar_num;
        mSarDen = sar_den;
        Message msg = mHandler.obtainMessage(SURFACE_SIZE);
        mVlcHandler.sendMessage(msg);
    }

    private final Handler mVlcHandler = new VideoPlayerHandler(this);

    private static class VideoPlayerHandler extends WeakHandler<VideoPlayerActivity> {
        public VideoPlayerHandler(VideoPlayerActivity mainActivity) {
            super(mainActivity);
        }

        @Override
        public void handleMessage(Message msg) {
            Log.v(TAG, Logging.getCurrentMethodName());
            VideoPlayerActivity activity = getOwner();
            if (activity == null) // WeakReference could be GC'ed early
                return;

            switch (msg.what) {
            case SURFACE_SIZE:
                activity.changeSurfaceSize();
                break;
            }
        }
    };

    private void changeSurfaceSize() {
        Log.v(TAG, Logging.getCurrentMethodName());
        uncoverVideoSurface();
        hideProgressDialog();
        // get screen size
        int dw = getWindow().getDecorView().getWidth();
        int dh = getWindow().getDecorView().getHeight();

        boolean isPortrait = getResources().getConfiguration().orientation == Configuration.ORIENTATION_PORTRAIT;
        if (dw > dh && isPortrait || dw < dh && !isPortrait) {
            int d = dw;
            dw = dh;
            dh = d;
        }
        if (dw * dh == 0)
            return;

        // compute the aspect ratio
        double ar, vw;
        double density = (double) mSarNum / (double) mSarDen;
        if (density == 1.0) {
            /* No indication about the density, assuming 1:1 */
            vw = mVideoWidth;
            ar = (double) mVideoWidth / (double) mVideoHeight;
        } else {
            /* Use the specified aspect ratio */
            vw = mVideoWidth * density;
            ar = vw / mVideoHeight;
        }

        // compute the display aspect ratio
        double dar = (double) dw / (double) dh;

        if (dar < ar)
            dh = (int) (dw / ar);
        else
            dw = (int) (dh * ar);

        // align width on 16bytes
        int alignedWidth = (mVideoWidth + mSurfaceAlign) & ~mSurfaceAlign;

        // force surface buffer size
        mSurfaceHolder.setFixedSize(alignedWidth, mVideoHeight);

        // set display size
        LayoutParams lp = mSurfaceView.getLayoutParams();
        lp.width = dw * alignedWidth / mVideoWidth;
        lp.height = dh;
        mSurfaceView.setLayoutParams(lp);

        // set frame size (crop if necessary)
        lp = mSurfaceFrame.getLayoutParams();
        lp.width = dw;
        lp.height = dh;
        mSurfaceFrame.setLayoutParams(lp);

        mSurfaceView.invalidate();
    }

    private void showVoutDialog() {
        Log.v(TAG, Logging.getCurrentMethodName());
        runOnUiThread(new Runnable() {
            public void run() {
                findViewById(R.id.progressAvSynchro).setVisibility(View.VISIBLE);
            }
        });
    }

    private void hideProgressDialog() {
        Log.v(TAG, Logging.getCurrentMethodName());
        runOnUiThread(new Runnable() {
            public void run() {
                findViewById(R.id.progressAvSynchro).setVisibility(View.GONE);
            }
        });
    }

    private final SurfaceHolder.Callback mSurfaceCallback = new Callback() {
        public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
            Log.v(TAG, Logging.getCurrentMethodName(holder, format, width, height));
            try {
                mPlayerImpl.attachSurface(holder.getSurface(), VideoPlayerActivity.this, width,
                                          height);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        public void surfaceCreated(SurfaceHolder holder) {
            Log.v(TAG, Logging.getCurrentMethodName(holder));
        }

        public void surfaceDestroyed(SurfaceHolder holder) {
            Log.v(TAG, Logging.getCurrentMethodName(holder));
            try {
                mPlayerImpl.detachSurface();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    };
}
