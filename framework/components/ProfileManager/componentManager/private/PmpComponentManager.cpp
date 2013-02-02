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


#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "configurator.h"
#include "EventHandler.hpp"

#include "PmpComponentManager.hpp"
#include "PmpCore.hpp"
#include "PmpPim.hpp"
#include "PmpInteractionProtocol.hpp"
#include "PmpSessionProtocol.hpp"
#include "ProfileRepository.hpp"
#include "ServiceRepository.hpp"
#include "PmpIpcProtocol.hpp"
#include "EventFactory.hpp"
#include "AmpClient.hpp"
#include "SystemControllerMsg.hpp"
#include "CSignalSemaphore.hpp"
#include "EventFactory.hpp"
#include "Exit.hpp"
#include "WaitingServiceManager.hpp"
#include "get_mac.hpp"


namespace iviLink
{

namespace PMP
{

PmpComponentManager::PmpComponentManager(conf::Configurator * pConfig/* = NULL*/)
    : mConfig(pConfig)
    , mProfileRepository(NULL)
    , mServiceRepository(NULL)
    , mCore(NULL)
    , mPim(NULL)
    , mInteractionProtocol(NULL)
    , mIpcProtocol(NULL)
    , mAmpClient(NULL)
    , mEventHandler(NULL)
    , mEventFactory(NULL)
    , mSCMsg(NULL)
    , mSemaphore(NULL)
    , mInitializationDone(false)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.PmpComponentManager")))
    , mRole(PMP_ROLE_NONE)
    , mRoleNubmer(-1)
    , mSessionManager(NULL)
	, mOtherRoleNumber(-1)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
#ifndef ANDROID
    std::string platform = "Ubuntu";
#else
    std::string platform = "Android";
#endif //ANDROID

    mProfileRepository = new ProfileRepository(mConfig);
    mServiceRepository = new ServiceRepository(mConfig);
    mSessionManager = new WaitingServiceManager();
    mCore = new PmpCore(mProfileRepository, platform);
    mPim = new PmpPim();
    mInteractionProtocol = new PmpInteractionProtocol();

    LOG4CPLUS_INFO(mLogger, "PMP interaction protocol created");


    mIpcProtocol = new PMP::Ipc::PmpIpcProtocol();

    LOG4CPLUS_INFO(mLogger, "IPC protocol created");

    mAmpClient = new AppMan::Ipc::AmpClient();
    mEventHandler = new Utils::EventHandler();

    mEventFactory = new EventFactory(mEventHandler, this,
    mProfileRepository, mCore, mPim, mIpcProtocol, mInteractionProtocol, mSessionManager);

    LOG4CPLUS_INFO(mLogger, "EventFactory created");

    mSemaphore = new CSignalSemaphore();

    #if !defined(NO_SYSTEM_CONTROLLER_LAUNCH)
    mSCMsg = new SystemControllerMsg();
    #endif // NO_SYSTEM_CONTROLLER_LAUNCH
}

PmpComponentManager::~PmpComponentManager()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    delete mEventFactory;
    delete mEventHandler;
    delete mAmpClient;
    delete mIpcProtocol;
    delete mInteractionProtocol;
    delete mPim;
    delete mCore;
    delete mServiceRepository;
    delete mProfileRepository;
    delete mSessionManager;
}

void PmpComponentManager::initPmp()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );

    assert(mConfig);
    assert(mProfileRepository);
    assert(mServiceRepository);
    assert(mCore);
    assert(mPim);
    assert(mInteractionProtocol);
    assert(mIpcProtocol);
    assert(mAmpClient);
    assert(mEventHandler);
    assert(mEventFactory);
    assert(mSessionManager);
    if (! mSCMsg)
    {
        LOG4CPLUS_WARN(mLogger, "No CS MSG!!!");
    }

    mCore->init(mPim, mInteractionProtocol->coreProtocol(), mEventFactory);
    mCore->reloadProfilesFromRepository();
    mCore->print();
    mPim->setIpc(mIpcProtocol);
    mPim->setCore(mCore);
    mPim->setEventFactory(mEventFactory);
    mPim->setPimProtocol(mInteractionProtocol->pimProtocol());
    mPim->setAmp(mAmpClient);
    mInteractionProtocol->init(mEventFactory);

    LOG4CPLUS_INFO(mLogger, "initPmp - subcomponents inited");

    if (mSCMsg)
    {
        mSCMsg->init(mEventFactory);
    }

    LOG4CPLUS_INFO(mLogger, "initPmp - SC inited");

    std::string ipcAddr;
    if (mConfig)
    {
        ipcAddr = mConfig->getParam("pmp_ipc_address");
    }

    LOG4CPLUS_INFO(mLogger, "initPmp - subcomponents inited");

    mIpcProtocol->init(mEventFactory->getIpcPimHandler(),
    mEventFactory->getIpcCoreHandler(), ipcAddr.empty() ? NULL : ipcAddr.c_str());
    mAmpClient->init(mEventFactory->getAmpHandler());

    if (!mInteractionProtocol->connect())
    {
        LOG4CPLUS_FATAL(mLogger, "Can't connect interaction protocol");
        killProcess(1);
    }

    if (!mInteractionProtocol->ready())
    {
        LOG4CPLUS_FATAL(mLogger, "Can't send ready");
        killProcess(1);
    }

    mSessionManager->init(mEventFactory,mInteractionProtocol->sessionProtocol(),
            mServiceRepository, mAmpClient);
    mEventHandler->init();
}

