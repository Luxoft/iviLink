 /****************************************************************************/
 /*                                                                          */
 /* This file is part of iviLINK 0.9 Technology Preview Samples Set.         */
 /*                                                                          */
 /* iviLINK 0.9 Technology Preview Samples Set is free software:             */
 /* you can redistribute it and/or modify                                    */
 /* it under the terms of the GNU General Public License as published by     */
 /* the Free Software Foundation, version 3 of the License.                  */
 /*                                                                          */
 /* iviLINK 0.9 Technology Preview Samples Set is distributed in the         */
 /* hope that it will be useful, but WITHOUT ANY WARRANTY; without           */
 /* even the implied warranty of MERCHANTABILITY or FITNESS FOR A            */
 /* PARTICULAR PURPOSE.  See the GNU General Public License for more details.*/
 /*                                                                          */
 /* You should have received a copy of the GNU General Public License        */
 /* along with iviLINK 0.9 Technology Preview.                               */
 /* If not, see <http://www.gnu.org/licenses/>.                              */
 /*                                                                          */
 /****************************************************************************/

#ifndef ICLIMATE_PROFILE_API_HPP
#define ICLIMATE_PROFILE_API_HPP

#include "framework/components/ProfileManager/include/CProfile.hpp"

class IClimateProfile_API : public AXIS::Profile::CProfile
{
   public:

      class Callbacks : public AXIS::Profile::IProfileCallbackProxy
      {
         public:
            virtual void messageReceived(std::string profile_id, std::string message) = 0;
      };

      virtual bool processEvent(Event event, bool need_send = false) = 0;

   protected:

      IClimateProfile_API(AXIS::Profile::IUid const& id, AXIS::Service::Uid const& sid) : CProfile(id, sid) {}
      virtual ~IClimateProfile_API() { }

}; 


#endif /* ICLIMATE_PROFILE_API_HPP */
