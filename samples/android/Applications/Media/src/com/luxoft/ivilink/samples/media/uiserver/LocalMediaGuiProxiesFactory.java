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
package com.luxoft.ivilink.samples.media.uiserver;

import java.lang.reflect.Proxy;

import com.luxoft.ivilink.samples.media.helpers.MainThreadInvocationHandler;
import com.luxoft.ivilink.samples.media.interfaces.MediaGui;
import com.luxoft.ivilink.samples.media.interfaces.MediaGui.Server;

public class LocalMediaGuiProxiesFactory {
    public static MediaGui getMediaGuiProxy() {
        return (MediaGui) Proxy.newProxyInstance(MediaGui.class.getClassLoader(),
                                                 new Class[] { MediaGui.class },
                                                 new MainThreadInvocationHandler(MediaGuiServer
                                                         .getInstance()));
    }

    public static Server getServerProxy() {
        return (Server) Proxy.newProxyInstance(Server.class.getClassLoader(),
                                               new Class[] { Server.class },
                                               new MainThreadInvocationHandler(MediaGuiServer
                                                       .getInstance()));
    }

}
