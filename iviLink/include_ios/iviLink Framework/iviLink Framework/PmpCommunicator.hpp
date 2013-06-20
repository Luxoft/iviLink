/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 21.03.2013
 *
 * Sends to and receives from the other side's PMP.
 */

#ifndef __iviLinkCoreStack__PmpCommunicator__
#define __iviLinkCoreStack__PmpCommunicator__

#include <deque>
#include <dispatch/dispatch.h>

#include "PmpRequestFrame.hpp"
#include "IChannelSupervisorObserver.hpp"

#include "PmpSessionManager.hpp"
#include "PmpCore.hpp"
#include "PmpPIM.hpp"

#include "Logger.hpp"
#include "CMutex.hpp"
#include "CThread.hpp"

namespace iviLink
{
namespace PMP
{
class PmpCommunicator: public ChannelSupervisor::IChannelSupervisorTubeObserver
{
    PmpCommunicator();
    virtual ~PmpCommunicator();
    PmpCommunicator(const PmpCommunicator&);
    PmpCommunicator& operator=(const PmpCommunicator&);
public:
    static PmpCommunicator * getInstance();
    /**
     * Allocate a channel and send PMP_REQ_CMP_MGR_READY request.
     */
    void startCommunication();
    /**
     * Unlock all local profiles and send notification to the other side.
     */
    void unlockAll();
    void unlockAuthenticationProfile();

    void resetState();

    void sendFrame(PMPFrame*);

protected:
    // from the IChannelSupervisorTubeObserver interface
    virtual void onDataReceived(const UInt32 channelID, const UInt32 readSize);
    virtual void onBufferOverflow(const UInt32 channelID);
    virtual void onChannelDeleted(const UInt32 channelID);
    virtual void onConnectionLost();

private:
    void onRequest(PMPFrame * frame);
    void onCmpMgrRequest(PMPFrame * frame);
    void requestAvailableComplements();
    void sendReady(); // send initial CMP_MGR request
private:
    PmpSessionManager mSessionManager;
    PmpCore * mCore;
    PmpPIM * mPIM;
    UInt32 mChannelID;
    static Logger mLogger;
    // concurrent queue for processing incoming frames
    dispatch_queue_t mProcessingPool;
    // serial queue for sending frames
    dispatch_queue_t mSendingQueue;
};
}
}

#endif /* defined(__iviLinkCoreStack__PmpCommunicator__) */
