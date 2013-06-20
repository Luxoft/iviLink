
/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 21.03.2013
 *
 * Connects iOS CIpcSockets with each other.
 */


#ifdef __APPLE__

#ifndef __iviLinkCoreStack__IpcSocketDispatcher__
#define __iviLinkCoreStack__IpcSocketDispatcher__

#include <map>
#include <set>
#include "IDispatcherClient.hpp"

namespace iviLink
{
    namespace Ipc
    {
        
        typedef std::set<IDispatcherClient*> tClientsSet;
        typedef std::map<Address, IDispatcherClient*> tListenersMap;
        
        class IpcSocketDispatcher
        {
            
            static Logger mLogger;
        private:
            IpcSocketDispatcher() {}
        public:
            static IpcSocketDispatcher * getInstance();
            /**
             * @return false if another listener is already bound to the same address
             */
            bool registerListener(IDispatcherClient *);
            
            void unregisterListener(IDispatcherClient *);
            
            /**
             * @return a PipeWriter that can be used to send data to the server 
             * (an empty shared pointer is returned if no server was found)
             */
            tPipeWriterPtr connectToServer(IDispatcherClient *, DirectionID& receivedDirectionID);
            
            void closeConnection(IDispatcherClient *);
            
        private:
            void insertClient(IDispatcherClient *);
        private:
            static IpcSocketDispatcher * mInstance;
            static CMutex mSingletonMutex;
            CMutex mClientsSetMutex;
            tClientsSet mClients;
            CMutex mListenersMutex;
            tListenersMap mListeners;
        };
    }
}

#endif /* defined(__iviLinkCoreStack__IpcSocketDispatcher__) */

#endif 
