/* 
 * iviLINK SDK, version 1.1.19
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012, Luxoft Professional Corp., member of IBS group
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
 
#ifndef __CPROFILE_IMPLEMENTATION_HPP
#define __CPROFILE_IMPLEMENTATION_HPP


#include <string>
#include <jni.h>

#include "NonnativeCallbacksWrapper.hpp"
#include "Logger.hpp"
#include "profileLib.hpp"

using iviLink::CBuffer;
using iviLink::Channel::tChannelId;

class CProfileImplementation: public iviLink::Channel::CChannelHandler,
							  public iviLink::CProfileApiBase
{
public:
	virtual std::string getName() const;
   	virtual UInt32 getVersion() const;
   	virtual iviLink::Profile::Uid getUid() const;
    virtual iviLink::Profile::ApiUid getProfileApiUid() const;

   	CProfileImplementation(JavaVM* jm, jobject profileExternal, iviLink::Profile::IProfileCallbackProxy* pCbProxy);
   	virtual ~CProfileImplementation();
      
   	void sendData(char* data, int sizeOfData, UInt32 channelID);

   	jobject getProfileCallbacks();

      jobject getExternalProfile();

   	UInt32 allocateChannel(std::string tag);

      UInt32 allocateChannelAsServer(std::string tag);
      
      UInt32 allocateChannelAsClient(std::string tag);

   	void deallocateChannel(UInt32 channelID);

   	// CChannelHandler
      virtual void onBufferOverflow(const iviLink::Channel::tChannelId channelId);
   	virtual void onBufferReceived(const tChannelId channel, CBuffer const& buffer);

   	virtual void onChannelDeleted(const UInt32 channel_id);

   	virtual void onConnectionLost();

   	virtual void onEnable();

   	virtual void onDisable();
   
private:
   	//Pointer to instance of class describing callbacks
   	NonnativeCallbacksWrapper* mpAppCallbacks;
   	Logger mLogger;

   	JavaVM * mpJM;
   	jobject mProfileExternal;

   	jmethodID midReceiveData;
      jmethodID midOnEnable;
      jmethodID midOnDisable;
      jmethodID midOnConnectionLost;
   	jmethodID midChannelDeleted;

      jmethodID midGetName;
      jmethodID midGetVersion;
      jmethodID midGetUID;
      jmethodID midGetProfileAPIUID;

      jmethodID midBufferOverflow;
};

#endif //ANDROID
