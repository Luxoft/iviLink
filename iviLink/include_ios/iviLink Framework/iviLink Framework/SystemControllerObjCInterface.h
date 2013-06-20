
/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 21.03.2013
 *
 * See SystemController.mm for implementations.
 */


#ifndef iviLinkCoreStack_SystemControllerObjCInterface_h
#define iviLinkCoreStack_SystemControllerObjCInterface_h

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus
    
void startConnectivityAgent();
void startChannelSupervisor();
void startPMP();
const char* getFilePath();
        
#ifdef __cplusplus
}
#endif // __cplusplus


#endif
