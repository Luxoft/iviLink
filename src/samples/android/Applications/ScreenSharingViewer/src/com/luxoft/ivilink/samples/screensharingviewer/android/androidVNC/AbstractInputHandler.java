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

import android.view.KeyEvent;
import android.view.MotionEvent;

/**
 * The VncCanvasActivity has several different ways of handling input from the touchscreen,
 * keyboard, buttons and trackball.  These will be represented by different implementations
 * of this interface.  Putting the different modes in different classes
 * will keep the logic clean.  The relevant Activity callbacks in VncCanvasActivity
 * are forwarded to methods in AbstractInputHandler.
 * <p>
 * It is expected that the implementations will be contained within
 * VncCanvasActivity, so they can do things like super.VncCanvasActivity.onXXX to invoke
 * default behavior.
 * @author Michael A. MacDonald
 *
 */
interface AbstractInputHandler {
	/**
	 * Note: Menu key code is handled before this is called
	 * @see android.app.Activity#onKeyDown(int keyCode, KeyEvent evt)
	 */
	boolean onKeyDown(int keyCode, KeyEvent evt);
	/**
	 * Note: Menu key code is handled before this is called
	 * @see android.app.Activity#onKeyUp(int keyCode, KeyEvent evt)
	 */
	boolean onKeyUp(int keyCode, KeyEvent evt);
	/* (non-Javadoc)
	 * @see android.app.Activity#onTrackballEvent(android.view.MotionEvent)
	 */
	boolean onTrackballEvent( MotionEvent evt);
	/* (non-Javadoc)
	 * @see android.app.Activity#onTrackballEvent(android.view.MotionEvent)
	 */
	boolean onTouchEvent( MotionEvent evt);
	
	/**
	 * Return a user-friendly description for this mode; it will be displayed in a toaster
	 * when changing modes.
	 * @return
	 */
	CharSequence getHandlerDescription();
	
	/**
	 * Return an internal name for this handler; this name will be stable across language
	 * and version changes
	 */
	String getName();
}
