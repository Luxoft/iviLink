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

import java.util.HashSet;
import java.util.Set;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import com.luxoft.ivilink.sdk.android.lib.utils.log.BigLog;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

/**
 * Receives discovery results, stores them as BluetoothDevices in a Set, if the
 * discovered device is paired with this one
 */
public class DeviceFoundReceiver extends BroadcastReceiver {
	private DiscoveringState mState;
	private BluetoothAdapter mAdapter;
	private Set<BluetoothDevice> foundPairedDevicesSet = new HashSet<BluetoothDevice>();

	private final static String tag = DeviceFoundReceiver.class.getName();

	public DeviceFoundReceiver(BluetoothAdapter adapter) {
		mState = DiscoveringState.FINISHED;
		mAdapter = adapter;
	}

	/**
	 * Get a Set of all found paired devices
	 * 
	 * @return all discovered paired devices
	 */
	public Set<BluetoothDevice> getFoundList() {
		return foundPairedDevicesSet;
	}

	enum DiscoveringState {
		IN_PROGRESS, FINISHED,
	};

	/**
	 * Forces to restart discovery if it has finished
	 */
	public void kick() {
		Log.v(tag, Logging.getCurrentMethodName());
		synchronized (this) {
			if (mState == DiscoveringState.FINISHED) {
				BigLog.i(tag, "Restarting discovery!", '!');
				mState = DiscoveringState.IN_PROGRESS;
				mAdapter.startDiscovery();
			}
		}
	}

	@Override
	public void onReceive(Context context, Intent intent) {
		Log.v(tag, Logging.getCurrentMethodName(context, intent));
		String action = intent.getAction();
		if (BluetoothDevice.ACTION_FOUND.equals(action)) {
			Log.i(tag, "found a device:");
			BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
			Log.i(tag, "Name: " + device.getName() + " Address: " + device.getAddress());
			if (device.getBondState() == BluetoothDevice.BOND_BONDED) {
				synchronized (foundPairedDevicesSet) {
					Log.v(tag, device.getAddress() + ":" + device.getName() + "we are paired!");
					foundPairedDevicesSet.add(device);
				}
			}
		} else if (BluetoothAdapter.ACTION_DISCOVERY_FINISHED.equals(action)) {
			Log.i(tag, "Discovery finished!");
			synchronized (this) {
				if (mState == DiscoveringState.IN_PROGRESS) {
					mState = DiscoveringState.FINISHED;
				}
			}
		}
	}
}
