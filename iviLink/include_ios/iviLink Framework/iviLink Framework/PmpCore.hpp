/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 22.03.2013
 * Communicates with PMP Core on the other side, supervises locking
 * and unlocking profiles.
 */

#ifndef __iviLinkCoreStack__PmpCore__
#define __iviLinkCoreStack__PmpCore__

#include <set>

#include "PmpRequestFrame.hpp"
#include "CSignalSemaphore.hpp"
#include "UID.hpp"

namespace iviLink
{
namespace PMP
{
class PmpCore
{
private:
    PmpCore();
    virtual ~PmpCore();
    PmpCore(const PmpCore&);
    PmpCore& operator=(const PmpCore&);
public:
    static PmpCore * getInstance();
    /**
     * Wait until notification comes from the other side
     * about unlock of the authentication profile.
     */
    void waitForAuthUnlocked();
    /**
     * Wait until PMP_REQ_CORE_UNLOCK_ALL comes from the other side,
     * and all remote profiles in the database become enabled.
     */
    void waitForAllUnlocked();
    /**
     * Enable local authentication profile in the database
     * and notify the other side.
     */
    void unlockAuthenticationProfile();
    /**
     * Enable all local profile in the database, and send notification
     * to the other side.
     */
    void unlockAll();

    void lockAll();

    void processFrame(PMPFrame*);
private:
    void onGetAvailableComplementsRequest();
    void onGetAvailableComplementsResponse(PMPFrame * frame);
    void onUnlockProfile(PMPFrame * frame);
private:
    CSignalSemaphore mAuthWaitSema;
    CSignalSemaphore mAllUnlockSema;

    static Logger mLogger;
};
}
}
#endif /* defined(__iviLinkCoreStack__PmpCore__) */
