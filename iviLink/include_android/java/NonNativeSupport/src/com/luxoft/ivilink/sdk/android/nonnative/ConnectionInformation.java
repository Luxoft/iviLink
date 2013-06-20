package com.luxoft.ivilink.sdk.android.nonnative;

import com.luxoft.ivilink.utils.Assertion;

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
