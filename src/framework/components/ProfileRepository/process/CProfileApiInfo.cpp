/* 
 *  iviLINK SDK, version 0.9 (for preview only)                                      
 *    http://www.ivilink.net                                                         
 *  Cross Platform Application Communication Stack for In-Vehicle Applications       
 *                                                                                   
 *  Copyright (C) 2012, Luxoft Professional Corp., member of IBS group               
 *                                                                                   
 *  This library is free software; you can redistribute it and/or                    
 *  modify it under the terms of the GNU Lesser General Public                       
 *  License as published by the Free Software Foundation; version 2.1.               
 *                                                                                   
 *  This library is distributed in the hope that it will be useful,                  
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                
 *  Lesser General Public License for more details.                                  
 *                                                                                   
 *  You should have received a copy of the GNU Lesser General Public                 
 *  License along with this library; if not, write to the Free Software              
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA   
 * 
 * 
 * 
 * 
 */

/***************************************************************************
 * Project           AXIS
 * (c) copyright     2012
 * Company           LUXOFT
 * @file             CProfileApiInfo.cpp
 * @brief            Source file for CProfileApiInfo class (part of Profile Repository Process)
 * @author           VPlachkov
 ***************************************************************************/

#include <cassert>
#include <cstdlib>

#include "utils/xml/include/pugixml.hpp"
#include "CProfileApiInfo.hpp"

namespace AXIS
{
   namespace ProfileRepository
   {

      Logger CProfileApiInfo::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileRepository.Process.CProfileApiInfo"));

      CProfileApiInfo::CProfileApiInfo()
      {
      }

      CProfileApiInfo::CProfileApiInfo(std::string xmlPath)
         : mXmlPath(xmlPath)
         , mFailed(false)
      {
         loadProfileApiInfo();
      }

      std::string CProfileApiInfo::xmlPath() const
      {
         return mXmlPath;
      }

      CProfileApiInfo::~CProfileApiInfo()
      {
      }

      UID CProfileApiInfo::uid() const
      {
         return mUid;
      }

      std::string CProfileApiInfo::role() const
      {
         return mRole;
      }

      int CProfileApiInfo::version() const
      {
         return mVersion;
      }

      void CProfileApiInfo::loadProfileApiInfo()
      {
         pugi::xml_document doc;
         pugi::xml_parse_result res = doc.load_file(mXmlPath.c_str());
         if (pugi::status_ok != res.status)
         {
            mFailed = true;
            LOG4CPLUS_ERROR(msLogger, "Error while parsing Profile API manifest");
            return;
         }
         pugi::xml_node profApi = doc.child("profileApi");
         mRole = profApi.child_value("role");
         mUid = UID(profApi.child_value("uid"));
         mVersion = atoi(profApi.child_value("version"));
      }

      void CProfileApiInfo::print() const
      {
         std::string print = "*** Profile API\n"
               " UID: " + mUid.value +
               " Role: " + mRole +
               " Version: " + convertIntegerToString(mVersion) +
               " XML Path: " + mXmlPath;
         LOG4CPLUS_INFO(msLogger, print);
      }

      bool CProfileApiInfo::failed() const
      {
         return mFailed;
      }
   }
}

