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

#import <UIKit/UIKit.h>

@interface EnteredPinView : UIView
- (id)initWithFrameAndPin: (CGRect)frame : (NSString*)pinText;
- (void)show;
- (void)hide;
- (void)changeDisplayedText: (NSString*)newText;
@end
