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
 * @file             CPMALError.hpp
 * @brief            Header for CPMALError class
 * @author           PIlin
 ***************************************************************************/

#ifndef CPMALERROR_HPP_
#define CPMALERROR_HPP_

#include "utils/misc/include/CError.hpp"

namespace AXIS {
namespace PMAL {

/**
 * Simplifies error construction in PMAL.
 * Class contains error codes for PMAL errors.
 */
class CPMALError: public CError
{
public:
   enum eCodes
   {
      ERROR_OTHER = 1,

      ERROR_UNKNOWN_PROFILE_UID,
      ERROR_PIM_INTERNAL,
      ERROR_PROFILE_INIT,
      ERROR_WRONG_PARAM,
      ERROR_MANIFEST_PARSING,

      ERROR_PMAL_ALREADY_INITIALIZED,
      ERROR_PMP_CONNECTION
   };

   explicit CPMALError(eCodes code, char const* pModuleID = "", const char* pDescription = "", eSeverity severity = ERROR) :
      CError((ErrorCode)code, pModuleID, severity, pDescription)
   {}

   explicit CPMALError(eCodes code, char const* pModuleID, std::string const& description, eSeverity severity = ERROR) :
      CError((ErrorCode)code, pModuleID, severity, description.c_str())
   {}

   CPMALError(CPMALError const& ref) :
      CError(ref)
   {}

   explicit CPMALError(CError const& ref) :
      CError(ref)
   {}

   virtual ~CPMALError()
   {}

   static CPMALError NoPMALError(char const* pModuleID="", char const* pDescription = "")
   {
      return CPMALError(CError::NoError(pModuleID, pDescription));
   }

};

}  // namespace ProfileManager
}  // namespace AXIS




#endif /* CPMALERROR_HPP_ */
