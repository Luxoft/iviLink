/*
 *
 * iviLINK SDK, version 1.0
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



#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <unistd.h>


#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>

#include "Buffer.hpp"

using namespace iviLink;

template <typename T>
void checkRead( T const& expected, Buffer& buffer, CppUnit::SourceLine sourceLine)
{
    T actual;
    CPPUNIT_NS::Asserter::failIf(false == buffer.read(actual),
        CPPUNIT_NS::Message( "assertion failed, buffer read error" ),
        sourceLine );

    using CPPUNIT_NS::assertion_traits;
    if ( !assertion_traits<T>::equal(expected,actual) )
    {
        ::CppUnit::Asserter::failNotEqual( assertion_traits<T>::toString(expected),
                assertion_traits<T>::toString(actual),
                sourceLine );
    }
}

#define ASSERT_READ( expected, buffer ) \
    checkRead( (expected), buffer, CPPUNIT_SOURCELINE() )

template <typename T>
void checkReadContainer( T const& expected, Buffer& buffer, CppUnit::SourceLine sourceLine)
{
    T actual;
    CPPUNIT_NS::Asserter::failIf(false == buffer.read(std::inserter(actual, actual.begin())),
        CPPUNIT_NS::Message( "assertion failed, buffer.readContainer() error" ),
        sourceLine );

    using CPPUNIT_NS::assertion_traits;

    if ( !assertion_traits<size_t>::equal(expected.size(), actual.size() ))
    {
        CPPUNIT_NS::Asserter::failNotEqual( assertion_traits<size_t>::toString(expected.size()),
                assertion_traits<size_t>::toString(actual.size()),
                sourceLine,
                "Size mistatch" );
    }

    if (!std::equal(expected.begin(), expected.end(), actual.begin()))
    {
        CPPUNIT_NS::Asserter::failNotEqual( "", "",
                sourceLine,
                "Sequnces are not equal" );
    }
}

#define ASSERT_CONTAINER_READ( expected, buffer ) \
    checkReadContainer( (expected), buffer, CPPUNIT_SOURCELINE() )

class BufferTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(BufferTest);
    CPPUNIT_TEST(testGetBuffer);
    CPPUNIT_TEST(testReadWriteNumbers);
    CPPUNIT_TEST(testGetSetPos);
    CPPUNIT_TEST(testReadWriteRaw);
    CPPUNIT_TEST(testReadWriteManyRaw);
    CPPUNIT_TEST(testReadWriteString);
    CPPUNIT_TEST(testStdList);
    CPPUNIT_TEST(testStdVector);
    CPPUNIT_TEST(testStdSet);
    CPPUNIT_TEST(testStdMap);
    CPPUNIT_TEST(testCSBuffer);
    CPPUNIT_TEST(testCopyBuffer);
    CPPUNIT_TEST_SUITE_END();

public:

    void testGetBuffer()
    {
        // set up
        UInt8 buf[3];
        UInt8* pBuf = buf;
        Buffer b(buf, 3);

        // test
        CPPUNIT_ASSERT_EQUAL(pBuf, b.getBuffer());
    }

    void testReadWriteNumbers()
    {
        // set up
        UInt8 buf[100];
        Buffer b(buf, sizeof(buf));

        const  Int8   i8  = -116;
        const UInt8  ui8  = 234;
        const  Int16  i16 = -12312;
        const UInt16 ui16 = 45678;
        const  Int32  i32 = -9239882;
        const UInt32 ui32 = 239492392;
        const  Int64  i64 = -9223372036854775807;
        const UInt64 ui64 = 18446744073709551615u;

        const UInt32 dataSize = 2*1 + 2*2 + 2*4 + 2*8;

        // test

        CPPUNIT_ASSERT_EQUAL(0u, b.getWritePosition());
        CPPUNIT_ASSERT_EQUAL(0u, b.getReadPosition());

        CPPUNIT_ASSERT(b.write( i8 ));
        CPPUNIT_ASSERT(b.write(ui8 ));
        CPPUNIT_ASSERT(b.write( i16));
        CPPUNIT_ASSERT(b.write(ui16));
        CPPUNIT_ASSERT(b.write( i32));
        CPPUNIT_ASSERT(b.write(ui32));
        CPPUNIT_ASSERT(b.write( i64));
        CPPUNIT_ASSERT(b.write(ui64));

        CPPUNIT_ASSERT_EQUAL(dataSize, b.getWritePosition());
        CPPUNIT_ASSERT_EQUAL(0u, b.getReadPosition());

        ASSERT_READ( i8 , b);
        ASSERT_READ(ui8 , b);
        ASSERT_READ( i16, b);
        ASSERT_READ(ui16, b);
        ASSERT_READ( i32, b);
        ASSERT_READ(ui32, b);
        ASSERT_READ( i64, b);
        ASSERT_READ(ui64, b);

        CPPUNIT_ASSERT_EQUAL(dataSize, b.getWritePosition());
        CPPUNIT_ASSERT_EQUAL(dataSize, b.getReadPosition());
    }

    void testGetSetPos()
    {
        // set up
        const UInt32 size = 10;
        UInt8 buf[size];
        Buffer b(buf, size);

        // test

        CPPUNIT_ASSERT_EQUAL(0u, b.getWritePosition());
        CPPUNIT_ASSERT_EQUAL(0u, b.getReadPosition());

        CPPUNIT_ASSERT(b.setWritePosition(3));
        CPPUNIT_ASSERT(b.setReadPosition(8));

        CPPUNIT_ASSERT_EQUAL(3u, b.getWritePosition());
        CPPUNIT_ASSERT_EQUAL(8u, b.getReadPosition());

        CPPUNIT_ASSERT(false == b.setWritePosition(10));
        CPPUNIT_ASSERT(false == b.setReadPosition(11));

        CPPUNIT_ASSERT_EQUAL(3u, b.getWritePosition());
        CPPUNIT_ASSERT_EQUAL(8u, b.getReadPosition());

        CPPUNIT_ASSERT(b.setWritePosition(9));
        CPPUNIT_ASSERT(b.setReadPosition(9));

        CPPUNIT_ASSERT_EQUAL(9u, b.getWritePosition());
        CPPUNIT_ASSERT_EQUAL(9u, b.getReadPosition());
    }

    void testReadWriteRaw()
    {
        // set up
        const char smallStr[]  = "small string";
        const char mediumStr[] = "buffer size equals the size of this string";
        const char largeStr[]  = "this string is larger than buffer size. what a pity!";

        const UInt32 smallStrSize = sizeof(smallStr);
        const UInt32 mediumStrSize = sizeof(mediumStr);
        const UInt32 largeStrSize = sizeof(largeStr);

        char tmpStr[largeStrSize + 10];

        const UInt32 bufSize = mediumStrSize;
        UInt8 buf[bufSize];
        Buffer b(buf, bufSize);

        // test small str
        CPPUNIT_ASSERT_EQUAL(0u, b.getWritePosition());
        CPPUNIT_ASSERT_EQUAL(0u, b.getReadPosition());

        CPPUNIT_ASSERT(b.write(smallStr, smallStrSize));

        CPPUNIT_ASSERT_EQUAL(smallStrSize, b.getWritePosition());
        CPPUNIT_ASSERT_EQUAL(0u, b.getReadPosition());

        CPPUNIT_ASSERT(0 == memcmp(b.getBuffer(), smallStr, smallStrSize));

        CPPUNIT_ASSERT(b.read(smallStrSize, tmpStr));
        CPPUNIT_ASSERT_EQUAL(smallStrSize, b.getWritePosition());
        CPPUNIT_ASSERT_EQUAL(smallStrSize, b.getReadPosition());
        CPPUNIT_ASSERT(0 == memcmp(tmpStr, smallStr, smallStrSize));

        // test medium str
        b.setWritePosition(0);
        b.setReadPosition(0);
        CPPUNIT_ASSERT_EQUAL(0u, b.getWritePosition());
        CPPUNIT_ASSERT_EQUAL(0u, b.getReadPosition());

        CPPUNIT_ASSERT(b.write(mediumStr, mediumStrSize));

        CPPUNIT_ASSERT_EQUAL(mediumStrSize, b.getWritePosition());
        CPPUNIT_ASSERT_EQUAL(0u, b.getReadPosition());

        CPPUNIT_ASSERT(0 == memcmp(b.getBuffer(), mediumStr, mediumStrSize));

        CPPUNIT_ASSERT(b.read(mediumStrSize, tmpStr));
        CPPUNIT_ASSERT_EQUAL(mediumStrSize, b.getWritePosition());
        CPPUNIT_ASSERT_EQUAL(mediumStrSize, b.getReadPosition());
        CPPUNIT_ASSERT(0 == memcmp(tmpStr, mediumStr, mediumStrSize));

        // test large str
        b.setWritePosition(0);
        b.setReadPosition(0);
        CPPUNIT_ASSERT_EQUAL(0u, b.getWritePosition());
        CPPUNIT_ASSERT_EQUAL(0u, b.getReadPosition());

        CPPUNIT_ASSERT(false == b.write(largeStr, largeStrSize));

        CPPUNIT_ASSERT_EQUAL(0u, b.getWritePosition());
        CPPUNIT_ASSERT_EQUAL(0u, b.getReadPosition());
    }

    void testReadWriteManyRaw()
    {
        const char raw1[] = "asdf";
        const char raw2[] = "qwerty";
        const char raw3[] = "zxcvbnm,.";

        char tmp[40];

        const UInt32 size = 40;
        UInt8 buf[size];
        Buffer b(buf, size);

        // test
        CPPUNIT_ASSERT(b.write(raw1, sizeof(raw1)));
        CPPUNIT_ASSERT(b.write(raw2, sizeof(raw2)));
        CPPUNIT_ASSERT(b.write(raw3, sizeof(raw3)));

        CPPUNIT_ASSERT(b.read(sizeof(raw1), tmp));
        CPPUNIT_ASSERT(0 == memcmp(tmp, raw1, sizeof(raw1)));

        CPPUNIT_ASSERT(b.read(sizeof(raw2), tmp));
        CPPUNIT_ASSERT(0 == memcmp(tmp, raw2, sizeof(raw2)));

        CPPUNIT_ASSERT(b.read(sizeof(raw3), tmp));
        CPPUNIT_ASSERT(0 == memcmp(tmp, raw3, sizeof(raw3)));
    }

    void testReadWriteString()
    {
        // set up
        const std::string small  = "small string";
        const std::string medium = "buffer size equals the size of this string";
        const std::string large  = "this string is larger than buffer size. what a pity!";

        const UInt32 smallSize = small.size() + sizeof(UInt32);
        const UInt32 mediumSize = medium.size() + sizeof(UInt32);
        // const UInt32 largeSize = large.size() + sizeof(UInt32);

        std::string tmp;

        const UInt32 bufSize = mediumSize;
        UInt8 buf[bufSize];
        Buffer b(buf, bufSize);

        // test small str
        CPPUNIT_ASSERT_EQUAL(0u, b.getWritePosition());
        CPPUNIT_ASSERT_EQUAL(0u, b.getReadPosition());

        CPPUNIT_ASSERT(b.write(small));

        CPPUNIT_ASSERT_EQUAL(smallSize, b.getWritePosition());
        CPPUNIT_ASSERT_EQUAL(0u, b.getReadPosition());

        CPPUNIT_ASSERT(b.read(tmp));
        CPPUNIT_ASSERT_EQUAL(smallSize, b.getWritePosition());
        CPPUNIT_ASSERT_EQUAL(smallSize, b.getReadPosition());
        CPPUNIT_ASSERT_EQUAL(small, tmp);

        // test medium str
        b.setWritePosition(0);
        b.setReadPosition(0);
        CPPUNIT_ASSERT_EQUAL(0u, b.getWritePosition());
        CPPUNIT_ASSERT_EQUAL(0u, b.getReadPosition());

        CPPUNIT_ASSERT(b.write(medium));

        CPPUNIT_ASSERT_EQUAL(mediumSize, b.getWritePosition());
        CPPUNIT_ASSERT_EQUAL(0u, b.getReadPosition());

        CPPUNIT_ASSERT(b.read(tmp));
        CPPUNIT_ASSERT_EQUAL(mediumSize, b.getWritePosition());
        CPPUNIT_ASSERT_EQUAL(mediumSize, b.getReadPosition());
        CPPUNIT_ASSERT_EQUAL(medium, tmp);

        // test large str
        b.setWritePosition(0);
        b.setReadPosition(0);
        CPPUNIT_ASSERT_EQUAL(0u, b.getWritePosition());
        CPPUNIT_ASSERT_EQUAL(0u, b.getReadPosition());

        CPPUNIT_ASSERT(false == b.write(large));

        CPPUNIT_ASSERT_EQUAL(4u, b.getWritePosition());
        CPPUNIT_ASSERT_EQUAL(0u, b.getReadPosition());
    }

    void testStdList()
    {
        UInt8 buf[100];
        Buffer b(buf, sizeof(buf));

        std::list<int> list;
        for (int i = -10; i < 10; ++i)
        {
            list.push_back(i);
        }

        CPPUNIT_ASSERT(b.write(list.begin(), list.end()));
        ASSERT_CONTAINER_READ(list, b);
    }

    void testStdVector()
    {
        UInt8 buf[100];
        Buffer b(buf, sizeof(buf));

        std::vector<int> vec;
        for (int i = -10; i < 10; ++i)
        {
            vec.push_back(i);
        }

        CPPUNIT_ASSERT(b.write(vec.begin(), vec.end()));
        ASSERT_CONTAINER_READ(vec, b);
    }

    void testStdSet()
    {
        UInt8 buf[100];
        Buffer b(buf, sizeof(buf));

        std::set<std::string> set;
        set.insert("asdf");
        set.insert("zcxv");
        set.insert("aqwersdf");
        set.insert("ayuyu");

        CPPUNIT_ASSERT(b.write(set.begin(), set.end()));
        ASSERT_CONTAINER_READ(set, b);
    }

    void testStdMap()
    {
        UInt8 buf[100];
        Buffer b(buf, sizeof(buf));

        std::map<std::string, int> map;
        map["asdf"] = 1;
        map["zcxv"] = 2;
        map["aqwersdf"] = 3;
        map["ayuyu"] = 4;

        CPPUNIT_ASSERT(b.write(map.begin(), map.end()));
        ASSERT_CONTAINER_READ(map, b);


        std::map< std::string, int> tmp;
        std::copy(map.begin(), map.end(), std::inserter(map, map.begin()));
    }

    void testCSBuffer()
    {
        const UInt32 size = 5;
        CSBuffer<size> buf;

        CPPUNIT_ASSERT_EQUAL(size, buf.getSize());
        CPPUNIT_ASSERT_EQUAL(0u, buf.getReadPosition());
        CPPUNIT_ASSERT_EQUAL(0u, buf.getWritePosition());

        Int32 v1 = 12341234;
        Int8  v2 = 127;
        Int8  v3 = 1;

        CPPUNIT_ASSERT(buf.write(v1));
        CPPUNIT_ASSERT(buf.write(v2));
        CPPUNIT_ASSERT(false == buf.write(v3));

        CPPUNIT_ASSERT_EQUAL(0u, buf.getReadPosition());
        CPPUNIT_ASSERT_EQUAL(5u, buf.getWritePosition());

        ASSERT_READ(v1, buf);
        ASSERT_READ(v2, buf);
        Int8 tmp;
        CPPUNIT_ASSERT(false == buf.read(tmp));

        CPPUNIT_ASSERT_EQUAL(5u, buf.getReadPosition());
        CPPUNIT_ASSERT_EQUAL(5u, buf.getWritePosition());
    }

    void testCopyBuffer()
    {
        const UInt32 size = 10;
        UInt8 arr[size];
        Buffer buf(arr, size);
        buf.setReadPosition(2);
        buf.setWritePosition(3);

        #define CHECK(b)                                                       \
                CPPUNIT_ASSERT_EQUAL(size, b.getSize());                        \
                CPPUNIT_ASSERT_EQUAL((intptr_t)arr, (intptr_t)b.getBuffer());   \
                CPPUNIT_ASSERT_EQUAL(2u, b.getReadPosition());                  \
                CPPUNIT_ASSERT_EQUAL(3u, b.getWritePosition());

        CHECK(buf);

        Buffer copy(buf);
        CHECK(copy);

        Buffer assigned(NULL, 0);
        assigned = buf;
        CHECK(assigned);
    }

};


CPPUNIT_TEST_SUITE_REGISTRATION( BufferTest );

