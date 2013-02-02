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
#include "IpcClient.hpp"
#include "IpcTestCommon.hpp"

using namespace iviLink::Ipc;

namespace iviLink
{
namespace Test
{

Logger IpcClient::mLogger = Logger::getInstance("utils.ipc.CIpc.test");

IpcClient::IpcClient(const std::string& connectionName)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mIpc = new CIpc(connectionName, *this);
}

IpcClient::~IpcClient()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    delete mIpc;
}

void IpcClient::testCaseShutdownOnRequest()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    BaseError err = mIpc->connect();
    assert (err.isNoError());
    mSema.wait();
}

void IpcClient::OnConnection(iviLink::Ipc::DirectionID)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
}

void IpcClient::OnConnectionLost(iviLink::Ipc::DirectionID)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    assert (false);
}

void IpcClient::OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
            UInt8* const pResponseBuffer, UInt32& bufferSize, iviLink::Ipc::DirectionID)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    std::string request((const char*)pPayload, payloadSize);
    LOG4CPLUS_INFO(mLogger, "Got request: " + request);
    if (request == std::string(TEST_REQUEST))
    {
        static std::string response(TEST_RESPONSE);
        memcpy(pResponseBuffer, response.c_str(), response.size());
        bufferSize = response.size();
    } 
    else if (request == std::string(DIE_REQUEST))
    {
        LOG4CPLUS_INFO(mLogger, "Exiting on synchro request!");
        exit(0);
    }
    else 
    {
        LOG4CPLUS_WARN(mLogger, "Unknown request!");
        assert(false);
    }
}

void IpcClient::OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
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
    iviLink::Test::IpcClient * mClient = new iviLink::Test::IpcClient(ADDRESS);
    mClient->testCaseShutdownOnRequest();
    delete mClient;
}
