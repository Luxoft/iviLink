/* 
 * 
 * iviLINK SDK, version 1.1.2
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
 * 
 */

#ifndef __ANDROID_APP_INFO_HPP
#define __ANDROID_APP_INFO_HPP

#include <string>

namespace iviLink
{
   namespace Android
   {
      // includes all android-specific information that must be passed from app to sdk
      struct AppInfo
      {
         std::string serviceRepoPath; // path to where ServiceRepository on the device is
         std::string privateDirPath;  // path to application's private directory 
         std::string launchInfo;      // application's launch info (see AndroidCommonLib: Utils class)
         // no initialization constructor provided for now - will be later when this is finalized
      };
   } //namespace Android
} // namespace iviLink


#endif //__ANDROID_APP_INFO_HPP
