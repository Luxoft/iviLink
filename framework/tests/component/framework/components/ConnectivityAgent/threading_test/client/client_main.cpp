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


#include <cassert>
#include <cstring>
#include <cstdlib>
#include <getopt.h>
#include <string>
#include <tr1/array>
#include <limits.h>

#include "utils/misc/Logger.hpp"
#include "utils/threads/CSignalSemaphore.hpp"
#include "utils/threads/CSignalSemaphoreInterproc.hpp"
#include "utils/threads/CMutex.hpp"
#include "utils/threads/CThreadPool.hpp"


#include "framework/components/ConnectivityAgent/generic/common/IChannelObserver.hpp"
#include "framework/components/ConnectivityAgent/generic/common/API.hpp"

#define ASSERT_CERROR(err)                              \
   do { if (!err.isNoError()) {                         \
      LOG4CPLUS(msLogger, static_cast<std::string>(err)); \
      assert(err.isNoError());                          \
   } } while(false)


class Test : public IChannelObserver
{
public:

   virtual void dataReceivedCallback(const UInt32 channel_id, const UInt32 read_size)//, const UInt32 free_size)
   {
      LOG4CPLUS_TRACE(msLogger, std::string("Test::bufferOverflowCallback ") + 
         "channel_id = " + convertIntegerToString(channel_id) +
         "read_size = "  + convertIntegerToString(read_size));
//         "free_size = "  + convertIntegerToString(free_size));
   }

   virtual void bufferOverflowCallback(const UInt32 channel_id)
   {
      LOG4CPLUS_TRACE(msLogger, "Test::bufferOverflowCallback channel_id = " + convertIntegerToString(channel_id));
   }

   virtual void channelDeletedCallback(const UInt32 channel_id)
   {
      LOG4CPLUS_TRACE(msLogger, "Test::channelDeletedCallback channel_id = " + convertIntegerToString(channel_id));
   }

   virtual void connectionLostCallback()
   {
      mConnLostSem.signal();
   }

public:
   
   virtual void go() = 0;

public:

   Test()
   {
      sync_test("Test()");
   }

   ~Test()
   {
      sync_test("~Test()");
   }

   static UInt32 allocChannel(TChannelPriority prio, IChannelObserver* pObserver)
   {
      UInt32 chid = nextChannel();
      ERROR_CODE err = ::allocateChannel(prio, chid, pObserver);
      if (err != ERR_OK)
      {
         LOG4CPLUS_ERROR(msLogger, "allocateChannel err = " + convertIntegerToString((int)err));
         assert(err != ERR_OK);
      }

      return chid;
   }

   static void closeChannel(UInt32 chid)
   {
      ERROR_CODE err = ::closeChannel(chid);
      if (err != ERR_OK)
      {
         LOG4CPLUS_ERROR(msLogger, "closeChannel err = " + convertIntegerToString((int)err));
         assert(err != ERR_OK);
      }
   }

   static UInt32 nextChannel()
   {
      mChannelCounterMutex.lock();
      UInt32 res = mChannelCounter++;
      mChannelCounterMutex.unlock();
      return res;
   }

   static void sync_test(const char* msg = "")
   {
      LOG4CPLUS_INFO(msLogger, "Test BEFORE sync_test: " + std::string(msg));
      
      mpCliSyncSem->signal();
      mpSrvSyncSem->wait();

      LOG4CPLUS_INFO(msLogger, "Test AFTER  sync_test: " + std::string(msg));
   }

public:

   CSignalSemaphore                          mConnLostSem;

   static UInt32                             mChannelCounter;
   static CMutex                             mChannelCounterMutex;
   static CSignalSemaphoreInterproc*         mpSrvSyncSem;
   static CSignalSemaphoreInterproc*         mpCliSyncSem;

private:
   static Logger msLogger;
};

class MultipleOpenCloseTest : public Test
{
   static Logger msLogger;
public:

   int max_count;
   MultipleOpenCloseTest() :
      max_count(10)
   {

   }

