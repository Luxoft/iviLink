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
 
package com.luxoft.ivilink.samples.navi;

import java.util.List;
import java.util.Map;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import android.app.Service;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.location.Location;
import android.os.Handler;
import android.os.IBinder;
import android.util.Log;
import android.widget.Toast;

import com.google.android.maps.GeoPoint;
import com.luxoft.ivilink.samples.navi.api.NaviAPI;

public class NaviService extends Service {
	private static final String tag = "iviLink.samples.Navi.NaviService";

	NaviSenderProfileProxy profileProxy;

	final int speed = 300;

	Handler poster = new Handler();

	List<String> lastMessage;

	private static List<GeoPoint> route = null;
	private static Map<GeoPoint, List<String>> tips = null;
	private static double[] angles = null;

	private static volatile boolean doNothingFlag = true;
	private static volatile boolean doNotDrawAnything = false;

	static Bitmap car;
	static Bitmap pin;
	static Bitmap rotatedCar;

	private static int currentPosition = -1;

	private static Location lastKnownLocation;

	private static Location currentLocation;

	private static Location targetLocation;

	@Override
	public IBinder onBind(Intent arg0) {
		return null;
	}

	@Override
	public int onStartCommand(Intent intent, int startId, int flags) {
		pin = BitmapFactory.decodeResource(getResources(), R.drawable.start);
		car = BitmapFactory.decodeResource(getResources(), R.drawable.vw50x25);
		rotatedCar = BitmapFactory.decodeResource(getResources(),
				R.drawable.vw50x25);
		new Thread(new Imitation()).start();
		profileProxy = new NaviSenderProfileProxy(NaviAPI.profileApi,
				Navi.SERVICE_UID);
		return super.onStartCommand(intent, startId, flags);
	}

	public static boolean doNothing() {
		return doNothingFlag;
	}

	public static void setDoNothing(boolean doNothingFlag) {
		NaviService.doNothingFlag = doNothingFlag;
	}

	public static boolean doNotDrawAnything() {
		return doNotDrawAnything;
	}

	public static void setDoNotDrawAnything(boolean doNotDrawAnything) {
		NaviService.doNotDrawAnything = doNotDrawAnything;
	}

	public double[] getAngles() {
		return angles;
	}

	public static void setAngles(double[] angles) {
		NaviService.angles = angles;
	}

	public Bitmap rotateCar() {
		Bitmap result;
		if (getCurrentPosition() == -1)
			return car;
		Matrix mtx = new Matrix();
		if (angles.length <= getCurrentPosition())
			return car;
		mtx.postRotate((float) angles[getCurrentPosition()]);
		try {
			result = Bitmap.createBitmap(car, 0, 0, car.getWidth(),
					car.getHeight(), mtx, true);
		} catch (Exception e) {
			e.printStackTrace();
			return car;
		}
		return result;
	}

	class Imitation implements Runnable {
		public void run() {
			while (true) {
				if (doNothingFlag) {
					try {
						Thread.sleep(speed);
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
					continue;
				}
				poster.post(new Runnable() {
					public void run() {
						if (targetLocation != null && getRoute() != null) {
							if (getCurrentPosition() < getRoute().size() - 1) {
								final Bitmap rotatedCarTemp = rotateCar();
								final GeoPoint t = getRoute().get(
										getCurrentPosition());
								setCurrentPosition(getCurrentPosition() + 1);
								currentLocation.setLatitude((double) t
										.getLatitudeE6() / 1e6d);
								currentLocation.setLongitude((double) t
										.getLongitudeE6() / 1e6d);
								rotatedCar = rotatedCarTemp;
								if (getTips() != null) {
									if (getTips().get(t) != null) {
										sendUpdate(getTips().get(t));
									}
								}
							} else {
								setCurrentPosition(-1);
								lastKnownLocation.setLatitude(targetLocation
										.getLatitude());
								lastKnownLocation.setLongitude(targetLocation
										.getLongitude());
								currentLocation.setLatitude(targetLocation
										.getLatitude());
								currentLocation.setLongitude(targetLocation
										.getLongitude());
								Toast.makeText(NaviService.this,
										"You have arrived at the destination",
										Toast.LENGTH_LONG).show();
								targetLocation = null;
								doNothingFlag = true;
								setRoute(null);
								Navi.routeToDraw = null;
								setTips(null);
							}
						}
					}
				});
				try {
					Thread.sleep(speed);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
		}
	}

	public void sendUpdate(List<String> messages) {
		if (lastMessage == null || !lastMessage.equals(messages)) {
			profileProxy.sendTips(messages);
			lastMessage = messages;
		}
	}

	public static List<GeoPoint> getRoute() {
		return route;
	}

	public static void setRoute(List<GeoPoint> route) {
		NaviService.route = route;
	}

	public static Bitmap getPin() {
		return pin;
	}

	public static Bitmap getCar() {
		if (getCurrentPosition() != -1) {
			if (route != null) {
				if (rotatedCar != null)
					return rotatedCar;
			}
		}
		return car;
	}

	private static Map<GeoPoint, List<String>> getTips() {
		return tips;
	}

	public static void setTips(Map<GeoPoint, List<String>> tips) {
		NaviService.tips = tips;
	}

	public static int getCurrentPosition() {
		return currentPosition;
	}

	public static void setCurrentPosition(int currentPosition) {
		NaviService.currentPosition = currentPosition;
	}

	public static Location getLastKnownLocation() {
		return NaviService.lastKnownLocation;
	}

	public static void setLastKnownLocation(Location lastKnownLocation) {
		NaviService.lastKnownLocation = lastKnownLocation;
	}

	public static Location getCurrentLocation() {
		return NaviService.currentLocation;
	}

	public static void setCurrentLocation(Location currentLocation) {
		NaviService.currentLocation = currentLocation;
	}

	public static Location getTargetLocation() {
		return NaviService.targetLocation;
	}

	public static void setTargetLocation(Location targetLocation) {
		NaviService.targetLocation = targetLocation;
	}

}
