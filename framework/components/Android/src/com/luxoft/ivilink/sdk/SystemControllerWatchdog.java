package com.luxoft.ivilink.sdk;

import android.util.Log;

import com.luxoft.ivilink.utils.log.Logging;

public class SystemControllerWatchdog {
	static {
		System.loadLibrary("SystemControllerWatchdog");
	}
	private Thread mThread;

	private SystemControllerWatchdogCallback mCallback;

	public SystemControllerWatchdog(SystemControllerWatchdogCallback callback) {
		mCallback = callback;
	}

	public interface SystemControllerWatchdogCallback {
		public void resetStack();
	}

	/**
	 * Launch the SystemControllerWatchdog
	 */
	public synchronized void start() {
		Log.v(this.toString(), Logging.getCurrentMethodName());
		if (mThread != null && mThread.isAlive()) {
			return;
		}
		(mThread = new Thread(new Runnable() {
			public void run() {
				startWD();
			}
		})).start();
	}

	// may be invoked from the native code
	private void doReset() {
		mCallback.resetStack();
	}

	private native void startWD();
}
