/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 21.03.2013
 */

#include <cassert>
#include "SystemControllerStub.hpp"
#include "SystemControllerObjCInterface.h"
#include "ApplicationRegistry.hpp"
#include "PmpCommunicator.hpp"

#include "AuthenticationDialogStub.hpp"
#include "AuthenticationStateMachine.hpp"

#include "L1InterfaceStub.hpp"

#include "FatalErrorHandler.h"
#include "Channel.hpp"

#define RESTART_FAILED_MSG "Sorry! Could not restart stack components"

namespace iviLink
{
namespace SystemController
{
using namespace SystemControllerMsgProtocol;

CMutex SystemControllerStub::mSingletonMutex;
SystemControllerStub * SystemControllerStub::mInstance = NULL;
Logger SystemControllerStub::mLogger = Logger::getInstance("SystemControllerStub");

SystemControllerStub::SystemControllerStub() :
        CThread("SystemControllerLinkNotification", true), ConnectivityAgentMsgProxy(
                "SysCtr_ConAgnt"), ChannelSupervisorMsgProxy("SysCtr_ChSprvsr"), AuthenticationAppMsgProxy(
                "SysCtrl_AuthApp"), mIsRewindingStack(false)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mAuthDialog = NULL;
    mAuthMachine = NULL;
    mRole = 0;
    mIsStarted = false;
    ConnectivityAgentMsgProxy::connectConnectivityAgent();
    ChannelSupervisorMsgProxy::connectChannelSupervisor();
    AuthenticationAppMsgProxy::connectAuthenticationApp();
    mIsLinkAlive = false;
    CThread::start();
}

SystemControllerStub::~SystemControllerStub()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    CThread::stop();
    mEventQueueSema.signal();
    CThread::join();
}

SystemControllerStub * SystemControllerStub::getInstance()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mSingletonMutex);
    if (mInstance == NULL)
    {
        mInstance = new SystemControllerStub();
    }
    return mInstance;
}

BaseError SystemControllerStub::onCounterCAConnected(UInt8 role)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mRole = role;
    pushComponentToStack(CONNECTIVITY_AGENT);
    startChannelSupervisor();
    return BaseError::NoError();
}

BaseError SystemControllerStub::onCounterCADisconnected()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    if (getLinkState())
    {
        setLinkState(false);
        pushEventToQueue(LINK_DOWN);
    }
    pushEventToQueue(DO_STACK_RESET);
    return BaseError::NoError();
}

void SystemControllerStub::onChannelSupervisorAvailable()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    pushComponentToStack(NEGOTIATOR, CONNECTIVITY_AGENT);
    ChannelSupervisorMsgProxy::sendRole(mRole == 1);
}

BaseError SystemControllerStub::onCounterCSConnected()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    pushComponentToStack(PROFILE_MANAGER, NEGOTIATOR);
    startPMP();
    return BaseError::NoError();
}

BaseError SystemControllerStub::onCounterCSDisconnected()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    return BaseError::NoError();
}

void SystemControllerStub::startStack()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mStartedMutex);
    if (!mIsStarted)
    {
        mIsStarted = true;
        resetStackRepresentation();
        startConnectivityAgent();
    }
}

void SystemControllerStub::onProfileManagerHandshakeDone()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    pushEventToQueue(START_AUTHENTICATION);
}

void SystemControllerStub::setLinkState(bool state)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker linkLock(mLinkMutex);
    mIsLinkAlive = state;
}

bool SystemControllerStub::getLinkState()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mLinkMutex);
    return mIsLinkAlive;
}

bool SystemControllerStub::canCommunicateWithOtherSide()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mStackMutex);
    if (mStackRepresentation.empty())
    {
        return false;
    }
    bool result = (mStackRepresentation.top() == AUTHENTICATON) && !mIsRewindingStack;
    return result;
}

void SystemControllerStub::onAuthenticationOK()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    if (mAuthMachine)
    {
        delete mAuthMachine;
        mAuthMachine = NULL;
        delete mAuthDialog;
        mAuthDialog = NULL;
    }
    setLinkState(true);
    pushEventToQueue(LINK_UP);
}

void SystemControllerStub::onAuthenticationNOK()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "unexpected method call");
    assert(false);
}

void SystemControllerStub::onAuthenticationCanceled()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    pushEventToQueue(DO_STACK_RESET);
}

void SystemControllerStub::onAuthenticationError()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    pushEventToQueue(DO_STACK_RESET);
}

void SystemControllerStub::pushEventToQueue(iviLink::SystemController::SystemEvents event)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mEventsMutex);
    mEventQueue.push_back(event);
    LOG4CPLUS_INFO(mLogger, "send system event: " + systemEventToString(event));
    mEventQueueSema.signal();
}

