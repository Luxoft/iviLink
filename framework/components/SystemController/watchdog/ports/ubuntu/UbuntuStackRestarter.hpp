#ifndef UBUNTU_STACK_RESTARTER_HPP
#define UBUNTU_STACK_RESTARTER_HPP

#include "IStackRestarter.hpp"
#include "Logger.hpp"

namespace iviLink
{

namespace SystemController
{

class UbuntuStackRestarter : public IStackRestarter
{
public:
    UbuntuStackRestarter()
    {
    }
    virtual void restartIviLinkFromWatchdog();
    virtual ~UbuntuStackRestarter()
    {
    }

private:
    static Logger mLogger;
};

}
}

#endif //UBUNTU_STACK_RESTARTER_HPP
