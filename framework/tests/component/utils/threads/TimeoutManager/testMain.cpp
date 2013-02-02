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


#include "utils/threads/CTimeoutManager.hpp"
#include "utils/threads/CSignalSemaphore.hpp"
#include <iostream>
#include <cassert>

volatile int gCount = 1;
CSignalSemaphore gSem;

class CSubscriber : public ITimeoutSubscriber
{
public:
   CSubscriber(const std::string& name, int number)
      : mName(name)
      , mNumber(number)
   {
   }

private:
   virtual void onTimeout()
   {
      std::cout << "Now (ms): " << CTimeoutManager::getCurMs() << std::endl;
      std::cout << mName << " " << mNumber<< std::endl << std::flush;
      assert(gCount == mNumber);
      ++gCount;
      if (4 == mNumber)
      {
//         gSem.signal();
      }
   }

   std::string mName;
   int mNumber;
};

int main(int, char **)
{
   Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("utils.Test"));

   PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));
   LOG4CPLUS_ERROR(logger, "Test start");

   CTimeoutManager * pInstance = CTimeoutManager::getInstance();

   pInstance->addSubscriber(new CSubscriber("\n### three ###\n", 3), 500);
   pInstance->addSubscriber(new CSubscriber("\n### one ###\n", 1), 450);

   CSubscriber * pSub = new CSubscriber("\n### empty ###\n", -10);
   pInstance->addSubscriber(pSub, 500);
   pInstance->addSubscriber(new CSubscriber("\n### four ###\n", 4), 900);
   pInstance->removeSubscriber(pSub);
   pInstance->addSubscriber(new CSubscriber("\n### two ###\n", 2), 450);

   gSem.waitTimeout(2000);

   if (5 == gCount)
   {
      LOG4CPLUS_ERROR(logger, "Test passed!\n OK OK OK OK OK OK OK OK OK OK OK OK");
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "Test failed, gCount = " + convertIntegerToString(gCount));
   }

   CTimeoutManager::deleteInstance();
   CTimeoutManager::getInstance(); // check quick deleteing of CTimeoutManager instance
   CTimeoutManager::deleteInstance();
}

