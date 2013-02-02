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
package com.luxoft.ivilink.sdk.android.nonnative;

import android.content.Context;
import android.os.Handler;
import android.util.Log;
import android.widget.Toast;

import com.luxoft.ivilink.sdk.android.lib.utils.Assertion;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

public class RetVal {

    private static Context applicationContext;
    private static Handler uiThreadHandler;

    static void setUpNotification(Context context, Handler uiHandler) {
        if (!isNotiSetUp()) {
            Assertion.check(context != null);
            Assertion.check(uiHandler != null);
            applicationContext = context;
            uiThreadHandler = uiHandler;
        }
    }

	public enum ErrorCodes {
		// should be kept up-to-date with c++ codes!
		IVILINK_NO_ERROR,
		UNKNOWN_ERROR,
		INITIALIZATION_ERROR,
		SERVICE_NOT_FOUND,
		SERVICE_ALREADY_LOADED,
		XML_PARSING_FAILED,
		CALLBACKS_NOT_REGISTERED,
		NO_PROFILES_FOUND,
		IPC_REQUEST_TIMEOUT,
		IPC_REQUEST_FAILED,
		PIM_ERROR, // for now: waiting for PMAL/PIM refactoring
		CHANNEL_NOT_FOUND,
		CHANNEL_ALLOCATION_WRONG_PRIORITY,
		CHANNEL_ALLOCATION_FATAL_ERROR,
		CHANNEL_DEALLOCATION_ERROR,
		SEND_TO_NOT_ALLOCATED_CHANNEL,
		// should always come right after c++ errors
		DESERIALIZATION_ERROR,
		// java-specific errors may be added here
		INVALID_CHANNEL_INFO,
        BUFFER_TOO_LARGE,
	}

    public final ErrorCodes mCode;
    public final String mMessage;
    private final boolean mIsRecoverable;

	/**
	 * @return <code>true</code> if this RetVal indicates successful result
	 */
	public boolean isNoError() {
		return mCode == ErrorCodes.IVILINK_NO_ERROR;
	}

    /**
     * @return <code>true</code> if the error indicated by this RetVal may be
     *         avoided by simply reattempting whatever it is you are trying to
     *         do
     */
    public boolean isRecoverable() {
        return mIsRecoverable;
    }

    /**
     * @return <code>true</code> if the error indicated by this RetVal is
     *         actually an error, and not a result of incorrect deserialization
     *         (may only happen if native error codes and java error codes were
     *         somehow modified and do not match after that)
     */
    public boolean isParsedCorrectly() {
        return mCode != ErrorCodes.DESERIALIZATION_ERROR;
    }

    private static boolean isNotiSetUp() {
        return uiThreadHandler != null && applicationContext != null;
    }

    RetVal(ErrorCodes code, String message, boolean recoverable) {
        Log.v(this.getClass().getSimpleName(), Logging.getCurrentMethodName(code, message));
        mCode = code;
        mMessage = message;
        mIsRecoverable = recoverable;
        if (!isNoError() && isNotiSetUp()) {
            uiThreadHandler.post(new Runnable() {
                public void run() {
                    Toast.makeText(applicationContext, RetVal.this.toString(), Toast.LENGTH_SHORT)
                            .show();
                }
            });
        }
    }

    static RetVal deserialize(String errorFromNative) {
        Log.v(RetVal.class.getSimpleName(), Logging.getCurrentMethodName(errorFromNative));
        String[] splitError = errorFromNative.split(":");
        Assertion.check(splitError.length >= 3);
        // if error message contained the separator symbol
        if (splitError.length > 3) {
            for (int i = 3; i < splitError.length; i++) {
                splitError[2] += splitError[i];
            }
        }
        int intCode;
        boolean recoverable;
        try {
            intCode = Integer.parseInt(splitError[0]);
            recoverable = Integer.parseInt(splitError[1]) == 1;
        } catch (Exception e) {
            // numberformatexception
            e.printStackTrace();
            return new RetVal(ErrorCodes.DESERIALIZATION_ERROR, splitError[2], true);
        }
        Assertion.check(intCode < ErrorCodes.DESERIALIZATION_ERROR.ordinal());
        if (intCode >= ErrorCodes.values().length - 1) {
            return new RetVal(ErrorCodes.DESERIALIZATION_ERROR, splitError[2], true);
        } else {
            // all ok
            return new RetVal(ErrorCodes.values()[intCode], splitError[2], recoverable);
        }
    }

    @Override
    public String toString() {
        return "Error Code: " + mCode.toString() + "; Message: " + mMessage + ";";
    }
}
