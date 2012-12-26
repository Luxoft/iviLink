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

/**
 * @author Michael A. MacDonald
 *
 */
class MetaKeyBase implements Comparable<MetaKeyBase> {
	int keySym;
	int mouseButtons;
	int keyEvent;
	String name;
	boolean isMouse;
	boolean isKeyEvent;
	
	MetaKeyBase(int mouseButtons, String name)
	{
		this.mouseButtons = mouseButtons;
		this.name = name;
		this.isMouse = true;
		this.isKeyEvent = false;
	}
	
	MetaKeyBase(String name, int keySym, int keyEvent)
	{
		this.name = name;
		this.keySym = keySym;
		this.keyEvent = keyEvent;
		this.isMouse = false;
		this.isKeyEvent = true;
	}
	
	MetaKeyBase(String name, int keySym)
	{
		this.name = name;
		this.keySym = keySym;
		this.isMouse = false;
		this.isKeyEvent = false;		
	}

	/* (non-Javadoc)
	 * @see java.lang.Comparable#compareTo(java.lang.Object)
	 */
	@Override
	public int compareTo(MetaKeyBase another) {
		return name.compareTo(another.name);
	}
}
