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


#include "get_mac.hpp"

#include <cerrno>
#include <cstring>
#include <cstdio>
#include <unistd.h>

#include <sys/socket.h>
#include <linux/if.h>
#include <netinet/ip.h>
#include <sys/ioctl.h>


#include "BaseError.hpp"
#include "Logger.hpp"

bool get_mac(char buff[13])
{

   int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
   if (sock == -1)
   {
      LOG4CPLUS_ERROR(Logger::getInstance("get_mac"), "socket error: " + BaseError::FormErrnoDescr(errno));
      return false;
   }

   struct sock_guard
   {
      int sock;
      sock_guard(int sock) : sock(sock) {}
      ~sock_guard() { close (sock); }
   } __sg(sock);

   ifreq ifr;
   ifconf ifc;
   char buf[1024];

   ifc.ifc_len = sizeof(buf);
   ifc.ifc_buf = buf;
   if (ioctl(sock, SIOCGIFCONF, &ifc) == -1) 
   { 
      LOG4CPLUS_ERROR(Logger::getInstance("get_mac"), "ioctl SIOCGIFCONF error: " + BaseError::FormErrnoDescr(errno));
      return false;
   }

   ifreq* it = ifc.ifc_req;
   ifreq const* const end = it + (ifc.ifc_len / sizeof(ifreq));
   bool found = false;
   for (; it != end; ++it)
   {
      strcpy(ifr.ifr_name, it->ifr_name);
      if (ioctl(sock, SIOCGIFFLAGS, &ifr) == 0)
      {
         if (! (ifr.ifr_flags & IFF_LOOPBACK)) 
         { // don't count loopback
            if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0) 
            {
               found = true;
               break;
            }
         }
      }
      else
      {
         LOG4CPLUS_ERROR(Logger::getInstance("get_mac"), "ioctl SIOCGIFCONF error: " + BaseError::FormErrnoDescr(errno));
         return false;
      }
   }

   if (found)
   {
      for (int i = 0; i < 6; ++i)
      {
         snprintf(buff + 2*i, 13 - 2*i, "%02x", ifr.ifr_hwaddr.sa_data[i]);
      }
   }

   return found;
}
