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

import com.google.android.maps.GeoPoint;

public class RouteStep 
{
    private String m_guide_note = "";
    private GeoPoint m_point;
    private String m_str_duration = "";
    private int    m_distance; 
    
    /**
     * !
     */
    public RouteStep ()
    {
       m_point = null;
    }
    
    /**
     * !
     */
    public RouteStep (GeoPoint i_point, String i_str)
    {
        this();
        m_point = i_point;
        m_guide_note = i_str;
    }
    
    /**
     * get guide point
     * @param i_str
     */
    public GeoPoint GetGeoPoint ()
    {
        return(m_point);
    }

    /**
     * set guide point
     * @param i_str
     */
    public void SetGeoPoint (GeoPoint i_point)
    {
        m_point = i_point;
    }
    
    /**
     * get guide note
     * @param i_str
     */
    public String GetGuideNote ()
    {
        return(m_guide_note);
    }

    /**
     * set guide note
     * @param i_str
     */
    public void SetGuideNote (String i_str)
    {
       m_guide_note = i_str;
    }
    
    /**
     * get duration text
     * @param i_str
     */
    public String GetDurationText ()
    {
       return(m_str_duration);
    }
    
    /**
     * set duration text
     * @param i_str
     */
    public void SetDurationText (String i_str)
    {
       m_str_duration = i_str;
    }
    
    /**
     * get distance text
     * @param i_str
     */
    public String GetDistanceText ()
    {
       String str = "";
       
       if(m_distance > 1000)
       {
          float value = (float)m_distance / 1000;
          str = String.format("%5.2f км", value).trim();
       }
       else
          str = String.format("%d м", m_distance).trim();
       
       return(str);
    }

    /**
     * get distance (in meters)
     * @param i_str
     */
    public int GetDistance ()
    {
       return(m_distance);
    }

    /**
     * set distance (in meters)
     * @param i_str
     */
    public void SetDistance (int i_value)
    {
       m_distance = i_value;
    }
    
}
