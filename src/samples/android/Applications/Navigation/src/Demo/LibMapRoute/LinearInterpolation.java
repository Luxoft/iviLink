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

import java.util.ArrayList;

import android.util.Log;

import com.google.android.maps.GeoPoint;

public class LinearInterpolation
{
   /**
    * interpolate distance between route points
    */
   public static ArrayList<GeoPoint> InterpolatePath (ArrayList<GeoPoint> i_points_list)
   {
      if(i_points_list == null){
    	  Log.i("Linear int", "got null input");
    		  
         return(null);
      }
      if(i_points_list.size() < 2)
      {
    	  Log.i("Linear int", "got input<2");
    	  
         return(null);
      }

      ArrayList<GeoPoint> arr_ret = new ArrayList<GeoPoint>();
      ArrayList<GeoPoint> arr_tmp;
      GeoPoint p1, p2;
      p1 = i_points_list.get(0);
      
      for(int i = 1; i < i_points_list.size(); i++)
      {
         p2 = i_points_list.get(i);
         arr_tmp = _Interpolate(p1, p2);
         arr_ret.addAll(arr_tmp);
         p1 = p2;
      }
      
      return(arr_ret);
   }
   
   /**
    * 
    * @param i_p1
    * @param i_p2
    */
   private static ArrayList<GeoPoint> _Interpolate (GeoPoint i_p1, GeoPoint i_p2)
   {
      ArrayList<GeoPoint> arr_ret = new ArrayList<GeoPoint>(); 
      GeoPoint point;
      double distance;
      int qty;
      int d_lat, d_lon;
      
      distance = _GetDistanceTo(i_p1, i_p2);
      
      if(distance != 0)
      {
         arr_ret.add(i_p1);
         qty = (int)(distance / 0.008f);
         
         if(qty > 1)
         {
            d_lat = (i_p2.getLatitudeE6() - i_p1.getLatitudeE6()) / qty;
            d_lon = (i_p2.getLongitudeE6() - i_p1.getLongitudeE6()) / qty;
            
            for(int i = 1; i < qty; i++)
            {
               point = new GeoPoint(i_p1.getLatitudeE6() + d_lat * i + d_lat,
                     i_p1.getLongitudeE6() + d_lon * i + d_lon);
               
               arr_ret.add(point);
            }
         }

         arr_ret.add(i_p2);
      }
      
      return(arr_ret);
   }

   /**
    * 
    * @param i_p1
    * @param i_p2
    * @return
    */
   private static double _GetDistanceTo (GeoPoint i_p1, GeoPoint i_p2)
   {
      int    r = 6371; // km
      double lat1 = (double)i_p1.getLatitudeE6() / 1E6;
      double lon1 = (double)i_p1.getLongitudeE6() / 1E6;
      double lat2 = (double)i_p2.getLatitudeE6() / 1E6;
      double lon2 = (double)i_p2.getLongitudeE6() / 1E6;
      double d_lat = Math.toRadians(lat2 - lat1);
      double d_lon = Math.toRadians(lon2 - lon1);
      
      lat1 = Math.toRadians(lat1);
      lat2 = Math.toRadians(lat2);

      double a = Math.sin(d_lat/2) * Math.sin(d_lat/2) +
              Math.sin(d_lon/2) * Math.sin(d_lon/2) * Math.cos(lat1) * Math.cos(lat2); 
      double c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1-a)); 
      double d = r * c;
      
      return(d);
   }
}
