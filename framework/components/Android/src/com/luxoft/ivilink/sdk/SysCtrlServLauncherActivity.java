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


package com.luxoft.ivilink.sdk;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.res.Configuration;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.luxoft.ivilink.sdk.android.lib.utils.ForApp;
import com.luxoft.ivilink.sdk.android.lib.utils.ProcessesHelper;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;
import com.luxoft.ivilink.sdk.bluetooth.BluetoothStatus;
import com.luxoft.ivilink.sdk.helpers.AssetsInstaller;
import com.luxoft.ivilink.sdk.helpers.Common;
import com.luxoft.ivilink.sdk.helpers.DirectoryHelper;

/*
 * Entry point of iviLink's daemon.
 * If SystemControllerService is not running,
 * it checks directories and starts the Service if everything is ok.
 * Displays stack's startup progress.
 */
public class SysCtrlServLauncherActivity extends Activity {

    private static boolean isActive = false;

    static boolean isActive() {
        return isActive;
    }

    /*
     * Contains the following options: shutdown, reset, cancel (do nothing) is
     * shown when: a) app's icon was clicked and the Service is running already
     * b) status bar notification was clicked
     */
    private AlertDialog mAlertDialog;
    /*
     * Displayed during stack's startup process
     */
    private Dialog mStackStartProgress;
    private ProgressBar mStackStartProgressBar;
    private TextView mStackStartProgressText;
    private com.luxoft.ivilink.sdk.helpers.Spinner mSpinner;
    /*
     * Receives requests to update progress's text
     */
    private ProgressDialogBroadcastReceiver mProgressReceiver = new ProgressDialogBroadcastReceiver();
    // tag for logging
    private static final String TAG = SysCtrlServLauncherActivity.class.getName();

    @Override
    public void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        registerReceiver(mProgressReceiver, new IntentFilter(Common.ifProgress));

