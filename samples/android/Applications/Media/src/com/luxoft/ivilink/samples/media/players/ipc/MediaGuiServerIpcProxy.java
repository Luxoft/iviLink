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
package com.luxoft.ivilink.samples.media.players.ipc;

import java.lang.reflect.Method;
import java.util.HashSet;
import java.util.Set;

import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;

import com.luxoft.ivilink.samples.media.interfaces.MediaGui;
import com.luxoft.ivilink.samples.media.interfaces.MediaGui.Client;

/**
 * When triggered, deserializes the method call and tries to invoke it with all
 * previously registered {@link MediaGui.Client} implementation instances.
 */
public class MediaGuiServerIpcProxy extends InvocationDeserializator implements
        MediaGui.Server {

    private Set<Client> mClients = new HashSet<Client>();
    private Context mContext;

    public MediaGuiServerIpcProxy(Context context) {
        super(Client.class.getMethods());
        mContext = context;
        mContext.registerReceiver(this, new IntentFilter(CommunicationProtocol.TO_GUI_CLIENT));
    }

    public void onContextDestroyed() {
        mContext.unregisterReceiver(this);
        mContext = null;
    }

    /**
     * Adds a client to the set of registered clients. After that its methods
     * will be called when relevant signals come from the
     * MediaGuiClientIpcProxy. A request for full state is sent to the server to
     * update the newly-registered client immediately.
     */
    public void registerClient(Client client) {
        synchronized (mClients) {
            if (mClients.add(client)) {
                onRequestUiState(client);
            }
        }
    }

    /**
     * Removes a client from the set of registered clients. After that its
     * methods will not called when relevant signals come from the
     * MediaGuiClientIpcProxy.
     */
    public void unregisterClient(Client client) {
        synchronized (mClients) {
            mClients.remove(client);
        }
    }

    public void onRequestUiState(Client requester) {
        if (mContext != null) {
            mContext.sendBroadcast(new Intent(CommunicationProtocol.TO_GUI_SERVER));
        }
    }

    @Override
    protected void onReceivedRemoteMethodCall(Method method, Object[] args) {
        synchronized (mClients) {
            for (Client client : mClients) {
                try {
                    method.invoke(client, args);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
    }
}
