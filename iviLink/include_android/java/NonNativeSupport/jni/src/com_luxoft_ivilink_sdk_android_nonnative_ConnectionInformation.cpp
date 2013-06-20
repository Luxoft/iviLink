#include <sstream>

#include "com_luxoft_ivilink_sdk_android_nonnative_ConnectionInformation.h"
#include "UnstableAPI.hpp"
#include "PublicError.hpp"
#include "StringConversion.hpp"

JNIEXPORT jstring JNICALL Java_com_luxoft_ivilink_sdk_android_nonnative_ConnectionInformation_getConnectionInformationNative(
        JNIEnv * env, jobject)
{
    ConnectionInformation conInfo;
    iviLink::Error err = ::getConnectionInformation(&conInfo);
    std::stringstream ss;
    ss << conInfo.getConnectionType() << "#!#" << conInfo.getRemoteAddress() << "#!#"
            << conInfo.getLocalAddress() << "#!#" << err.serializeForJNI();
    return iviLink::Android::StringConversion::stdToJ(ss.str(), env);
}
