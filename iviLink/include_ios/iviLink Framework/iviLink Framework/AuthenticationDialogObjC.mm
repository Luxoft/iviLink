
/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 28.03.2013
 * Authentication UI implementation. This file has ARC disabled, so MRR should be done.
 */

#import <Foundation/Foundation.h>
#import <UIKit/UIAlertView.h>
#import "EnteredPinView.h"
#include "AuthenticationDialogStub.hpp"
#include "SystemControllerStub.hpp"

#include "Logger.hpp"

#define PIN_LENGTH 4

static Logger mLogger = Logger::getInstance("iOS::AuthenticaitonDialog");

using namespace iviLink::Authentication;

@interface AuthenticationDialogObjC : NSObject<UIAlertViewDelegate, UITextFieldDelegate>
{
    EnteredPinView * mPinView;
    UIAlertView * mAlertView;
    void * mNativeDialog; // pointer to AuthenticationDialogStub instance
}
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex;

- (BOOL)alertViewShouldEnableFirstOtherButton:(UIAlertView *)alertView;

- (BOOL)textField:(UITextField *)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string;

- (void) showNumPad;

- (void) showPin:(NSString*) pinText;

- (void) hidePin;

- (void) setNativeImpl: (void*) nativeImplementation;
@end

@implementation AuthenticationDialogObjC

- (void) showNumPad
{
    dispatch_async(dispatch_get_main_queue(), ^{
        LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
        UIAlertView * dialog = [[UIAlertView alloc] initWithTitle:@"iviLink Authentication"
                                                         message:@"Enter 4-digit PIN"
                                                        delegate:self
                                               cancelButtonTitle:@"Cancel"
                                               otherButtonTitles:@"OK", nil];
        
        dialog.alertViewStyle = UIAlertViewStylePlainTextInput;
        [[dialog textFieldAtIndex: 0] setKeyboardType:UIKeyboardTypeNumberPad];
        [[dialog textFieldAtIndex: 0] setTextAlignment:NSTextAlignmentCenter];
        [[dialog textFieldAtIndex: 0] setDelegate:self];
        [dialog show];
        [dialog release];
        mAlertView = dialog;
    });
}

- (void) showPin:(NSString *)pinText
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    dispatch_async(dispatch_get_main_queue(), ^{
        LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
        if ([mPinView superview] != nil) // is on screen
        {
            LOG4CPLUS_INFO(mLogger, "pin view was already on screen, updating text to: " + (std::string)[pinText UTF8String]);
            [mPinView changeDisplayedText: pinText];
            return;
        }
        LOG4CPLUS_INFO(mLogger, "pin view is not on screen");
        static const CGFloat screenWidth = [UIScreen mainScreen].bounds.size.width;
        static const CGFloat screenHeight = [UIScreen mainScreen].bounds.size.height;
        static const CGFloat popupHeight = 80;
        static const CGFloat popupWidth = 150;
        if (mPinView == nil) // window was not shown before
        {
            mPinView = [[EnteredPinView alloc] initWithFrameAndPin:CGRectMake(screenWidth/2.0 - popupWidth/2.0,
                                                                              screenHeight/2.0 - popupHeight/2.0,
                                                                              popupWidth,
                                                                              popupHeight) : pinText];
        }
        else // window was shown before, all objects are allocated, just changing label's text
        {
            LOG4CPLUS_TRACE_METHOD(mLogger, "pin view has already been created, updating text and showing");
            [mPinView changeDisplayedText: pinText];
        }
        LOG4CPLUS_INFO(mLogger, "showing pin view");
        [mPinView show];
    });
}

- (void) hidePin
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    dispatch_async(dispatch_get_main_queue(), ^{
        LOG4CPLUS_INFO(mLogger, "pin view hiding");
        [mPinView hide];
        [mAlertView dismissWithClickedButtonIndex:0 animated:NO];
        iviLink::SystemController::SystemControllerStub::getInstance()->onHidNumPad();
    });
}

- (void) dealloc
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    if (mPinView != nil)
    {
        LOG4CPLUS_TRACE_METHOD(mLogger, "pin view != nil");
        assert([mPinView superview] == nil);
        [mPinView release];
        mPinView = nil;
    }
    [super dealloc];
}


- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mAlertView = nil;
    if (!mNativeDialog)
    {
        LOG4CPLUS_WARN(mLogger, "!mNativeDialog");
        return;
    }
    if (buttonIndex == 0)
    {
        ((AuthenticationDialogStub*)mNativeDialog)->onCancelClicked();
    }
    else
    {
        std::string pin([[[alertView textFieldAtIndex: 0] text] UTF8String]);
        ((AuthenticationDialogStub*)mNativeDialog)->onOkClicked(pin);
    }
}

- (BOOL)textField:(UITextField *)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    NSUInteger newLength = [textField.text length] + [string length] - range.length;
    // cannot enter more than PIN_LENGTH symbols
    return newLength <= PIN_LENGTH;
}

- (BOOL)alertViewShouldEnableFirstOtherButton:(UIAlertView *)alertView
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    // can press ok only when PIN_LENGTH digits are entered
    return [[[alertView textFieldAtIndex: 0 ] text ] length ] == PIN_LENGTH;
}

- (void)setNativeImpl: (void*) nativeImplementation
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mNativeDialog = nativeImplementation;
}
@end

AuthenticationDialogStub::AuthenticationDialogStub(AuthenticationStateMachine * machine)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mMachine = machine;
    mImpl = (void*)[[AuthenticationDialogObjC alloc] init];
    [(id)mImpl setNativeImpl:(void*) this];
}

void AuthenticationDialogStub::showNumPad()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    [(id)mImpl showNumPad];
}

void AuthenticationDialogStub::onOkClicked(std::string const& pinLine)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mFilledPin = pinLine;
    UInt32 pinFull = 0;
    stringstream(pinLine) >> pinFull;
    int digits[PIN_LENGTH];
    for (int i = PIN_LENGTH - 1; i >= 0; i--)
    {
        div_t divresult = div(pinFull, 10);
        digits[i] = divresult.rem;
        pinFull = divresult.quot;
        LOG4CPLUS_DEBUG(mLogger, "PIN: digit[" + convertIntegerToString(i) + "] is " + convertIntegerToString(digits[i]));
    }
    if (mMachine != NULL)
    {
        mMachine->sendPin(digits[0], digits[1], digits[2], digits[3]);
    }
}

void AuthenticationDialogStub::onCancelClicked()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    if (mMachine != NULL)
    {
        mMachine ->cancelAuthentication();
    }
}

void AuthenticationDialogStub::lockInput()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    [(id)mImpl showPin:[NSString stringWithFormat: @"%s", mFilledPin.c_str()]];
}

AuthenticationDialogStub::~AuthenticationDialogStub()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    [(id)mImpl setNativeImpl: NULL];
    [(id)mImpl release];
}

void AuthenticationDialogStub::unlockInput()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    [(id)mImpl hidePin];
    [(id)mImpl showNumPad];
}

void AuthenticationDialogStub::hideNumPad()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    [(id)mImpl setNativeImpl: NULL];
    [(id)mImpl hidePin];
}
