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

import java.nio.ByteBuffer;

import android.util.Log;
import android.util.SparseArray;

import com.luxoft.ivilink.sdk.android.lib.utils.Assertion;
import com.luxoft.ivilink.sdk.android.lib.utils.log.BigLog;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;
import com.luxoft.ivilink.sdk.android.nonnative.RetVal.ErrorCodes;

/**
 * General profile class. All profile implementations must inherit it and
 * implement user-specified profile API.
 */
public abstract class AbstractProfile {
    // set from the native code, in case of refactoring don't forget to update
    // the jni layer
    private long mNativeProfileInstance;
    private long mNativeProfileHolder;

    static {
        System.loadLibrary("AndroidCommonLibNative");
    }

    /**
     * Contains DataChannel instances for all allocated channels.
     */
    private SafeAllocatedChannelsMap<DataChannel> mAllocatedChannelsMap = new SafeAllocatedChannelsMap<DataChannel>();

    /**
     * Wraps SparseArray to provide thread safety.
     */
    private class SafeAllocatedChannelsMap<E> {
        private SparseArray<E> mArray = new SparseArray<E>();

        E get(int key) {
            synchronized (mArray) {
                return mArray.get(key);
            }
        }

        void append(int key, E value) {
            synchronized (mArray) {
                mArray.append(key, value);
            }
        }

        void delete(int key) {
            synchronized (mArray) {
                mArray.delete(key);
            }
        }

        boolean contains(int key) {
            synchronized (mArray) {
                return mArray.get(key) != null;
            }
        }
    }

    /**
     * Logging tag.
     */
    private final String tag = "ProfileImplementationJava";

    /**
     * Profile callbacks. The pointer to the Object that was used in CApp's
     * registerProfileCallbacks for this profile's ProfileAPI.
     */

    protected final Object getProfileApiCallbacks() {
        return getProfileApiCallbacks(mNativeProfileInstance);
    }

    /**
     * Constructor, is invoked in ProfileCreator, shouldn't be called directly.
     * All subclasses' constructors must have the same signature.
     * 
     * @param profileIUID
     *            Profile implementation UID.
     * @param serviceUID
     *            Service UID.
     * @param nativeCallbacksPointer
     *            Serialized pointer to NonnativeCallbacksWrapper (where java
     *            callbacks can be obtained)
     */
    public AbstractProfile(String profileIUID, String serviceUID, long nativeCallbacksPointer) {
        Log.v(tag, Logging.getCurrentMethodName(profileIUID, serviceUID, nativeCallbacksPointer));
        createNativeProfile(profileIUID, serviceUID, nativeCallbacksPointer);
        BigLog.i(tag, "mNativeProfileHolder = " + mNativeProfileHolder, '!');
        BigLog.i(tag, "mNativeProfileInstance = " + mNativeProfileInstance, '!');
    }

    /**
     * Creates a NativeProfileImplementation instance. During this call
     * mNativeProfileInstance and mNativeProfileHolder are initialized from the
     * jni layer.
     * 
     * @param profileIUID
     *            Profile implementation UID.
     * @param serviceUID
     *            Service UID.
     * @param nativeCallbacksPointer
     *            Serialized pointer to FakeCallbacks (where java callbacks can
     *            be obtained)
     */
    private native void createNativeProfile(String profileIUID, String serviceUID,
            long nativeCallbacksPointer);

    /**
     * Getter for native ProfileHolfer instance.
     * 
     * @return pointer to the ProfileHolder instance that contains our
     *         NativeProfileImplementation instance
     */
    public final long getNativeProfileHolder() {
        return mNativeProfileHolder;
    }

    private native Object getProfileApiCallbacks(long profileInstance);

    /**
     * Allocates channel with ChannelSupervisor.
     * 
     * @param tag
     *            Channel tag
     * @return Channel ID number
     */
    protected final RetVal allocateChannel(DataChannel info) {
        Log.v(tag, Logging.getCurrentMethodName(info));
        Assertion.check(info != null);
        Assertion.check(info.getTag() != null);
        String result = allocateChannelNative(mNativeProfileInstance, info.getTag(), info
                .getPriority().getIntegerValue());
        return processAllocationResult(result, info);
    }

