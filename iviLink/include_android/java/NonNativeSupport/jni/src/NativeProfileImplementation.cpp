#include "NativeProfileImplementation.hpp"
#include "JniThreadHelper.hpp"
#include "StringConversion.hpp"	

NativeProfileImplementation::NativeProfileImplementation(JavaVM* jm, jobject profileExternal,
        iviLink::Profile::IProfileCallbackProxy* pCbProxy) :
        mpAppCallbacks(static_cast<NonnativeCallbacksWrapper*>(pCbProxy)), mLogger(
                Logger::getInstance("NativeProfileImplementation")), mProfileExternal(
                profileExternal), mpJM(jm)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    iviLink::Android::JniThreadHelper jth(mpJM);
    JNIEnv * env = jth.getEnv();
    jclass claz = env->GetObjectClass(mProfileExternal);
    midReceiveData = env->GetMethodID(claz, "onDataReceived", "([BI)V");
    midOnEnable = env->GetMethodID(claz, "onEnable", "()V");
    midOnDisable = env->GetMethodID(claz, "onDisable", "()V");
    midOnConnectionLost = env->GetMethodID(claz, "onConnectionLost", "()V");
    midChannelDeleted = env->GetMethodID(claz, "onChannelDeleted", "(I)V");

    midGetName = env->GetMethodID(claz, "getName", "()Ljava/lang/String;");
    midGetVersion = env->GetMethodID(claz, "getVersion", "()I");
    midGetUID = env->GetMethodID(claz, "getUid", "()Ljava/lang/String;");
    midGetProfileAPIUID = env->GetMethodID(claz, "getProfileApiUid", "()Ljava/lang/String;");

    midBufferOverflow = env->GetMethodID(claz, "onBufferOverflow", "(I)V");
}

NativeProfileImplementation::~NativeProfileImplementation()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    iviLink::Android::JniThreadHelper jth(mpJM);
    JNIEnv * env = jth.getEnv();
    env->DeleteGlobalRef(mProfileExternal);
}

jobject NativeProfileImplementation::getProfileCallbacks()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    return mpAppCallbacks->getCallbacksImplementation();
}

jobject NativeProfileImplementation::getExternalProfile()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    return mProfileExternal;
}

iviLink::Error NativeProfileImplementation::checkPrioritySerialization(const int priority) const
{
    if (priority == ePlainData || priority == eRealTime)
    {
        return iviLink::Error::NoError();
    }
    return iviLink::Error(iviLink::CHANNEL_ALLOCATION_WRONG_PRIORITY,
            "Check that java's enum matches the c++ one (Types.hpp)", false);
}

iviLink::Error NativeProfileImplementation::allocateChannel(const std::string& tag,
        UInt32& channelID, const int priority)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    iviLink::Error err = checkPrioritySerialization(priority);
    if (!err.isNoError())
    {
        return err;
    }
    return iviLink::Channel::allocateChannel(tag, this, channelID,
            static_cast<TChannelPriority>(priority));
}

iviLink::Error NativeProfileImplementation::allocateChannelAsServer(const std::string& tag,
        UInt32& channelID, const int priority)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    iviLink::Error err = checkPrioritySerialization(priority);
    if (!err.isNoError())
    {
        return err;
    }
    return iviLink::Channel::allocateChannelAsServer(tag, this, channelID,
            static_cast<TChannelPriority>(priority));
}

iviLink::Error NativeProfileImplementation::allocateChannelAsClient(const std::string& tag,
        UInt32& channelID, const int priority)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    iviLink::Error err = checkPrioritySerialization(priority);
    if (!err.isNoError())
    {
        return err;
    }
    return iviLink::Channel::allocateChannelAsClient(tag, this, channelID,
            static_cast<TChannelPriority>(priority));
}

iviLink::Error NativeProfileImplementation::deallocateChannel(const UInt32 channelID)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    return iviLink::Channel::deallocateChannel(channelID);
}

iviLink::Error NativeProfileImplementation::sendData(const char* data, const UInt32 sizeOfData,
        const UInt32 channelID)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    return iviLink::Channel::sendBuffer(channelID, data, sizeOfData);
}

void NativeProfileImplementation::onBufferReceived(const tChannelId channel, Buffer const& buffer)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    iviLink::Android::JniThreadHelper jth(mpJM);
    JNIEnv * env = jth.getEnv();
    jbyteArray dataToJava = env->NewByteArray(buffer.getSize());
    env->SetByteArrayRegion(dataToJava, 0, buffer.getSize(), (jbyte*) buffer.getBuffer());
    env->CallVoidMethod(mProfileExternal, midReceiveData, dataToJava, channel);
}

void NativeProfileImplementation::onChannelDeleted(const UInt32 channel_id)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    iviLink::Channel::deallocateChannel(channel_id);
    iviLink::Android::JniThreadHelper jth(mpJM);
    JNIEnv * env = jth.getEnv();
    env->CallVoidMethod(mProfileExternal, midChannelDeleted, static_cast<jint>(channel_id));
}

void NativeProfileImplementation::onConnectionLost()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    iviLink::Android::JniThreadHelper jth(mpJM);
    JNIEnv * env = jth.getEnv();
    env->CallVoidMethod(mProfileExternal, midOnConnectionLost);
}

void NativeProfileImplementation::onEnable()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    iviLink::Android::JniThreadHelper jth(mpJM);
    JNIEnv * env = jth.getEnv();
    env->CallVoidMethod(mProfileExternal, midOnEnable);
}

void NativeProfileImplementation::onDisable()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    iviLink::Android::JniThreadHelper jth(mpJM);
    JNIEnv * env = jth.getEnv();
    env->CallVoidMethod(mProfileExternal, midOnDisable);
}

std::string NativeProfileImplementation::getName() const
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    iviLink::Android::JniThreadHelper jth(mpJM);
    JNIEnv * env = jth.getEnv();
    jstring jName = static_cast<jstring>(env->CallObjectMethod(mProfileExternal, midGetName));
    return iviLink::Android::StringConversion::jToStd(jName, env);
}

UInt32 NativeProfileImplementation::getVersion() const
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    iviLink::Android::JniThreadHelper jth(mpJM);
    JNIEnv * env = jth.getEnv();
    return static_cast<UInt32>(env->CallIntMethod(mProfileExternal, midGetVersion));
}

iviLink::Profile::Uid NativeProfileImplementation::getUid() const
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    iviLink::Android::JniThreadHelper jth(mpJM);
    JNIEnv * env = jth.getEnv();
    jstring jUid = static_cast<jstring>(env->CallObjectMethod(mProfileExternal, midGetUID));
    return iviLink::Android::StringConversion::jToStd(jUid, env);
}

iviLink::Profile::ApiUid NativeProfileImplementation::getProfileApiUid() const
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    iviLink::Android::JniThreadHelper jth(mpJM);
    JNIEnv * env = jth.getEnv();
    jstring jApiUid = static_cast<jstring>(env->CallObjectMethod(mProfileExternal,
            midGetProfileAPIUID));
    return iviLink::Android::StringConversion::jToStd(jApiUid, env);
}

void NativeProfileImplementation::onBufferOverflow(const iviLink::Channel::tChannelId channelId)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    iviLink::Android::JniThreadHelper jth(mpJM);
    JNIEnv * env = jth.getEnv();
    env->CallVoidMethod(mProfileExternal, midBufferOverflow, static_cast<int>(channelId));
}
