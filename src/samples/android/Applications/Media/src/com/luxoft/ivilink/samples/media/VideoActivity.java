/* 
 * iviLINK SDK, version 1.1.19
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012, Luxoft Professional Corp., member of IBS group
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
import android.content.BroadcastReceiver;
import android.content.res.Configuration;
import android.media.MediaPlayer.TrackInfo;
import android.os.Bundle;
import android.os.Handler;
import android.view.MotionEvent;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.RelativeLayout;
import android.widget.Toast;
import android.widget.VideoView;

import com.luxoft.ivilink.samples.media.controllers.ConnectedController;
import com.luxoft.ivilink.samples.media.customwidgets.SelectedArrayAdapter;
import com.luxoft.ivilink.samples.media.helpers.MediaSource;
import com.luxoft.ivilink.samples.media.helpers.TrackInformation;
import com.luxoft.ivilink.samples.media.interfaces.ButtonProcessor;
import com.luxoft.ivilink.samples.media.interfaces.JniPlayerWrapper;
import com.luxoft.ivilink.samples.media.interfaces.MediaGui;
import com.luxoft.ivilink.samples.media.interfaces.Scalable;
import com.luxoft.ivilink.samples.media.interfaces.Scrollable;
import com.luxoft.ivilink.samples.media.interfaces.VideoVisibilityController;
import com.luxoft.ivilink.samples.media.players.AudioPlayer;
import com.luxoft.ivilink.samples.media.players.VideoPlayer;
import com.luxoft.ivilink.samples.splash.Splash;
import com.luxoft.ivilink.samples.splash.Splash.Length;
import com.luxoft.ivilink.sdk.android.lib.utils.ForApp;
import com.luxoft.ivilink.sdk.android.lib.utils.log.BigLog;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

public class VideoActivity extends Activity implements OnItemClickListener, View.OnClickListener,
		MediaGui, Scalable, Scrollable, VideoVisibilityController {

	private ProgressDialog progressDialog;

	private final static int POPUP_HIDE_MILLIS = 3000;

	private final static String tag = "iviLink.samples.Media.Activity";
	private final static char logHighlight = '~';

	Handler poster;
	private final int VIDEO_INFLATE_TIMEOUT_MILLIS = 500;

	BroadcastReceiver killer;

	private Bridge bridge;

	private ConnectedController currentController;

	ButtonProcessor buttonProcessor;

	private VideoView videoPlayer;
	private RelativeLayout videoViewBackground;
	private Button aPlay, aPause, aStop, aPrev, aNext; // pop-up buttons
	private Button playButton, pauseButton, stopButton, prevButton, nextButton;
	private Button syncButton, toggleButton;

	private RelativeLayout.LayoutParams fullScreenParams;
	private RelativeLayout.LayoutParams windowParams;
	private ImageView videoListCoverPort, videoListCover;

	private AdditionalButtonsListener abl = new AdditionalButtonsListener();
	private ScaleButtonListener sbl = new ScaleButtonListener();

	private long addBtnsTouchedTimestamp;

	private LinearLayout verticalView, horizontalView;
	private ListView listView;
	private SelectedArrayAdapter listAdapter;

	boolean videoErrorFlag, audioErrorFlag;

	boolean isListVisible;

	@Override
	public void onCreate(Bundle icicle) {
		poster = new Handler();
		BigLog.i(tag, Logging.getCurrentMethodName(icicle), logHighlight);
		killer = ForApp.registerReceiverFromKillerApp(this);
		ForApp.setWindowFlagsToKeepDeviceOn(this);
		super.onCreate(icicle);
		MediaSource.getInstance().setMediaGui(this);
		listAdapter = new SelectedArrayAdapter(this, android.R.layout.simple_list_item_1,
				new ArrayList<TrackInformation>());
		listAdapter.setNotifyOnChange(true);
		setContentView(R.layout.video);
		horizontalView = (LinearLayout) findViewById(R.id.videoLand);
		verticalView = (LinearLayout) findViewById(R.id.videoPort);

		videoListCover = ((ImageView) findViewById(R.id.videoListCoverPort));
		videoListCoverPort = ((ImageView) findViewById(R.id.videoListCover));
		videoViewBackground = (RelativeLayout) findViewById(R.id.videoHost);

		videoPlayer = (VideoView) findViewById(R.id.videoView);
		videoPlayer.setVisibility(View.VISIBLE);

		videoPlayer.setVisibility(View.VISIBLE);
		videoPlayer.setOnTouchListener(new VideoOnClickListener());

		initAdditionalButtons(abl);
		setSuitableConfiguration();

		setFullScreenMode();
		listVisible();
		getScaleControls();

		poster.postDelayed(new VideoInflater(), VIDEO_INFLATE_TIMEOUT_MILLIS);

		currentController = new ConnectedController((Scalable) this);

		bridge = Bridge.createInstance(this, (MediaGui) this, (JniPlayerWrapper) currentController);
		buttonProcessor = (ButtonProcessor) bridge;
		AudioPlayer aPlayer = new AudioPlayer(this, buttonProcessor, currentController,
				(Scrollable) this);
		VideoPlayer vPlayer = new VideoPlayer(this, buttonProcessor, currentController,
				(Scrollable) this, videoPlayer, this);
		currentController.setPlayers(aPlayer, vPlayer);

		Splash.displaySplashScreen(this, new Runnable() {
			public void run() {
				BigLog.i(tag, "Splash done!", logHighlight);
				bridge.startApp();
			}
		}, Length.LONG);
	}

	@Override
	public void onPause() {
		super.onPause();
		BigLog.i(tag, Logging.getCurrentMethodName(), logHighlight);
		buttonProcessor.onStopClicked();
		android.os.Process.killProcess(android.os.Process.myPid());
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
		unregisterReceiver(killer);
	}

	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);
		BigLog.i(tag, Logging.getCurrentMethodName(newConfig), logHighlight);
		setSuitableConfiguration();
	}

	class VideoInflater implements Runnable {
		public void run() {
			if (currentController.isVideoPlayingOrPaused()) {
				if (currentController.isFullScreen()) {
					setFullScreenMode();
				} else {
					setWindowMode();
				}
			}
			poster.postDelayed(this, VIDEO_INFLATE_TIMEOUT_MILLIS);
		}
	}

	// from the VideoVisibilityController interface
	public void listVisible() {
		isListVisible = true;
		videoViewBackground.setVisibility(View.GONE);
		videoListCover.setVisibility(View.GONE);
		videoListCoverPort.setVisibility(View.GONE);
	}

	public void videoVisible() {
		isListVisible = false;
		videoViewBackground.setVisibility(View.VISIBLE);
		videoListCover.setVisibility(View.VISIBLE);
		videoListCoverPort.setVisibility(View.VISIBLE);
	}

	private void getScaleControls() {
		((Button) findViewById(R.id.videoScale)).setOnClickListener(sbl);
		((Button) findViewById(R.id.videoAddScale)).setOnClickListener(sbl);
		((Button) findViewById(R.id.videoScalePort)).setOnClickListener(sbl);
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

	private void setAddBtnsVisibility(int visibility) {
		addBtnsTouchedTimestamp = System.currentTimeMillis();
		videoViewBackground.findViewById(R.id.videoAdditionalButtons).setVisibility(visibility);
		if (visibility == View.VISIBLE) {
			new Handler().postDelayed(new AddButtonsHider(addBtnsTouchedTimestamp),
					POPUP_HIDE_MILLIS);
		}
	}

	private class VideoOnClickListener implements View.OnTouchListener {
		public boolean onTouch(View v, MotionEvent event) {
			if (currentController.isFullScreen()) {
				setAddBtnsVisibility(View.VISIBLE);
			}
			return true;
		}
	}

	private void initAdditionalButtons(View.OnClickListener ocl) {
		aNext = ((Button) findViewById(R.id.videoAddNext));
		aNext.setOnClickListener(ocl);
		aPrev = (Button) findViewById(R.id.videoAddPrev);
		aPrev.setOnClickListener(ocl);
		aStop = (Button) findViewById(R.id.videoAddStop);
		aStop.setOnClickListener(ocl);
		aPlay = (Button) findViewById(R.id.videoAddPlay);
		aPlay.setOnClickListener(ocl);
		aPause = (Button) findViewById(R.id.videoAddPause);
		aPause.setOnClickListener(ocl);
	}

	private void setSuitableConfiguration() {
		getControls();
		bindControls();
		videoViewBackground.setBackgroundColor(0);
		if (getResources().getConfiguration().orientation == Configuration.ORIENTATION_LANDSCAPE) {
			horizontalView.setVisibility(View.VISIBLE);
			verticalView.setVisibility(View.GONE);
			listView = (ListView) findViewById(R.id.videolist);
			fullScreenParams = new RelativeLayout.LayoutParams(horizontalView.getLayoutParams());
			videoViewBackground.setBackgroundResource(R.drawable.video_back);
		} else if (getResources().getConfiguration().orientation == Configuration.ORIENTATION_PORTRAIT) {
			verticalView.setVisibility(View.VISIBLE);
			horizontalView.setVisibility(View.GONE);
			fullScreenParams = new RelativeLayout.LayoutParams(verticalView.getLayoutParams());
			listView = (ListView) findViewById(R.id.videolistPort);
			videoViewBackground.setBackgroundResource(R.drawable.port_black);
		}
		if (isListVisible) {
			listVisible();
		} else {
			videoVisible();
		}
		listView.setAdapter(listAdapter);
		listView.setOnItemClickListener(this);
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

	// from the onClickListener interface
	public void onClick(View v) {
		if (!currentController.canProcessButtons()) {
			BigLog.i(tag, "Cannot process buttons in this state", logHighlight);
			return;
		}
		switch (v.getId()) {
		case R.id.videoPlay:
		case R.id.videoPlayPort:
			BigLog.i(tag, "Play clicked", logHighlight);
			buttonProcessor.onPlayClicked();
			break;
		case R.id.videoPause:
		case R.id.videoPausePort:
			BigLog.i(tag, "Pause clicked", logHighlight);
			buttonProcessor.onPauseClicked();
			break;
		case R.id.videoStop:
		case R.id.videoStopPort:
			BigLog.i(tag, "Stop clicked", logHighlight);
			buttonProcessor.onStopClicked();
			break;
		case R.id.videoNext:
		case R.id.videoNextPort:
			BigLog.i(tag, "Next clicked", logHighlight);
			buttonProcessor.onNextClicked();
			break;
		case R.id.videoPrev:
		case R.id.videoPrevPort:
			BigLog.i(tag, "Prev clicked", logHighlight);
			buttonProcessor.onPrevClicked();
			break;
		case R.id.sync:
		case R.id.syncPort:
			BigLog.i(tag, "Sync clicked", logHighlight);
			buttonProcessor.onSyncClicked();
			break;
		case R.id.toggle:
		case R.id.togglePort:
			BigLog.i(tag, "Toggle clicked", logHighlight);
			buttonProcessor.onToggleClicked();
			break;
		default:
			BigLog.e(tag, "Unknown button clicked?", logHighlight);
		}
	}

	// from the onItemClickListener interface
	public void onItemClick(AdapterView<?> arg0, View arg1, int position, long arg3) {
		if (currentController.canProcessButtons())
			buttonProcessor.onPositionClicked(position, listAdapter.getSelectedPosition());
	}

	// from the Gui interface
	public void setSelectedPosition(final int position) {
		BigLog.i(tag, Logging.getCurrentMethodName(position), logHighlight);
		runOnUiThread(new Runnable() {
			public void run() {
				listAdapter.setSelectedPosition(position);
				listView.smoothScrollToPosition(position);
			}
		});
	}

	public void showPlayIcon() {
		BigLog.i(tag, Logging.getCurrentMethodName(), logHighlight);
		runOnUiThread(new Runnable() {
			public void run() {
				listAdapter.showPlayingIcon();
				listView.smoothScrollToPosition(listAdapter.getSelectedPosition());
			}
		});
	}

	public void showPauseIcon() {
		BigLog.i(tag, Logging.getCurrentMethodName(), logHighlight);
		runOnUiThread(new Runnable() {
			public void run() {
				listAdapter.showPausedIcon();
				listView.smoothScrollToPosition(listAdapter.getSelectedPosition());
			}
		});
	}

	public void showNoIcon() {
		BigLog.i(tag, Logging.getCurrentMethodName(), logHighlight);
		runOnUiThread(new Runnable() {
			public void run() {
				listAdapter.hideIcon();
				listView.smoothScrollToPosition(listAdapter.getSelectedPosition());
			}
		});
	}

	public int getSelectedPosition() {
		return listAdapter.getSelectedPosition();
	}

	public void clearPlayList() {
		BigLog.i(tag, Logging.getCurrentMethodName(), logHighlight);
		runOnUiThread(new Runnable() {
			public void run() {
				listAdapter.clear();
			}
		});
	}

	public void populateList(final String name, final boolean isLocal) {
		BigLog.i(tag, Logging.getCurrentMethodName(name, isLocal), logHighlight);
		runOnUiThread(new Runnable() {
			public void run() {
				listAdapter.add(new TrackInformation(name, isLocal));
				listAdapter.notifyDataSetChanged();
			}
		});
	}

	public void setSyncState(final boolean isOn) {
		BigLog.i(tag, Logging.getCurrentMethodName(isOn), logHighlight);
		runOnUiThread(new Runnable() {
			public void run() {
				findViewById(R.id.sync).setBackgroundResource(
						(isOn ? R.drawable.land_sync_active : R.drawable.land_sync_inactive));
				findViewById(R.id.syncPort).setBackgroundResource(
						(isOn ? R.drawable.port_sync_active : R.drawable.port_sync_inactive));
			}
		});
	}

	public void setToggleState(final boolean isEnabled) {
		BigLog.i(tag, Logging.getCurrentMethodName(isEnabled), logHighlight);
		runOnUiThread(new Runnable() {
			public void run() {
				findViewById(R.id.toggle).setEnabled(isEnabled);
				findViewById(R.id.togglePort).setEnabled(isEnabled);
				findViewById(R.id.toggle).setBackgroundResource(R.drawable.land_toggle);
				findViewById(R.id.togglePort).setBackgroundResource(R.drawable.port_toggle);
			}
		});
	}

	public void highlightToggle() {
		BigLog.i(tag, Logging.getCurrentMethodName(), logHighlight);
		runOnUiThread(new Runnable() {
			public void run() {
				findViewById(R.id.toggle).setBackgroundResource(R.drawable.toggle_tab_active);
				findViewById(R.id.togglePort).setBackgroundResource(
						R.drawable.toggle_tab_active_port);
			}
		});
	}

	public void playVisible() {
		BigLog.i(tag, Logging.getCurrentMethodName(), logHighlight);
		runOnUiThread(new Runnable() {
			public void run() {
				aPause.setVisibility(View.GONE);
				aPlay.setVisibility(View.VISIBLE);
				((Button) findViewById(R.id.videoPause)).setVisibility(View.GONE);
				((Button) findViewById(R.id.videoPausePort)).setVisibility(View.GONE);
				((Button) findViewById(R.id.videoPlay)).setVisibility(View.VISIBLE);
				((Button) findViewById(R.id.videoPlayPort)).setVisibility(View.VISIBLE);
			}
		});
	}

	public void pauseVisible() {
		BigLog.i(tag, Logging.getCurrentMethodName(), logHighlight);
		runOnUiThread(new Runnable() {
			public void run() {
				aPlay.setVisibility(View.GONE);
				aPause.setVisibility(View.VISIBLE);
				((Button) findViewById(R.id.videoPlay)).setVisibility(View.GONE);
				((Button) findViewById(R.id.videoPlayPort)).setVisibility(View.GONE);
				((Button) findViewById(R.id.videoPause)).setVisibility(View.VISIBLE);
				((Button) findViewById(R.id.videoPausePort)).setVisibility(View.VISIBLE);
			}
		});
	}

	public void lockGUI() {
		BigLog.i(tag, Logging.getCurrentMethodName(), logHighlight);
		runOnUiThread(new Runnable() {
			public void run() {
				if (progressDialog == null) {
					progressDialog = new ProgressDialog(VideoActivity.this);
					progressDialog.setMessage("Loading...");
					progressDialog.setCancelable(false);
					progressDialog.show();
				}
			}
		});
	}

	public void unlockGUI() {
		BigLog.i(tag, Logging.getCurrentMethodName(), logHighlight);
		runOnUiThread(new Runnable() {
			public void run() {
				if (progressDialog != null && progressDialog.isShowing()) {
					progressDialog.dismiss();
					progressDialog = null;
				}
			}
		});
	}

	// from the Scalable interface
	public void setWindowMode() {
		runOnUiThread(new Runnable() {
			public void run() {
				setAddBtnsVisibility(View.GONE);
				videoViewBackground.setBackgroundColor(0);
				int[] location = new int[2];
				listView.getLocationOnScreen(location);
				windowParams = new RelativeLayout.LayoutParams(listView.getLayoutParams());
				if (getResources().getConfiguration().orientation == Configuration.ORIENTATION_LANDSCAPE) {
					windowParams.setMargins(location[0], location[1], horizontalView.getWidth()
							- listView.getWidth() - location[0], horizontalView.getHeight()
							- listView.getHeight() - location[1]);
				} else { // portrait
					windowParams.setMargins(location[0], location[1], verticalView.getWidth()
							- listView.getWidth() - location[0], verticalView.getHeight()
							- listView.getHeight() - location[1]);
				}
				videoViewBackground.setLayoutParams(windowParams);
			}
		});
	}

	public void setFullScreenMode() {
		runOnUiThread(new Runnable() {
			public void run() {
				if (getResources().getConfiguration().orientation == Configuration.ORIENTATION_LANDSCAPE) {
					videoViewBackground.setBackgroundResource(R.drawable.video_back);
				} else {
					videoViewBackground.setBackgroundResource(R.drawable.port_black);
				}
				videoViewBackground.setLayoutParams(fullScreenParams);
			}
		});
	}

	// Scrollable interface implementation
	public void scrollToSelectedPosition() {
		BigLog.i(tag, Logging.getCurrentMethodName(), logHighlight);
		runOnUiThread(new Runnable() {
			public void run() {
				listView.smoothScrollToPosition(listAdapter.getSelectedPosition());
			}
		});
	}

	// internal classes
	private class ScaleButtonListener implements View.OnClickListener {
		public void onClick(View v) {
			BigLog.i(tag, "One of the scale buttons was clicked", logHighlight);
			currentController.onScaleButtonClicked();
		}
	}

	private class AdditionalButtonsListener implements android.view.View.OnClickListener {
		public void onClick(View v) {
			if (!currentController.canProcessPopupButtons()) {
				BigLog.i(tag, "Cannot process pop-up buttons in this state", logHighlight);
				return;
			}
			switch (v.getId()) {
			case R.id.videoAddNext:
				buttonProcessor.onNextClicked();
				break;
			case R.id.videoAddPause:
				buttonProcessor.onPauseClicked();
				break;
			case R.id.videoAddPlay:
				buttonProcessor.onPlayClicked();
				break;
			case R.id.videoAddPrev:
				buttonProcessor.onPrevClicked();
				break;
			case R.id.videoAddStop:
				buttonProcessor.onStopClicked();
				break;
			}
		}
	}

	private class AddButtonsHider implements Runnable {
		long timestamp;

		public AddButtonsHider(long timestamp) {
			this.timestamp = timestamp;
		}

		public void run() {
			if (addBtnsTouchedTimestamp == timestamp)
				videoViewBackground.findViewById(R.id.videoAdditionalButtons).setVisibility(
						View.GONE);
		}
	}

}
