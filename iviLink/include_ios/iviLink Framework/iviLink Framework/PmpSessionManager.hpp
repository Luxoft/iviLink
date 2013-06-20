/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 21.03.2013
 *
 * Waiting services logic: currently not implemented (only sends negotiation numbers).
 */

#ifndef __iviLinkCoreStack__PmpSessionManager__
#define __iviLinkCoreStack__PmpSessionManager__

#include "PmpRequestFrame.hpp"
#include "Logger.hpp"

namespace iviLink
{
namespace PMP
{
enum Role
{
    PMP_ROLE_MASTER, PMP_ROLE_SLAVE, PMP_ROLE_NONE
};

class PmpSessionManager
{
public:
    PmpSessionManager();
    virtual ~PmpSessionManager();
    void processFrame(PMPFrame *);
    void onSendNegotiateNumber();
private:
    void onGotNegotiateNumber(UInt32 number);
    void onGotWaitingServicesList(); // TODO ???
    void genAndSendRoleNumber();
    void checkRole();
private:
    UInt32 mRoleNumber;
    UInt32 mOtherSideRoleNumber;
    Role mRole;
    static Logger mLogger;
};
}
}

#endif /* defined(__iviLinkCoreStack__PmpSessionManager__) */
