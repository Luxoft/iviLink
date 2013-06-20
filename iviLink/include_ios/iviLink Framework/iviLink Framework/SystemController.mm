
/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 21.03.2013
 */


#import "SystemController.h"
#import "NegotiatorThread.h"
#import "L1Thread.h"
#import "ProfileManagerThread.h"

#include "SystemControllerObjCInterface.h"
#include "SystemControllerStub.hpp"

static NegotiatorThread * negotiator = nil;
static L1Thread * connectivityAgent = nil;
static ProfileManagerThread * pmp = nil;

@implementation SystemController

+ (void) startStack
{
    iviLink::SystemController::SystemControllerStub::getInstance()->startStack();
}


void startConnectivityAgent()
{
    connectivityAgent = [[L1Thread alloc] init];
    [connectivityAgent start];
}

void startChannelSupervisor()
{
    negotiator = [[NegotiatorThread alloc] init];
    [negotiator start];
}

void startPMP()
{
    pmp = [[ProfileManagerThread alloc] init];
    [pmp start];
}


const char* getFilePath()
{
    std::string pathStd([NSHomeDirectory() UTF8String]);
    pathStd += "/Documents/";
    return pathStd.c_str();
}

@end
