/**
 * @ingroup             Home Screen Status Bar Utils
 * @author              Elena Bratanova
 * @date                14.05.2013
 */

package com.luxoft.ivilink.utils.statusbar;

import com.luxoft.ivilink.utils.log.Logging;

import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class StatusBarClientSide {
    private final static String TAG = StatusBarClientSide.class.getName();

    /**
     * Send broadcast with data to the status bar.
     * 
     * @param context
     * @param sender
     *            ID of the sender - if a message from a sender the with the
     *            same ID is already displayed in the status bar, it will be
     *            replaced with this one
     * @param message
     * @param persistent
     *            <code>true</code> if the message should stay in the status bar
     *            after the sending application has died
     */
    public static void notifyStatusBar(Context context, String sender, String message,
            boolean persistent) {
        Log.v(TAG, Logging.getCurrentMethodName(context, sender, message, persistent));
        Intent intent = new Intent(StatusBarProtocol.STATUS_BAR_INTENT_ACTION);
        intent.putExtra(StatusBarProtocol.SENDER_TAG, sender);
        intent.putExtra(StatusBarProtocol.MESSAGE_TAG, message);
        intent.putExtra(StatusBarProtocol.PID_TAG, android.os.Process.myPid());
        intent.putExtra(StatusBarProtocol.PERSISTENT_TAG, persistent);
        context.sendBroadcast(intent);
    }

    public static void notifyStatusBar(Context context, String sender, String message) {
        notifyStatusBar(context, sender, message, false);
    }
}