    private native String allocateChannelNative(long profileInstance, String tag,
            int serializedPriority);

    /**
     * Allocates channel with ChannelSupervisor. It is expected that the paired
     * profile will call allocateChannelAsClient with the same tag
     * 
     * @param tag
     *            Channel tag
     * @return Channel ID number
     */
    protected final RetVal allocateChannelAsServer(DataChannel info) {
        Log.v(tag, Logging.getCurrentMethodName(info));
        Assertion.check(info != null);
        Assertion.check(info.getTag() != null);
        String result = allocateChannelAsServerNative(mNativeProfileInstance, info.getTag(), info
                .getPriority().getIntegerValue());
        return processAllocationResult(result, info);
    }

    private native String allocateChannelAsServerNative(long profileInstance, String tag,
            int serializedPriority);

    /**
     * Allocates channel with ChannelSupervisor. It is expected that the paired
     * profile will call allocateChannelAsServer with the same tag
     * 
     * @param tag
     *            Channel tag
     * @return Channel ID number
     */
    protected final RetVal allocateChannelAsClient(DataChannel info) {
        Log.v(tag, Logging.getCurrentMethodName(info));
        Assertion.check(info != null);
        Assertion.check(info.getTag() != null);
        String result = allocateChannelAsClientNative(mNativeProfileInstance, info.getTag(), info
                .getPriority().getIntegerValue());
        return processAllocationResult(result, info);
    }

    private native String allocateChannelAsClientNative(long nativeProfileInstance, String tag,
            int serializedPriority);

    private RetVal processAllocationResult(String allocationResult, DataChannel info) {
        String[] channelIDandError = allocationResult.split("#");
        Assertion.check(channelIDandError.length >= 2);
        if (channelIDandError.length > 2) {
            for (int i = 2; i < channelIDandError.length; i++) {
                channelIDandError[1] += channelIDandError[i];
            }
        }
        try {
            info.setID(Integer.parseInt(channelIDandError[0]));
        } catch (NumberFormatException e) {
            e.printStackTrace();
            return new RetVal(ErrorCodes.DESERIALIZATION_ERROR, "could not parse channel number",
                    false);
        }
        RetVal result = RetVal.deserialize(channelIDandError[1]);
        info.setValid(result.isNoError());
        if (result.isNoError()) {
            mAllocatedChannelsMap.append(info.getID(), info);
        }
        return result;
    }

    /**
     * Deallocates channel with ChannelSupervisor.
     * 
     * @param channelID
     *            Previously allocated channel ID number.
     */
    protected final RetVal deallocateChannel(DataChannel info) {
        Log.v(tag, Logging.getCurrentMethodName(info));
        if (info == null || !info.isValid()) {
            return new RetVal(ErrorCodes.INVALID_CHANNEL_INFO, "DataChannel provided: "
                    + (info == null ? "null" : info.toString()) + " is not valid", false);
        }
        if (!mAllocatedChannelsMap.contains(info.getID())) {
            return new RetVal(ErrorCodes.CHANNEL_NOT_FOUND, "DataChannel provided: "
                    + info.toString() + " does not match any of the previously allocated channels",
                    false);
        }
        String result = deallocateChannelNative(mNativeProfileInstance, info.getID());
        RetVal err = RetVal.deserialize(result);
        if (err.isNoError()) {
            mAllocatedChannelsMap.delete(info.getID());
            info.invalidate();
        }
        return err;
    }

    private native String deallocateChannelNative(long profileInstance, int channelID);

