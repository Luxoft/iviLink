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
 
package com.luxoft.ivilink.samples.media.helpers;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;

import com.luxoft.ivilink.sdk.android.lib.utils.log.BigLog;

import android.content.Context;
import android.util.Log;

public class VlcServerLoader {
	private Thread vlcThread;
	private static final String tag = "iviLink.samples.Media.VlcServerHandler";
	private static final char logHighlight = 'L';
	private static VlcServerLoader instance = new VlcServerLoader();

	static {
		BigLog.i(tag, "in static loader block ", logHighlight);
		boolean hasNeon = false;
		try {
			FileReader fileReader = new FileReader("/proc/cpuinfo");
			BufferedReader br = new BufferedReader(fileReader);
			String line;
			while ((line = br.readLine()) != null) {
				Log.v(tag, "/proc/cpuinfo: " + line);
				if (line.contains("neon")) {
					hasNeon = true;
				}
			}
			fileReader.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		if (hasNeon) {
			BigLog.i(tag, "has NEON! Loading neon lib", logHighlight);
			System.loadLibrary("vlcjni");
		} else {
			BigLog.i(tag, "is a NO_NEON device! Loading non-neon lib",
					logHighlight);
			System.loadLibrary("vlcjninoneon");
		}
	}

	private VlcServerLoader() {
	}

	public static VlcServerLoader getInstance() {
		return instance;
	}

	public synchronized boolean launchVLC(Context context,
			final int pipeReadDescriptor) {
		if (vlcThread != null && vlcThread.isAlive()) {
			BigLog.w(tag, "vlc server has already started", logHighlight);
			return false;
		}
		Log.v(tag, "pipe descriptor is: " + pipeReadDescriptor);
		vlcThread = new Thread(new Runnable() {
			public void run() {
				launchVlc(pipeReadDescriptor);
				BigLog.e(tag, "vlc server stopped!", logHighlight);
			}
		});
		vlcThread.start();
		return true;
	}

	private native void launchVlc(int pipeFds);
}
