package com.luxoft.ivilink.sdk.bluetooth;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import com.luxoft.ivilink.utils.log.BigLog;
import com.luxoft.ivilink.utils.log.Logging;
import com.luxoft.ivilink.sdk.helpers.BigIntegerGenerator;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.util.Log;

/**
 * Opens a BluetoothServerSocket and accepts connections for the
 * ACCEPT_TIMEOUT_MILLIS time in a separate thread. May be restarted if needed.
 */
public class BluetoothServer implements Runnable {

	private static final int ACCEPT_TIMEOUT_MILLIS = 12500;

	class ReceivedInfo {
		long timestamp;
		BluetoothSocket socket;

		public ReceivedInfo(long time, BluetoothSocket sock) {
			timestamp = time;
			socket = sock;
		}
	}

	private final static String tag = BluetoothServer.class.getSimpleName();
	private volatile boolean isRunning = false;
	public BluetoothSocket mConnectedSock;
	List<ReceivedInfo> clients = new ArrayList<ReceivedInfo>();
	private BluetoothAdapter mAdapter;

	public BluetoothServer(BluetoothAdapter adapter) {
		Log.v(tag, Logging.getCurrentMethodName(adapter));
		mAdapter = adapter;
	}

	/**
	 * Starts the thread if it is not running
	 */
	public void kick() {
		Log.v(tag, Logging.getCurrentMethodName());
		synchronized (this) {
			if (!isRunning) {
				isRunning = true;
				BigLog.i(tag, "restarting thread!", '!');
				new Thread(this).start();
			}
		}
	}

	/**
	 * Opens a BluetoothServerSocket and accepts connections for the
	 * ACCEPT_TIOMEOUT_MILLIS time.
	 */
	public void run() {
		Log.v(tag, Logging.getCurrentMethodName());
		BigLog.i(tag, "thread has started!", '!');
		BluetoothServerSocket servSock = null;
		while (servSock == null) {
			try {
				servSock = mAdapter.listenUsingRfcommWithServiceRecord(
						BigIntegerGenerator.nextSessionId(), BluetoothHelper.sppUUID);
			} catch (IOException e) {
				e.printStackTrace();
				servSock = null;
			}
		}
		BigLog.i(tag, "server socket created!", '!');
		long timeToAccept = ACCEPT_TIMEOUT_MILLIS;
		final long beginTime = System.currentTimeMillis();
		while (timeToAccept > 0) {
			try {
				Log.w(tag, "Now accepting!");
				BluetoothSocket sock = servSock.accept((int) timeToAccept);
				Log.i(tag, "accepted somebody!");
				synchronized (clients) {
					clients.add(new ReceivedInfo(System.currentTimeMillis(), sock));
				}
				long currentTime = System.currentTimeMillis();
				timeToAccept = ACCEPT_TIMEOUT_MILLIS - (currentTime - beginTime);
				BigLog.i(tag, "Time remaining: " + timeToAccept + " ms", '!');
			} catch (IOException e) {
				BigLog.e(tag, "exited on timeout", '!');
				e.printStackTrace();
				break;
			}
		}
		BigLog.i(tag, "closing server socket!", '!');
		try {
			servSock.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
		BigLog.i(tag, "thread has finished!", '!');
		synchronized (this) {
			isRunning = false;
		}
	}

}
