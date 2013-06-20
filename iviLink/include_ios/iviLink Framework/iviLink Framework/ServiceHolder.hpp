/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 26.03.2013
 *
 * Loads and unloads a service, does profile instances management. 
 * (Similar to CService)
 */

#ifndef __iviLinkCoreStack__CService__
#define __iviLinkCoreStack__CService__
#include <tr1/memory>
#include "UID.hpp"
#include "Logger.hpp"
#include "CMutex.hpp"
#include "ProfileHolder.hpp"
#include "PublicError.hpp"

typedef std::tr1::shared_ptr<std::string> tStringPtr;
namespace iviLink
{
namespace App
{
typedef std::map<Profile::Uid, Profile::ProfileHolder*> tLoadedProfilesMap;
class ServiceHolder
{
public:
    ServiceHolder(std::string const&);
    ~ServiceHolder();
    iviLink::Error load();
    iviLink::Error unload();
    iviLink::Error incomingProfile(tStringPtr profileUid, tStringPtr profileImplUid);
    iviLink::Error incomingProfileDied(tStringPtr profileImplUid);

    void silentUnload();

    bool allProfilesLoaded();
    bool allProfilesDead();

    std::string getUid()
    {
        return mUid;
    }
private:
    void cleanUpAfterError();
private:
    std::string mUid;
    std::string mComplementUid;
    Logger mLogger;
    tLoadedProfilesMap mProfilesMap;
};
}
}

#endif /* defined(__iviLinkCoreStack__CService__) */
