/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 26.03.2013
 *
 * Implementation of this class should be provided by SDK user
 */

#ifndef iviLinkCoreStack_ServicesInfo_h
#define iviLinkCoreStack_ServicesInfo_h

#include <string>
#include <map>
#include <list>

#include "BaseProfileApi.hpp"
#include "ServicesInfoTypes.hpp"

namespace iviLink
{
namespace PMP
{
class Database;
class PmpPIM;
}

/**
 * Should be implemented by the SDK user.
 */

class ServicesInfoProvider
{
    friend class PMP::Database;
    friend class PMP::PmpPIM;
public:
    ServicesInfoProvider()
    {
    }
    ~ServicesInfoProvider()
    {
    }
private:
    /**
     * The map should be filled with services, api and profiles info.
     * This method is called only once, so any attempts to optimize are unnecessary.
     */
    tServiceInfoMap getKnownServices() const;
    /**
     * Factory method. It should be able to create a profile
     * implementation for any uid that is listed in the map returned from getKnownServices()
     * as a local profile implementation (see ProfileInfo::mUid)
     */
    BaseProfileApi * createProfileImpl(Profile::Uid const& profileUid, UInt32 requestedVersion, iviLink::Profile::IProfileCallbackProxy*) const;
};
}

#endif
