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
 * See FatalErrorHandlerObjC.m for implementation.
 */

#ifndef iviLinkCoreStack_FatalErrorHandler_h
#define iviLinkCoreStack_FatalErrorHandler_h

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
void showDialogAndDie(const char* message);
#ifdef __cplusplus
}
#endif // __cplusplus
#endif
