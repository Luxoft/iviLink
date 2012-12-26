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

import android.app.Activity;
import android.os.Environment;
import android.util.Log;

import com.luxoft.ivilink.samples.media.helpers.MediaSource;
import com.luxoft.ivilink.samples.media.interfaces.ButtonProcessor;
import com.luxoft.ivilink.samples.media.interfaces.JniPlayerWrapper;
import com.luxoft.ivilink.samples.media.interfaces.MediaGui;
import com.luxoft.ivilink.sdk.android.lib.utils.ForApp;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

public class Bridge implements ButtonProcessor {
	private boolean hasNativeStarted;
	private static Bridge instance;
	private Activity activity;
	private MediaGui gui;
	private final static String tag = "iviLink.samples.Media.Bridge";

	static {
		System.loadLibrary("application");
	}

	public static Bridge createInstance(VideoActivity activity, MediaGui gui,
			JniPlayerWrapper player) {
		Log.v(tag, Logging.getCurrentMethodName(activity, gui, player));
		synchronized (Bridge.class) {
			if (instance == null) {
				instance = new Bridge(activity, gui, player);
			}
		}
		return instance;
	}

	Bridge(VideoActivity act, MediaGui gui, JniPlayerWrapper player) {
		Log.v(tag, Logging.getCurrentMethodName(act, gui, player));
		this.activity = act;
		this.gui = gui;
		try {
			storeObjects(gui, player, MediaSource.getInstance());
		} catch (UnsatisfiedLinkError e) {
			e.printStackTrace();
		}
	}

	private native void storeObjects(MediaGui gui, JniPlayerWrapper player,
			MediaSource source);

	public static Bridge getInstance() {
		assert (instance != null) : "Bridge has not been created!";
		return instance;
	}

	public void startApp() {
		Log.v(tag, Logging.getCurrentMethodName());
		if (!hasNativeStarted) {
			new Thread(new Runnable() {
				public void run() {
					try {
						startApp(ForApp.getServicePath(),
								ForApp.getLaunchInfo(activity),
								ForApp.getInternalPath(activity), MediaSource.mediaPath);
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

	private native void startApp(String serviceRepo, String launchInfo,
			String internalPath, String pathToMedia);

	private native void playClicked();

	private native void pauseClicked();

	private native void stopClicked();

	private native void nextClicked();

	private native void prevClicked();

	private native void posClicked(int position, int oldPosition);

	private native void completed();

	private native void syncClicked();

	private native void toggleClicked();

	// from the ButtonProcessor interface
	public void onPlayClicked() {
		Log.v(tag, Logging.getCurrentMethodName());
		if (hasNativeStarted)
			playClicked();
	}

	public void onPauseClicked() {
		Log.v(tag, Logging.getCurrentMethodName());
		if (hasNativeStarted)
			pauseClicked();
	}

	public void onStopClicked() {
		Log.v(tag, Logging.getCurrentMethodName());
		if (hasNativeStarted)
			stopClicked();
	}

	public void onNextClicked() {
		Log.v(tag, Logging.getCurrentMethodName());
		if (hasNativeStarted)
			nextClicked();
	}

	public void onPrevClicked() {
		Log.v(tag, Logging.getCurrentMethodName());
		if (hasNativeStarted)
			prevClicked();
	}

	public void onPositionClicked(int position, int oldPosition) {
		Log.v(tag, Logging.getCurrentMethodName(position, oldPosition));
		if (hasNativeStarted)
			posClicked(position, oldPosition);
	}

	public void onCompletion() {
		Log.v(tag, Logging.getCurrentMethodName());
		if (hasNativeStarted)
			completed();
	}

	public void onSyncClicked() {
		Log.v(tag, Logging.getCurrentMethodName());
		if (hasNativeStarted)
			syncClicked();
	}

	public void onToggleClicked() {
		Log.v(tag, Logging.getCurrentMethodName());
		if (hasNativeStarted)
			toggleClicked();
	}
}
