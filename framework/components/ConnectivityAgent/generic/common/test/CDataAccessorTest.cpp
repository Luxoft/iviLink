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


#include <cstring>
#include <cppunit/extensions/HelperMacros.h>

#include "CDataAccessor.hpp"

using namespace iviLink::ConnectivityAgent;

/**
 * Test fixture for Frame struct
 */
class DataAccessorTest: public CppUnit::TestFixture {
   CPPUNIT_TEST_SUITE( DataAccessorTest);
   CPPUNIT_TEST(testRawArray);
   CPPUNIT_TEST_SUITE_END();

public:

   void setUp()
   {
   }

   void tearDown()
   {
   }


   void testRawArray()
   {
      //setup
      UInt8 data[] = "012";
      CDataAccessor a;
      a.setChannelID(123);
      a.setErrorCode(234);
      a.setOpCode(345);
      a.setData(data, sizeof(data));

      UInt8 array[100];

      a.printContent();

      // test

      UInt32 size = a.getObjectSize();
      a.copyToRawArray(array);

      CPPUNIT_ASSERT_EQUAL(sizeof(data) + 4*4, size);

      CDataAccessor b(array, size);
      b.printContent();
      CPPUNIT_ASSERT_EQUAL(123u, b.getChannelID());
      CPPUNIT_ASSERT_EQUAL(234u, b.getErrorCode());
      CPPUNIT_ASSERT_EQUAL(345u, b.getOpCode());
      CPPUNIT_ASSERT_EQUAL(sizeof(data), b.getDataSize());

      CPPUNIT_ASSERT(0 == memcmp(data, b.getData(), sizeof(data)));
   }


};
CPPUNIT_TEST_SUITE_REGISTRATION( DataAccessorTest );