std::string SystemControllerStub::systemEventToString(
        iviLink::SystemController::SystemEvents event) const
{
    switch (event)
    {
    case LINK_UP:
        return "LINK_UP";
    case LINK_DOWN:
        return "LINK_DOWN";
    case START_AUTHENTICATION:
        return "START_AUTHENTICATION";
    case DO_STACK_RESET:
        return "DO_STACK_RESET";
    case ERR_EMPTY_QUEUE:
        return "ERR_EMPTY_QUEUE";
    case KILL_CONN_AGENT:
        return "KILL_CONN_AGENT";
    case KILL_NEGOTIATOR:
        return "KILL_NEGOTIATOR";
    case PROCEED_KILLING_AUTHENTICATION:
        return "PROCEED_KILLING_AUTHENTICATION";
    default:
        LOG4CPLUS_FATAL(mLogger, "unknown system event: " + convertIntegerToString((int) event));
        assert(false);
    }
    return "";
}

std::string SystemControllerStub::stackComponentToString(
        iviLink::SystemController::StackComponents component) const
{
    switch (component)
    {
    case CONNECTIVITY_AGENT:
        return "CONNECTIVITY_AGENT";
    case NEGOTIATOR:
        return "NEGOTIATOR";
    case PROFILE_MANAGER:
        return "PROFILE_MANAGER";
    case AUTHENTICATON:
        return "AUTHENTICATON";
    case ERR_EMPTY:
        return "ERR_EMPTY";
    default:
        LOG4CPLUS_FATAL(mLogger, "unknown component: " + convertIntegerToString((int) component));
        assert(false);
    }
    return "";
}

SystemEvents SystemControllerStub::popEventFromQueue()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mEventsMutex);
    if (mEventQueue.empty())
    {
        return ERR_EMPTY_QUEUE;
    }
    SystemEvents result = mEventQueue.front();
    mEventQueue.pop_front();
    return result;
}

void SystemControllerStub::onChannelSupervisorNotAvailable()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    popComponentFromStack(NEGOTIATOR);
    pushEventToQueue(KILL_CONN_AGENT);
}

void SystemControllerStub::pushComponentToStack(StackComponents component,
        StackComponents predecessor)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mStackMutex);
    if (predecessor != ERR_EMPTY)
    {
        assert(!mStackRepresentation.empty());
        assert(mStackRepresentation.top() == predecessor);
    }
    mStackRepresentation.push(component);
    LOG4CPLUS_DEBUG(mLogger, "Added component to stack: " + stackComponentToString(component));
}

StackComponents SystemControllerStub::popComponentFromStack(StackComponents expected)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mStackMutex);
    if (mStackRepresentation.empty())
    {
        return ERR_EMPTY;
    }
    if (expected != ERR_EMPTY)
    {
        LOG4CPLUS_DEBUG(mLogger,
                "expecting component in stack: " + stackComponentToString(expected));
        assert(mStackRepresentation.top() == expected);
    }
    StackComponents result = mStackRepresentation.top();
    LOG4CPLUS_DEBUG(mLogger, "pop component from stack: " + stackComponentToString(result));
    mStackRepresentation.pop();
    return result;
}

void SystemControllerStub::resetStackRepresentation()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mStackMutex);
    while (!mStackRepresentation.empty())
    {
        mStackRepresentation.pop();
    }
    mIsRewindingStack = false;
}

void SystemControllerStub::processStackResetRequest()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mStackMutex);
    if (mStackRepresentation.empty())
    {
        LOG4CPLUS_INFO(mLogger, "system event: DO_STACK_RESET stack is empty");
        return;
    }
    if (mIsRewindingStack)
    {
        LOG4CPLUS_INFO(mLogger, "system event: DO_STACK_RESET reset in progress");
        return;
    }
    mIsRewindingStack = true;
    switch (mStackRepresentation.top())
    {
    case AUTHENTICATON:
    {
        mStackRepresentation.pop();
        LOG4CPLUS_INFO(mLogger, "stack has: AUTHENTICATON");
        if (mAuthMachine)
        {
            LOG4CPLUS_INFO(mLogger, "stack has: AUTHENTICATON && mAuthMachine != NULL");
            mAuthDialog->hideNumPad(); // request to hide the ui elements on the main thread
            return; // have to wait until hiding is done - see onHidNumPad callback
        }
    } // no break, as there must be profile manager on the stack before auth, and we can process its reset on same thread
    case PROFILE_MANAGER:
    {
        if (mStackRepresentation.top() != PROFILE_MANAGER)
        {
            LOG4CPLUS_FATAL(mLogger, "Stack was supposed to have PROFILE_MANAGER");
            showDialogAndDie(RESTART_FAILED_MSG);
        }
        else
        {
            LOG4CPLUS_INFO(mLogger, "stack has: PROFILE_MANAGER");
            PMP::PmpCommunicator::getInstance()->resetState();
            mStackRepresentation.pop();
        }
    } // no break
    case NEGOTIATOR:
    {
        if (mStackRepresentation.top() != NEGOTIATOR)
        {
            LOG4CPLUS_FATAL(mLogger, "Stack was supposed to have NEGOTIATOR");
            showDialogAndDie(RESTART_FAILED_MSG);
        }
        else
        {
            pushEventToQueue(KILL_NEGOTIATOR);
        }
    }
        break;
    case CONNECTIVITY_AGENT:
    {
        LOG4CPLUS_INFO(mLogger, "stack has only: CONNECTIVITY_AGENT");
        pushEventToQueue(KILL_CONN_AGENT);
    }
        break;
    default:
        break;
    }
}

