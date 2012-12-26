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
 
/**
 * Copyright (C) 2009 Michael A. MacDonald
 */
package com.luxoft.ivilink.samples.screensharingviewer.android.androidVNC;

import com.antlersoft.android.db.*;

/**
 * @author Michael A. MacDonald
 *
 */
@TableInterface(TableName="MOST_RECENT",ImplementingIsAbstract=false,ImplementingClassName="MostRecentBean")
public interface IMostRecentBean {
	@FieldAccessor
	long get_Id();
	@FieldAccessor(Name="CONNECTION_ID")
	long getConnectionId();
	@FieldAccessor(Name="SHOW_SPLASH_VERSION")
	long getShowSplashVersion();
	@FieldAccessor(Name="TEXT_INDEX")
	long getTextIndex();
}
