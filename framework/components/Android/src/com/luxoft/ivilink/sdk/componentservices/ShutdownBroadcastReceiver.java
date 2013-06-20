package com.luxoft.ivilink.sdk.componentservices;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import com.luxoft.ivilink.utils.log.Logging;

public class ShutdownBroadcastReceiver extends BroadcastReceiver {
	@Override
	public void onReceive(Context context, Intent intent) {
		Log.v(context.toString(), Logging.getCurrentMethodName(context, intent));
		context.unregisterReceiver(this);
		android.os.Process.killProcess(android.os.Process.myPid());
	}
}
