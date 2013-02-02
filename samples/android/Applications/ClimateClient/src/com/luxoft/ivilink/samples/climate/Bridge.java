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


package com.luxoft.ivilink.samples.climate;

import android.os.Handler;
import android.util.Log;
import com.luxoft.ivilink.sdk.android.lib.utils.ForApp;

public class Bridge {
	static {
		System.loadLibrary("iviLinkApplicationLibraryShared");
		System.loadLibrary("ClimateState");
		System.loadLibrary("climateapp");
	}

	Handler poster;
	ClimateActivity activity;

	static private Bridge instance;
	static boolean hasNativeStarted;

	// in case this get changed, don't forget to update native code as well
	native void offPressed();

	native void autoPressed();

	native void acPressed();

	native void dualPressed();

	native void recircPressed();

	native void frontPressed();

	native void rearPressed();

	native void fanDownPressed();

	native void fanMiddlePressed();

	native void fanTopPressed();

	native void fanIncrease();

	native void fanDecrease();

	native void drvTempUp();

	native void drvTempDown();

	native void passTempUp();

	native void passTempDown();

	native void saveState();

	native void reloadState();

	private native void startApp(String serviceRepoPath, String launchInfo,
			String internalPath);

	public void startNativeApp() {
		if (!hasNativeStarted) {
			hasNativeStarted = true;
			startApp(ForApp.getServicePath(), ForApp.getLaunchInfo(activity),
					ForApp.getInternalPath(activity));
		} else {
			Log.i("ClimateApp.Bridge", "Native app has already started");
		}
	}

	public static Bridge getInstance(ClimateActivity cca, Handler poster) {
		synchronized (Bridge.class) {
			if (instance == null) {
				instance = new Bridge();
			}
			instance.activity = cca;
			instance.poster = poster;
		}
		return instance;
	}

	void autoState(final boolean status) {
		poster.post(new Runnable() {
			public void run() {
				try {
					activity.autoState(status);
				} catch (NullPointerException e) {
					e.printStackTrace();
				}
			}
		});
	}

	void recircState(final boolean status) {
		poster.post(new Runnable() {
			public void run() {
				try {
					activity.recircState(status);
				} catch (NullPointerException e) {
					e.printStackTrace();
				}
			}
		});
	}

	void acState(final boolean status) {
		poster.post(new Runnable() {
			public void run() {
				try {
					activity.acState(status);
				} catch (NullPointerException e) {
					e.printStackTrace();
				}
			}
		});
	}

	void frontState(final boolean status) {
		poster.post(new Runnable() {
			public void run() {
				try {
					activity.frontState(status);
				} catch (NullPointerException e) {
					e.printStackTrace();
				}
			}
		});
	}

	void rearState(final boolean status) {
		poster.post(new Runnable() {
			public void run() {
				try {
					activity.rearState(status);
				} catch (NullPointerException e) {
					e.printStackTrace();
				}
			}
		});
	}

	void fanLowState(final boolean status) {
		poster.post(new Runnable() {
			public void run() {
				try {
					activity.fanLowState(status);
				} catch (NullPointerException e) {
					e.printStackTrace();
				}
			}
		});
	}

	void fanMidState(final boolean status) {
		poster.post(new Runnable() {
			public void run() {
				try {
					activity.fanMidState(status);
				} catch (NullPointerException e) {
					e.printStackTrace();
				}
			}
		});
	}

	void fanHighState(final boolean status) {
		poster.post(new Runnable() {
			public void run() {
				try {
					activity.fanHighState(status);
				} catch (NullPointerException e) {
					e.printStackTrace();
				}
			}
		});
	}

	void passTemp(final int temp) {
		poster.post(new Runnable() {
			public void run() {
				try {
					activity.passTemp(temp);
				} catch (NullPointerException e) {
					e.printStackTrace();
				}
			}
		});
	}

	void driverTemp(final int temp) {
		poster.post(new Runnable() {
			public void run() {
				try {
					activity.driverTemp(temp);
				} catch (NullPointerException e) {
					e.printStackTrace();
				}
			}
		});
	}

	void syncState(final boolean status) {
		poster.post(new Runnable() {
			public void run() {
				try {
					activity.syncState(status);
				} catch (NullPointerException e) {
					e.printStackTrace();
				}
			}
		});
	}

	void fanSpeed(final int speed) {
		poster.post(new Runnable() {
			public void run() {
				try {
					activity.fanSpeed(speed);
				} catch (NullPointerException e) {
					e.printStackTrace();
				}
			}
		});
	}

	void fanOffState(final boolean status) {
		poster.post(new Runnable() {
			public void run() {
				try {
					activity.fanOffState(status);
				} catch (NullPointerException e) {
					e.printStackTrace();
				}
			}
		});
	}

	void onShowLoadingScreen() {
		poster.post(new Runnable() {
			public void run() {
				try {
					activity.onShowLoadingScreen();
				} catch (NullPointerException e) {
					e.printStackTrace();
				}
			}
		});
	}

	void onHideLoadingScreen() {
		poster.post(new Runnable() {
			public void run() {
				try {
					activity.onHideLoadingScreen();
				} catch (NullPointerException e) {
					e.printStackTrace();
				}
			}
		});
	}

}
