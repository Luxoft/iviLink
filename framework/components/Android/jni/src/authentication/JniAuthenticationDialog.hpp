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
#ifndef JNI_AUTHENTICATION_DIALOG_HPP
#define JNI_AUTHENTICATION_DIALOG_HPP

#include <jni.h>
#include "IAuthenticationDialog.hpp"
#include "Logger.hpp"

namespace iviLink
{
namespace Authentication
{

class JniAuthenticationDialog : public IAuthenticationDialog
{

public:
    JniAuthenticationDialog(JavaVM* jm, jobject callbacks);
    virtual ~JniAuthenticationDialog();
    //from the IAuthenticationDialog interface
    virtual void showNumPad();
    virtual void hideNumPad();
    virtual void showRetryPopup();
    virtual void resetNumPad();
    virtual void lockInput();
    virtual void unlockInput();

private:
    void callJavaMethod(const char * methodName);
    JavaVM * mJM;
    jobject mCallbacks;
    Logger mLogger;
};

}
}
#endif //JNI_AUTHENTICATION_DIALOG_HPP
