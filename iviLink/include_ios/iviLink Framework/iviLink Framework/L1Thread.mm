
/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 21.03.2013
 */

#import "L1Thread.h"

#include "L1InterfaceStub.hpp"
#include "ConnectivityAgentConfig.hpp"

@implementation L1Thread

- (void) main
{
    iviLink::ConnectivityAgent::L1::L1InterfaceStub::getInstance()->start(false, first_lsn_path);
}

@end
