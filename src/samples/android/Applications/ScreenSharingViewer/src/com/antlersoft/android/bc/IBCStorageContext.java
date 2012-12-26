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
 * Copyright (c) 2010 Michael A. MacDonald
 */
package com.antlersoft.android.bc;

import android.content.Context;

import java.io.File;

/**
 * Provides a way to access the directory on external storage as returned by
 * Context.getExternal... added in API 8 that will work with earlier API releases.
 * @author Michael A. MacDonald
 *
 */
public interface IBCStorageContext {
	/**
	 * 
	 * @param context Context within the application with which the storage will be associated
	 * @param type May be null; if specified, references a sub-directory within the base directory
	 * for the app in the external storage
	 * @return File representing abstract path of storage directory; refer to android.os.Environment to
	 * see if the path is actually accessible
	 */
	public File getExternalStorageDir(Context context, String type);
}