    /**
     * Send data to the other side.
     * 
     * @param data
     *            Data to send (currently limited to 4084 bytes)
     * @param channelID
     *            Previously allocated channel ID number.
     */
    protected final RetVal sendData(byte[] data, DataChannel info) {
        if (info == null || !info.isValid()) {
            return new RetVal(ErrorCodes.INVALID_CHANNEL_INFO, "DataChannel provided: "
                    + (info == null ? "null" : info.toString()) + " is not valid", false);
        }
        if (!mAllocatedChannelsMap.contains(info.getID())) {
            return new RetVal(ErrorCodes.CHANNEL_NOT_FOUND, "DataChannel provided: "
                    + info.toString() + " does not match any of the previously allocated channels",
                    false);
        }
        if (data.length > 4084) {
            return new RetVal(ErrorCodes.BUFFER_TOO_LARGE, "Buffer is too large: " + data.length,
                    false);
        }
        Log.v(tag, Logging.getCurrentMethodName(data, info));
        String result = sendDataNative(mNativeProfileInstance, data, info.getID());
        return RetVal.deserialize(result);
    }

    /**
     * Send data to the other side.
     * 
     * @param data
     *            Data to send (currently limited to 4084 bytes)
     * @param channelID
     *            Previously allocated channel ID number.
     */
    protected final RetVal sendData(ByteBuffer buffer, DataChannel info) {
        return sendData(buffer.array(), info);
    }

    private final native String sendDataNative(long profileInstance, byte[] data, int channelID);

    /**
     * Gets profile name (for internal use).
     * 
     * @return Profile name
     */
    public abstract String getName();

    /**
     * Gets profile version (for internal use).
     * 
     * @return Profile version
     */
    public abstract int getVersion();

    /**
     * Gets profile UID (for internal use).
     * 
     * @return Profile UID
     */
    public abstract String getUid();

    /**
     * Gets profile API UID (for internal use).
     * 
     * @return Profile API UID
     */
    public abstract String getProfileApiUid();

    /**
     * The following methods are callbacks invoked from the background thread.
     * Should be overriden.
     */

    /**
     * Invoked when the profile has been successfully loaded (just after
     * creation) in PMAL/PIM.
     * 
     */
    protected abstract void onEnable();

    /**
     * Invoked when the profile is being unloaded in PMAL/PIM.
     * 
     */
    protected abstract void onDisable();

    protected final void onChannelDeleted(int channelID) {
        Log.v(tag, Logging.getCurrentMethodName(channelID));
        DataChannel channel = mAllocatedChannelsMap.get(channelID);
        if (channel != null) {
            channel.setValid(false);
            channel.setID(0);
            onChannelDeleted(channel);
        } else {
            Log.e(tag, "Could not find this channel: " + channelID + " among allocated channels");
        }
    }

    /**
     * Invoked when one of the previously allocated channels has been
     * deallocated from the other side
     * 
     * @param channel
     *            DataChannel info on the deallocated channel
     * 
     */
    protected abstract void onChannelDeleted(DataChannel channel);

    /**
     * Invoked when the Connectivity Agent connection has been broken.
     * 
     */
    protected abstract void onConnectionLost();

    protected final void onDataReceived(byte[] data, int channelID) {
        Log.v(tag, Logging.getCurrentMethodName(data, channelID));
        DataChannel channel = mAllocatedChannelsMap.get(channelID);
        if (channel != null) {
            onDataReceived(ByteBuffer.wrap(data), channel);
        } else {
            Log.e(tag, "Could not find this channel: " + channelID + " among allocated channels");
        }
    }

    /**
     * Invoked when data comes from the other side. Should be overridden by
     * subclasses
     * 
     * @param data
     *            Data sent from the other side
     * @param channel
     *            Previously allocated channel used to send the data
     * 
     */
    protected abstract void onDataReceived(ByteBuffer data, DataChannel channel);

    protected final void onBufferOverflow(int channelID) {
        Log.v(tag, Logging.getCurrentMethodName(channelID));
        DataChannel channel = mAllocatedChannelsMap.get(channelID);
        if (channel != null) {
            onBufferOverflow(channel);
        } else {
            Log.e(tag, "Could not find this channel: " + channelID + " among allocated channels");
        }
    }

    /**
     * Invoked when the send/receive buffer of a previously allocated channel is
     * filled, and no data can be sent/received.
     * 
     * @param channel
     */
    protected abstract void onBufferOverflow(DataChannel channel);
}
