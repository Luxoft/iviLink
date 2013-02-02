/* 
 * iviLINK SDK, version 1.2
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012-2013, Luxoft Professional Corp., member of IBS group
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; version 2.1.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * 
 */ 
#include <sys/time.h>

#include "IpcServer.hpp"
#include "IpcTestCommon.hpp"

using namespace iviLink::Ipc;

namespace iviLink
{
namespace Test
{

Logger IpcServer::mLogger = Logger::getInstance("utils.ipc.CIpc.test");

IpcServer::IpcServer(const std::string& connectionName, int clientsNumber)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mClientsNumber = clientsNumber;
    LOG4CPLUS_INFO(mLogger, "Clients number: " + convertIntegerToString(mClientsNumber));
    mIpc = new CIpc(connectionName, *this);
}

IpcServer::~IpcServer()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    delete mIpc;
}

void IpcServer::testCaseShutdownOnRequest()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mIpc->beginWaitForConnection();
    for (int i = 0; i < mClientsNumber; i++)
    {
        mConnectionSema.wait();
    }
    mIpc->endWaitForConnection();
    assert (mIpc->isConnected());
    assert (mClients.size() == mClientsNumber);
    LOG4CPLUS_INFO(mLogger, "Established connection with all expected clients: " + convertIntegerToString(mClientsNumber));
    if (mClientsNumber == 1)
    {
        sendPing();
        sendKill();
    }
    else
    {
        for (int i = 0; i < mClientsNumber; i++)
        {
            sendPing(&mClients.at(i));
        }
        for (int i = 0; i < mClientsNumber; i++)
        {
            sendKill(&mClients.at(i));
        }

    }
    LOG4CPLUS_INFO(mLogger, "Test passed successfully!");
}

void IpcServer::sendPing(iviLink::Ipc::DirectionID * pDirID)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    std::string request(TEST_REQUEST);
    std::string response(TEST_RESPONSE);
    UInt8 * writeBuf = new UInt8(request.size());
    memcpy(writeBuf, request.c_str(), request.size());
    UInt8 * readBuf = new UInt8(response.size());
    UInt32 respSize = response.size();
    BaseError err = mIpc->request(mMsgIdGen.getNext(), writeBuf, request.size(), readBuf, respSize, pDirID);
    assert (err.isNoError());
    LOG4CPLUS_INFO(mLogger, "Request sent successfully");
    assert (respSize == response.size());
    LOG4CPLUS_INFO(mLogger, "Response size is correct");
    assert (response == std::string((const char*)readBuf, respSize));
    LOG4CPLUS_INFO(mLogger, "Response content is correct");
    delete writeBuf;
    delete readBuf;
}

void IpcServer::sendKill(iviLink::Ipc::DirectionID * pDirID)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    std::string dieRequest(DIE_REQUEST);
    UInt8 * writeBuf = new UInt8(dieRequest.size());
    memcpy(writeBuf, dieRequest.c_str(), dieRequest.size());
    UInt8 * readBuf = new UInt8(1);
    UInt32 respSize = 1;
    timeval begin, end;
    double requestTime;
    gettimeofday(&begin, NULL);
    BaseError err = mIpc->request(mMsgIdGen.getNext(), writeBuf, dieRequest.size(), readBuf, respSize, pDirID);
    assert (!err.isNoError());
    LOG4CPLUS_INFO(mLogger, "Request returned an error as expected");
    assert (err.getCode() == CIpcError::ERROR_CONNECTION_LOST);
    LOG4CPLUS_INFO(mLogger, "Request returned an error and the error code is as expected");
    gettimeofday(&end, NULL);
    requestTime = (end.tv_sec - begin.tv_sec) * 1000.0;      // sec to ms
    requestTime += (end.tv_usec - begin.tv_usec) / 1000.0;   // us to ms
    std::ostringstream s;
    s << "(" << requestTime << ") ms";
    LOG4CPLUS_INFO(mLogger, "Time elapsed requesting: " + s.str());
    assert (requestTime < 100);
    LOG4CPLUS_INFO(mLogger, "Request's time is within the expected limits");
    delete writeBuf;
    delete readBuf;
}

void IpcServer::OnConnection(iviLink::Ipc::DirectionID dirId)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mClients.push_back(dirId);
    mConnectionSema.signal();
}

void IpcServer::OnConnectionLost(iviLink::Ipc::DirectionID dirID)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "Lost connection with: " + convertIntegerToString(dirID));
}

void IpcServer::OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
            UInt8* const pResponseBuffer, UInt32& bufferSize, iviLink::Ipc::DirectionID)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    assert (false);
}

void IpcServer::OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
            iviLink::Ipc::DirectionID)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    assert (false);
}

}
}


int main (int argc, char ** argv)
{
    PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));
    int clientsNumber = 1;
    if (argc > 1)
    {
        clientsNumber = atoi(argv[1]);
    }
    iviLink::Test::IpcServer * mServ = new iviLink::Test::IpcServer(ADDRESS, clientsNumber);
    mServ->testCaseShutdownOnRequest();
    delete mServ;
}