   virtual void go()
   {
      LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

      for (int i = 0; i < max_count; ++i)
      {
         UInt32 chid = allocChannel(ePlainData, this);
         sync_test("MultipleOpenCloseTest alloc sync");
         closeChannel(chid);
         sync_test("MultipleOpenCloseTest close sync");
      }
   }
};

class MultipleOpenCloseNoSyncTest : public Test
{
   static Logger msLogger;
public:

   int max_count;
   MultipleOpenCloseNoSyncTest() :
      max_count(10)
   {

   }

   virtual void go()
   {
      LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

      for (int i = 0; i < max_count; ++i)
      {
         UInt32 chid = allocChannel(ePlainData, this);
         closeChannel(chid);
      }
   }
};

class MultipleCloseUnallocatedChannelNoSyncTest : public Test
{
   static Logger msLogger;
public:

   int max_count;
   MultipleCloseUnallocatedChannelNoSyncTest() :
      max_count(10)
   {

   }

   virtual void go()
   {
      LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

      for (int i = 0; i < max_count; ++i)
      {
         UInt32 chid = rand() % UINT_MAX + 1;
         closeChannel(chid);
      }
   }
};

class MultipleOpenCloseMultithreadTest : public Test
{
   static Logger msLogger;
public:

   const UInt32 count;
   CThreadPool* mpPool;   

   MultipleOpenCloseMultithreadTest() :
      count(20),
      mpPool(NULL)
   {

   }

   struct JobBarrierData : public IThreadPoolJobData
   {
      pthread_barrier_t* pBar;
      CSignalSemaphore* pSem;
   };

   static void job(IThreadPoolJobData const* pData)
   {
      struct Observer : public IChannelObserver
      {
         virtual void dataReceivedCallback(const UInt32 channel_id, const UInt32 read_size)//, const UInt32 free_size)
         {
            LOG4CPLUS_TRACE(msLogger, std::string("Observer::bufferOverflowCallback ") + 
               "channel_id = " + convertIntegerToString(channel_id) +
               "read_size = "  + convertIntegerToString(read_size));// +
//               "free_size = "  + convertIntegerToString(free_size));
         }

         virtual void bufferOverflowCallback(const UInt32 channel_id)
         {
            LOG4CPLUS_TRACE(msLogger, "Observer::bufferOverflowCallback channel_id = " + convertIntegerToString(channel_id));
         }

         virtual void channelDeletedCallback(const UInt32 channel_id)
         {
            LOG4CPLUS_TRACE(msLogger, "Observer::channelDeletedCallback channel_id = " + convertIntegerToString(channel_id));
         }

         virtual void connectionLostCallback()
         {
            LOG4CPLUS_TRACE(msLogger, "Observer::connectionLostCallback");
         }
      };
      Observer obs;

      LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

      JobBarrierData const* jdb = static_cast<JobBarrierData const*>(pData);

      LOG4CPLUS_INFO(msLogger, "job begin, waiting");
      int res = pthread_barrier_wait(jdb->pBar);

      LOG4CPLUS_INFO(msLogger, "job wait done, allocating");

      UInt32 chid = allocChannel(ePlainData, &obs);
      closeChannel(chid);

      LOG4CPLUS_INFO(msLogger, "job close done, waiting");
      res = pthread_barrier_wait(jdb->pBar);
      if (res == PTHREAD_BARRIER_SERIAL_THREAD)
      {
         jdb->pSem->signal();
         LOG4CPLUS_INFO(msLogger, "signaling to main thread");
      }
      LOG4CPLUS_INFO(msLogger, "job done");
   }

   virtual void go()
   {
      LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

      pthread_barrier_t bar;
      if (int err = pthread_barrier_init(&bar, NULL, count))
      {
         LOG4CPLUS_ERROR(msLogger, "pthread_barrier_init err = " + convertIntegerToString(err) + strerror(err));
         assert(false && "pthread_barrier_init err");
      }

      mpPool = new CThreadPool(count);
      CSignalSemaphore sem;

      for (UInt32 i = 0; i < count; ++i)
      {
         JobBarrierData* jbd = new JobBarrierData;
         jbd->pBar = &bar;
         jbd->pSem = &sem;
         CThreadPoolJob job(MultipleOpenCloseMultithreadTest::job, jbd);
         mpPool->addJob(job);
      }
      
      LOG4CPLUS_INFO(msLogger, "sem wait");
      sem.wait();
      LOG4CPLUS_INFO(msLogger, "sem wait done");

      delete mpPool;
   }
};

