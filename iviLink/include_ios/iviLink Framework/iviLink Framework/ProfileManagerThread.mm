
/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 21.03.2013
 */

#import "ProfileManagerThread.h"
#include "PmpCommunicator.hpp"

@implementation ProfileManagerThread

- (void) main
{
    iviLink::PMP::PmpCommunicator::getInstance()->startCommunication();
}
@end
