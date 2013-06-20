package com.luxoft.ivilink.sdk.componentservices;

import android.content.BroadcastReceiver;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.wifi.WifiManager;
import android.net.wifi.WifiManager.MulticastLock;
import android.util.Log;
import android.widget.Toast;
import android.content.Context;

import com.luxoft.ivilink.sdk.CConnectivityAgentWrapper;
import com.luxoft.ivilink.sdk.bluetooth.BluetoothHelper;
import com.luxoft.ivilink.sdk.helpers.Common;
import com.luxoft.ivilink.sdk.helpers.StartupProgress;

/**
 * In this service Connectivity Agent process is run
 */
public class ConnectivityAgentService extends ProtoComponentService {

	CConnectivityAgentWrapper wrapper;
	WifiManager wifi;
	MulticastLock multicastLock;
	ProgressBroadcastReceiver progressBR = new ProgressBroadcastReceiver();

	@Override
	protected StartupProgress getProgress() {
		return StartupProgress.CONNECTIVITY_AGENT;
	}

	@Override
	protected void startComponent(Intent intent) {
		BluetoothHelper bluetooth = null;
		if (intent.getBooleanExtra("bluetooth", false)) {
			bluetooth = BluetoothHelper.getInstance(this);
			if (bluetooth.isBluetoothAvaliable()) {
				bluetooth.init();
			} else {
				bluetooth = null;
			}
		}

		registerReceiver(progressBR, new IntentFilter(Common.ifProgress));

		wifi = (WifiManager) getSystemService(Context.WIFI_SERVICE);
		multicastLock = wifi.createMulticastLock("com.luxoft.ivilink.sdk.CConnectivityAgent");
		multicastLock.acquire();

		wrapper = new CConnectivityAgentWrapper();
		wrapper.start(bluetooth);
	}

	class ProgressBroadcastReceiver extends BroadcastReceiver {
		@Override
		public void onReceive(Context context, Intent intent) {
			int progressValue = intent.getIntExtra(Common.progressValue, 0);
			if (progressValue >= StartupProgress.CHANNEL_SUPERVISOR.getProgress()) {
				Log.i(this.getClass().getSimpleName(), "releasing MulticastLock");
				try {
					multicastLock.release();
					unregisterReceiver(this);
				} catch (Exception e) {
					e.printStackTrace();
					Toast.makeText(context, "Error releasing multicast lock: " + e.getMessage(),
							Toast.LENGTH_SHORT).show();
				}
			}

		}
	}
}
