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
 * Copyright 2010 Michael A. MacDonald
 */
package com.luxoft.ivilink.samples.screensharingviewer.android.androidVNC;

import android.os.Handler;
import android.os.SystemClock;
import android.view.MotionEvent;

/**
 * Specialization of panner that moves the mouse instead of panning the screen
 * 
 * @author Michael A. MacDonald
 *
 */
class MouseMover extends Panner {

	public MouseMover(VncCanvasActivity act, Handler hand) {
		super(act, hand);
	}

	/* (non-Javadoc)
	 * @see java.lang.Runnable#run()
	 */
	@Override
	public void run() {
		long interval = SystemClock.uptimeMillis() - lastSent;
		lastSent += interval;
		double scale = (double)interval / 50.0;
		VncCanvas canvas = activity.vncCanvas;
		//Log.v(TAG, String.format("panning %f %d %d", scale, (int)((double)velocity.x * scale), (int)((double)velocity.y * scale)));
		if ( canvas.processPointerEvent((int)(canvas.mouseX + ((double)velocity.x * scale)), (int)(canvas.mouseY + ((double)velocity.y * scale)), MotionEvent.ACTION_MOVE, 0, false, false))
		{
			if (updater.updateVelocity(velocity, interval))
			{
				handler.postDelayed(this, 50);
			}
			else
			{
				//Log.v(TAG, "Updater requests stop");
				stop();
			}
		}
		else
		{
			//Log.v(TAG, "Panning failed");
			stop();
		}
	}

}
