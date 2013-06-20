#ifndef I_AUTHENTICATION_DIALOG_HPP
#define I_AUTHENTICATION_DIALOG_HPP

namespace iviLink
{
namespace Authentication
{

class IAuthenticationDialog
{
public:
    /**
     * Show the dialog with inputbox
     */
    virtual void showNumPad() = 0;
    /**
     * Hide the dialog with inputbox
     */
    virtual void hideNumPad() = 0;
    /**
     * Make some notification to user that he should reattempt
     */
    virtual void showRetryPopup() = 0;
    /**
     * Line with entered pin should be cleared
     */
    virtual void resetNumPad() = 0;
    /**
     * Cannot press number buttons or ok,
     * can press Cancel though in this state
     */
    virtual void lockInput() = 0;
    /**
     * Can press number buttons, ok and cancel
     */ 
    virtual void unlockInput() = 0;
};

} //namespace Authentication
} //namespace iviLink

#endif //I_AUTHENTICATION_DIALOG_HPP