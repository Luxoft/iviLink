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

/*
 * stores seat's angle and offsets, helps keep them within limits
 */
public class SeatPosition {
	private final int backAngleDef = 20;
	private int bottomX;
	private int bottomY;
	private int backAngle = backAngleDef;
	Limits verticalL;
	Limits horizontalL;
	Limits angleL;

	public SeatPosition(Limits vertical, Limits horizontal, Limits angle) {
		verticalL = vertical;
		horizontalL = horizontal;
		angleL = angle;
	}

	public int getBackAngleDef() {
		return backAngleDef;
	}

	public int getBottomX() {
		return bottomX;
	}

	public void setBottomX(int bottomX) {
		this.bottomX=horizontalL.fullCheck(bottomX);
	}

	public int getBottomY() {
		return bottomY;
	}

	public void setBottomY(int bottomY) {
		this.bottomY=verticalL.fullCheck(bottomY);
	}

	public int getBackAngle() {
		return backAngle;
	}

	public void setBackAngle(int backAngle) {
		this.backAngle=angleL.fullCheck(backAngle);
	}

	public int getXOffset() {
		int result= getPossibleAverageX()-bottomX;
		return result;
	}

	public int getYOffset() {
		int result = getPossibleAverageY()-bottomY;
		return result;
	}
	
	public int getPossibleAverageX(){
		return horizontalL.middle();
	}
	
	public int getPossibleAverageY(){
		return verticalL.middle();
	}
}
