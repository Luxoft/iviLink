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


package com.luxoft.ivilink.samples.seat;

import android.os.Handler;
import android.util.Log;

import com.luxoft.ivilink.sdk.android.lib.utils.ForApp;

//  glues java and c code together and takes care of ui interaction thread safety
class BridgeC {
	static {
		System.loadLibrary("iviLinkApplicationLibraryShared");
		System.loadLibrary("SeatState");
		System.loadLibrary("seatclient");
	}


	private SeatActivity activity;
	private Handler poster;
	private boolean isNativeStarted;// = false;
	Thread mThread;

	final String tag = "iviLink.Samples.SeatApp.Bridge";

	private BridgeC(SeatActivity activity, Handler poster) {
		setActivity(activity);
		setHandler(poster);
	}

	private void setHandler(Handler poster) {
		this.poster = poster;
	}

	private void setActivity(SeatActivity activity) {
		this.activity = activity;
	}

	private static BridgeC instance;

	static BridgeC getInstance(SeatActivity activity, Handler poster) {
		synchronized (BridgeC.class) {
			if (instance == null) {
				instance = new BridgeC(activity, poster);
			} else {
				instance.setActivity(activity);
				instance.setHandler(poster);
			}
		}
		return instance;
	}

	void startApp() {
		if (!isNativeStarted) {
			(mThread = new Thread(new Runnable() {
				public void run() {
					startApp(ForApp.getLaunchInfo(activity),
							ForApp.getServicePath(),
							ForApp.getInternalPath(activity),
							ForApp.getInternalPath(activity) + "/state.pb");
				}
			})).start();
			isNativeStarted = true;
		} else {
			Log.i(tag, "startApp(): native app is already started!");
		}
	}

	private native void startApp(String launchInfo, String serviceRepo,
			String internalPath, String statePath);

	// change heater state

	native void driverHeaterRequest();

	native void passHeaterRequest();

	// change seat selection
	native void driverSeatRequest();

	native void passSeatRequest();

	// change angle
	native void backLeftRequest();

	native void backRightRequest();

	// move chair
	native void bottomUpRequest();

	native void bottomDownRequest();

	native void bottomLeftRequest();

	native void bottomRightRequest();

	native void saveState();

	private native void loadState();

	public void initRequest() {
		if (mThread == null)
			return;
		if (!mThread.isAlive()) {
			Log.i(tag,
					"loadState(): native app is fully initialized! Will do initRequest");
			loadState();
		} else {
			poster.postDelayed(new Runnable() {
				public void run() {
					if (!mThread.isAlive()) {
						Log.i(tag,
								"loadState(): native app is fully initialized! Will do initRequest");
						loadState();
					} else {
						Log.i(tag,
								"loadState(): native app is not fully initialized yet!");
						poster.postDelayed(this, 100);
					}
				}
			}, 100);
		}
	}

	void onHeaterDriver(final int heaterVal) {
		poster.post(new Runnable() {
			public void run() {
				activity.onHeaterDriverState(heaterVal);
			}
		});
	}

	void onHeaterPassenger(final int heaterVal) {
		poster.post(new Runnable() {
			public void run() {
				activity.onHeaterPassState(heaterVal);
			}
		});
	}

	void onSetDriver() {
		poster.post(new Runnable() {
			public void run() {
				activity.onSwitchToDriver();
			}
		});
	}

	void onSetPassenger() {
		poster.post(new Runnable() {
			public void run() {
				activity.onSwitchToPassenger();
			}
		});
	}

	void onBottomX(final int x) {
		poster.post(new Runnable() {
			public void run() {
				activity.onSeatBottomX(x);
			}
		});
	}

	void onBottomY(final int y) {
		poster.post(new Runnable() {
			public void run() {
				activity.onSeatBottomY(y);
			}
		});
	}

	void onBackAngle(final int angle) {
		poster.post(new Runnable() {
			public void run() {
				activity.onSeatBackAngle(angle);
			}
		});
	}

	void onBackX(final int x) {
		// empty stub
	}

	void onBackY(final int y) {
		// empty stub
	}

	void onShowSeat() {
		// empty stub
	}

	void onShowLoadingScreen() {
		poster.post(new Runnable() {
			public void run() {
				activity.onShowLoadingScreen();
			}
		});
	}

	void onHideLoadingScreen() {
		poster.post(new Runnable() {
			public void run() {
				activity.onHideLoadingScreen();
			}
		});
	}
}
