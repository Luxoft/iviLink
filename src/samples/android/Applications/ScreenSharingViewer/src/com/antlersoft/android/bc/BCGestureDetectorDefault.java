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
package com.antlersoft.android.bc;

import android.content.Context;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;

/**
 * @author Michael A. MacDonald
 */
public class BCGestureDetectorDefault implements IBCGestureDetector {

	/* (non-Javadoc)
	 * @see com.antlersoft.android.bc.IBCGestureDetector#createGestureDetector(android.content.Context, android.view.GestureDetector.OnGestureListener)
	 */
	@Override
	public GestureDetector createGestureDetector(Context context,
			OnGestureListener listener) {
		return new GestureDetector(context, listener);
	}

}
