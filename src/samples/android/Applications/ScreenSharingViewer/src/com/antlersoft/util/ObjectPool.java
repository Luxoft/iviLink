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
package com.antlersoft.util;

/**
 * A pool of reusable object of a given type.  You get the object from a Entry, which you get
 * by calling reserve().  When you are done with the object, you call release() passing the Entry.
 * <p>
 * Failing to call release() does not leak memory--but you will not get the benefits
 * of reusing the object. You will run into contention issues if you
 * call release() while still holding a reference to the pool object.
 * @author Michael A. MacDonald
 *
 */
public abstract class ObjectPool<R> {
	public static class Entry<S> {
		S item;
		Entry<S> nextEntry;
		
		Entry(S i, Entry<S> n)
		{
			item = i;
			nextEntry = n;
		}
		
		public S get() {
			return item;
		}
	}
	
	private Entry<R> next;
	public ObjectPool()
	{
		next = null;
	}
	
	public Entry<R> reserve()
	{
		if (next == null)
		{
			next = new Entry<R>(itemForPool(), null);
		}
		Entry<R> result = next;
		next = result.nextEntry;
		result.nextEntry = null;
		
		return result;
	}
	
	public void release(Entry<R> entry)
	{
		entry.nextEntry = next;
		next = entry;
	}
	
	protected abstract R itemForPool();
}
