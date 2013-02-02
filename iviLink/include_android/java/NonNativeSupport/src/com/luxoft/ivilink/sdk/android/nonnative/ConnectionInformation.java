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

import com.luxoft.ivilink.sdk.android.lib.utils.Assertion;

public class ConnectionInformation {
    static {
        System.loadLibrary("AndroidCommonLibNative");
    }

    private final static String DEFAULT_VALUE = "n/a";
    private String mTypeName = DEFAULT_VALUE;
    private String mRemoteAddress = DEFAULT_VALUE;
    private String mLocalAddress = DEFAULT_VALUE;

    public String getTypeName() {
        return mTypeName;
    }

    public String getRemoteAddress() {
        return mRemoteAddress;
    }

    public String getLocalAddress() {
        return mLocalAddress;
    }

    /**
     * Obtain connection information from the Connectivity Agent. In case of
     * success fields of the class will be initialized. In case of an error,
     * they will contain default values.
     * 
     * @return <code>no error</code> in case of success, some error with code
     *         indicating the problem otherwise.
     */
    public RetVal getInformation() {
        String result = getConnectionInformationNative();
        String[] splitResult = result.split("#!#");
        Assertion.check(splitResult.length == 4);
        RetVal retVal = RetVal.deserialize(splitResult[3]);
        if (retVal.isNoError()) {
            mTypeName = splitResult[0];
            mRemoteAddress = splitResult[1];
            mLocalAddress = splitResult[2];
        } else {
            mTypeName = DEFAULT_VALUE;
            mRemoteAddress = DEFAULT_VALUE;
            mLocalAddress = DEFAULT_VALUE;
        }
        return retVal;
    }

    private native String getConnectionInformationNative();
}
