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

import java.io.IOException;
import java.security.KeyStore.LoadStoreParameter;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import java.util.Map;

import Demo.LibMapRoute.MapRoute;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.res.Configuration;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Point;
import android.graphics.Rect;
import android.location.Address;
import android.location.Geocoder;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.MotionEvent;
import android.widget.Toast;

import com.google.android.maps.GeoPoint;
import com.google.android.maps.MapActivity;
import com.google.android.maps.MapView;
import com.google.android.maps.Overlay;
import com.google.android.maps.Projection;
import com.luxoft.ivilink.samples.navi.api.NaviAPI;
import com.luxoft.ivilink.samples.splash.Splash;
import com.luxoft.ivilink.samples.splash.Splash.Length;
import com.luxoft.ivilink.sdk.android.lib.utils.ForApp;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;
import com.luxoft.ivilink.sdk.android.nonnative.ApplicationStateCallbacks;
import com.luxoft.ivilink.sdk.android.nonnative.CApp;

public class Navi extends MapActivity implements ApplicationStateCallbacks {
	final String tag = "iviLink.Samples.Navi";

	final static String SERVICE_UID = "NavigationMapService";

	BroadcastReceiver killer;

	CApp app;

	enum VisibleZone {
		NA, ENTERED, LEFT
	}

	GeoPoint center = null;

	MapView map;
	LocationManager locMan;
	String lastMessage = null;

	Handler poster = new Handler();

	boolean isVisible = false;

	class GeoPointXtended {
		GeoPoint gp = null;
		int externalIndex = -1;

		public GeoPointXtended(GeoPoint gp, int extIdx) {
			this.gp = gp;
			this.externalIndex = extIdx;
		}
	}

	LastKnownLocationGetter locGetter;
	Dialog calculating, loadingDialog, serviceLoad;
	Handler hndlr = new Handler();

	MapRoute router;
	List<GeoPoint> route = null;
	public static ArrayList<ArrayList<GeoPointXtended>> routeToDraw = null;
	List<GeoPoint> filteredRoute = null;
	Map<GeoPoint, List<String>> tips = null;
	int windowSize = 10;
	double[] angles;
	Runnable imitation;

	final int speed = 400;

	@Override
	protected boolean isRouteDisplayed() {
		return false;
	}

	@Override
	public void onCreate(Bundle icicle) {
		super.onCreate(icicle);
		ForApp.setWindowFlagsToKeepDeviceOn(this);
		killer = ForApp.registerReceiverFromKillerApp(this);
		setContentView(R.layout.navi);
		map = (MapView) findViewById(R.id.mapview);

		map.getOverlays().add(new TapOverlay());
		map.getOverlays().add(new DrawRoutePath());
		map.getOverlays().add(new PointsOverlay());
		router = new MapRoute();

		loadingDialog = ProgressDialog.show(this, "", "Detecting your current location...", true);
		loadingDialog.setCancelable(false);

		app = new CApp(this, "NavigationMapService");
	}

	@Override
	public void onResume() {
		super.onResume();
		if (NaviService.getLastKnownLocation() == null) {
			locMan = (LocationManager) this.getSystemService(Context.LOCATION_SERVICE);
			locMan.requestLocationUpdates(LocationManager.NETWORK_PROVIDER, 100, 0.01f,
					locGetter = new LastKnownLocationGetter());
		}
		Splash.displaySplashScreen(this, new Runnable() {
			public void run() {
				app.initInIVILink(Navi.this);
			}
		}, Length.SHORT);
		startService(new Intent(this, NaviService.class));
	}

	@Override
	public void onWindowFocusChanged(boolean hasFocus) {
		super.onWindowFocusChanged(hasFocus);
		if (hasFocus) {
			if (NaviService.getRoute() != null) {
				Log.i(tag, "getting visible area post resume");
				getVisibleArea(map.getZoomLevel());
				map.postInvalidate();
			}
		}
	}

	@Override
	public void onPause() {
		super.onStop();
		routeToDraw = null;
		if (loadingDialog != null && loadingDialog.isShowing()) {
			loadingDialog.dismiss();
		}
		unregisterReceiver(killer);
		android.os.Process.killProcess(android.os.Process.myPid());
	}

