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
