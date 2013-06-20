/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 22.04.2013
 *
 * View for displaying entered PIN.
 */

#import <QuartzCore/QuartzCore.h>
#import "EnteredPinView.h"
#include "Logger.hpp"

static Logger mLogger = Logger::getInstance("iOS::AuthenticationDialog::PinView");

@interface EnteredPinView()
{
    UIActivityIndicatorView * mSpinner;
    UILabel * mPinLabel;
    UIWindow * mWindow;
}
@end

@implementation EnteredPinView

- (id)initWithFrameAndPin: (CGRect)frame : (NSString*)pinText
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    self = [super initWithFrame:frame];
    if (self) {
        static const CGFloat spinnerSize = 30;
        self.backgroundColor = [UIColor colorWithRed:0
                                               green:0
                                                blue:0
                                               alpha:0.5];
        self.clipsToBounds = YES;
        self.layer.cornerRadius = 10.0;
        mSpinner = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
        mSpinner.frame = CGRectMake(self.frame.size.width * 0.1,
                                    (self.frame.size.height - spinnerSize)/2,
                                    spinnerSize,
                                    spinnerSize);
        
        mPinLabel = [[UILabel alloc] initWithFrame: CGRectMake(self.frame.size.width * 0.15 + spinnerSize,
                                                               self.frame.size.height * 0.25,
                                                               self.frame.size.width * 0.6,
                                                               self.frame.size.height * 0.5)];
        mPinLabel.backgroundColor = [UIColor clearColor];
        mPinLabel.textColor = [UIColor whiteColor];
        mPinLabel.numberOfLines = 0;
        mPinLabel.textAlignment = NSTextAlignmentCenter;
        mPinLabel.text = [@"PIN: " stringByAppendingString: pinText];
        
        [self addSubview: mSpinner];
        [self addSubview: mPinLabel];
    }
    return self;
}

- (void)show
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mWindow = [[UIWindow alloc] initWithFrame: [[UIScreen mainScreen] bounds]];
    [mWindow setWindowLevel: UIWindowLevelNormal];
    [mWindow setHidden: NO];
    [mWindow setBackgroundColor : [UIColor clearColor]];
    [mWindow makeKeyAndVisible];
    [mWindow addSubview: self];
    [mSpinner startAnimating];
}

- (void)hide
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    [mSpinner stopAnimating];
    [self removeFromSuperview];
    [mWindow setHidden: YES];
    mWindow = nil;
}

- (void)changeDisplayedText:(NSString *)newText
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    [mPinLabel setText: [@"PIN: " stringByAppendingString: newText]];
}

- (void)dealloc
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    [mPinLabel removeFromSuperview];
    mPinLabel = nil;
    [mSpinner removeFromSuperview];
    mSpinner = nil;
}
@end