	public void filterRoute(int window) {
		Log.i("Navi", "filter route window is: " + window);
		for (int i = window; i < route.size() - window; i++) {
			double avgLat = 0;
			double avgLon = 0;
			for (int k = i - window; k <= i + window; k++) {
				avgLat += filteredRoute.get(k).getLatitudeE6();
				avgLon += filteredRoute.get(k).getLongitudeE6();
			}
			avgLat /= (double) (window * 2 + 1);
			avgLon /= (double) (window * 2 + 1);
			filteredRoute.set(i, new GeoPoint((int) avgLat, (int) avgLon));
		}
	}

	public void getRawAngles(double[] angles) {
		for (int i = 0; i < angles.length; i++) {
			Point p1 = new Point();
			Point p2 = new Point();
			map.getProjection().toPixels(filteredRoute.get(i), p1);
			map.getProjection().toPixels(filteredRoute.get(i + 1), p2);
			angles[i] = getAngleBtw2Points(p1, p2);
		}
	}

	public void filterAngles(double angles[], int window) {
		double[] rawCopies = new double[angles.length];
		System.arraycopy(angles, 0, rawCopies, 0, angles.length);
		// ver 3
		for (int i = window; i < angles.length - window; i++) {
			double avg = 0;
			for (int k = i - window; k <= i + window; k++) {
				avg += rawCopies[k];
			}
			if (Math.abs(angles[i] - avg / (double) (window * 2 + 1)) <= Math.abs(0.15 * angles[i])) {
				angles[i] = avg / (double) (window * 2 + 1);
			}
		}
	}

	class TapOverlay extends Overlay {
		private boolean isPinch = false;

		@Override
		public boolean onTap(GeoPoint p, MapView map) {
			if (isPinch) {
				return false;
			} else {
				Log.i("Navi", "TAP!");
				if (p != null) {
					if (NaviService.getLastKnownLocation() == null) {
						Location lastKnownLocation = new Location("last known location");
						lastKnownLocation.setLatitude((double) (p.getLatitudeE6()) / 1e6d);
						lastKnownLocation.setLongitude((double) (p.getLongitudeE6()) / 1e6d);
						NaviService.setLastKnownLocation(lastKnownLocation);
						Location currentLocation = new Location("Current location");
						currentLocation.setLatitude((double) (p.getLatitudeE6()) / 1e6d);
						currentLocation.setLongitude((double) (p.getLongitudeE6()) / 1e6d);
						NaviService.setCurrentLocation(currentLocation);
					} else {
						if (NaviService.getTargetLocation() == null) {
							NaviService.setTargetLocation(new Location("target"));
						} else {
							NaviService.getLastKnownLocation().setLatitude(
									NaviService.getCurrentLocation().getLatitude());
							NaviService.getLastKnownLocation().setLongitude(
									NaviService.getCurrentLocation().getLongitude());
						}
						NaviService.getTargetLocation().setLatitude(
								(double) (p.getLatitudeE6()) / 1e6d);
						NaviService.getTargetLocation().setLongitude(
								(double) (p.getLongitudeE6()) / 1e6d);
						new CalculateRouteTask().execute((Void) null);
					}
					return true;
				} else {
					return false;
				}
			}
		}

		@Override
		public boolean onTouchEvent(MotionEvent e, MapView mapView) {
			if (e.getAction() == MotionEvent.ACTION_UP) {
				poster.post(new MoveListener());
			}
			int fingers = e.getPointerCount();
			if (e.getAction() == MotionEvent.ACTION_DOWN) {
				isPinch = false;
			}
			if (e.getAction() == MotionEvent.ACTION_MOVE && fingers == 2) {
				isPinch = true;
			}
			return super.onTouchEvent(e, mapView);
		}
	}

	class MoveListener implements Runnable {
		public void run() {
			if (center == null) {
				center = new GeoPoint(map.getMapCenter().getLatitudeE6(), map.getMapCenter()
						.getLongitudeE6());
				poster.postDelayed(this, 20);
			} else {
				if (center.equals(map.getMapCenter())) {
					if (map != null) {
						getVisibleArea(map.getZoomLevel());
						map.postInvalidate();
					}
					center = null;
				} else {
					center = new GeoPoint(map.getMapCenter().getLatitudeE6(), map.getMapCenter()
							.getLongitudeE6());
					poster.postDelayed(this, 20);
				}
			}
		}
	}

