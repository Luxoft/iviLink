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
import java.util.List;
import java.util.Map;

import com.google.android.maps.GeoPoint;

public abstract class SomeClass
{
   protected Map<GeoPoint, List<String>> routeTips=null; //list of strings should contain filenames of things that should be voiced at certain points
   protected List<GeoPoint> route=null; //should contain all points of the route
       
   public abstract boolean calculateRoute(GeoPoint start, GeoPoint end); //works as a setter for route and routeTips
       
   public Map<GeoPoint, List<String>> getRouteTips(){
       return routeTips;
   }
       
   public List<GeoPoint> getRoute(){
       return route;
   }
}
 
