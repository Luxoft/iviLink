#ifndef ANDROID_STACK_RESTARTER_HPP
#define ANDROID_STACK_RESTARTER_HPP

#include <jni.h>

#include "IStackRestarter.hpp"

namespace iviLink
{

namespace SystemController
{

class AndroidStackRestarter : public IStackRestarter
{
public:
    AndroidStackRestarter(JavaVM * jm, jobject watchdogWrapper);
    virtual void restartIviLinkFromWatchdog();
    virtual ~AndroidStackRestarter();

private:
    JavaVM * mJavaVM;
    jobject mRestartPerformer;
};

}
}

#endif //ANDROID_STACK_RESTARTER_HPP
