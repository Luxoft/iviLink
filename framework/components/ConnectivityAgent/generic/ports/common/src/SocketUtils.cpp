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


#include "SocketUtils.hpp"

#include <arpa/inet.h>

static char const* sockNtoPHost_worker(sockaddr const* sa, socklen_t salen, char* str, size_t strlen)
{
   switch (sa->sa_family)
   {
   case AF_INET:
      {
         sockaddr_in const* sin = reinterpret_cast<sockaddr_in const*>(sa);

         if (inet_ntop(AF_INET, &sin->sin_addr, str, strlen) == 0)
         {
            return 0;
         }
         return str;
      }

#ifdef   IPV6
      case AF_INET6:
         {
            sockaddr_in6 const* sin6 = reinterpret_cast<sockaddr_in6 const*>(sa);

            if (inet_ntop(AF_INET6, &sin6->sin6_addr, str, strlen) == 0)
            {
               return 0;
            }
            return str;
         }
#endif

         // AF_UNIX was disabled - code needs to be checked to support
         // abstract paths - they have '\0' as first byte and then the string.
//#ifdef   AF_UNIX
//      case AF_UNIX:
//      {
//         sockaddr_un *unp = reinterpret_cast<sockaddr_un*>(sa);
//
//         // OK to have no pathname bound to the socket: happens on
//         //  every connect() unless client calls bind() first.
//         if (unp->sun_path[0] == 0)
//         {
//
//         }
//         else
//         {
//            snprintf(str, strlen, "%s", unp->sun_path);
//         }
//         return (str);
//      }
//#endif

#ifdef   HAVE_SOCKADDR_DL_STRUCT
      case AF_LINK:
         {
            sockaddr_dl const* sdl = reinterpret_cast<sockaddr_dl const*>(sa);

            if (sdl->sdl_nlen > 0)
            {
               snprintf(str, strlen, "%*s", sdl->sdl_nlen, &sdl->sdl_data[0]);
               return 0;
            }
            else
            {
               //snprintf(str, sizeof(str), "AF_LINK, index=%d", sdl->sdl_index);
               return 0;
            }
            return str;
         }

#endif

      default:
         //snprintf(str, sizeof(str), "sock_ntop_host: unknown AF_xxx: %d, len %d", sa->sa_family, salen);
         return 0;
      }
      return 0;

}

char const* iviLink::ConnectivityAgent::HAL::sockNtoPHost(sockaddr const* sa, socklen_t salen)
{
   const size_t strlen = 128; /* Unix domain is largest */
   static char str[strlen];

   return sockNtoPHost_worker(sa, salen, str, strlen);
}

char const* iviLink::ConnectivityAgent::HAL::sockNtoPHost_r(sockaddr const* sa, socklen_t salen)
{
   const size_t strlen = 128; /* Unix domain is largest */
   char* str = new char[strlen];

   const char* res = sockNtoPHost_worker(sa, salen, str, strlen);
   if (!res)
      delete [] str;

   return res;
}
