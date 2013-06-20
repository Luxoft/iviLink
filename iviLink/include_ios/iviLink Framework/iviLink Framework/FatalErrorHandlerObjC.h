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

#import <Foundation/Foundation.h>

@interface FatalErrorHandlerObjC : NSObject

+ (void) showDialogAndDie: (const char*) message;

@end
