#ifndef UBUNTU_LAUNCH_INFO_CHECKER_HPP
#define UBUNTU_LAUNCH_INFO_CHECKER_HPP

#include "ILaunchInfoProcessor.hpp"

namespace iviLink
{
namespace AMP
{

class UbuntuLaunchInfoProcessor : public ILaunchInfoProcessor
{
public:
    UbuntuLaunchInfoProcessor(){}
    virtual bool isLaunchInfoValid(const std::string& launchInfo) const;
    virtual std::string trimLaunchInfo(const std::string& launchInfo) const;
    virtual ~UbuntuLaunchInfoProcessor () {}
};

}
}

#endif //UBUNTU_LAUNCH_INFO_CHECKER_HPP