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
package com.luxoft.ivilink.sdk.bluetooth;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;

import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.IntentFilter;
import android.util.Log;

import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;
import com.luxoft.ivilink.sdk.bluetooth.BluetoothServer.ReceivedInfo;

/**
 * Non-native complement for the CAndroidBluetoothConnectionFinder and
 * CAndroidBluetoothAdapter native classes (ConnectivityAgent/HAL)
 * 
 */
public class BluetoothHelper {
	final static String tag = "iviLink.SDK.ConnectivityAgent.BluetoothHelper";
	// TODO test this with real non-android spp device
	final static UUID sppUUID = UUID.fromString("936da01f-9abd-4d9d-80c7-02af85c822a8");
	BluetoothSocket clientSocket;
	BluetoothSocket mainConnectedSocket;

	InputStream sockInStream;
	OutputStream sockOutStream;
	long freeTimestamp;

	BluetoothAdapter adapter;
	DeviceFoundReceiver dvr;
	static private BluetoothHelper instance;
	BluetoothServer server;

	boolean bluetoothAvaliable = true;
	boolean isBroken = true;

	Service contextService;

	/**
	 * Returns a previously created instance of the BluetoothHelper, or
	 * constructs one if there is none
	 * 
	 * @param contextService
	 *            Is used to register broadcast receiver for device discovery
	 * @return BluetoothHelper instance
	 */
	public static BluetoothHelper getInstance(Service contextService) {
		synchronized (BluetoothHelper.class) {
			if (instance == null) {
				instance = new BluetoothHelper(contextService);
			}
		}
		return instance;
	}

	/**
	 * Private constructor for singleton behaviour
	 * 
	 * @param contextService
	 */
	private BluetoothHelper(Service contextService) {
		Log.v(tag, Logging.getCurrentMethodName(contextService));
		this.contextService = contextService;
		adapter = BluetoothAdapter.getDefaultAdapter();
		if (adapter == null) {
			Log.e(tag, "Bluetooth is not supported on this device! There is nothing we can do");
			bluetoothAvaliable = false;
			return;
		}
		if (!adapter.isEnabled()) {
			Log.e(tag, "Bluetooth is off");
			bluetoothAvaliable = false;
			return;
		}

		Log.i(tag, "This device's bluetooth name: " + adapter.getName());
	}

	/**
	 * Constructs BluetoothServer, constructs DeviceFoundReceiver and registers
	 * it for the following actions: BluetoothDevice.ACTION_FOUND and
	 * BluetoothAdapter.ACTION_DISCOVERY_FINISHED
	 */
	public void init() {
		Log.v(tag, Logging.getCurrentMethodName());
		dvr = new DeviceFoundReceiver(adapter);
		contextService.registerReceiver(dvr, new IntentFilter(BluetoothDevice.ACTION_FOUND));
		contextService.registerReceiver(dvr, new IntentFilter(
				BluetoothAdapter.ACTION_DISCOVERY_FINISHED));
		server = new BluetoothServer(adapter);
	}

	/**
	 * Check if connection is up. Invoked from the native
	 * CAndroidBluetoothAdapter class
	 * 
	 * @return <code>true</code> if link is down, <code>false</code> otherwise
	 */
	@SuppressWarnings("unused")
	private boolean isBroken() {
		boolean broken = !(bluetoothAvaliable && !isBroken);
		if (broken) {
			Log.e(tag, "java isBroken called : !!! TRUE !!!");
		} else {
			Log.i(tag, "java isBroken called : FALSE");
		}

		return broken;
	}

	public boolean isBluetoothAvaliable() {
		return bluetoothAvaliable;
	}

	/**
	 * Iterates over the list of discovered devices and tries to connect to
	 * those, whose MAC-address is greater than current device's
	 */
	private void processDiscoveredDevices() {
		Log.v(tag, Logging.getCurrentMethodName());
		freeTimestamp = -1;
		synchronized (dvr.getFoundList()) {
			if (dvr.getFoundList().isEmpty()) {
				Log.w(tag, "foundPairedDevicesList is empty");
				return;
			}
			for (BluetoothDevice device : dvr.getFoundList()) {
				Log.i(tag, "remote addr: " + device.getAddress());
				if (device.getAddress().compareTo(adapter.getAddress()) >= 0) {
					// if greater than ours - try to connect
					Log.i(tag, "trying to connect to " + device.getAddress());
					try {
						clientSocket = device.createRfcommSocketToServiceRecord(sppUUID);
						clientSocket.connect();
					} catch (IOException e) {
						e.printStackTrace();
						clientSocket = null;
					}
					if (clientSocket != null) {
						freeTimestamp = System.currentTimeMillis();
						Log.i(tag, "Connected to somebody!");
						Log.i(tag, "Connected to : " + device.getAddress());
						break;
					}
				} else {
					Log.i(tag, "other side should connect");
				}
			}
		}
	}

