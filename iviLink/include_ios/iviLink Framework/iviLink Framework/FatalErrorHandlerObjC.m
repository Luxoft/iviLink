/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 25.03.2013
 *
 * Can be used to display an alert dialog in case of an irrecoverable error.
 * Process will be exited after showDialogAndDie call.
 */

#import <UIKit/UIAlertView.h>

#import "FatalErrorHandlerObjC.h"
#include "FatalErrorHandler.h"

@interface FatalErrorDialogDelegate : NSObject<UIAlertViewDelegate>
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex;
@end

@implementation FatalErrorDialogDelegate
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    exit(0);
}
@end

static FatalErrorDialogDelegate * delegate;

@implementation FatalErrorHandlerObjC

+ (void) showDialogAndDie:(const char *)message
{
    delegate = [[FatalErrorDialogDelegate alloc] init];
    dispatch_async(dispatch_get_main_queue(), ^{
        UIAlertView * alert = [[UIAlertView alloc] initWithTitle:@"iviLink"
                                                         message:[NSString stringWithFormat:@"%s", message]
                                                        delegate:delegate
                                               cancelButtonTitle:@"Exit"
                                               otherButtonTitles: nil];
        [alert show];
    });
}

void showDialogAndDie(const char* message)
{
    [FatalErrorHandlerObjC showDialogAndDie:message];
}

@end
