/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 22.03.2013
 * Communicates with PIM on the other side (processing of profile creation
 * and death notifications/requests)
 */

#ifndef __iviLinkCoreStack__PmpPIM__
#define __iviLinkCoreStack__PmpPIM__
#include "PmpRequestFrame.hpp"
#include "Logger.hpp"
#include "ProfileHolder.hpp"
#include "BaseProfileApi.hpp"

namespace iviLink
{
namespace PMP
{
class PmpPIM
{
private:
    PmpPIM();
    virtual ~PmpPIM();
public:
    static PmpPIM * getInstance();
    void processFrame(PMPFrame *);
public:
    /**
     * Create profile implementation for the given uid using ServicesInfoProvider.
     * @return NULL if ServicesInfoProvider cannot create it
     */
    Profile::ProfileHolder * createProfile(Profile::IProfileCallbackProxy * proxy,
            std::string const&, std::string const&, std::string const&);
    void sendCreateProfileRequest(Service::Uid const&, Profile::Uid const&, Profile::IUid const&);
    void sendProfileDiedRequest(iviLink::Profile::IUid const&, iviLink::Service::Uid const&);
private:
    void onCreateProfileRequest(PMPFrame*);
    void onProfileDiedRequest(PMPFrame*);
private:
    static Logger mLogger;
};
}
}

#endif /* defined(__iviLinkCoreStack__PmpPIM__) */
