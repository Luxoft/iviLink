#ifndef ANDROID_LAUNCHER_HPP
#define ANDROID_LAUNCHER_HPP

#include <jni.h>

#include "ILauncher.hpp"
#include "Logger.hpp"
#include "CMutex.hpp"

namespace iviLink
{
namespace AMP
{

class AndroidLauncher : public ILauncher
{
public:
    AndroidLauncher(JavaVM * jm, jclass launcher);

    virtual void init(ILauncherHandler * handler);

    virtual void launchApplication(const std::string & launchInfo);
    virtual void deinit();
    virtual ~AndroidLauncher() {}

private:
    static Logger mLogger;
    JavaVM * mJavaVM;
    jclass mLaunchPerformer;
    jmethodID mLaunchMethod;
    ILauncherHandler * mHandler;
    CMutex mInitedMutex;
};

}
}

#endif //ANDROID_LAUNCHER_HPP