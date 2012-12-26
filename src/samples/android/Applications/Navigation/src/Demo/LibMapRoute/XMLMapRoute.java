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
import java.util.ArrayList;

import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;

import org.xml.sax.Attributes;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import org.xml.sax.XMLReader;
import org.xml.sax.helpers.DefaultHandler;

import android.util.Log;

import com.google.android.maps.GeoPoint;

public class XMLMapRoute extends DefaultHandler
{
   static String                      m_status;
   boolean                            m_active_step     = false;
   boolean                            m_active_duration = false;
   boolean                            m_active_distance = false;
   boolean                            m_active_polyline = false;
   StringBuffer                       m_buff            = null;
   boolean                            m_buffering       = false;

   private static ArrayList<GeoPoint> m_points_list     = null;
   public static ArrayList<RouteStep> m_route_steps     = null;
   public static RouteStep            m_general_info    = null;

   /**
    * !
    */
   public XMLMapRoute()
   {
      super();
      Clean();
   }

   /**
    * clean all
    */
   public static void Clean()
   {
      m_points_list = new ArrayList<GeoPoint>();
      m_route_steps = new ArrayList<RouteStep>();
      m_general_info = new RouteStep();
      m_status = "Fail";
   }

   /**
    * get
    * 
    * @return
    */
   public static synchronized ArrayList<GeoPoint> GetGeoPoints()
   {
      return m_points_list;
   }

   public static String GetStatusStr()
   {
      return (m_status);
   }

   /**
    * Called when tag starts
    */
   @Override
   public void startElement(String uri, String localName, String qName,
         Attributes attributes) throws SAXException
   {
      if (localName.equals("status"))
         m_buffering = true;

      if (localName.equals("step"))
         m_active_step = true;

      if (localName.equals("duration"))
         m_active_duration = true;

      if (localName.equals("distance"))
         m_active_distance = true;

      if (localName.equals("html_instructions"))
         m_buffering = true;

      if (localName.equals("text"))
         m_buffering = true;

      if (localName.equals("value"))
         m_buffering = true;

      if (localName.equals("points"))
         m_buffering = true;

      if (localName.equals("overview_polyline"))
         m_active_polyline = true;

      if (m_buffering)
         m_buff = new StringBuffer("");
   }

   /**
    * Called when tag closing
    */
   @Override
   public void endElement(String uri, String localName, String qName)
         throws SAXException
   {
      if (localName.equals("status"))
      {
         m_buffering = false;
         m_status = m_buff.toString();
         Log.i("XmlMapRoute:status", m_status);
         return;
      }

      if (localName.equals("step"))
         m_active_step = false;

      if (localName.equals("duration"))
         m_active_duration = false;

      if (localName.equals("distance"))
         m_active_distance = false;

      if (localName.equals("html_instructions"))
      {
         m_buffering = false;
         _GetLastRouteStep().SetGuideNote(m_buff.toString());
         //Log.i("Xml:step", m_buff.toString());
         return;
      }

      if (localName.equals("text"))
      {
         m_buffering = false;
         RouteStep route_node = m_general_info;
         if (m_active_step)
            route_node = _GetLastRouteStep();

         if (m_active_duration)
            route_node.SetDurationText(m_buff.toString());
      }

      if (localName.equals("value"))
      {
         m_buffering = false;
         RouteStep route_node = m_general_info;
         if (m_active_step)
            route_node = _GetLastRouteStep();

         if (m_active_distance)
         {
            int value = Integer.parseInt(m_buff.toString());
            route_node.SetDistance(value);
         }
      }

      if (m_active_polyline) // polyline points
      {
         if (localName.equals("overview_polyline"))
            m_active_polyline = false;
      }
      else
      //step line points
      {
         if (localName.equals("points"))
         {
            m_buffering = false;

            try
            {
               //points data coming before the description tag!
               RouteStep step = new RouteStep();
               m_route_steps.add(step);
               _DecodePoints(step, m_buff.toString());
            }
            catch (Exception e)
            {
               Log.e("Xml", "problem with points decoder!");
            }
         }
      }
   }

   /**
    * Called to get tag characters
    */
   @Override
   public void characters(char[] ch, int start, int length) throws SAXException
   {
      if (m_buffering)
      {
         m_buff.append(ch, start, length);
      }
   }

   /**
    * Decode string with GeoPoints data
    * 
    * @param i_data
    *           = compressed data
    */
   private void _DecodePoints(RouteStep i_step, String i_data)
   {
      boolean first_point = true;
      byte[] byte_arr = i_data.getBytes();
      int index = 0, len = byte_arr.length;
      int lat = 0, lng = 0;

      while (index < len)
      {
         int b, shift = 0, result = 0;
         do
         {
            b = byte_arr[index++] - 63;
            result |= (b & 0x1f) << shift;
            shift += 5;
         }
         while (b >= 0x20);
         int dlat = ((result & 1) != 0 ? ~(result >> 1) : (result >> 1));
         lat += dlat;

         shift = 0;
         result = 0;
         do
         {
            b = byte_arr[index++] - 63;
            result |= (b & 0x1f) << shift;
            shift += 5;
         }
         while (b >= 0x20);
         int dlng = ((result & 1) != 0 ? ~(result >> 1) : (result >> 1));
         lng += dlng;

         GeoPoint p = new GeoPoint((int) ((lat / 1E5) * 1E6),
               (int) ((lng / 1E5) * 1E6));

         if (first_point)
         {
            i_step.SetGeoPoint(p);
            first_point = false;
         }
         m_points_list.add(p);
      }
   }

   /**
    * Get last added route step node and add a first route node in case of empty
    * list
    * 
    * @return
    */
   public RouteStep _GetLastRouteStep()
   {
      int index;

      if (m_route_steps.size() == 0)
         m_route_steps.add(new RouteStep());

      index = m_route_steps.size() - 1;

      return (m_route_steps.get(index));
   }

   /**
    * Parse input stream to xml with route (format of online service)
    * 
    * @param i_stream
    * @return
    */
   public static XMLMapRoute Parse(InputStream i_stream)
   {
      XMLMapRoute route = null;

      try
      {
         SAXParserFactory spf = SAXParserFactory.newInstance();
         SAXParser sp = spf.newSAXParser();
         XMLReader xr = sp.getXMLReader();

         XMLMapRoute my_handler = new XMLMapRoute();
         xr.setContentHandler(my_handler);
         InputSource input_source = new InputSource(i_stream);
         input_source.setEncoding("utf-8");
         xr.parse(input_source);

         route = my_handler;
      }
      catch (Exception e)
      {
         Log.i("GMaps_route_parse", e.getMessage());
      }

      return (route);
   }
}
