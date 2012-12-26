/* 
 * iviLINK SDK, version 1.1.19
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012, Luxoft Professional Corp., member of IBS group
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

import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

import android.util.Log;

/**
 * General profile class. All profile implementations should inherit it and
 * implement user-specified profile API.
 */
public abstract class AbstractProfile {
	private final String tag = AbstractProfile.class.getName();
	protected final Object profileCallbacks;

	/**
	 * Constructor. Invoked in ProfileCreator. Shouldn't be called directly.
	 * 
	 * @param profileIUID
	 *            Profile implementation UID.
	 * @param serviceUID
	 *            Service UID.
	 * @param nativeCallbacksPointer
	 *            Serialized pointer to FakeCallbacks (where java callbacks can
	 *            be obtained)
	 */
	public AbstractProfile(String profileIUID, String serviceUID, byte[] nativeCallbacksPointer) {
		Log.v(tag, Logging.getCurrentMethodName(profileIUID, serviceUID, nativeCallbacksPointer));
		createNativeProfile(profileIUID, serviceUID, nativeCallbacksPointer);
		profileCallbacks = getProfileCallbacks();
	}

	/**
	 * Creates native CProfileImplementation instance.
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
			byte[] nativeCallbacksPointer);

	/**
	 * Getter for native CProfileImplementation instance.
	 * 
	 * @return Serialized pointer to the internal CProfileImplementation
	 *         instance
	 */
	public final native byte[] getNativeCProfileInstance();

	/**
	 * Getter for native CProfileImplementation instance.
	 * 
	 * @return Serialized pointer to the internal CProfileImplementation
	 *         instance
	 */
	private native Object getProfileCallbacks();

	/**
	 * Allocates channel with ChannelSupervisor.
	 * 
	 * @param tag
	 *            Channel tag
	 * @return Channel ID number
	 */
	protected final native int allocateChannel(String tag);

	/**
	 * Allocates channel with ChannelSupervisor. It is expected that the paired
	 * profile will call allocateChannelAsClient with the same tag
	 * 
	 * @param tag
	 *            Channel tag
	 * @return Channel ID number
	 */
	protected final native int allocateChannelAsServer(String tag);

	/**
	 * Allocates channel with ChannelSupervisor. It is expected that the paired
	 * profile will call allocateChannelAsServer with the same tag
	 * 
	 * @param tag
	 *            Channel tag
	 * @return Channel ID number
	 */
	protected final native int allocateChannelAsClient(String tag);

	/**
	 * Deallocates channel with ChannelSupervisor.
	 * 
	 * @param channelID
	 *            Previously allocated channel ID number.
	 */
	protected final native void deallocateChannel(int channelID);

	/**
	 * Send data to the other side.
	 * 
	 * @param data
	 *            Data to send (currently limited to 4084 bytes)
	 * @param channelID
	 *            Previously allocated channel ID number.
	 */
	protected final native void sendData(byte[] data, int channelID);

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
	protected void onEnable() {
		Log.v(tag, Logging.getCurrentMethodName());
	}

	/**
	 * Invoked when the profile has been unloaded in PMAL/PIM.
	 * 
	 */
	protected void onDisable() {
		Log.v(tag, Logging.getCurrentMethodName());
	}

	/**
	 * Invoked when one of the previously allocated channels has been
	 * deallocated from the other side
	 * 
	 * @param channelID
	 *            Channel ID of the deallocated channel
	 * 
	 */
	protected void onChannelDeleted(int channelID) {
		Log.v(tag, Logging.getCurrentMethodName(channelID));
	}

	/**
	 * Invoked when the Connectivity Agent connection has been broken.
	 * 
	 */
	protected void onConnectionLost() {
		Log.v(tag, Logging.getCurrentMethodName());
	}

	/**
	 * Invoked when the data comes from the other side
	 * 
	 * @param data
	 *            Data sent from the other side
	 * @param channelID
	 *            Channel used to send the data
	 * 
	 */
	protected void onDataReceived(byte[] data, int channelID) {
		Log.v(tag, Logging.getCurrentMethodName(data, channelID));
	}

	/**
	 * Invoked when send/receive buffer is filled, and no data can be
	 * sent/received
	 * 
	 * @param channelID
	 */
	protected void onBufferOverflow(int channelID) {
		Log.v(tag, Logging.getCurrentMethodName(channelID));
	}
}
