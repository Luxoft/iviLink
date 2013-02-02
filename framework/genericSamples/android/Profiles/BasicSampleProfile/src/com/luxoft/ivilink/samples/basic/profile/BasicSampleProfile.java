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
package com.luxoft.ivilink.samples.basic.profile;

import java.nio.BufferUnderflowException;
import java.nio.ByteBuffer;

import android.util.Log;

import com.luxoft.ivilink.samples.basic.api.BasicSampleAPI;
import com.luxoft.ivilink.sdk.android.lib.utils.Assertion;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;
import com.luxoft.ivilink.sdk.android.nonnative.AbstractProfile;
import com.luxoft.ivilink.sdk.android.nonnative.DataChannel;
import com.luxoft.ivilink.sdk.android.nonnative.RetVal;

public class BasicSampleProfile extends AbstractProfile implements BasicSampleAPI.ProfileAPI {

    private DataChannel mChannelInfo;

    private final String TAG = BasicSampleProfile.class.getName();

    BasicSampleAPI.ApplicationAPI castProfileCallbacks;

    private final static byte SEND_OPERANDS = 1;
    private final static byte SEND_RESULT = 2;

    public BasicSampleProfile(String profileIUID, String serviceUID, long nativeCallbacksPointer) {
        super(profileIUID, serviceUID, nativeCallbacksPointer);
        if (getProfileApiCallbacks() instanceof BasicSampleAPI.ApplicationAPI) {
            castProfileCallbacks = (BasicSampleAPI.ApplicationAPI) getProfileApiCallbacks();
        } else {
            throw new RuntimeException("wrong profile callbacks were supplied: "
                    + getProfileApiCallbacks().getClass().getName());
        }
    }

    @Override
    protected void onEnable() {
        Log.v(TAG, Logging.getCurrentMethodName());
        RetVal error = allocateChannel(mChannelInfo = new DataChannel("CBasicSampleProfileImpl"));
        Assertion.check(error.isNoError(), "Could not allocate channel");
        Log.i(TAG, "allocated channel: " + mChannelInfo.toString());
    }

    @Override
    protected void onDataReceived(ByteBuffer data, DataChannel channel) {
        Log.v(TAG, Logging.getCurrentMethodName(data, channel));
        Log.i(TAG, "got data: " + Logging.bytesToHexString(data));
        try {
            byte command = data.get();
            if (command == SEND_RESULT) {
                castProfileCallbacks.receiveResult(data.get());
            } else if (command == SEND_OPERANDS) {
                castProfileCallbacks.receiveOperands(data.get(), data.get());
            } else {
                Log.e(TAG, "Unknown command: " + command);
            }
        } catch (BufferUnderflowException e) {
            e.printStackTrace();
        }
    }

    // from BasicSampleAPI.ProfileAPI
    public void sendOperands(byte a, byte b) {
        Log.v(TAG, Logging.getCurrentMethodName(a, b));
        byte[] data = new byte[3];
        data[0] = SEND_OPERANDS;
        data[1] = a;
        data[2] = b;
        Assertion.check(sendData(data, mChannelInfo).isNoError());
    }

    public void sendResult(byte res) {
        Log.v(TAG, Logging.getCurrentMethodName(res));
        byte[] data = new byte[2];
        data[0] = SEND_RESULT;
        data[1] = res;
        Assertion.check(sendData(data, mChannelInfo).isNoError());
    }

    // from AbstractProfile - the following methods must be implemented
    @Override
    public String getName() {
        Log.v(TAG, Logging.getCurrentMethodName());
        return "BSP";
    }

    @Override
    public int getVersion() {
        Log.v(TAG, Logging.getCurrentMethodName());
        return 1;
    }

    @Override
    public String getUid() {
        Log.v(TAG, Logging.getCurrentMethodName());
        return "BasicSampleProfileImpl_UID";
    }

    @Override
    public String getProfileApiUid() {
        Log.v(TAG, Logging.getCurrentMethodName());
        return BasicSampleAPI.API_NAME;
    }

    @Override
    protected void onDisable() {
        Log.v(TAG, Logging.getCurrentMethodName());
        if (mChannelInfo.isValid()) {
            deallocateChannel(mChannelInfo);
        }
    }

    @Override
    protected void onChannelDeleted(DataChannel channel) {
        Log.v(TAG, Logging.getCurrentMethodName(channel));
    }

    @Override
    protected void onConnectionLost() {
        Log.v(TAG, Logging.getCurrentMethodName());
    }

    @Override
    protected void onBufferOverflow(DataChannel channel) {
        Log.v(TAG, Logging.getCurrentMethodName(channel));
    }

}
