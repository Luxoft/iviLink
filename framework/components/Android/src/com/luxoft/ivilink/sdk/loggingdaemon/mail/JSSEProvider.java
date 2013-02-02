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
package com.luxoft.ivilink.sdk.loggingdaemon.mail;

import java.security.AccessController;
import java.security.Provider;

/**
 * Code taken from here: http://stackoverflow.com/questions/2020088/
 */
public final class JSSEProvider extends Provider {

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	public JSSEProvider() {
		super("HarmonyJSSE", 1.0, "Harmony JSSE Provider");
		AccessController
				.doPrivileged(new java.security.PrivilegedAction<Void>() {
					public Void run() {
						put("SSLContext.TLS",
								"org.apache.harmony.xnet.provider.jsse.SSLContextImpl");
						put("Alg.Alias.SSLContext.TLSv1", "TLS");
						put("KeyManagerFactory.X509",
								"org.apache.harmony.xnet.provider.jsse.KeyManagerFactoryImpl");
						put("TrustManagerFactory.X509",
								"org.apache.harmony.xnet.provider.jsse.TrustManagerFactoryImpl");
						return null;
					}
				});
	}
}
