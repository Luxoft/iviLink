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


package com.luxoft.ivilink.samples.seat.helpers;

import android.util.Log;

/*
 * Helper class for the SeatPosition class.
 */
public class Limits {
	final String tag  = "SeatClientSample.Limits";
	private int low;
	private int high;
	public Limits(int l, int h){
		if(l<=h){
			low = l;
			high = h;
		}else{
			low = h;
			high = l;
		}
	}
	
	int getLow(){
		return low;
	}
	
	int getHigh(){
		return high;
	}
	
	private boolean within(int checkedValue){
		return checkedValue>=low && checkedValue <=high;
	}
	
	private int closestTo(int checkedValue){
		if(checkedValue-low <high-checkedValue){
			return low;
		}
		return high;
	}
	
	public int fullCheck(int value){
		if(within(value)){
			return value;
		}else{
			Log.w(tag, "Value "+value+" is not within limits ["+low+","+high+"];");
			int result = closestTo(value);
			Log.w(tag, "closest acceptable value is "+result);
			return result;
		}
	}
	
	int middle(){
		return low+(int)((double)(high-low)/2.0);
	}
}