	class PointsOverlay extends Overlay {
		@Override
		public boolean draw(Canvas canvas, MapView mapView, boolean shadow, long when) {
			if (NaviService.doNotDrawAnything())
				return super.draw(canvas, mapView, shadow, when);
			GeoPoint p;
			Point screenPts;
			// draw last known location
			if (NaviService.getLastKnownLocation() != null) {
				p = new GeoPoint((int) (NaviService.getLastKnownLocation().getLatitude() * 1E6d),
						(int) (NaviService.getLastKnownLocation().getLongitude() * 1E6d));
				screenPts = new Point();
				mapView.getProjection().toPixels(p, screenPts);
				canvas.drawBitmap(NaviService.getPin(), screenPts.x
						- NaviService.getPin().getWidth() / 2, screenPts.y
						- NaviService.getPin().getHeight(), null);
			}

			// draw target location
			if (NaviService.getTargetLocation() != null) {
				p = new GeoPoint((int) (NaviService.getTargetLocation().getLatitude() * 1E6d),
						(int) (NaviService.getTargetLocation().getLongitude() * 1E6d));
				screenPts = new Point();
				mapView.getProjection().toPixels(p, screenPts);
				canvas.drawBitmap(NaviService.getPin(), screenPts.x
						- (int) (NaviService.getPin().getWidth() / 2), screenPts.y
						- NaviService.getPin().getHeight(), null);
			}

			// draw current location
			if (NaviService.getCurrentLocation() != null) {
				p = new GeoPoint((int) (NaviService.getCurrentLocation().getLatitude() * 1E6d),
						(int) (NaviService.getCurrentLocation().getLongitude() * 1E6d));
				screenPts = new Point();
				mapView.getProjection().toPixels(p, screenPts);
				canvas.drawBitmap(NaviService.getCar(), screenPts.x
						- NaviService.getCar().getWidth() / 2, screenPts.y
						- NaviService.getCar().getHeight() / 2, null);
			}
			return true;
		}
	}

	boolean isLocationVisible(GeoPoint gp) {
		Rect currentMapBoundsRect = new Rect();
		Point currentDevicePosition = new Point();

		map.getProjection().toPixels(gp, currentDevicePosition);
		map.getDrawingRect(currentMapBoundsRect);

		return currentMapBoundsRect.contains(currentDevicePosition.x, currentDevicePosition.y);

	}

	int chooseStepSize() {
		int zoomLevel = 0;
		try {
			zoomLevel = map.getZoomLevel();
			Log.i(tag, "zoom level is " + zoomLevel);
		} catch (Exception e) {
			Log.i(tag, e.getMessage());
			return -1;
		}
		if (zoomLevel >= 16)
			return 1;
		if (zoomLevel == 15)
			return 3;
		if (zoomLevel == 14)
			return 4;
		if (zoomLevel == 13)
			return 7;
		if (zoomLevel == 12)
			return 11;
		if (zoomLevel == 11)
			return 18;
		if (NaviService.getRoute().size() > 900) {
			return (int) Math.round((double) NaviService.getRoute().size() / 30d);
		}
		return 20;
	}

	class DrawRoutePath extends Overlay {
		@Override
		public boolean draw(Canvas canvas, MapView mapView, boolean shadow, long when) {
			if (routeToDraw != null) {
				for (ArrayList<GeoPointXtended> currentSet : routeToDraw) {
					boolean first_point = true;
					Paint paint = new Paint();
					paint.setAntiAlias(true);
					Point point1 = new Point();
					Point point2 = new Point();
					Projection projection = mapView.getProjection();
					for (GeoPointXtended gpx : currentSet) {
						if (first_point) {
							projection.toPixels(gpx.gp, point2);
							first_point = false;
						} else {
							point1.set(point2.x, point2.y);
							projection.toPixels(gpx.gp, point2);
							if (gpx.externalIndex < NaviService.getCurrentPosition()) {
								paint.setStrokeWidth(2);
								paint.setColor(Color.CYAN);
							} else {
								paint.setStrokeWidth(4);
								paint.setColor(Color.BLUE);
							}
							canvas.drawLine((float) point1.x, (float) point1.y, (float) point2.x,
									(float) point2.y, paint);
						}
					}
				}
			}
			return super.draw(canvas, mapView, shadow, when);
		}
	}

