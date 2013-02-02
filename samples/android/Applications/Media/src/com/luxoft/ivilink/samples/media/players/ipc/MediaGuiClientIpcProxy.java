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

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;

import com.luxoft.ivilink.samples.media.interfaces.MediaGui;
import com.luxoft.ivilink.samples.media.interfaces.MediaGui.Server;
import com.luxoft.ivilink.samples.media.uiserver.LocalMediaGuiProxiesFactory;

/**
 * Wraps a {@link MediaGui.Client} stub into an InvokationHandler. All calls to
 * the stub are serialized into an Intent and are broadcast.
 */
public class MediaGuiClientIpcProxy extends InvocationSerializator<MediaGui.Client> {

    private Context mContext;
    private Server mServerProxy;

    /**
     * Registers a client stub in the {@link MediaGuiServer}
     * 
     * @param context
     *            will be used to send intents with serialized method calls to
     *            actual clients
     */
    public MediaGuiClientIpcProxy(Context context) {
        super(context, MediaGui.Client.class);
        mContext = context;
        mServerProxy = LocalMediaGuiProxiesFactory.getServerProxy();
        mServerProxy.registerClient(getMockInstance());
        mContext.registerReceiver(mBroadcastReceiver, new IntentFilter(
                CommunicationProtocol.TO_GUI_SERVER));
    }

    @Override
    public void onContextDestroyed() {
        mContext.unregisterReceiver(mBroadcastReceiver);
        mContext = null;
    }

    private BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            mServerProxy.onRequestUiState(getMockInstance());
        }
    };

    @Override
    protected String getIntentAction() {
        return CommunicationProtocol.TO_GUI_CLIENT;
    }

}
