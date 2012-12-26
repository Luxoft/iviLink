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

import android.view.View;
import android.view.HapticFeedbackConstants;

/**
 * Implementation for SDK version >= 3
 * @author Michael A. MacDonald
 */
class BCHapticDefault implements IBCHaptic {

	/* (non-Javadoc)
	 * @see com.antlersoft.android.bc.IBCHaptic#performLongPressHaptic(android.view.View)
	 */
	@Override
	public boolean performLongPressHaptic(View v) {
		return v.performHapticFeedback(HapticFeedbackConstants.LONG_PRESS,HapticFeedbackConstants.FLAG_IGNORE_GLOBAL_SETTING|HapticFeedbackConstants.FLAG_IGNORE_VIEW_SETTING
				);
	}

	/* (non-Javadoc)
	 * @see com.antlersoft.android.bc.IBCHaptic#setIsHapticEnabled(android.view.View, boolean)
	 */
/*
 * 	@Override
	public boolean setIsHapticEnabled(View v, boolean enabled) {
		return v.setHapticFeedbackEnabled(hapticFeedbackEnabled)
	}
*/
}
