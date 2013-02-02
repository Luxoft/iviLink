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


#include "Buffer.hpp"
#include "byteOrder.hpp"

using namespace iviLink;

// In this file I had to broke some of our code style rules with the ntoh(),
// hton(), read() and write() methods. I think, this helped to read code.


/**
 * Type for static functions - byte order converters for given type T
 */
template <typename T>
struct CEndianConverter
{
   /**
    * Host to network order conversion
    * @param value
    * @return converted value
    */
   static T hton(T const& value);

   /**
    * Network to host order conversion
    * @param value
    * @return converted value
    */
   static T ntoh(T const& value);
};

template<> UInt8  CEndianConverter<UInt8 >::hton(const UInt8  & val) { return val; }
template<>  Int8  CEndianConverter< Int8 >::hton(const  Int8  & val) { return val; }
template<> UInt16 CEndianConverter<UInt16>::hton(const UInt16 & val) { return ByteOrder::hton16(val); }
template<>  Int16 CEndianConverter< Int16>::hton(const  Int16 & val) { return ByteOrder::ntoh16(val); }
template<> UInt32 CEndianConverter<UInt32>::hton(const UInt32 & val) { return ByteOrder::hton32(val); }
template<>  Int32 CEndianConverter< Int32>::hton(const  Int32 & val) { return ByteOrder::ntoh32(val); }
template<> UInt64 CEndianConverter<UInt64>::hton(const UInt64 & val) { return ByteOrder::hton64(val); }
template<>  Int64 CEndianConverter< Int64>::hton(const  Int64 & val) { return ByteOrder::ntoh64(val); }
template<> float  CEndianConverter<float >::hton(const float  & val) { return ByteOrder::hton32(*reinterpret_cast<UInt32 const*>(&val)); }
template<> double CEndianConverter<double>::hton(const double & val) { return ByteOrder::hton64(*reinterpret_cast<UInt64 const*>(&val)); }

template<> UInt8  CEndianConverter<UInt8 >::ntoh(const UInt8  & val) { return val; }
template<>  Int8  CEndianConverter< Int8 >::ntoh(const  Int8  & val) { return val; }
template<> UInt16 CEndianConverter<UInt16>::ntoh(const UInt16 & val) { return ByteOrder::ntoh16(val); }
template<>  Int16 CEndianConverter< Int16>::ntoh(const  Int16 & val) { return ByteOrder::ntoh16(val); }
template<> UInt32 CEndianConverter<UInt32>::ntoh(const UInt32 & val) { return ByteOrder::ntoh32(val); }
template<>  Int32 CEndianConverter< Int32>::ntoh(const  Int32 & val) { return ByteOrder::ntoh32(val); }
template<> UInt64 CEndianConverter<UInt64>::ntoh(const UInt64 & val) { return ByteOrder::ntoh64(val); }
template<>  Int64 CEndianConverter< Int64>::ntoh(const  Int64 & val) { return ByteOrder::ntoh64(val); }
template<> float  CEndianConverter<float >::ntoh(const float  & val) { return ByteOrder::ntoh32(*reinterpret_cast<UInt32 const*>(&val)); }
template<> double CEndianConverter<double>::ntoh(const double & val) { return ByteOrder::ntoh64(*reinterpret_cast<UInt64 const*>(&val)); }

Buffer::Buffer(UInt8* pBuffer, UInt32 size, bool endianConvert/* = true*/) :
   mSize(size),
   mReadPos(0),
   mWritePos(0),
   mpBuffer(pBuffer),
   mEndianConvert(endianConvert)
{

}

Buffer::~Buffer()
{

}

Buffer::Buffer(Buffer const& ref) :
   mSize(ref.mSize),
   mReadPos(ref.mReadPos),
   mWritePos(ref.mWritePos),
   mpBuffer(ref.mpBuffer),
   mEndianConvert(ref.mEndianConvert)
{}

