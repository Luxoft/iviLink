/**
 * @ingroup             Home Screen Status Bar Utils
 * @author              Elena Bratanova
 * @date                14.05.2013
 */

package com.luxoft.ivilink.utils.statusbar;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import android.app.ActivityManager;
import android.app.ActivityManager.RunningAppProcessInfo;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;

import com.luxoft.ivilink.utils.log.Logging;

public class StatusBarServerSide {
    private final static String TAG = StatusBarServerSide.class.getName();
    private static final int CHECK_PID_TIMEOUT_MILLIS = 1000;
    private Context mApplicationContext;
    private static final IntentFilter INTENT_FILTER = new IntentFilter(StatusBarProtocol.STATUS_BAR_INTENT_ACTION);

    private Handler mHandler = new Handler(Looper.getMainLooper());
    private Map<String, Integer> mPidMap = new HashMap<String, Integer>();

    private volatile boolean mWatchdogStopFlag = false;

    // all of callback's methods are invoked on the main thread
    public interface Callback {
        public void onMessage(String sender, String message);

        public void onSenderDied(String sender);
    }

    private Callback mCallback;

    private Runnable mWatchdog = new Runnable() {
        public void run() {
            while (!mWatchdogStopFlag) {
                try {
                    Thread.sleep(CHECK_PID_TIMEOUT_MILLIS);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }

                synchronized (mPidMap) {
                    List<String> keysToRemove = new ArrayList<String>();
                    for (String sender : mPidMap.keySet()) {
                        if (!isPidActive(mPidMap.get(sender))) {
                            Log.w(TAG, "Process of sender " + sender + " has died, notifying");
                            notifySenderDied(sender);
                            keysToRemove.add(sender);
                        }
                    }

                    for (String key : keysToRemove) {
                        mPidMap.remove(key);
                    }
                }
            }
        }
    };

    private BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            Log.v(TAG, Logging.getCurrentMethodName(context, intent));
            String sender = intent.getStringExtra(StatusBarProtocol.SENDER_TAG);
            String message = intent.getStringExtra(StatusBarProtocol.MESSAGE_TAG);
            int pid = intent.getIntExtra(StatusBarProtocol.PID_TAG, -1);
            if (sender != null && message != null && pid != -1) {
                Log.i(TAG, "got intent: " + sender + " " + message);
                mCallback.onMessage(sender, message);
                Log.i(TAG, "sender's " + sender + " pid is " + pid);
                if (!intent.getBooleanExtra(StatusBarProtocol.PERSISTENT_TAG, false)) {
                    synchronized (mPidMap) {
                        mPidMap.put(sender, pid);
                    }
                }
            } else {
                Log.w(TAG, "Could not parse intent: " + sender + " " + message);
            }
        }
    };

    public StatusBarServerSide(Callback callback, Context context) {
        mCallback = callback;
        mApplicationContext = context.getApplicationContext();
        registerListener();
    }

    public void unregisterListener() {
        try {
            mApplicationContext.unregisterReceiver(mReceiver);
        } catch (IllegalStateException e) {
            // may happen if unregister is called more than once
            e.printStackTrace();
        }
        mWatchdogStopFlag = true;
        synchronized (mPidMap) {
            mPidMap.clear();
        }
    }

    public void registerListener() {
        mApplicationContext.registerReceiver(mReceiver, INTENT_FILTER);
        new Thread(mWatchdog).start();
    }

    private boolean isPidActive(int pid) {
        ActivityManager activityManager = (ActivityManager) mApplicationContext
                .getSystemService(Context.ACTIVITY_SERVICE);
        List<RunningAppProcessInfo> procInfos = activityManager.getRunningAppProcesses();
        for (int i = 0; i < procInfos.size(); i++) {
            if (procInfos.get(i).pid == pid) {
                Log.v(TAG, "process with pid " + pid + " and name: " + procInfos.get(i).processName + " is alive!");
                return true;
            }
        }
        return false;
    }

    private void notifySenderDied(final String sender) {
        mHandler.post(new Runnable() {
            public void run() {
                mCallback.onSenderDied(sender);
            }
        });
    }
}
