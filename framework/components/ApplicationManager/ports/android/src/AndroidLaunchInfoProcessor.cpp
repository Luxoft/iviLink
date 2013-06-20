#include "AndroidLaunchInfoProcessor.hpp"
#include "JniThreadHelper.hpp"
#include "StringConversion.hpp"

using namespace iviLink::Android;

namespace iviLink
{
namespace AMP
{

AndroidLaunchInfoProcessor::AndroidLaunchInfoProcessor(JavaVM * javaVM, jobject checker)
: mLogger(Logger::getInstance("iviLink.AMP.AndroidLaunchInfoProcessor"))
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mJavaVM = javaVM;
    JniThreadHelper jth(mJavaVM);
    JNIEnv * env = jth.getEnv();
    mChecksPerformer = env->NewGlobalRef(checker);
    jclass checkerClaz = env->GetObjectClass(mChecksPerformer);
    assert (checkerClaz != 0);
    mCheckMethod = env->GetMethodID(checkerClaz, "isLaunchInfoValid","(Ljava/lang/String;)Z");
    assert (mCheckMethod != 0);
    mTrimMethod = env->GetMethodID(checkerClaz, "trimLaunchInfo","(Ljava/lang/String;)Ljava/lang/String;");
    assert (mTrimMethod != 0);
}

bool AndroidLaunchInfoProcessor::isLaunchInfoValid(const std::string& launchInfo) const
{
    JniThreadHelper jth(mJavaVM);
    JNIEnv * env = jth.getEnv();
    jstring jLaunchInfo = StringConversion::stdToJ(launchInfo, env);
    return env->CallBooleanMethod(mChecksPerformer, mCheckMethod, jLaunchInfo);
}

std::string AndroidLaunchInfoProcessor::trimLaunchInfo(const std::string& launchInfo) const
{
    JniThreadHelper jth(mJavaVM);
    JNIEnv * env = jth.getEnv();
    jstring jLaunchInfo = StringConversion::stdToJ(launchInfo, env);
    jstring trimmedInfo = (jstring)env->CallObjectMethod(mChecksPerformer, mTrimMethod, jLaunchInfo);
    return StringConversion::jToStd(trimmedInfo, env);
}

AndroidLaunchInfoProcessor::~AndroidLaunchInfoProcessor ()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    JniThreadHelper jth(mJavaVM);
    JNIEnv * env = jth.getEnv();
    env->DeleteGlobalRef(mChecksPerformer);
}

}
}
