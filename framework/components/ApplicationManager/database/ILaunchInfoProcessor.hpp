#ifndef I_LAUNCH_INFO_CHECKER_HPP
#define I_LAUNCH_INFO_CHECKER_HPP

#include <string>

namespace iviLink
{
namespace AMP
{

class ILaunchInfoProcessor
{
public:
    virtual bool isLaunchInfoValid(const std::string& launchInfo) const = 0;
    virtual std::string trimLaunchInfo(const std::string& launchInfo) const = 0;
    virtual ~ILaunchInfoProcessor() {}
};

}
}

#endif //I_LAUNCH_INFO_CHECKER_HPP