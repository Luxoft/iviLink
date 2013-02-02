/* 
 * iviLINK SDK, version 1.2
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012-2013, Luxoft Professional Corp., member of IBS group
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; version 2.1.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * 
 */ 
#include "JniAuthenticationDialog.hpp"
#include "JniThreadHelper.hpp"

namespace iviLink
{
namespace Authentication
{

using namespace iviLink::Android;
JniAuthenticationDialog::JniAuthenticationDialog(JavaVM* jm, jobject callbacks)
:
    mLogger(Logger::getInstance("JniAuthenticationDialog"))
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mJM = jm;
    JniThreadHelper jth(mJM);
    JNIEnv* env = jth.getEnv();
    mCallbacks = env->NewGlobalRef(callbacks);
}

JniAuthenticationDialog::~JniAuthenticationDialog()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    JniThreadHelper jth(mJM);
    JNIEnv* env = jth.getEnv();
    env->DeleteGlobalRef(mCallbacks);
}

void JniAuthenticationDialog::showNumPad()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    callJavaMethod("showNumPad");
}

void JniAuthenticationDialog::hideNumPad()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    callJavaMethod("hideNumPad");
}

void JniAuthenticationDialog::showRetryPopup()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    callJavaMethod("showRetryPopup");
}

void JniAuthenticationDialog::resetNumPad()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    callJavaMethod("resetNumPad");
}


void JniAuthenticationDialog::lockInput()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    callJavaMethod("lockInput");
}
void JniAuthenticationDialog::unlockInput()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    callJavaMethod("unlockInput");
}

void JniAuthenticationDialog::callJavaMethod(const char * methodName)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "Invoking: " + (std::string)methodName);
    JniThreadHelper jth(mJM);
    JNIEnv* env = jth.getEnv();
    jclass claz = env->GetObjectClass(mCallbacks);
    jmethodID mid = env->GetMethodID(claz, methodName, "()V");
    env->CallVoidMethod(mCallbacks, mid);
}

}
}
