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


#ifndef CIPCPROTOCOL_HPP_
#define CIPCPROTOCOL_HPP_

#include <string>

#include "IPmpIpcToCore.hpp"
#include "IPmpIpcToPim.hpp"
#include "IPmpPimForIpc.hpp"
#include "IPmpCoreForIpc.hpp"

#include "ICallbackHandler.hpp"

#include "Logger.hpp"
#include "BaseError.hpp"

#include "CBufferManager.hpp"

namespace iviLink
{
namespace Ipc
{

class CIpc;

}  // namespace Ipc

namespace ProfileManager{
   namespace Ipc {

struct PmMessage;

}  // namespace Ipc
}

namespace PMP {

namespace Ipc {

class PmpIpcProtocol :
    public iviLink::Ipc::ICallbackHandler
{
public:

    // from IPMPIpcToPIM

    virtual bool incomingProfile(iviLink::BaseUid const& profileUid,
            iviLink::Profile::ApiUid const& papiUid,
            iviLink::Profile::IUid const& piuid,
            iviLink::Service::Uid const& serviceUID,
            iviLink::Ipc::DirectionID const& dirId);

    virtual bool profileDied(iviLink::Profile::IUid const& piuid,
            iviLink::Ipc::DirectionID const& dirId);

    virtual bool generatePIUidResponse(const iviLink::Profile::IUid& piuid,
            iviLink::Ipc::DirectionID const& dirId);

    virtual bool getManifestResponse(const std::string & manifest,
            iviLink::Ipc::DirectionID const& dirId);

    virtual bool getProfileLibPathResponse(const std::string & path,
            iviLink::Ipc::DirectionID const& dirId);

    virtual bool findProfilesResponse(const std::list<iviLink::Profile::Uid> & profiles,
            iviLink::Ipc::DirectionID const& dirId);

    explicit PmpIpcProtocol();
    virtual ~PmpIpcProtocol();

    bool connect();

    bool isConnected() const;

    void init(IPmpPimForIpc* pPim, IPmpCoreForIpc* pCore, char const* pPmpAddress = NULL);

protected:
    // from ICallbackHandler

    virtual void OnConnection(iviLink::Ipc::DirectionID);

    virtual void OnConnectionLost(iviLink::Ipc::DirectionID);

    virtual void OnRequest(iviLink::Ipc::MsgID id,
            UInt8 const* pPayload, UInt32 payloadSize,
            UInt8* const pResponseBuffer, UInt32& bufferSize,
            iviLink::Ipc::DirectionID);

    virtual void OnAsyncRequest(iviLink::Ipc::MsgID id,
            UInt8 const* pPayload, UInt32 payloadSize,
            iviLink::Ipc::DirectionID dirId);


    private:

    void processGeneratePIUIDRequest     (iviLink::Ipc::DirectionID const& dirId);
    void processReleasePIUIDRequest      (ProfileManager::Ipc::PmMessage const* const req, iviLink::Ipc::DirectionID const& dirId);
    void processCreateProfileRequest     (ProfileManager::Ipc::PmMessage const* const req, iviLink::Ipc::DirectionID const& dirId);
    void processProfileDiedRequest       (ProfileManager::Ipc::PmMessage const* const req, iviLink::Ipc::DirectionID const& dirId);
    void processDisableByUidRequest      (ProfileManager::Ipc::PmMessage const* const req, iviLink::Ipc::DirectionID const& dirId);
    void processEnableByUidRequest       (ProfileManager::Ipc::PmMessage const* const req, iviLink::Ipc::DirectionID const& dirId);
    void processEnableAllRequest         (ProfileManager::Ipc::PmMessage const* const req, iviLink::Ipc::DirectionID const& dirId);
    void processGetManifestRequest       (ProfileManager::Ipc::PmMessage const* const req, iviLink::Ipc::DirectionID const& dirId);
    void processGetProfileLibPathRequest(ProfileManager::Ipc::PmMessage const* const req, iviLink::Ipc::DirectionID const& dirId);
    void processFindProfilesRequest      (ProfileManager::Ipc::PmMessage const* const req, iviLink::Ipc::DirectionID const& dirId);
    void processReadyToServeRequest      (ProfileManager::Ipc::PmMessage const* const req, iviLink::Ipc::DirectionID const& dirId);

private:

    class MsgIdGen
    {
    public:
        MsgIdGen();
        ~MsgIdGen();

        iviLink::Ipc::MsgID getNext();
    private:
        iviLink::Ipc::MsgID mId;
    };

    MsgIdGen mMsgIdGen;
    iviLink::Ipc::CIpc * mIpc;
    iviLink::Ipc::Helpers::CBufferManager mReadBufMgr;
    iviLink::Ipc::Helpers::CBufferManager mWriteBufMgr;
    IPmpPimForIpc * mPim;
    IPmpCoreForIpc * mCore;

    static Logger msLogger;
};

}  // namespace Ipc
}  // namespace ProfileManager
}  // namespace iviLink


#endif /* CIPCPROTOCOL_HPP_ */