void SystemControllerStub::onHidNumPad()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mStackMutex);
    if (mIsRewindingStack) // onHidNumPad may also be called when authentication is successful
    {
        LOG4CPLUS_INFO(mLogger, "mIsRewindingStack");
        pushEventToQueue(PROCEED_KILLING_AUTHENTICATION);
    }
}

void SystemControllerStub::threadFunc()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    while (!getStopFlag())
    {
        mEventQueueSema.wait();
        SystemEvents event = popEventFromQueue();
        LOG4CPLUS_INFO(mLogger, "system event: " + systemEventToString(event));
        switch (event)
        {
        case LINK_UP:
            PMP::PmpCommunicator::getInstance()->unlockAll();
            App::ApplicationRegistry::getInstance()->sendLinkUpNotification();
            break;
        case LINK_DOWN:
            App::ApplicationRegistry::getInstance()->sendLinkDownNotification();
            break;
        case START_AUTHENTICATION:
        {
            mAuthMachine = new Authentication::AuthenticationStateMachine(
                    std::string(getFilePath()));
            mAuthDialog = new Authentication::AuthenticationDialogStub(mAuthMachine);
            mAuthMachine->startAuthentication(mAuthDialog);
            pushComponentToStack(AUTHENTICATON, PROFILE_MANAGER);
            PMP::PmpCommunicator::getInstance()->unlockAuthenticationProfile();
            if (mRole == 1)
            {
                mAuthMachine->onInitDone(iviLink::LAUNCHED_BY_USER);
            }
            else
            {
                mAuthMachine->onInitDone(iviLink::LAUNCHED_BY_IVILINK);
            }
        }
            break;
        case DO_STACK_RESET:
            processStackResetRequest();
            break;
        case PROCEED_KILLING_AUTHENTICATION:
        {
            if (!mAuthDialog)
            {
                LOG4CPLUS_FATAL(mLogger, "trying to delete authentication machine twice!");
                showDialogAndDie(RESTART_FAILED_MSG);
            }
            else
            {
                delete mAuthMachine;
                mAuthMachine = NULL;
                delete mAuthDialog;
                mAuthDialog = NULL;
            }
            mStackMutex.lock();
            if (!mIsRewindingStack)
            {
                LOG4CPLUS_FATAL(mLogger,
                        "After PROCEED_KILLING_AUTHENTICATION stack is expected to be in rewinding state!");
                showDialogAndDie(RESTART_FAILED_MSG);
            }
            if (mStackRepresentation.top() == PROFILE_MANAGER)
            {
                LOG4CPLUS_INFO(mLogger, "stack has: PROFILE_MANAGER");
                PMP::PmpCommunicator::getInstance()->resetState();
                mStackRepresentation.pop();
            }
            else
            {
                LOG4CPLUS_FATAL(mLogger, "Stack was expected to have PROFILE_MANAGER in it!");
                showDialogAndDie(RESTART_FAILED_MSG);
            }

            if (mStackRepresentation.top() == NEGOTIATOR)
            {
                LOG4CPLUS_INFO(mLogger, "stack has: NEGOTIATOR");
                pushEventToQueue(KILL_NEGOTIATOR);
            }
            else
            {
                LOG4CPLUS_FATAL(mLogger, "Stack was expected to have NEGOTIATOR!");
                showDialogAndDie(RESTART_FAILED_MSG);
            }
            mStackMutex.unlock();
        }
            break;
        case KILL_NEGOTIATOR:
            iviLink::Channel::deinitChannelLib();
            if (!ChannelSupervisorMsgProxy::requestShutDown().isNoError())
            {
                LOG4CPLUS_FATAL(mLogger, "could not shut down negotiator");
                showDialogAndDie(RESTART_FAILED_MSG);
            }
            break;
        case KILL_CONN_AGENT:
        {
            iviLink::ConnectivityAgent::L1::L1InterfaceStub::getInstance()->deleteInstance();
            deinitConnectivityAgentProxy();
            resetStackRepresentation();
            startConnectivityAgent();
        }
            break;
        case ERR_EMPTY_QUEUE:
            LOG4CPLUS_WARN(mLogger, "system event: ERR_EMPTY_QUEUE");
            break;
        default:
            LOG4CPLUS_FATAL(mLogger,
                    "unknown system event: " + convertIntegerToString((int) event));
            assert(false);
            break;
        }
    }
}
}
}
