/* 
 * 
 * iviLINK SDK, version 1.1.2
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
 * 
 */

#include "framework/components/SystemController/android/jni/src/com_luxoft_ivilink_sdk_CChannelSupervisorWrapper.hpp"
#include "utils/misc/Logger.hpp"
#include "utils/misc/Types.hpp"

#include "framework/components/ChannelSupervisor/NegotiatorProcess/private/NegotiatorProcess.hpp"
#include "framework/components/ChannelSupervisor/NegotiatorProcess/private/NegotiatorStates.hpp"
#include "framework/components/ChannelSupervisor/NegotiatorProcess/private/Negotiator.hpp"

using namespace iviLink::ChannelSupervisor;

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_sdk_CChannelSupervisorWrapper_startCS
(JNIEnv *, jobject)
{
	Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("NegotiatorProcess.main"));
	LOG4CPLUS_INFO(logger, "Negotiator Process started...");
	std::string cs_sock_addr;
	Negotiator* negotiator = new Negotiator(cs_sock_addr);
	NegotiatorIPCHandler * ipcHandler = negotiator->getHandler();
	ipcHandler->IPCConnect();
	ipcHandler->IPCWait();
	LOG4CPLUS_INFO(logger, "Negotiator Process ends...");
	ipcHandler->IPCDisconnect();
	delete negotiator;
}
