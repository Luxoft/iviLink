#ifndef ANDROID_LAUNCH_INFO_CHECKER_HPP
#define ANDROID_LAUNCH_INFO_CHECKER_HPP

#include "ILaunchInfoProcessor.hpp"
#include "Logger.hpp"

#include <jni.h>

namespace iviLink
{
namespace AMP
{

class AndroidLaunchInfoProcessor : public ILaunchInfoProcessor
{
public:
    AndroidLaunchInfoProcessor(JavaVM * JavaVM, jobject checker);

    virtual bool isLaunchInfoValid(const std::string& launchInfo) const;
    virtual std::string trimLaunchInfo(const std::string& launchInfo) const;
    virtual ~AndroidLaunchInfoProcessor ();

private:
    JavaVM * mJavaVM;
    jobject mChecksPerformer;
    jmethodID mCheckMethod;
    jmethodID mTrimMethod;

    Logger mLogger;
};

}
}

#endif //ANDROID_LAUNCH_INFO_CHECKER_HPP