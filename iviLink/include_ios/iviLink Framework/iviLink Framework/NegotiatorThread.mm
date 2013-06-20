
/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 21.03.2013
 */


#import "NegotiatorThread.h"
#include "Negotiator.hpp"
#include "NegotiatorIPCHandler.hpp"
#include "NegotiatorConstants.hpp"

using namespace iviLink::ChannelSupervisor;

@implementation NegotiatorThread

- (void) main
{
    Negotiator * negotiator = new Negotiator(std::string(IPC_NEGOTIATOR_ADDRESS));
    negotiator->init();
    negotiator->waitShutdown();
    delete negotiator;
}
@end