	/**
	 * Performs search: tries to connect to suitable discovered devices and
	 * checks if someone has connected to the BluetoothServer. Invoked from the
	 * native CAndroidBluetoothConnectionFinder class
	 * 
	 * @return <code>0</code> if no connection has been found, <code>1</code> if
	 *         connected as a server, <code>2</code> if connected as a client
	 * 
	 */
	@SuppressWarnings("unused")
	private int performSearch() {
		Log.v(tag, Logging.getCurrentMethodName());
		// TODO here was a sleep. Investigate its necessity.
		if (!bluetoothAvaliable) {
			Log.e(tag, "Bluetooth not avaliable");
			return 0;
		}
		dvr.kick();
		server.kick();
		processDiscoveredDevices();

		synchronized (server.clients) {
			if (clientSocket == null && server.clients.isEmpty()) {
				isBroken = true;
				Log.e(tag, "found nothing, till next time!");
				return 0;
			}
			if (clientSocket != null && server.clients.isEmpty()) {
				Log.i(tag, "is client");
				mainConnectedSocket = clientSocket;
				isBroken = false;
				return 2;
			}
			if (clientSocket == null && !server.clients.isEmpty()) {
				ReceivedInfo newestInfo = null;
				for (ReceivedInfo info : server.clients) {
					if (newestInfo == null) {
						newestInfo = info;
					} else {
						if (info.timestamp > newestInfo.timestamp) {
							newestInfo = info;
						}
					}
				}
				mainConnectedSocket = newestInfo.socket;
				isBroken = false;
				Log.i(tag, "is server");
				return 1;
			}

			if (clientSocket != null && !server.clients.isEmpty()) {
				ReceivedInfo newestInfo = null;
				for (ReceivedInfo info : server.clients) {
					if (newestInfo == null) {
						newestInfo = info;
					} else {
						if (info.timestamp > newestInfo.timestamp) {
							newestInfo = info;
						}
					}
				}
				if (newestInfo.timestamp > freeTimestamp) {
					try {
						clientSocket.close();
					} catch (IOException e) {
						e.printStackTrace();
					}
					mainConnectedSocket = newestInfo.socket;
					isBroken = false;
					Log.i(tag, "is server");
					return 1;
				} else {
					Log.i(tag, "is client");
					mainConnectedSocket = clientSocket;
					isBroken = false;
					return 2;
				}
			}
			return 0;
		}
	}

	/**
	 * Writes data to the output stream of the connected Bluetooth socket.
	 * Invoked from the CAndroidBluetoothAdapter native class.
	 * 
	 * @param data
	 *            data to be sent
	 * @return <code>true</code> if everything went fine, <code>false</code> in
	 *         case of some error
	 */
	@SuppressWarnings("unused")
	private boolean sendData(byte[] data) {
		Log.v(tag, Logging.getCurrentMethodName(data));
		if ((data == null) || isBroken) {
			Log.e(tag, "bluetooth connection is broken or !data");
			return false;
		} else {
			Log.i(tag, "sending data:");
			Log.i(tag, new String(data));
		}
		if (sockOutStream == null) {
			try {
				sockOutStream = mainConnectedSocket.getOutputStream();
			} catch (IOException e) {
				isBroken = true;
				e.printStackTrace();
				return false;
			}
		}
		try {
			sockOutStream.write(data);
		} catch (Exception e) {
			isBroken = true;
			e.printStackTrace();
			return false;
		}
		return true;
	}