        if (getIntent().getBooleanExtra(Common.resetDialog, false)) {
            /*
             * activity was started via a pending intent from a status bar
             * notification
             */
            createAlertDialogThreeButtons();
            return;
        }
        if (!ProcessesHelper.isServiceRunning(this, SystemControllerService.class.getName())) {
            ForApp.setWindowFlagsToKeepDeviceOn(this);
            new InstallFilesTask().execute((Void) null);
        } else {
            Log.i(TAG, "Service is running already");
            createAlertDialogThreeButtons();
        }

    }

    @Override
    public void onResume() {
        Log.v(TAG, Logging.getCurrentMethodName());
        super.onResume();
        isActive = true;
    }

    @Override
    public void onPause() {
        Log.v(TAG, Logging.getCurrentMethodName());
        super.onPause();
        isActive = false;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        unregisterReceiver(mProgressReceiver);
        if (mSpinner != null) {
            mSpinner.stop();
        }
    }

    @Override
    public void onBackPressed() {
        Log.v(TAG, "Overriding back button");
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        Log.v(TAG, "overriding onConfigurationChanged");
        super.onConfigurationChanged(newConfig);
    }

    void createAlertDialogOneButton(String message) {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setMessage(message).setCancelable(false)
                .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        mAlertDialog.cancel();
                        SysCtrlServLauncherActivity.this.finish();
                    }
                });
        mAlertDialog = builder.create();
        mAlertDialog.show();
    }

    void createAlertDialogThreeButtons() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setMessage("Shutdown/restart IVILink?").setCancelable(false)
                .setPositiveButton("Shutdown", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        mAlertDialog.cancel();
                        SysCtrlServLauncherActivity.this.finish();
                        sendBroadcast(new Intent(Common.serviceBR).putExtra(Common.misc,
                                                                            Common.shutdown));
                    }
                }).setNeutralButton("Restart", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        mAlertDialog.cancel();
                        SysCtrlServLauncherActivity.this.finish();
                        sendBroadcast(new Intent(Common.serviceBR).putExtra(Common.misc,
                                                                            Common.reset));
                    }
                }).setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        mAlertDialog.cancel();
                        SysCtrlServLauncherActivity.this.finish();
                    }
                });
        mAlertDialog = builder.create();
        mAlertDialog.show();
    }

    class ProgressDialogBroadcastReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            String reqString = intent.getStringExtra(Common.message);
            int progressValue = intent.getIntExtra(Common.progressValue, 0);
            Log.i(this.getClass().getSimpleName(), "got intent with: " + reqString + " "
                    + progressValue);
            if (reqString != null)
                setProgressText(reqString, progressValue);
        }
    }

    /**
     * Displays message with progress bar value in a Dialog
     */
    void setProgressText(String msg, int progressValue) {
        if (msg.equals(Common.doneLaunch)) {
            if (mStackStartProgress != null) {
                if (mStackStartProgress.isShowing())
                    mStackStartProgress.dismiss();
            }
            this.finish();
            return;
        }
        if (mStackStartProgress == null) {
            mStackStartProgress = createProgressDialog();
            mStackStartProgressBar = (ProgressBar) mStackStartProgress
                    .findViewById(R.id.progressBar);
            mStackStartProgressText = (TextView) mStackStartProgress
                    .findViewById(R.id.progressText);
            mStackStartProgressBar.setProgress(progressValue);
            mStackStartProgressText.setText(msg);
            mStackStartProgress.show();
            mStackStartProgress.setCancelable(false);
        } else {
            mStackStartProgressBar.setProgress(progressValue);
            mStackStartProgressText.setText(msg);
        }
    }

    private Dialog createProgressDialog() {
        Dialog result = new Dialog(this, R.style.custom_dialog);
        result.setContentView(R.layout.progressdisplay);
        ImageView spinner = (ImageView) result.findViewById(R.id.spinnerImage);
        spinner.setScaleType(ImageView.ScaleType.CENTER);
        mSpinner = new com.luxoft.ivilink.sdk.helpers.Spinner(new Handler(), spinner, 100,
                getResources());
        result.setTitle("iviLink stack is starting...");
        return result;
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        // we get here after BluetoothHelper.requestDiscovery
        if (resultCode == RESULT_CANCELED) {
            Log.w(TAG, "RFESULT_CANCELED");
            startService(new Intent(this, SystemControllerService.class).putExtra("bluetooth",
                                                                                  false));
        } else {
            Log.i(TAG, "RESULT_OK, will be using bluetooth!");
            startService(new Intent(this, SystemControllerService.class)
                    .putExtra("bluetooth", true));
        }
    }

    private class InstallFilesTask extends AsyncTask<Void, Void, String> {
        private ProgressDialog mInstallationProgress;

        @Override
        protected void onPreExecute() {
            mInstallationProgress = new ProgressDialog(SysCtrlServLauncherActivity.this);
            mInstallationProgress.setMessage("Checking installed profiles...");
            mInstallationProgress.setCancelable(false);
            mInstallationProgress.setCanceledOnTouchOutside(false);
            mInstallationProgress.show();
        }

        protected String doInBackground(Void... nulls) {
            if (!DirectoryHelper.checkExternalStorageState()) {
                return "Please mount external storage and try again";
            }
            new AssetsInstaller().moveAssetsToExternalStorage(SysCtrlServLauncherActivity.this);
            if (!DirectoryHelper.checkProjectDirs()) {
                /*
                 * this may happen if the package was build without including
                 * profiles and xmls and they were not installed on the device
                 */
                return DirectoryHelper.createMissingDirs();
            }
            return null;
        }

        @Override
        protected void onPostExecute(String result) {
            mInstallationProgress.dismiss();
            if (result != null) {
                createAlertDialogOneButton(result);
            } else {
                setProgressText("Initializing...", 0);
                if (BluetoothStatus.isBluetoothUsable()) {
                    Log.i(TAG, "Bluetooth is available!");
                    BluetoothStatus.requestDiscoverable(SysCtrlServLauncherActivity.this);
                } else {
                    Log.e(TAG, "Bluetooth is not avaliable!");
                    startService(new Intent(SysCtrlServLauncherActivity.this,
                            SystemControllerService.class).putExtra("bluetooth", false));
                }
            }
        }
    }

}
