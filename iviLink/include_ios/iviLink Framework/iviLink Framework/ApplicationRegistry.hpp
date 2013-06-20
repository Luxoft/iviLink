/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 26.03.2013
 *
 * All "living" applications in the process can be obtained here.
 */

#ifndef __iviLinkCoreStack__ApplicationRegistry__
#define __iviLinkCoreStack__ApplicationRegistry__

#include "Application.hpp"
#include "BaseUid.hpp"
#include "SystemControllerStub.hpp"

namespace iviLink
{
namespace App
{
typedef std::set<Application*> tAppSet;
class ApplicationRegistry
{
    friend class SystemController::SystemControllerStub;
private:
    ApplicationRegistry();
    ~ApplicationRegistry();
    ApplicationRegistry(const ApplicationRegistry&);
    ApplicationRegistry& operator=(const ApplicationRegistry&);
public:
    static ApplicationRegistry * getInstance();
    void registerApplication(Application *);
    void deregisterApplication(Application *);

    iviLink::Application * getApplicationWithEnabledService(iviLink::Service::Uid const& service);
    iviLink::Application * getApplicationWithActiveService(iviLink::Service::Uid const& service);

private:
    void sendLinkUpNotification();
    void sendLinkDownNotification();
private:
    static Logger mLogger;
    tAppSet mApplications;
    CMutex mAppSetMutex;
};
}
}

#endif /* defined(__iviLinkCoreStack__ApplicationRegistry__) */
