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


package com.luxoft.ivilink.samples.climate.helpers;

import org.json.JSONException;
import org.json.JSONObject;

import android.util.Log;
import android.view.View.OnClickListener;
import android.widget.Button;

public class ButtonStateSetter {
	private final Button button;
	private final int activeDrawable;
	private final int inactiveDrawable;
	private final String jsonTag;

	public ButtonStateSetter(Button button, int trueDrawable, int falseDrawable, String jsonTag) {
		this.button = button;
		this.activeDrawable = trueDrawable;
		this.inactiveDrawable = falseDrawable;
		this.jsonTag = jsonTag;
	}

	public void setState(boolean isActive) {
		button.setBackgroundResource(isActive ? activeDrawable : inactiveDrawable);
	}

	public String getTag(){
		return jsonTag;
	}

	public void setOnClickListener(OnClickListener listener) {
		button.setOnClickListener(listener);
	}
}
