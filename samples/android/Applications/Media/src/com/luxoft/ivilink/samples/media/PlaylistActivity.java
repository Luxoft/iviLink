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

import java.util.ArrayList;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Intent;
import android.content.res.Configuration;
import android.media.AudioManager;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.ListView;

import com.luxoft.ivilink.samples.media.customwidgets.SelectedArrayAdapter;
import com.luxoft.ivilink.samples.media.helpers.MediaSource;
import com.luxoft.ivilink.samples.media.interfaces.ButtonProcessor;
import com.luxoft.ivilink.samples.media.interfaces.MediaGui;
import com.luxoft.ivilink.samples.media.players.AudioPlayer;
import com.luxoft.ivilink.samples.media.players.ipc.ButtonProcessorProxy;
import com.luxoft.ivilink.samples.media.structs.MediaType;
import com.luxoft.ivilink.samples.media.structs.TrackInformation;
import com.luxoft.ivilink.samples.media.uiserver.LocalMediaGuiProxiesFactory;
import com.luxoft.ivilink.sdk.android.lib.utils.ForApp;
import com.luxoft.ivilink.sdk.android.lib.utils.ProcessesHelper;
import com.luxoft.ivilink.sdk.android.lib.utils.log.BigLog;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

public class PlaylistActivity extends Activity implements OnItemClickListener,
        View.OnClickListener, MediaGui.Client {

    private ProgressDialog mServiceLoadingDialog;

    private final static String TAG = PlaylistActivity.class.getName();
    private final static char LOG_HIGHLIGHT = '~';

    private ButtonProcessor mButtonProcessor;
    private ButtonProcessorProxy mButtonProcessorProxy;

    private Button playButton, pauseButton, stopButton, prevButton, nextButton;
    private Button syncButton, toggleButton;

    private LinearLayout verticalView, horizontalView;
    private ListView mListView;
    private SelectedArrayAdapter mListAdapter;

    private MediaGui.Server mGuiServer;

    private String getTag() {
        return TAG + this.hashCode();
    }

    @Override
    public void onCreate(Bundle icicle) {
        BigLog.i(getTag(), Logging.getCurrentMethodName(icicle), LOG_HIGHLIGHT);
        setVolumeControlStream(AudioManager.STREAM_MUSIC);
        ForApp.setWindowFlagsToKeepDeviceOn(this);
        super.onCreate(icicle);
        MediaSource.getInstance().setMediaGui(this);
        mListAdapter = new SelectedArrayAdapter(this, android.R.layout.simple_list_item_1,
                new ArrayList<TrackInformation>());
        mListAdapter.setNotifyOnChange(true);
        setContentView(R.layout.playlist_layout);
        horizontalView = (LinearLayout) findViewById(R.id.videoLand);
        verticalView = (LinearLayout) findViewById(R.id.videoPort);
        setSuitableConfiguration();

        mButtonProcessorProxy = new ButtonProcessorProxy(this);
        mButtonProcessor = mButtonProcessorProxy.getMockInstance();
        findViewById(R.id.quitApp).setOnClickListener(mQuitAppListener);
        findViewById(R.id.quitAppPort).setOnClickListener(mQuitAppListener);

        mGuiServer = LocalMediaGuiProxiesFactory.getServerProxy();
    }

    @Override
    public void onResume() {
        super.onResume();
        BigLog.i(getTag(), Logging.getCurrentMethodName(), LOG_HIGHLIGHT);
        AudioPlayer.registerActivityContext(this);
        mGuiServer.registerClient(this);
        sendBroadcast(new Intent(MediaPlayerService.class.getName())
                .putExtra(MediaPlayerService.BACKGROUND_MODE, false));
    }

    @Override
    public void onPause() {
        super.onPause();
        BigLog.i(getTag(), Logging.getCurrentMethodName(), LOG_HIGHLIGHT);
        AudioPlayer.unregisterActivityContext();
        mGuiServer.unregisterClient(this);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        BigLog.i(getTag(), Logging.getCurrentMethodName(), LOG_HIGHLIGHT);
        onUnlockGUI();
    }

    @Override
    public void onBackPressed() {
        BigLog.i(getTag(), Logging.getCurrentMethodName(), LOG_HIGHLIGHT);
        sendBroadcast(new Intent(MediaPlayerService.class.getName())
                .putExtra(MediaPlayerService.BACKGROUND_MODE, true));
        super.onBackPressed();
    }

    @Override
    public void onUserLeaveHint() {
        BigLog.i(getTag(), Logging.getCurrentMethodName(), LOG_HIGHLIGHT);
        sendBroadcast(new Intent(MediaPlayerService.class.getName())
                .putExtra(MediaPlayerService.BACKGROUND_MODE, true));
        super.onUserLeaveHint();
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        BigLog.i(getTag(), Logging.getCurrentMethodName(newConfig), LOG_HIGHLIGHT);
        setSuitableConfiguration();
    }

    private void getControls() {
        if (getResources().getConfiguration().orientation == Configuration.ORIENTATION_LANDSCAPE) {
            playButton = (Button) findViewById(R.id.videoPlay);
            pauseButton = (Button) findViewById(R.id.videoPause);
            nextButton = (Button) findViewById(R.id.videoNext);
            prevButton = (Button) findViewById(R.id.videoPrev);
            stopButton = (Button) findViewById(R.id.videoStop);
            syncButton = (Button) findViewById(R.id.sync);
            toggleButton = (Button) findViewById(R.id.toggle);
        } else if (getResources().getConfiguration().orientation == Configuration.ORIENTATION_PORTRAIT) {
            playButton = (Button) findViewById(R.id.videoPlayPort);
            pauseButton = (Button) findViewById(R.id.videoPausePort);
            nextButton = (Button) findViewById(R.id.videoNextPort);
            prevButton = (Button) findViewById(R.id.videoPrevPort);
            stopButton = (Button) findViewById(R.id.videoStopPort);
            syncButton = (Button) findViewById(R.id.syncPort);
            toggleButton = (Button) findViewById(R.id.togglePort);
        }
    }

    private void setSuitableConfiguration() {
        getControls();
        bindControls();
        if (getResources().getConfiguration().orientation == Configuration.ORIENTATION_LANDSCAPE) {
            horizontalView.setVisibility(View.VISIBLE);
            verticalView.setVisibility(View.GONE);
            mListView = (ListView) findViewById(R.id.videolist);
        } else if (getResources().getConfiguration().orientation == Configuration.ORIENTATION_PORTRAIT) {
            verticalView.setVisibility(View.VISIBLE);
            horizontalView.setVisibility(View.GONE);
            mListView = (ListView) findViewById(R.id.videolistPort);
        }
        mListView.setAdapter(mListAdapter);
        mListView.setOnItemClickListener(this);
    }

    private void bindControls() {
        playButton.setOnClickListener(this);
        pauseButton.setOnClickListener(this);
        stopButton.setOnClickListener(this);
        prevButton.setOnClickListener(this);
        nextButton.setOnClickListener(this);
        syncButton.setOnClickListener(this);
        toggleButton.setOnClickListener(this);
    }

    private OnClickListener mQuitAppListener = new OnClickListener() {

        public void onClick(View v) {
            android.os.Process.killProcess(android.os.Process.myPid());
        }
    };

    // from the onClickListener interface
    public void onClick(View v) {
        switch (v.getId()) {
        case R.id.videoPlay:
        case R.id.videoPlayPort:
            BigLog.i(getTag(), "Play clicked", LOG_HIGHLIGHT);
            mButtonProcessor.onPlayClicked();
            break;
        case R.id.videoPause:
        case R.id.videoPausePort:
            BigLog.i(getTag(), "Pause clicked", LOG_HIGHLIGHT);
            mButtonProcessor.onPauseClicked();
            break;
        case R.id.videoStop:
        case R.id.videoStopPort:
            BigLog.i(getTag(), "Stop clicked", LOG_HIGHLIGHT);
            mButtonProcessor.onStopClicked();
            break;
        case R.id.videoNext:
        case R.id.videoNextPort:
            BigLog.i(getTag(), "Next clicked", LOG_HIGHLIGHT);
            mButtonProcessor.onNextClicked();
            break;
        case R.id.videoPrev:
        case R.id.videoPrevPort:
            BigLog.i(getTag(), "Prev clicked", LOG_HIGHLIGHT);
            mButtonProcessor.onPrevClicked();
            break;
        case R.id.sync:
        case R.id.syncPort:
            BigLog.i(getTag(), "Sync clicked", LOG_HIGHLIGHT);
            mButtonProcessor.onSyncClicked();
            break;
        case R.id.toggle:
        case R.id.togglePort:
            BigLog.i(getTag(), "Toggle clicked", LOG_HIGHLIGHT);
            mButtonProcessor.onToggleClicked();
            break;
        default:
            BigLog.e(getTag(), "Unknown button clicked?", LOG_HIGHLIGHT);
        }
    }

    // from the onItemClickListener interface
    public void onItemClick(AdapterView<?> arg0, View arg1, int position, long arg3) {
        mButtonProcessor.onPositionClicked(position, mListAdapter.getSelectedPosition());
    }

    // media gui client implementation

    public void onShowPauseButton() {
        Log.v(getTag(), Logging.getCurrentMethodName());
        findViewById(R.id.videoPause).setVisibility(View.VISIBLE);
        findViewById(R.id.videoPausePort).setVisibility(View.VISIBLE);
    }

    public void onHidePauseButton() {
        Log.v(getTag(), Logging.getCurrentMethodName());
        findViewById(R.id.videoPause).setVisibility(View.GONE);
        findViewById(R.id.videoPausePort).setVisibility(View.GONE);
    }

    public void onShowPlayButton() {
        Log.v(getTag(), Logging.getCurrentMethodName());
        findViewById(R.id.videoPlay).setVisibility(View.VISIBLE);
        findViewById(R.id.videoPlayPort).setVisibility(View.VISIBLE);
    }

    public void onHidePlayButton() {
        Log.v(getTag(), Logging.getCurrentMethodName());
        findViewById(R.id.videoPlay).setVisibility(View.GONE);
        findViewById(R.id.videoPlayPort).setVisibility(View.GONE);
    }

    public void onActivateSync() {
        Log.v(getTag(), Logging.getCurrentMethodName());
        findViewById(R.id.sync).setBackgroundResource(R.drawable.land_sync_active);
        findViewById(R.id.syncPort).setBackgroundResource(R.drawable.port_sync_active);
    }

    public void onDeactivateSync() {
        Log.v(getTag(), Logging.getCurrentMethodName());
        findViewById(R.id.sync).setBackgroundResource(R.drawable.land_sync_inactive);
        findViewById(R.id.syncPort).setBackgroundResource(R.drawable.port_sync_inactive);
    }

    public void onEnableToggle() {
        Log.v(getTag(), Logging.getCurrentMethodName());
        findViewById(R.id.toggle).setEnabled(true);
        findViewById(R.id.togglePort).setEnabled(true);
        findViewById(R.id.toggle).setBackgroundResource(R.drawable.land_toggle);
        findViewById(R.id.togglePort).setBackgroundResource(R.drawable.port_toggle);
    }

    public void onHighlightToggle() {
        Log.v(getTag(), Logging.getCurrentMethodName());
        findViewById(R.id.toggle).setBackgroundResource(R.drawable.toggle_tab_active);
        findViewById(R.id.togglePort).setBackgroundResource(R.drawable.toggle_tab_active_port);
    }

    public void onDisableToggle() {
        Log.v(getTag(), Logging.getCurrentMethodName());
        findViewById(R.id.toggle).setEnabled(false);
        findViewById(R.id.togglePort).setEnabled(false);
        findViewById(R.id.toggle).setBackgroundResource(R.drawable.land_toggle);
        findViewById(R.id.togglePort).setBackgroundResource(R.drawable.port_toggle);
    }

    public void onSetSelectedPosition(int position) {
        Log.v(getTag(), Logging.getCurrentMethodName(position));
        mListAdapter.setSelectedPosition(position);
        mListView.smoothScrollToPosition(position);
    }

    public void onShowPlayIcon() {
        Log.v(getTag(), Logging.getCurrentMethodName());
        mListAdapter.showPlayingIcon();
    }

    public void onShowPauseIcon() {
        Log.v(getTag(), Logging.getCurrentMethodName());
        mListAdapter.showPausedIcon();
    }

    public void onShowNoIcon() {
        Log.v(getTag(), Logging.getCurrentMethodName());
        mListAdapter.hideIcon();
    }

    public void onAddToPlaylist(String name, boolean isLocal, boolean isAudio) {
        Log.v(getTag(), Logging.getCurrentMethodName(name, isLocal, isAudio));
        mListAdapter.add(new TrackInformation(name, isLocal, isAudio ? MediaType.AUDIO
                : MediaType.VIDEO));
        mListAdapter.notifyDataSetChanged();
    }

    public void onClearPlaylist() {
        Log.v(getTag(), Logging.getCurrentMethodName());
        mListAdapter.clear();
        mListAdapter.notifyDataSetChanged();
    }

    public void onLockGUI() {
        Log.v(getTag(), Logging.getCurrentMethodName());
        if (mServiceLoadingDialog == null) {
            mServiceLoadingDialog = new ProgressDialog(PlaylistActivity.this);
            mServiceLoadingDialog.setMessage("Loading...");
            mServiceLoadingDialog.setCancelable(false);
            mServiceLoadingDialog.show();
        }
    }

    public void onUnlockGUI() {
        Log.v(getTag(), Logging.getCurrentMethodName());
        if (mServiceLoadingDialog != null && mServiceLoadingDialog.isShowing()) {
            mServiceLoadingDialog.dismiss();
            mServiceLoadingDialog = null;
        }
    }

    public void onCurrentlySelectedTrack(String name) {
    }

}
