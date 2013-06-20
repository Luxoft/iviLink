
/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 21.03.2013
 */


#ifdef __APPLE__

#include <cassert>

#include "IpcSocketDispatcher.hpp"

namespace iviLink
{
    namespace Ipc
    {
        
        Logger IpcSocketDispatcher::mLogger = Logger::getInstance("IpcSocketDispatcher");
        IpcSocketDispatcher* IpcSocketDispatcher::mInstance = NULL;
        CMutex IpcSocketDispatcher::mSingletonMutex;
        
        IpcSocketDispatcher * IpcSocketDispatcher::getInstance()
        {
            MutexLocker lock(mSingletonMutex);
            if (mInstance == NULL)
            {
                mInstance = new IpcSocketDispatcher();
            }
            return mInstance;
        }
        
        bool IpcSocketDispatcher::registerListener(IDispatcherClient * client)
        {
            LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
            LOG4CPLUS_INFO(mLogger, "registering listener to addr: " + client->getAddress());
            insertClient(client);
            MutexLocker lock(mListenersMutex);
            tListenersMap::iterator iter = mListeners.find(client->getAddress());
            if (iter != mListeners.end())
            {
                return false;
            }
            mListeners[client->getAddress()] = client;
            return true;
        }
        
        void IpcSocketDispatcher::unregisterListener(IDispatcherClient * client)
        {
            LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
            MutexLocker lock(mListenersMutex);
            tListenersMap::iterator iter = mListeners.find(client->getAddress());
            if (iter != mListeners.end() && (*iter).second == client)
            {
                LOG4CPLUS_INFO(mLogger, "erasing listener for addr: " + client->getAddress());
                mListeners.erase(iter);
            }
        }
        
        tPipeWriterPtr IpcSocketDispatcher::connectToServer(IDispatcherClient * client, DirectionID& receivedDirectionID)
        {
            LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
            LOG4CPLUS_INFO(mLogger, "trying to connect to addr: " + client->getAddress());
            receivedDirectionID = -1;
            insertClient(client);
            IDispatcherClient * server = NULL;
            {
                MutexLocker lock(mListenersMutex);
                tListenersMap::iterator iter = mListeners.find(client->getAddress());
                if (iter != mListeners.end())
                {
                    LOG4CPLUS_INFO(mLogger, "Found server for address: " + client->getAddress());
                    server = (*iter).second;
                }
            }
            if (server == NULL)
            {
                return tPipeWriterPtr();
            }
            receivedDirectionID = server->onClientConnected(client);
            return server->getPipeWriter();
        }
        
        void IpcSocketDispatcher::closeConnection(IDispatcherClient * client)
        {
            LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
            MutexLocker lock(mClientsSetMutex);
            tClientsSet::iterator findIt = mClients.find(client);
            if (findIt == mClients.end())
            {
                LOG4CPLUS_WARN(mLogger, "Client not registered!");
                return;
            }
            mClients.erase(findIt);
            tClientsSet::iterator iter = mClients.begin();
            for (; iter != mClients.end(); iter ++)
            {
                if ((*iter)->getAddress() == client->getAddress())
                {
                    (*iter)->onClientDisconnectedFromDispatcher(client);
                }
            }
        }
        
        void IpcSocketDispatcher::insertClient(IDispatcherClient * client)
        {
            LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
            MutexLocker lock(mClientsSetMutex);
            mClients.insert(client);
        }
    }    
}

#endif