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

public interface ApplicationStateCallbacks {
    /**
     * Invoked after the application has been registered in Application Manager.
     * 
     * @param isStartedByUser
     *            true if the application has been started by user; false
     *            otherwise
     */
    public void onInitDone(boolean isStartedByUser);

    /**
     * Called before loading an incoming service.
     * 
     * @param serviceUID
     *            UID of an incoming service
     */
    public void onIncomingServiceBeforeLoading(String serviceUID);

    /**
     * Called after loading an incoming service.
     * 
     * @param serviceUID
     *            UID of a loaded incoming service
     */
    public void onIncomingServiceAfterLoading(String serviceUID);

    /**
     * Called if an error has occurred during service loading.
     * 
     * @param service
     *            UID of an incoming service that caused the error
     */
    public void onServiceLoadError(String serviceUID);

    /**
     * Called when a service session ends.
     * 
     * @param service
     *            UID of a service whose session has ended
     */
    public void onServiceDropped(String serviceUID);

    /**
     * Invoked when connection to the other side has been broken.
     */
    public void onPhysicalLinkDown();

    /**
     * Invoked when connection to the other side has been established.
     */
    public void onLinkEstablished();
}