	/**
	 * Reads data from the connected bluetooth socket. Invoked from the
	 * CAndroidBluetoothAdapter native class.
	 * 
	 * @param size
	 *            desirable size of data to be read (currently ignored)
	 * @return read data, or <code>null</code> in case of broken connection
	 * 
	 */
	@SuppressWarnings("unused")
	private byte[] readData(int size) {
		Log.v(tag, Logging.getCurrentMethodName(size));
		if (sockInStream == null) {
			try {
				sockInStream = mainConnectedSocket.getInputStream();
			} catch (IOException e) {
				isBroken = true;
				e.printStackTrace();
				return null;
			}
		}
		byte[] result = null;
		try {
			int available;
			int idleCounter = 0;
			while ((available = sockInStream.available()) == 0) {
				idleCounter++;

				if (idleCounter > 20) {
					Log.i(tag, "*/*/*/*/*/*/*/*/*/*/*/*/");
					Log.i(tag, "*/*/*/*/*/*/*/*/*/*/*/*/");
					Log.i(tag, "*/*/*/*/*/*/*/*/*/*/*/*/");
					Log.i(tag, "IDLE > 20");
					Log.i(tag, "*/*/*/*/*/*/*/*/*/*/*/*/");
					Log.i(tag, "*/*/*/*/*/*/*/*/*/*/*/*/");
					Log.i(tag, "*/*/*/*/*/*/*/*/*/*/*/*/");
					int byteFromSock = 0;
					try {
						Log.i(tag, "*/*/*/*/*/*/ BEFORE READ 1 BYTE */*/*/*/*/*/");
						byteFromSock = sockInStream.read();
						Log.i(tag, "*/*/*/*/*/*/ AFTER READ 1 BYTE */*/*/*/*/*/");
					} catch (IOException e) {
						Log.i(tag, "*/*/*/*/*/*/*/*/*/*/*/*/");
						Log.i(tag, "*/*/*/*/*/*/*/*/*/*/*/*/");
						Log.i(tag, "*/*/*/*/*/*/*/*/*/*/*/*/");
						Log.i(tag, "READ FAAAAAAAAAIL");
						Log.i(tag, "*/*/*/*/*/*/*/*/*/*/*/*/");
						Log.i(tag, "*/*/*/*/*/*/*/*/*/*/*/*/");
						Log.i(tag, "*/*/*/*/*/*/*/*/*/*/*/*/");
						e.printStackTrace();
						isBroken = true;
						return null;
					}
					if (byteFromSock == -1) {
						Log.i(tag, "*/*/*/*/*/*/*/*/*/*/*/*/");
						Log.i(tag, "*/*/*/*/*/*/*/*/*/*/*/*/");
						Log.i(tag, "*/*/*/*/*/*/*/*/*/*/*/*/");
						Log.i(tag, "byteFromSock == -1");
						Log.i(tag, "*/*/*/*/*/*/*/*/*/*/*/*/");
						Log.i(tag, "*/*/*/*/*/*/*/*/*/*/*/*/");
						Log.i(tag, "*/*/*/*/*/*/*/*/*/*/*/*/");
						idleCounter = 0;
					} else {
						result = new byte[1];
						result[0] = (byte) byteFromSock;
						Log.i(tag, "*/*/*/*/*/*/*/*/*/*/*/*/");
						Log.i(tag, "*/*/*/*/*/*/*/*/*/*/*/*/");
						Log.i(tag, "*/*/*/*/*/*/*/*/*/*/*/*/");
						Log.i(tag, "ONE BYTE IS READ");
						Log.i(tag, "*/*/*/*/*/*/*/*/*/*/*/*/");
						Log.i(tag, "*/*/*/*/*/*/*/*/*/*/*/*/");
						Log.i(tag, "*/*/*/*/*/*/*/*/*/*/*/*/");
						return result;
					}
				}
				Thread.sleep(100); // TODO optimize this
			}
			Log.i(tag, "	available: " + available);
			result = new byte[available];
			Log.i(tag, "+|+|+|+|+|+|+|+|+ BEFORE READ " + available + " BYTES +|+|+|+|+|+|+|+|+");
			int read = sockInStream.read(result);
			Log.i(tag, "+|+|+|+|+|+|+|+|+ AFTER READ " + available + " BYTES +|+|+|+|+|+|+|+|+");
			Log.i(tag, "READ: " + read);
			Log.i(tag, new String(result));
		} catch (Exception e) {
			e.printStackTrace();
			isBroken = true;
			return null;
		}
		Log.i(tag, "data received:");
		Log.i(tag, new String(result));
		return result;
	}
}
