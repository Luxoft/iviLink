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


package com.luxoft.ivilink.samples.basic;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.widget.TextView;

import com.luxoft.ivilink.samples.basic.api.BasicSampleAPI;
import com.luxoft.ivilink.sdk.android.lib.utils.ForApp;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;
import com.luxoft.ivilink.sdk.android.nonnative.ApplicationStateCallbacks;
import com.luxoft.ivilink.sdk.android.nonnative.ConnectionInformation;
import com.luxoft.ivilink.sdk.android.nonnative.IviLinkApplication;
import com.luxoft.ivilink.sdk.android.nonnative.ProfileProxyFactory;
import com.luxoft.ivilink.sdk.android.nonnative.RetVal;

public class BasicSampleActivity extends Activity implements ApplicationStateCallbacks,
        BasicSampleAPI.ApplicationAPI {

    IviLinkApplication mApp;
    BasicSampleAPI.ProfileAPI mProfileProxy;
    TextView mTextView;
    final static String TAG = BasicSampleActivity.class.getCanonicalName();
    BroadcastReceiver killer;

    @Override
    public void onCreate(Bundle icicle) {
        Log.v(TAG, Logging.getCurrentMethodName(icicle));
        super.onCreate(icicle);

        killer = ForApp.registerReceiverFromKillerApp(this);

        ForApp.setWindowFlagsToKeepDeviceOn(this);

        setContentView(R.layout.sampleapp);
        mTextView = (TextView) findViewById(R.id.resultsView);
        mTextView.setSingleLine(false);
        mTextView.setMovementMethod(new ScrollingMovementMethod());
        mTextView.setText("Hello iviLink!\n");
        ConnectionInformation info = new ConnectionInformation();
        if (info.getInformation().isNoError()) {
            mTextView.append("Connection type: " + info.getTypeName() + "\n");
            mTextView.append("Local addr: " + info.getLocalAddress() + "\n");
            mTextView.append("Remote addr: " + info.getRemoteAddress() + "\n");
        } else {
            mTextView.append("Could not get connection information! \n");
        }

        mApp = IviLinkApplication.createInstance((Activity) this, BasicSampleAPI.SERVICE_NAME);
    }

    @Override
    public void onResume() {
        Log.v(TAG, Logging.getCurrentMethodName());
        super.onResume();
        mApp.initInIVILink((ApplicationStateCallbacks) this);
    }

    @Override
    public void onPause() {
        Log.v(TAG, Logging.getCurrentMethodName());
        super.onPause();
        unregisterReceiver(killer);
        android.os.Process.killProcess(android.os.Process.myPid());
    }

    public void onInitDone(boolean isStartedByUser) {
        Log.v(TAG, Logging.getCurrentMethodName(isStartedByUser));
        if (!mApp.isLinkAlive()) {
            throw new RuntimeException();
        }
        if (isStartedByUser) {
            mApp.registerProfileCallbacks(BasicSampleAPI.API_NAME,
                                          (BasicSampleAPI.ApplicationAPI) this);
            mProfileProxy = ProfileProxyFactory.createProfileProxy(BasicSampleAPI.SERVICE_NAME,
                                                                   BasicSampleAPI.API_NAME,
                                                                   BasicSampleAPI.ProfileAPI.class);
            final RetVal loadResult = mApp.loadService(BasicSampleAPI.SERVICE_NAME);
            runOnUiThread(new Runnable() {
                public void run() {
                    mTextView.append("Loading service: " + BasicSampleAPI.SERVICE_NAME + "\n");
                }
            });
            if (loadResult.isNoError()) {
                mProfileProxy.sendOperands((byte) 0x4, (byte) 0x10);
            } else {
                runOnUiThread(new Runnable() {
                    public void run() {
                        mTextView.append("Failed to load service: " + BasicSampleAPI.SERVICE_NAME
                                + "\n");
                        mTextView.append(loadResult.toString() + "\n");
                    }
                });
                Log.e(TAG, "Failed to load service");
            }
        }
    }

    public void onIncomingServiceBeforeLoading(final String serviceUID) {
        Log.v(TAG, Logging.getCurrentMethodName(serviceUID));
        mProfileProxy = ProfileProxyFactory.createProfileProxy(serviceUID, BasicSampleAPI.API_NAME,
                                                               BasicSampleAPI.ProfileAPI.class);
        mApp.registerProfileCallbacks(BasicSampleAPI.API_NAME, (BasicSampleAPI.ApplicationAPI) this);
        runOnUiThread(new Runnable() {
            public void run() {
                mTextView.append("Incoming service: " + serviceUID + "\n");
            }
        });
    }

    public void onIncomingServiceAfterLoading(String serviceUID) {
        Log.v(TAG, Logging.getCurrentMethodName(serviceUID));
    }

    public void onServiceLoadError(final String serviceUID) {
        Log.v(TAG, Logging.getCurrentMethodName(serviceUID));
        runOnUiThread(new Runnable() {
            public void run() {
                mTextView.append("Service load error: " + serviceUID + "\n");
            }
        });
    }

    public void onServiceDropped(final String serviceUID) {
        Log.v(TAG, Logging.getCurrentMethodName(serviceUID));
        runOnUiThread(new Runnable() {
            public void run() {
                mTextView.append("Service dropped: " + serviceUID + "\n");
            }
        });
    }

    public void onPhysicalLinkDown() {
        Log.v(TAG, Logging.getCurrentMethodName());
    }

    public void onLinkEstablished() {
        Log.v(TAG, Logging.getCurrentMethodName());
    }

    /**
     * Application callback. Invoked by profile from a background thread.
     */
    public void receiveOperands(final byte a, final byte b) {
        Log.v(TAG, Logging.getCurrentMethodName(a, b));
        mProfileProxy.sendResult((byte) (a + b));
        runOnUiThread(new Runnable() {
            public void run() {
                mTextView.append("Got operands: " + a + ", " + b + "\n");
            }
        });
    }

    /**
     * Application callback. Invoked by profile from a background thread.
     */
    public void receiveResult(final byte res) {
        Log.v(TAG, Logging.getCurrentMethodName(res));
        runOnUiThread(new Runnable() {
            public void run() {
                mTextView.append("Got a result: " + res + "\n");
            }
        });
    }
}