void PmpComponentManager::uninitPmp()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mEventHandler->deinit();
    mAmpClient->disconnect();
    mInteractionProtocol->disconnect();
}


void PmpComponentManager::onChannelDeleted()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    if (mSCMsg)
    {
        mSCMsg->requestDisconnected();
    }
    signal();
}

void PmpComponentManager::onConnectionLost()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    if (mSCMsg)
    {
        mSCMsg->requestDisconnected();
    }
    signal();
}

void PmpComponentManager::onReady()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    assert(mEventFactory);
    mEventFactory->resetProfilesState();
}

void PmpComponentManager::onAmpConnection()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
}

void PmpComponentManager::wait()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    assert(mSemaphore);
    mSemaphore->wait();
}

void PmpComponentManager::signal()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    assert(mSemaphore);
    mSemaphore->signal();
}

void PmpComponentManager::onAmpConnectionLost()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    if (mSCMsg)
    {
        mSCMsg->requestDisconnected();
    }
    signal();
}

void PmpComponentManager::endInitialization()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );

    assert(mIpcProtocol);
    assert(mAmpClient);

    if (mInitializationDone)
    {
        LOG4CPLUS_WARN(mLogger, "Initialization done");
        return;
    }
    mInitializationDone = true;

    if (!mIpcProtocol->connect())
    {
        LOG4CPLUS_FATAL(mLogger, "Can't connect IPC protocol");
        killProcess(1);
    }

    LOG4CPLUS_INFO(mLogger, "IPC protocol connected");

    if (mSCMsg)
    {
        mSCMsg->requestConnected();
        LOG4CPLUS_INFO(mLogger, "Sent request to SC");
    }
    else
    {
        LOG4CPLUS_WARN(mLogger, "Started without SC connection possibility");
    }

    bool res = false;
    for(int i=0; i<30; ++i)
    {
        LOG4CPLUS_INFO(mLogger, "Connecting to AMP...");
        if (mAmpClient->connect())
        {
            res = true;
            break;
        }
        usleep(250000);
    }
    if (!res)
    {
        LOG4CPLUS_FATAL(mLogger, "No AMP connection");
        killProcess(1);
    }
    LOG4CPLUS_INFO(mLogger, "AMP connected");

    char macAddr[13] = "";

    bool mres = get_mac(macAddr);
    int mac = mres ? atoi(macAddr) : 0;
    srand(mac + time(NULL) + getpid());
    negotiateRole();
    if (-1 != mOtherRoleNumber)
    {
    	onNegotiateRole(mOtherRoleNumber);
    }
}

Role PmpComponentManager::getRole() const
{
    return mRole;
}

void PmpComponentManager::negotiateRole()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );

    genRandomNumber();
    mInteractionProtocol->sessionProtocol()->sendNegotiateNumber(mRoleNubmer);
}

void PmpComponentManager::genRandomNumber()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );

    mRoleNubmer = abs(rand()+1);
    LOG4CPLUS_INFO(mLogger, "Generated random number: " + convertIntegerToString(mRoleNubmer));
}

void PmpComponentManager::onNegotiateRole(UInt32 number)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    LOG4CPLUS_INFO(mLogger, "mRoleNumber:" +  convertIntegerToString(mRoleNubmer) +
            " number: " + convertIntegerToString(number));
    if (-1 == mRoleNubmer)
    {
    	LOG4CPLUS_INFO(mLogger, "RoleNegotiationBackOrder");
    	mOtherRoleNumber = number;
    }
    else if (mRoleNubmer == number)
    {
        negotiateRole();
    }
    else
    {
        if (mRoleNubmer > number)
        {
            mRole = PMP_ROLE_MASTER;
        }
        else
        {
            mRole = PMP_ROLE_SLAVE;
        }
        checkWaitingServices();
    }
}

void PmpComponentManager::checkWaitingServices()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    if (PMP_ROLE_NONE == getRole())
    {
        LOG4CPLUS_INFO(mLogger, "NONE ROLE");
        return;
    }
    else if  (PMP_ROLE_MASTER == getRole())
    {
        LOG4CPLUS_INFO(mLogger, "MASTER");
        if (mSessionManager->isLocalWaitingServicesSet() &&
                mSessionManager->isRemoteWaitingServicesSet())
        {
            mEventFactory->startWaitingService(true);
        }
    }
    else
    {
        LOG4CPLUS_INFO(mLogger, "SLAVE");
        if (mSessionManager->isLocalWaitingServicesSet())
        {
            mEventFactory->startWaitingService(false);
        }
    }
}

}
}
