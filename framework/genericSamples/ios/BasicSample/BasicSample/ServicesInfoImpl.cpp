
/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 29.03.2013
 *
 * BasicSample test.
 */

#include <iviLink/ServicesInfoProvider.hpp>

#include "CBasicSampleProfileImpl.hpp"

using namespace iviLink;

tServiceInfoMap ServicesInfoProvider::getKnownServices() const
{
    tServiceInfoMap result;
    ServiceInfo basicSample("BasicSampleService", "BasicSampleProfile_PAPI_UID", "BasicSampleProfileImpl_UID", 1);
    result[basicSample.mServiceUid.value()] = basicSample;
    return result;
}

BaseProfileApi * ServicesInfoProvider::createProfileImpl(const Profile::Uid &profileUid, UInt32 version, iviLink::Profile::IProfileCallbackProxy * callback) const
{
    if (profileUid.value() == "BasicSampleProfileImpl_UID" && version == 1)
    {
        return new CBasicSampleProfileImpl(callback);
    }
    return NULL;
}