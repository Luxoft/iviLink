/* 
 * 
 * iviLINK SDK, version 1.0
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



#ifndef CCLIMATEHUPROFILECALLBACKS_HPP
#define CCLIMATEHUPROFILECALLBACKS_HPP

#include "utils/misc/Logger.hpp"
#include "samples/linux/Profiles/ProfileAPI/IClimateHUProfile.hpp"
#include "IProfileObserver.hpp"

class CClimateHUProfileCallbacks
        :
        public IClimateHUProfile::Callbacks
{
public:
        explicit CClimateHUProfileCallbacks(IProfileObserver* pObserver)
           : pOwner(pObserver)
           , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("samples.Profiles.CClimateHUProfile.Callbacks")))
        {
           LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
        }
        virtual void handleError(CError const & error)
        {
           LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
           LOG4CPLUS_ERROR(mLogger, static_cast<std::string>(error));
        }

        /**
      * Callback for profile created on the other side; 
      * change temperature settings request
      * @param event specify the temperature event
      */
     virtual void onTempRequest(ETempEvent event)
        {
           LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
            pOwner->onTempRequest(event);
        }

        /**
      * Callback for profile created on the other side; 
      * change fan settings request
      * @param event specify the fan event
      */
     virtual void onFanRequest(EFanEvent event)
        {
           LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
            pOwner->onFanRequest(event);
        }

        /**
      * Callback for profile created on the other side; 
      * change heater settings request
      * @param event specify the heater event
      */
     virtual void onHeaterRequest(EHeaterEvent event)
        {
           LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
            pOwner->onHeaterRequest(event);
        }

	/**
      * Callback for profile created on the other side; 
      * change general settings request
      * @param event kind of settings
      */
     virtual void onGeneralRequest(EGeneralRequest event)
        {
      LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
            pOwner->onGeneralRequest(event);
        }

        /**
      * Callback for profile created on the other side;
      * initialization state request
      */
     virtual void onInitRequest()
        {
           LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
            pOwner->onInitRequest();
        }

    private:

        IProfileObserver* pOwner;
        Logger mLogger;
};

#endif // CCLIMATEHUPROFILECALLBACKS_HPP
