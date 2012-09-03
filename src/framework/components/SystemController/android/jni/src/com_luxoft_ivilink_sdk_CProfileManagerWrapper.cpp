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

#include "framework/components/SystemController/android/jni/src/com_luxoft_ivilink_sdk_CProfileManagerWrapper.hpp"
#include "framework/components/ProfileManager/PMP/componentManager/CPmpComponentManager.hpp"
#include "framework/components/ProfileManager/PMP/process/SystemControllerMsg.hpp"
#include "framework/libraries/AppMan/Pmp/CAppManPmpController.hpp"

#include "utils/threads/CSignalSemaphore.hpp"
#include "utils/configurator/configurator.h"

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_sdk_CProfileManagerWrapper_startPM
  (JNIEnv *, jobject)
{
   iviLink::conf::Configurator config("");
   CSignalSemaphore sem;
#if !defined(NO_SYSTEM_CONTROLLER_LAUNCH)
   iviLink::PMP::SystemControllerMsg sysCtrl(&sem);
#endif // NO_SYSTEM_CONTROLLER_LAUNCH
   iviLink::PMP::CPmpComponentManager::instance(&config);
   while (true)
   {
      iviLink::AppMan::Ipc::CAppManPmpController * appctrl = iviLink::AppMan::Ipc::CAppManPmpController::instance();
      if (!appctrl)
         return;
      if (appctrl->checkConnection())
         break;
      if (0 == sem.tryWait())
         return;

      // waiting for app man connection
      sleep(1);
   }
   sysCtrl.requestConnected();
   sem.wait();
}
