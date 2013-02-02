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

import com.luxoft.ivilink.samples.seat.R;

import android.content.res.Configuration;
import android.widget.Button;

/*
 * manages the look of the driver/passenger buttons
 */

public class ChairSelection {
	static Button driverB, passengerB;
	
	static public void setButtons(Button driver, Button passenger){
		driverB=driver;
		passengerB=passenger;
	}
	
	static private ChairSelectionOptions currentSeat = ChairSelectionOptions.DRIVER;

	static public void setDriverBtnActive(int orientation) {
		currentSeat = ChairSelectionOptions.DRIVER;
		if (orientation == Configuration.ORIENTATION_PORTRAIT) {
			driverB.setBackgroundResource(R.drawable.port_driver_active);
			passengerB.setBackgroundResource(R.drawable.port_passenget_inactive);
		} else {
			driverB.setBackgroundResource(R.drawable.land_driver_active);
			passengerB.setBackgroundResource(R.drawable.land_passenget_inactive);
		}
	}

	static public void setPassengerBtnActive(int orientation) {
		currentSeat = ChairSelectionOptions.PASSENGER;
		if (orientation == Configuration.ORIENTATION_PORTRAIT) {
			driverB.setBackgroundResource(R.drawable.port_driver_inactive);
			passengerB.setBackgroundResource(R.drawable.port_passenger_active);
		} else {
			driverB.setBackgroundResource(R.drawable.land_driver_inactive);
			passengerB.setBackgroundResource(R.drawable.land_passenger_active);
		}
	}

	static public boolean isDriverSelected() {
		return currentSeat == ChairSelectionOptions.DRIVER;
	}

	static public boolean isPassengerSelected() {
		return currentSeat == ChairSelectionOptions.PASSENGER;
	}
}
