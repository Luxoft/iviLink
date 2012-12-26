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
 

#ifndef CPROFILEREPOERROR_HPP_
#define CPROFILEREPOERROR_HPP_

#include "CError.hpp"
#include "Types.hpp"

namespace iviLink
{
   namespace ProfileRepository
   {

      /**
       * Class with Profile Repository errors
       */
      class CProfileRepoError: public CError
      {
      public:
         /**
          * Enumeration with codes of exceptions
          */
         enum eCodes
         {
            ERROR_UID_ALREADY_EXISTS   = 1,
            ERROR_DATABASE_WRITE       = 2,
            ERROR_DATABASE_READ        = 3,
            ERROR_MANIFEST             = 4,
            ERROR_READING_FILE         = 5,
            ERROR_EMPTY_DATABASE       = 6,
            ERROR_UNKNOWN_UID          = 7,
            ERROR_NO_DLL               = 8,
            ERROR_NO_PLATFORM_INFO     = 9,
            ERROR_NO_LIBRARY           = 10,
            ERROR_IPC                  = 11,
            ERROR_OTHER                = 12
         };

         /**
          * Constructor
          */
         explicit CProfileRepoError(eCodes code, const char* pDescription = "", eSeverity severity = ERROR) :
            CError((ErrorCode)code, "ProfileRepository", severity, pDescription)
         {}

         /**
          * Constructor
          */
         explicit CProfileRepoError(eCodes code, std::string const& description, eSeverity severity = ERROR) :
            CError((ErrorCode)code, "ProfileRepository", severity, description.c_str())
         {}

         /**
          * Copy constructor
          */
         CProfileRepoError(CProfileRepoError const& ref) :
            CError(ref)
         {}

         /**
          * Destructor
          */
         virtual ~CProfileRepoError()
         {}

         /**
          * Returns object of CError with no error
          */
         static CError NoProfileRepoError(const char* pDescription = "")
         {
            return CError::NoError("ProfileRepository", pDescription);
         }
      };

   }
}


#endif /* CPROFILEREPOERROR_HPP_ */
