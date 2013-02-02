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


#include "PmpInteractionProtocol.hpp"

#include "PmpProtocol.hpp"
#include "PmpCoreProtocol.hpp"
#include "PmpPimProtocol.hpp"
#include "PmpSessionProtocol.hpp"

namespace iviLink
{
namespace PMP
{

Logger PmpInteractionProtocol::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.interaction.PmpInteractionProtocol"));

PmpInteractionProtocol::PmpInteractionProtocol()
    : mPmpProtocol(new PmpProtocol())
    , mCoreProtocol(new PmpCoreProtocol())
    , mPimProtocol(new PmpPimProtocol())
    , mSessionProtocol(new PmpSessionProtocol())
    , mEventFactory(NULL)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

PmpInteractionProtocol::~PmpInteractionProtocol()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    delete mPmpProtocol;
    delete mCoreProtocol;
    delete mPimProtocol;
}

bool PmpInteractionProtocol::connect()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    assert(mPmpProtocol);
    return mPmpProtocol->connect();
}

void PmpInteractionProtocol::disconnect()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    assert(mPmpProtocol);
    mPmpProtocol->disconnect();
}

bool PmpInteractionProtocol::ready()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    assert(mPmpProtocol);
    return mPmpProtocol->ready();
}

bool PmpInteractionProtocol::isReady() const
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    assert(mPmpProtocol);
    return mPmpProtocol->isReady();
}

PmpCoreProtocol * PmpInteractionProtocol::coreProtocol()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    assert(mCoreProtocol);
    return mCoreProtocol;
}

PmpPimProtocol * PmpInteractionProtocol::pimProtocol()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    assert(mPimProtocol);
    return mPimProtocol;
}

PmpSessionProtocol * PmpInteractionProtocol::sessionProtocol()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    assert(mSessionProtocol);
    return mSessionProtocol;
}


void PmpInteractionProtocol::init(EventFactory * eventFactory)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    assert(mCoreProtocol);
    assert(mPimProtocol);
    assert(mPmpProtocol);
    assert(mSessionProtocol);

    mEventFactory = eventFactory;

    mCoreProtocol->initEventFactory(eventFactory);
    mCoreProtocol->initPmpProtocol(mPmpProtocol);

    mPimProtocol->initEventFactory(eventFactory);
    mPimProtocol->initPmpProtocol(mPmpProtocol);

    mPmpProtocol->init(mEventFactory, mCoreProtocol, mPimProtocol, mSessionProtocol);
    mSessionProtocol->initEventFactory(mEventFactory);
    mSessionProtocol->initPmpProtocol(mPmpProtocol);
}

} /* namespace PMP */
} /* namespace iviLink */
