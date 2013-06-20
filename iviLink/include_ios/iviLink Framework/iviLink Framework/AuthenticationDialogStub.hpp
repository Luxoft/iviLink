/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 28.03.2013
 * Authentication UI. See AuthenticationDialogObjC.mm for implementation.
 */

#ifndef __iviLinkCoreStack__AuthenticationDialogStub__
#define __iviLinkCoreStack__AuthenticationDialogStub__

#include "IAuthenticationDialog.hpp"

#include "AuthenticationStateMachine.hpp"
namespace iviLink
{
namespace Authentication
{
class AuthenticationDialogStub: public IAuthenticationDialog
{
public:
    AuthenticationDialogStub(AuthenticationStateMachine*);
    ~AuthenticationDialogStub();
    virtual void showNumPad(); // show UIAlertView with input box
    virtual void hideNumPad(); // hides all ui
    virtual void showRetryPopup()
    {
    } // not implemented
    virtual void resetNumPad()
    {
    } // not implemented, all logic is moved to unlockInput
    virtual void lockInput(); // show dialog with spinner and entered pin
    virtual void unlockInput(); // show UIAlertView with input box, hide dialog with spinner

    void onOkClicked(std::string const& pinLine);
    void onCancelClicked();

private:
    AuthenticationStateMachine * mMachine;
    // pointer to an instance of AuthenticationDialogObjC
    void * mImpl;
    std::string mFilledPin;
};

} //namespace Authentication
} //namespace iviLink

#endif /* defined(__iviLinkCoreStack__AuthenticationDialogStub__) */
