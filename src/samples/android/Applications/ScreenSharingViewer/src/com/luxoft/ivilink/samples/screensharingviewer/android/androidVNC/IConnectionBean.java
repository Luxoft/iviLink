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
@TableInterface(ImplementingClassName="AbstractConnectionBean",TableName="CONNECTION_BEAN")
interface IConnectionBean {
	@FieldAccessor
	long get_Id();
	@FieldAccessor
	String getNickname();
	@FieldAccessor
	String getAddress();
	@FieldAccessor
	int getPort();
	@FieldAccessor
	String getPassword();
	@FieldAccessor
	String getColorModel();
	/**
	 * Records bitmap data implementation selection.  0 for auto, 1 for force full bitmap, 2 for force tiled
	 * <p>
	 * For historical reasons, this is named as if it were just a boolean selection for auto and force full.
	 * @return 0 for auto, 1 for force full bitmap, 2 for forced tiled
	 */
	@FieldAccessor
	long getForceFull();
	@FieldAccessor
	String getRepeaterId();
	@FieldAccessor
	String getInputMode();
	@FieldAccessor(Name="SCALEMODE")
	String getScaleModeAsString();
	@FieldAccessor
	boolean getUseLocalCursor();
	@FieldAccessor
	boolean getKeepPassword();
	@FieldAccessor
	boolean getFollowMouse();
	@FieldAccessor
	boolean getUseRepeater();
	@FieldAccessor
	long getMetaListId();
	@FieldAccessor(Name="LAST_META_KEY_ID")
	long getLastMetaKeyId();
	@FieldAccessor(DefaultValue="false")
	boolean getFollowPan();
	@FieldAccessor
	String getUserName();
	@FieldAccessor
	String getSecureConnectionType();
	@FieldAccessor(DefaultValue="true")
	boolean getShowZoomButtons();
	@FieldAccessor(Name="DOUBLE_TAP_ACTION")
	String getDoubleTapActionAsString();
}
