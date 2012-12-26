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
 
package Demo.LibMapRoute;

import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import android.util.Log;

import com.google.android.maps.GeoPoint;

public class MapRoute extends SomeClass {
	public static final String MyAppTag = "LibMapRoute";

	@Override
	public boolean calculateRoute(GeoPoint start, GeoPoint end) {

		boolean isInterpolated = false;
		InputStream stream = _GetMapRouteXML(start, end);

		if (stream != null) {
			XMLMapRoute route_xml = XMLMapRoute.Parse(stream);

			if (route_xml != null) {
				if (XMLMapRoute.GetStatusStr().equals("OK")) {
					// interpolate route points
					//if (XMLMapRoute.GetGeoPoints().size() < 5000) {
						Log.i("Navi", "route is not too long, interpolating");
						route = LinearInterpolation.InterpolatePath(XMLMapRoute
								.GetGeoPoints());
						isInterpolated=true;
					//}else{
						//route=XMLMapRoute.GetGeoPoints();
					//}
					Log.i("Route counter", "route size is: "
							+ (route == null ? "null" : route.size()));
					List<String> arr_strings;

					if (XMLMapRoute.m_route_steps.size() > 0)
						routeTips = new HashMap<GeoPoint, List<String>>();

					for (RouteStep step : XMLMapRoute.m_route_steps) {
						arr_strings = _Decode(step.GetGuideNote(),
								step.GetDistance());
						routeTips.put(step.GetGeoPoint(), arr_strings);
					}
				}
			}
		}
		return isInterpolated;
	}

	/*
	 * get route xml (over http)
	 */
	private InputStream _GetMapRouteXML(GeoPoint i_src_point,
			GeoPoint i_dst_point) {
		InputStream stream = null;

		double dlat, dlon;
		dlat = (double) i_src_point.getLatitudeE6() / 1E6d;
		dlon = (double) i_src_point.getLongitudeE6() / 1E6d;
		String src_place = dlat + "," + dlon;

		dlat = (double) i_dst_point.getLatitudeE6() / 1E6d;
		dlon = (double) i_dst_point.getLongitudeE6() / 1E6d;
		String dst_place = dlat + "," + dlon;

		String urlString = "http://maps.googleapis.com/maps/api/directions/xml?origin="
				+ src_place
				+ "&destination="
				+ dst_place
				+ "&sensor=true&language=ru";
		Log.i("Get map route", "url string=" + urlString);
		HttpURLConnection urlConnection = null;
		URL url = null;

		try {
			url = new URL(urlString.toString());
			urlConnection = (HttpURLConnection) url.openConnection();
			urlConnection.setRequestMethod("GET");
			urlConnection.setDoOutput(true);
			urlConnection.setDoInput(true);
			urlConnection.connect();
			stream = urlConnection.getInputStream();
		} catch (Exception e) {
			Log.e(MapRoute.MyAppTag, "HttpGetRoute: " + e.getMessage());
		}

		return (stream);
	}

	/**
	 * convert guide note to list of sound files names
	 * 
	 * @param i_node_query
	 * @return
	 */
	private ArrayList<String> _Decode(String i_str_text, int i_distance) {
		ArrayList<String> arr = new ArrayList<String>();
		Log.i("TTS", "i_str_text: " + i_str_text);

		if (i_str_text.toLowerCase().contains("поверните")
				|| i_str_text.toLowerCase().contains("сверните")
				|| i_str_text.toLowerCase().contains("поворот")
				|| i_str_text.toLowerCase().contains("держитесь")) {
			arr.add("povernite");

			if (i_str_text.toLowerCase().contains("налево"))
				arr.add("nalevo");

			if (i_str_text.toLowerCase().contains("левее"))
				arr.add("levee");

			if (i_str_text.toLowerCase().contains("направо"))
				arr.add("napravo");

			if (i_str_text.toLowerCase().contains("правее"))
				arr.add("pravee");

			arr.add("cherez");
		} else
			arr.add("prodolz_dvizhenie");

		try {
			ArrayList<String> arr_dig = _ConvertDigits(i_distance);

			if (arr_dig != null)
				arr.addAll(arr_dig);
		} catch (NumberFormatException e) {
			Log.e(MapRoute.MyAppTag, "TTS::Speak: " + e.toString());
		}

		return (arr);
	}

	/**
	 * @param i_value
	 */
	private ArrayList<String> _ConvertDigits(int i_value) {
		ArrayList<String> ret_arr = new ArrayList<String>();
		int dv, value = i_value;
		boolean km = false;

		dv = i_value / 10000;
		if (dv > 0) {
			ret_arr.add("d" + String.valueOf(dv) + "0");
			km = true;
		}

		value = i_value % 10000;
		dv = value / 1000;
		if (dv > 0) {
			ret_arr.add("d" + String.valueOf(dv));
			km = true;
		}

		if (km) {
			String node = _SelectKiloMeter(dv);
			if (node.length() != 0)
				ret_arr.add(node);
		}

		value = value % 1000;
		dv = value / 100;
		if (dv > 0)
			ret_arr.add("d" + String.valueOf(dv) + "00");

		value = value % 100;

		if (value > 19) {
			dv = value / 10;
			if (dv > 0)
				ret_arr.add("d" + String.valueOf(dv) + "0");

			dv = value % 10;
			if (dv > 0)
				ret_arr.add("d" + String.valueOf(dv));
		} else {
			dv = value;
			ret_arr.add("d" + String.valueOf(dv));
		}

		String node = _SelectMeter(dv);
		if (node.length() != 0)
			ret_arr.add(node);

		return (ret_arr);
	}

	/**
	 * @param i_meters
	 * @return
	 */
	private String _SelectKiloMeter(int i_kilometers) {
		String node = "";

		if (i_kilometers == 0)
			node = "kilometrov";
		else if (i_kilometers == 1)
			node = "kilometer";
		else if ((i_kilometers > 1) && (i_kilometers < 5))
			node = "kilometra";
		else
			node = "kilometrov";

		return (node);
	}

	/**
	 * @param i_meters
	 * @return
	 */
	private String _SelectMeter(int i_meters) {
		String node = "";

		if (i_meters == 0)
			node = "metrov";
		else if (i_meters == 1)
			node = "meter";
		else if ((i_meters > 1) && (i_meters < 5))
			node = "metra";
		else
			node = "metrov";

		return (node);
	}
}