Buffer& Buffer::operator=(Buffer const& ref)
{
   mSize = ref.mSize;
   mReadPos = ref.mReadPos;
   mWritePos = ref.mWritePos;
   mpBuffer = ref.mpBuffer;
   mEndianConvert = ref.mEndianConvert;
   return *this;
}

UInt8* Buffer::getBuffer() const
{
   return mpBuffer;
}

template <typename T>
bool Buffer::write_internal(T val)
{
   if (sizeof(T) > mSize - mWritePos)
      return false;
   if (mEndianConvert)
      *reinterpret_cast<T*>(mpBuffer + mWritePos) = CEndianConverter<T>::hton(val);
   else
      *reinterpret_cast<T*>(mpBuffer + mWritePos) = val;
   mWritePos += sizeof(T);
   return true;
}

template <typename T>
bool Buffer::read_internal(T & val) const
{
   if (sizeof(T) > mSize - mReadPos)
      return false;
   if (mEndianConvert)
      val = CEndianConverter<T>::ntoh(*reinterpret_cast<T const*>(mpBuffer + mReadPos));
   else
      val = *reinterpret_cast<T const*>(mpBuffer + mReadPos);
   mReadPos += sizeof(T);
   return true;
}


bool Buffer::write( Int8  value) { return write_internal(value); }
bool Buffer::write(UInt8  value) { return write_internal(value); }
bool Buffer::write( Int16 value) { return write_internal(value); }
bool Buffer::write(UInt16 value) { return write_internal(value); }
bool Buffer::write( Int32 value) { return write_internal(value); }
bool Buffer::write(UInt32 value) { return write_internal(value); }
bool Buffer::write( Int64 value) { return write_internal(value); }
bool Buffer::write(UInt64 value) { return write_internal(value); }

bool Buffer::read( Int8  & value) const { return read_internal(value); }
bool Buffer::read(UInt8  & value) const { return read_internal(value); }
bool Buffer::read( Int16 & value) const { return read_internal(value); }
bool Buffer::read(UInt16 & value) const { return read_internal(value); }
bool Buffer::read( Int32 & value) const { return read_internal(value); }
bool Buffer::read(UInt32 & value) const { return read_internal(value); }
bool Buffer::read( Int64 & value) const { return read_internal(value); }
bool Buffer::read(UInt64 & value) const { return read_internal(value); }

bool Buffer::write(void const* pBuffer, UInt32 size)
{
   if (size > mSize - mWritePos)
      return false;

   UInt8 const* p = static_cast<UInt8 const*>(pBuffer);
   std::copy(p, p + size, mpBuffer + mWritePos);
   mWritePos += size;
   return true;
}

bool Buffer::read(UInt32 size, void * pBuffer) const
{
   if (size > mSize - mReadPos)
      return false;

   UInt8 *const p = static_cast<UInt8*>(pBuffer);
   UInt8 const* buf = mpBuffer + mReadPos;
   std::copy(buf, buf + size, p);
   mReadPos += size;
   return true;
}

bool Buffer::write(std::string const& value)
{
   const UInt32 valSize = value.size();

   if (!write(valSize))
      return false;
   if (valSize > mSize - mWritePos)
      return false;

   return write(value.c_str(), valSize);
}

bool Buffer::read(std::string & value) const
{
   UInt32 valSize = 0;
   if (!read(valSize))
      return false;
   if (valSize > mSize - mReadPos)
      return false;

   value.assign(reinterpret_cast<char const*>(mpBuffer + mReadPos), valSize);
   mReadPos += valSize;

   return true;
}

UInt32 Buffer::getSize() const
{
   return mSize;
}

UInt32 Buffer::getReadPosition() const
{
   return mReadPos;
}

UInt32 Buffer::getWritePosition() const
{
   return mWritePos;
}

bool Buffer::setReadPosition(UInt32 pos) const
{
   if (pos >= mSize)
   {
      return false;
   }

   mReadPos = pos;
   return true;
}

bool Buffer::setWritePosition(UInt32 pos)
{
   if (pos >= mSize)
   {
      return false;
   }

   mWritePos = pos;
   return true;
}
