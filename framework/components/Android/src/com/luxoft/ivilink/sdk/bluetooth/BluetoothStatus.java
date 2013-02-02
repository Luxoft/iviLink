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

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.content.Intent;
import android.util.Log;

/**
 * Checks if Bluetooth may be used and requests discoverable permission
 */
public class BluetoothStatus {

	/**
	 * Opens dialog window asking permission to make the device discoverable. By
	 * default the discoverable duration is indefinite, however on different
	 * devices the behaviour may vary
	 * 
	 * @param activity
	 *            Activity that will receive onActivityResult
	 */
	public static void requestDiscoverable(Activity activity) {
		Intent discoverableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_DISCOVERABLE);
		discoverableIntent.putExtra(BluetoothAdapter.EXTRA_DISCOVERABLE_DURATION, 0);
		activity.startActivityForResult(discoverableIntent, 0);
	}

	/**
	 * Checks if Bluetooth may be used on the device
	 * 
	 * @return false if Bluetooth is not supported or turned off, true otherwise
	 */
	public static boolean isBluetoothUsable() {
		BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
		if (adapter == null) {
			Log.e(BluetoothStatus.class.getSimpleName(),
					"Bluetooth is not supported on this device! There is nothing we can do");
			return false;
		}
		if (!adapter.isEnabled()) {
			Log.e(BluetoothStatus.class.getSimpleName(), "Bluetooth is off");
			return false;
		}
		return true;
	}
}
