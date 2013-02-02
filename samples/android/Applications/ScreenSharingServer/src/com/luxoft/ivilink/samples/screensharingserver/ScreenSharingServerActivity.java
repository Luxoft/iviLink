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
package com.luxoft.ivilink.samples.screensharingserver;

import com.luxoft.ivilink.sdk.android.lib.utils.ForApp;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

import android.app.Activity;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;

public class ScreenSharingServerActivity extends Activity {
    static final String TAG = "ScreenSharingServerActivity";

    Dialog vncDialog;
    ProgressDialog progress;
    VncButtonListener mvbl = new VncButtonListener();

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.i(TAG, Logging.getCurrentMethodName());
        ForApp.setWindowFlagsToKeepDeviceOn(this);
        registerReceiver(mReceiver, new IntentFilter(Settings.INTENT_TAG));

        startService(new Intent(this, ScreenSharingServer.class).putExtra(Settings.LAUNCH_INFO,
                ForApp.getLaunchInfo(ScreenSharingServerActivity.this)).putExtra(
                Settings.INTERNAL_PATH, ForApp.getInternalPath(ScreenSharingServerActivity.this)));
    }
    
    @Override
    public void onDestroy() {
        super.onDestroy();
        try {
            unregisterReceiver(mReceiver);
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        }
    }

    public BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            Log.i(TAG, Logging.getCurrentMethodName());
            if (intent.getBooleanExtra(Settings.SHOW_START_PROGRESS, false)) {
                progress = new ProgressDialog(ScreenSharingServerActivity.this,
                        ProgressDialog.STYLE_SPINNER);
                progress.show();
                progress.setCancelable(false);
                progress.setMessage("Launching ScreenSharingServer");
                vncDialog = new Dialog(ScreenSharingServerActivity.this);
            } else if (intent.getBooleanExtra(Settings.SHOW_VNC_DIALOG, false)) {
                startVncDialog();
            } else if (intent.getBooleanExtra(Settings.DISMISS_PROGRESS, false)) {
                progress.dismiss();
                finish();
            }
        }
    };

    public Dialog createVncDialog() {
        Log.i(TAG, Logging.getCurrentMethodName());
        Dialog dialog = new Dialog(ScreenSharingServerActivity.this);
        dialog.setContentView(R.layout.vncdialog);
        dialog.setTitle("Please confirm");
        TextView txt = (TextView) dialog.findViewById(R.id.vncDialogText);
        txt.setText("Start screen sharing session as a server?\n");
        Button yes = (Button) dialog.findViewById(R.id.vncOk);
        yes.setOnClickListener(mvbl);
        Button no = (Button) dialog.findViewById(R.id.vncCancel);
        no.setOnClickListener(this.mvbl);
        return dialog;
    }

    public void startVncDialog() {
        Log.i(TAG, Logging.getCurrentMethodName());
        if (vncDialog != null) {
            if (vncDialog.isShowing()) {
                Log.i(TAG, "Duplicate vnc requests!");
                return;
            }
        }
        vncDialog = createVncDialog();
        vncDialog.show();
    }

    class VncButtonListener implements OnClickListener {
        @Override
        public void onClick(View v) {
            Log.i(TAG, Logging.getCurrentMethodName());
            switch (v.getId()) {
                case R.id.vncOk:
                    Log.i(TAG, "VNC dialog : OK");
                    sendBroadcast(new Intent(Settings.INTENT_TAG).putExtra(Settings.VNC_DIALOG_OK,
                            true));
                    vncDialog.dismiss();
                    finish();
                    break;
                case R.id.vncCancel:
                    Log.i(TAG, "VNC dialog : CANCEL");
                    sendBroadcast(new Intent(Settings.INTENT_TAG).putExtra(
                            Settings.VNC_DIALOG_CANCEL, true));
                    vncDialog.dismiss();
                    android.os.Process.killProcess(android.os.Process.myPid());
                    break;
            }
        }
    }
}
