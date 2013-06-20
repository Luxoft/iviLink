/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 21.03.2013
 *
 * System Controller stub - communicates with CA and Negotiator via IPC, 
 * other components simpy access it as a singleton. Launches components in correct order.
 */

#ifndef __iviLinkCoreStack__SystemControllerStub__
#define __iviLinkCoreStack__SystemControllerStub__

#include <deque>
#include <stack>

#include "ChannelSupervisorMsgProxy.hpp"
#include "ConnectivityAgentMsgProxy.hpp"
#include "AuthenticationAppMsgProxy.hpp"
#include "AuthenticationStateMachine.hpp"
#include "AuthenticationDialogStub.hpp"

#include "CMutex.hpp"
#include "Logger.hpp"

#include "CThread.hpp"

namespace iviLink
{
namespace SystemController
{
enum SystemEvents
{
    START_AUTHENTICATION,
    LINK_UP, // getLinkState: false->true notification
    LINK_DOWN, // getLinkState: true->false notification
    DO_STACK_RESET,
    KILL_NEGOTIATOR,
    KILL_CONN_AGENT,
    PROCEED_KILLING_AUTHENTICATION,
    ERR_EMPTY_QUEUE,
};

enum StackComponents
{
    CONNECTIVITY_AGENT, NEGOTIATOR, PROFILE_MANAGER, AUTHENTICATON, ERR_EMPTY,
};

class SystemControllerStub: public SystemControllerMsgProtocol::ChannelSupervisorMsgProxy,
        public SystemControllerMsgProtocol::ConnectivityAgentMsgProxy,
        public SystemControllerMsgProtocol::AuthenticationAppMsgProxy,
        public CThread
{
private:
    SystemControllerStub();
    virtual ~SystemControllerStub();
    SystemControllerStub(const SystemControllerStub&);
    SystemControllerStub& operator=(const SystemControllerStub&);
public:
    static SystemControllerStub * getInstance();

    virtual BaseError onCounterCAConnected(UInt8 role);
    virtual BaseError onCounterCADisconnected();
    virtual BaseError onCounterCSConnected();
    virtual BaseError onCounterCSDisconnected();

    virtual void onChannelSupervisorAvailable();
    virtual void onChannelSupervisorNotAvailable();

    virtual void onConnectivityAgentAvailable()
    {
    }
    virtual void onConnectivityAgentNotAvailable()
    {
    }

    virtual void onAuthenticationAppAvailable()
    {
    }
    virtual void onAuthenticationAppNotAvailable()
    {
    }

    virtual void onAuthenticationOK();
    virtual void onAuthenticationNOK();
    virtual void onAuthenticationCanceled();
    virtual void onAuthenticationError();
    /**
     * Called by PmpCommunicator when it has allocated its channel
     * and has exchanged available profile complements.
     */
    void onProfileManagerHandshakeDone();
    /**
     * Called by AuthenticationDialogObjC. Used when restarting stack for synchronization purposes
     * (because the dialog must be hidden on the UI thread, and we cannot continue with restarting
     * while it is visible).
     */
    void onHidNumPad();
public:
    /**
     * Starts connectivity agent, if nothing has been started yet; does nothing otherwise.
     * Called from Application's initInIviLink().
     */
    void startStack();
    /**
     * @return true if stack is connected and authenticated
     */
    bool getLinkState();
    /**
     * @return true if all components up to Authentication have been started
     * (same as getLinkState, but does not require completed authentication)
     */
    bool canCommunicateWithOtherSide();
private:
    /**
     * if predesessor's value is specified, it is checked by an assertion that
     * predecessor is the current top of the stack
     */
    void pushComponentToStack(StackComponents component, StackComponents predecessor = ERR_EMPTY);

    void resetStackRepresentation();
    /**
     * if expected's value is specified, it is checked by an assertion that
     * the return value is equal to it
     */
    StackComponents popComponentFromStack(StackComponents expected = ERR_EMPTY);

    void processStackResetRequest();
private:
    std::string systemEventToString(SystemEvents event) const;
    std::string stackComponentToString(StackComponents component) const;
private:
    void setLinkState(bool state);
    void pushEventToQueue(SystemEvents event);
    SystemEvents popEventFromQueue();
    virtual void threadFunc();
private:
    UInt8 mRole; // Connectivity Agent role

    bool mIsStarted; // flag if the stack has been started
    CMutex mStartedMutex;

    static Logger mLogger;
    static SystemControllerStub * mInstance;
    static CMutex mSingletonMutex;

    CMutex mLinkMutex;
    bool mIsLinkAlive; // true if stack is connected and authentified

    std::deque<SystemEvents> mEventQueue; // processed in the internal thread
    CMutex mEventsMutex;
    CSignalSemaphore mEventQueueSema;

    /**
     * Stores all currently active components in a LIFO order.
     */
    std::stack<StackComponents> mStackRepresentation;
    bool mIsRewindingStack; // true if stack reset is in progress
    CMutex mStackMutex;

    Authentication::AuthenticationStateMachine * mAuthMachine;
    Authentication::AuthenticationDialogStub * mAuthDialog;
};
}
}

#endif /* defined(__iviLinkCoreStack__SystemControllerStub__) */