class DataLostAfterChannelAlloc : public Test
{
   static Logger msLogger;
public:

   int max_count;


   DataLostAfterChannelAlloc() : 
      max_count(1000)
   {

   }

   virtual void go()
   {
      LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

      UInt8 data[1] = { 42 };

      for (int i = 0; i < max_count; ++i)
      {
         UInt32 chid = allocChannel(ePlainData, this);
         //sync_test("DataLostAfterChannelAlloc alloc sync");

         ERROR_CODE err = sendData(chid, data, 1);
         assert(ERR_OK == err && "sendData error");
         // if (ERR_OK != err)
         // {
         //    LOG4CPLUS_WARN(msLogger, "sendData err = " + covertIntegerToString((int)err));
         // }

         sync_test("DataLostAfterChannelAlloc data sync");

         closeChannel(chid);
         sync_test("DataLostAfterChannelAlloc close sync");
      }
   }

};


UInt32 Test::mChannelCounter = 1;
CMutex Test::mChannelCounterMutex;
CSignalSemaphoreInterproc* Test::mpCliSyncSem = NULL;
CSignalSemaphoreInterproc* Test::mpSrvSyncSem = NULL;
Logger Test::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.test.threading_test.client.Test"));
Logger MultipleOpenCloseTest::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.test.threading_test.client.MultipleOpenCloseTest"));
Logger MultipleOpenCloseNoSyncTest::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.test.threading_test.client.MultipleOpenCloseNoSyncTest"));
Logger MultipleOpenCloseMultithreadTest::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.test.threading_test.client.MultipleOpenCloseMultithreadTest"));
Logger DataLostAfterChannelAlloc::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.test.threading_test.client.DataLostAfterChannelAlloc"));

int main(int argc, char** argv)
{
   PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));

   static struct option long_options[] =
   {
      { "ca_unix_addr",   required_argument, 0, 'u' },
      { 0, 0, 0, 0 }
   };

   std::tr1::array<char, 1024> ca_sock_path   = {{0}};

   int option_index = 0;
   int c;

   while (true)
   {
      c = getopt_long(argc, argv, "u:", long_options, &option_index);

      if (-1 == c)
         break;

      switch (c)
      {
      case 'u':
         //Log("option u = '%s'", optarg);
         strncpy(ca_sock_path.begin(), optarg, ca_sock_path.size());
         ca_sock_path[ca_sock_path.size() - 1] = 0;
         break;
      default:
         //Log("unknown param %c (%d)", c, c);
         exit(1);
         break;
      }
   }

   if (ca_sock_path[0] != 0)
   {
      extern char const * gpCA_UNIX_SOCK_PATH;
      gpCA_UNIX_SOCK_PATH = ca_sock_path.begin();
   }

   // please run client first
   CSignalSemaphoreInterproc::unlink("/connectivity_agent_test_server");
   CSignalSemaphoreInterproc::unlink("/connectivity_agent_test_client");

   Test::mpCliSyncSem = new CSignalSemaphoreInterproc("/connectivity_agent_test_client");
   Test::mpSrvSyncSem = new CSignalSemaphoreInterproc("/connectivity_agent_test_server");

   
   // {
   //    MultipleOpenCloseTest test; test.go();
   // }

   // {
   //    MultipleOpenCloseNoSyncTest test; test.go();
   // }

   // {
   //    MultipleOpenCloseMultithreadTest test; test.go();
   // }

   {
      DataLostAfterChannelAlloc test; test.go();
   }


   delete Test::mpSrvSyncSem;
   delete Test::mpCliSyncSem;

   return 0;
}