	private float getAngleBtw2Points(Point i_pnt_start, Point i_pnt_end) {
		Point p1 = new Point();
		p1.set(i_pnt_end.x - i_pnt_start.x, i_pnt_end.y - i_pnt_start.y);
		Point p2 = new Point();
		p2.set(0, 10);
		double cos = (p1.x * p2.x + p1.y * p2.y)
				/ Math.sqrt((p1.x * p1.x + p1.y * p1.y) * (p2.x * p2.x + p2.y * p2.y));

		float angle = (float) ((Math.acos(cos) * 180) / Math.PI);
		double[] v1 = { i_pnt_end.x - i_pnt_start.x, i_pnt_end.y - i_pnt_start.y };
		double[] v2 = { 0, 1 };
		double c = (v1[0] * v2[1] - v1[1] * v2[0]);
		if (c > 0) {
			angle = Math.abs(angle) * (-1);
		} else {
			angle = Math.abs(angle);
		}
		return angle;
	}

	@Override
	public void onUserInteraction() {
		// stub
	}

	void postProcessTips() {
		List<List<String>> tipsValues = new ArrayList<List<String>>();
		List<GeoPoint> keys = new ArrayList<GeoPoint>();
		for (int i = 0; i < route.size(); i++) {
			if (tips.get(route.get(i)) != null) {
				keys.add(route.get(i));
				tipsValues.add(tips.get(route.get(i)));
			}
		}
		int i;
		for (i = keys.size() - 1; i < 0; i--) {
			if (!tipsValues.get(i).equals(tipsValues.get(i - 1))) {
				break;
			}
		}
		List<String> temp = tipsValues.get(i);
		temp.add(0, "dest");
		int index = 1;
		while (true) {
			if (index == temp.size())
				return;
			if (temp.get(index).equals("cherez") || temp.get(index).equals("prodolz_dvizhenie")) {
				Log.i("Navi", "cherez or dvizh!");
				if (temp.get(index).equals("prodolz_dvizhenie")) {
					temp.set(index, " ");
				}
				break;
			} else {
				temp.set(index, " ");
				index++;
			}
		}
		if (!temp.contains("cherez")) {
			temp.add(1, "cherez");
		}
		Log.i("Navi", "Post processing got i=" + i);
		for (int j = i; j < tips.size(); j++) {
			tips.remove(keys.get(j));
			tips.put(keys.get(i), temp);
		}
	}

	class LastKnownLocationGetter implements LocationListener {
		public void onLocationChanged(Location location) {
			Log.i("Navi", "LAST KNOWN LOCATION GETTER");
			NaviService.setCurrentLocation(locMan
					.getLastKnownLocation(LocationManager.NETWORK_PROVIDER));
			if (loadingDialog != null && loadingDialog.isShowing()) {
				loadingDialog.dismiss();
			}
			if (NaviService.getCurrentLocation() == null) {
				Log.e("Navi", "last known location is NULL");
			} else {
				locMan.removeUpdates(this);
				Location lastKnownLocation = new Location("last");
				lastKnownLocation.setLatitude(NaviService.getCurrentLocation().getLatitude());
				lastKnownLocation.setLongitude(NaviService.getCurrentLocation().getLongitude());
				NaviService.setLastKnownLocation(lastKnownLocation);
				GeoPoint p;
				p = new GeoPoint((int) (NaviService.getCurrentLocation().getLatitude() * 1E6),
						(int) (NaviService.getCurrentLocation().getLongitude() * 1E6));
				map.getController().setCenter(p);
				map.getController().setZoom(16);
			}
		}

		public void onProviderDisabled(String provider) {
		} // stub

		public void onProviderEnabled(String provider) {
		} // stub

		public void onStatusChanged(String provider, int status, Bundle extras) {
		} // stub
	}

	private class CalculateRouteTask extends AsyncTask<Void, Integer, Integer> {
		int zoom;

		@Override
		public void onPreExecute() {
			calculating = ProgressDialog.show(Navi.this, "", "Calculating route...", true);
			calculating.setCancelable(false);
			NaviService.setDoNothing(true);
			zoom = map.getZoomLevel();
			NaviService.setDoNotDrawAnything(true);
			routeToDraw = null;
		}

