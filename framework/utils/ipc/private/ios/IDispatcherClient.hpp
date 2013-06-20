/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 21.03.2013
 */

#ifdef __APPLE__
#ifndef iviLinkCoreStack_IDispatcherClient_h
#define iviLinkCoreStack_IDispatcherClient_h

#include "PipeWriter.hpp"
#include "ipc_common.hpp"

#define IOS_IPC_BUF_SIZE 5000

namespace iviLink
{
    namespace Ipc
    {
        class IDispatcherClient
        {
        public:
            virtual ~IDispatcherClient(){}
            /**
             * will be called when a client with the same address appears and this instance was registered as a listener
             * @return direction id given to the client
             */
            virtual DirectionID onClientConnected(IDispatcherClient * client) = 0;
            /**
             * broadcast notification to all clients with same address
             * @param client client that has closed its connections
             */
            virtual void onClientDisconnectedFromDispatcher(IDispatcherClient* client) = 0;
            virtual tPipeWriterPtr getPipeWriter() const = 0;
            virtual Address getAddress() const = 0;
        };
    }
}

#endif

#endif