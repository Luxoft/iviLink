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