		@Override
		public void onProgressUpdate(Integer... param) {
			map.getController().setZoom(param[0].intValue());
		}

		@Override
		protected Integer doInBackground(Void... params) {
			GeoPoint start = new GeoPoint(
					(int) (NaviService.getLastKnownLocation().getLatitude() * 1e6d),
					(int) (NaviService.getLastKnownLocation().getLongitude() * 1e6d));
			Log.i("Navi",
					"Start coordinates are: " + start.getLatitudeE6() + ", "
							+ start.getLongitudeE6());
			GeoPoint end = new GeoPoint(
					(int) (NaviService.getTargetLocation().getLatitude() * 1e6), (int) (NaviService
							.getTargetLocation().getLongitude() * 1e6));
			Log.i("Navi",
					"Start coordinates are: " + end.getLatitudeE6() + ", " + end.getLongitudeE6());
			router.calculateRoute(start, end);
			route = router.getRoute();
			NaviService.setRoute(route);
			if (route != null) {
				tips = router.getRouteTips();
				postProcessTips();
				NaviService.setTips(tips);
			} else {
				Log.i("Navi", "route is null");
				return Integer.valueOf(-1);
			}
			filteredRoute = new ArrayList<GeoPoint>(route.size());
			for (int i = 0; i < route.size(); i++) {
				GeoPoint p = route.get(i);
				GeoPoint temp = new GeoPoint(p.getLatitudeE6(), p.getLongitudeE6());
				filteredRoute.add(i, temp);
			}
			NaviService.setCurrentPosition(0);
			Log.i("Navi",
					"route size: " + route.size() + ", filtered route:" + filteredRoute.size());
			filterRoute(6); // win of 13
			angles = new double[route.size() - 1];
			publishProgress(Integer.valueOf(18)); // set map to desired
			try {
				Thread.sleep(50);
			} catch (InterruptedException e) {
			}
			getRawAngles(angles);
			filterAngles(angles, 1);
			NaviService.setAngles(angles);
			return Integer.valueOf(1);
		}

