#ifndef STACK_RESTARTER_HPP
#define STACK_RESTARTER_HPP

namespace iviLink
{

namespace SystemController
{

class IStackRestarter
{
public:
    /**
     * restarts the stack in a platform-dependent way 
     */
    virtual void restartIviLinkFromWatchdog() = 0;
    virtual ~IStackRestarter()
    {
    }
};

}
}

#endif //STACK_RESTARTER_HPP