		protected void onPostExecute(Integer result) {
			NaviService.setDoNotDrawAnything(false);
			calculating.dismiss();
			map.getController().setZoom(zoom);
			if (result.intValue() == -1) {
				Toast.makeText(Navi.this, "Could not acquire a route.", Toast.LENGTH_LONG).show();
				return;
			}
			float latitude = (float) (route.get(route.size() - 1).getLatitudeE6()) / 1E6f;
			float longitude = (float) (route.get(route.size() - 1).getLongitudeE6()) / 1E6f;
			NaviService.getTargetLocation().setLatitude(latitude);
			NaviService.getTargetLocation().setLongitude(longitude);
			Geocoder geocoder = new Geocoder(getApplicationContext(), Locale.getDefault());
			String res = null;
			try {
				List<Address> list = geocoder.getFromLocation(NaviService.getTargetLocation()
						.getLatitude(), NaviService.getTargetLocation().getLongitude(), 1);
				if (list != null && list.size() > 0) {
					Address address = list.get(0);
					res = address.getAddressLine(0) + " ," + address.getAddressLine(1);
				}
				if (res != null)
					Toast.makeText(Navi.this,
							"New target: " + res + " \nPress Back button to cancel trip",
							Toast.LENGTH_LONG).show();
			} catch (IOException e) {
				Log.e("Navi", "Impossible to connect to Geocoder", e);
			}
			NaviService.setDoNothing(false);
			getVisibleArea(map.getZoomLevel());
		}
	}

	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);
		Log.v(tag, "onConfigurationChanged()");
	}

	@Override
	public void onBackPressed() {
		if (NaviService.doNothing()) {
			Log.v(tag, "doing nothing - exit on back press");
			android.os.Process.killProcess(android.os.Process.myPid());
			return;
		}
		// cancel trip
		if (NaviService.getRoute() != null && NaviService.getCurrentPosition() != -1) {
			NaviService.getLastKnownLocation().setLatitude(
					(double) NaviService.getRoute().get(NaviService.getCurrentPosition())
							.getLatitudeE6() / 1e6d);
			NaviService.getLastKnownLocation().setLongitude(
					(double) NaviService.getRoute().get(NaviService.getCurrentPosition())
							.getLongitudeE6() / 1e6d);
			NaviService.getCurrentLocation().setLatitude(
					(double) NaviService.getRoute().get(NaviService.getCurrentPosition())
							.getLatitudeE6() / 1e6d);
			NaviService.getCurrentLocation().setLongitude(
					(double) NaviService.getRoute().get(NaviService.getCurrentPosition())
							.getLongitudeE6() / 1e6d);
			Toast.makeText(Navi.this, "Trip has been cancelled", Toast.LENGTH_LONG).show();
			NaviService.setCurrentPosition(-1);
			NaviService.setTargetLocation(null);
			NaviService.setDoNothing(true);
			NaviService.setRoute(null);
			NaviService.setTips(null);
			routeToDraw = null;
			Toast.makeText(this, "Press Back again to exit", Toast.LENGTH_LONG).show();
		}
	}

	void getVisibleArea(int zoomLevel) {
		if (NaviService.getRoute() == null)
			return;
		if (routeToDraw == null) {
			routeToDraw = new ArrayList<ArrayList<Navi.GeoPointXtended>>();
		} else {
			routeToDraw.clear();
		}
		int step = chooseStepSize();
		VisibleZone vz = VisibleZone.NA;
		ArrayList<GeoPointXtended> currentSet = null;
		for (int i = 0; i < NaviService.getRoute().size(); i += step) {
			if (isLocationVisible(NaviService.getRoute().get(i))) {
				if (vz != VisibleZone.ENTERED) {
					currentSet = new ArrayList<Navi.GeoPointXtended>();
					routeToDraw.add(currentSet);
					if (i - 1 >= 0) {
						currentSet
								.add(new GeoPointXtended(NaviService.getRoute().get(i - 1), i - 1));
					}
				}
				vz = VisibleZone.ENTERED;
				currentSet.add(new GeoPointXtended(NaviService.getRoute().get(i), i));
			} else {
				if (vz == VisibleZone.ENTERED) {
					currentSet.add(new GeoPointXtended(NaviService.getRoute().get(i), i));
					vz = VisibleZone.LEFT;
				}
			}
		}
	}

	public void onInitDone(boolean isStartedByUser) {
		Log.i(tag, Logging.getCurrentMethodName(isStartedByUser));
		if (isStartedByUser) {
			app.registerProfileCallbacks(NaviAPI.profileApi, this);
			showServiceLoadingDialog();
			boolean loadResult = app.loadService(SERVICE_UID);
			hideServiceLoadingDialog();
			if (!loadResult) {
				Log.e(tag, "Failed to load service");
				android.os.Process.killProcess(android.os.Process.myPid());
			}
		}

	}

	public void onIncomingServiceBeforeLoading(String serviceUID) {
		Log.i(tag, Logging.getCurrentMethodName(serviceUID));
		app.registerProfileCallbacks(NaviAPI.profileApi, this);
		showServiceLoadingDialog();
	}

	public void onIncomingServiceAfterLoading(String serviceUID) {
		Log.i(tag, Logging.getCurrentMethodName(serviceUID));
		hideServiceLoadingDialog();
	}

	private void showServiceLoadingDialog() {
		runOnUiThread(new Runnable() {
			public void run() {
				if (serviceLoad != null && serviceLoad.isShowing()) {
					serviceLoad.dismiss();
				}
				serviceLoad = ProgressDialog.show(Navi.this, "",
						"Service is being loaded...", true);
				serviceLoad.setCancelable(false);
			}
		});
	}

	private void hideServiceLoadingDialog() {
		runOnUiThread(new Runnable() {
			public void run() {
				if (serviceLoad != null && serviceLoad.isShowing()) {
					serviceLoad.dismiss();
				}
			}
		});
	}

	public void onServiceLoadError(String serviceUID) {
		Log.i(tag, Logging.getCurrentMethodName(serviceUID));
		android.os.Process.killProcess(android.os.Process.myPid());
	}

	public void onServiceDropped(String serviceUID) {
		Log.i(tag, Logging.getCurrentMethodName(serviceUID));
		android.os.Process.killProcess(android.os.Process.myPid());
	}

	public void onPhysicalLinkDown() {
		Log.i(tag, Logging.getCurrentMethodName());
		android.os.Process.killProcess(android.os.Process.myPid());

	}

	public void onLinkEstablished() {
		Log.i(tag, Logging.getCurrentMethodName());
	}
}
