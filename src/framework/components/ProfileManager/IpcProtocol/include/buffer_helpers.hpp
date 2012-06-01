/* 
 *  iviLINK SDK, version 0.9 (for preview only)                                      
 *    http://www.ivilink.net                                                         
 *  Cross Platform Application Communication Stack for In-Vehicle Applications       
 *                                                                                   
 *  Copyright (C) 2012, Luxoft Professional Corp., member of IBS group               
 *                                                                                   
 *  This library is free software; you can redistribute it and/or                    
 *  modify it under the terms of the GNU Lesser General Public                       
 *  License as published by the Free Software Foundation; version 2.1.               
 *                                                                                   
 *  This library is distributed in the hope that it will be useful,                  
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                
 *  Lesser General Public License for more details.                                  
 *                                                                                   
 *  You should have received a copy of the GNU Lesser General Public                 
 *  License along with this library; if not, write to the Free Software              
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA   
 * 
 * 
 * 
 * 
 */

/***************************************************************************
 * Project           AXIS
 * (c) copyright     2012
 * Company           LUXOFT
 * @file             buffer_helpers.hpp
 * @brief            Header file for CBufferReader and CBufferWriter helpers
 * @author           PIlin
 ***************************************************************************/

#ifndef BUFFER_HELPERS_HPP_
#define BUFFER_HELPERS_HPP_

#include <string>

#include "utils/misc/include/CError.hpp"

namespace AXIS {

class CUid;

namespace ProfileManager {
namespace Ipc {
namespace Helpers {

/**
 * Helper class used to simplify data serialization.
 */
class CBufferWriter
{
public:

   /**
    * Constructor
    * @param pBuffer pointer to buffer where data will be written
    * @param bufSize size of buffer with data
    */
   CBufferWriter(UInt8* pBuffer, UInt32 bufSize):
      mpBuffer(pBuffer),
      mFullSize(bufSize),
      mUsedSize(0)
   {
   }

   /**
    * @return the size of filled space in buffer
    */
   UInt32 getUsedSize() const { return mUsedSize; }

   /**
    * Writes value of type T to buffer.
    * _WARNING_: writing the pointer is, probably, a bad idea and can lead to
    * SEGFAULT on reading side.
    *
    * @param val value to be written
    *
    * @retval NO_ERROR write successful
    * @retval 1 insufficient buffer size
    */
   template <typename T>
   CError write(T const& val)
   {
      if (sizeof(T) > mFullSize - mUsedSize)
         return CError(1, moduleName, CError::ERROR, "insufficient buffer size (from generic write)");
      *reinterpret_cast<T*>(mpBuffer + mUsedSize) = val;
      mUsedSize += sizeof(T);
      return CError::NoError(moduleName);
   }

   /**
    * Writes the std::pair<K, V> into buffer
    *
    * @param val std::pair to be written
    *
    * @retval NO_ERROR write successful
    * @retval 1 insufficient buffer size
    */
   template <typename K, typename V>
   CError write(std::pair<K, V> const& val)
   {
      CError err = write(val.first);
      if (!err.isNoError())
         return err;
      err = write(val.second);
      if (!err.isNoError())
         return err;
      return CError::NoError(moduleName);
   }

   /**
    * Writes range of values to buffer.
    * Range [first, last) is specified by input iterators.
    *
    * @param first begin of the range
    * @param last end of the range
    *
    * @retval NO_ERROR write successful
    * @retval 1 insufficient buffer size
    */
   template <typename InputIter>
   CError write(InputIter first, InputIter last)
   {
      UInt8* curPos = mpBuffer + mUsedSize;
      UInt32 size = 0;

      CError err = write(size);
      if (!err.isNoError())
         return err;

      for (; first != last; ++first, ++size)
      {
         err = write(*first);
         if (!err.isNoError())
            return err;
      }

      *reinterpret_cast<UInt32*>(curPos) = size;
      return CError::NoError(moduleName);
   }

private:

   CBufferWriter(CBufferWriter const&);
   CBufferWriter& operator=(CBufferWriter const&);

private:
   UInt8* mpBuffer;  /// pointer to buffer with data
   UInt32 mFullSize; /// maximal size of buffer
   UInt32 mUsedSize; /// amount of data in buffer

   static char const* const moduleName;
};

template <> CError CBufferWriter::write<AXIS::CUid>(AXIS::CUid const& val);
template <> CError CBufferWriter::write<std::string>(std::string const& val);


/**
 * Helper class used to simplify data de-serialization
 */
class CBufferReader
{
public:

   /**
    * Constructor.
    * @param pBuffer pointer to buffer with data to be read
    * @param bufSize size of buffer with data
    */
   CBufferReader(UInt8 const* pBuffer, UInt32 bufSize):
      mpBuffer(pBuffer),
      mFullSize(bufSize),
      mUsedSize(0)
   {
   }

   /**
    * Reads the value of type T from buffer.
    * _WARNING_: reading the pointer is, probably, a bad idea and can lead to
    * SEGFAULT on reading side.
    *
    * @param val the value to be read.
    *
    * @retval NO_ERROR write successful
    * @retval 1 insufficient buffer size
    */
   template <typename T>
   CError read(T & val)
   {
      if (sizeof(T) > mFullSize - mUsedSize)
         return CError(1, moduleName, CError::ERROR, "insufficient buffer size (from generic write)");
      val = *reinterpret_cast<T const*>(mpBuffer + mUsedSize);
      mUsedSize += sizeof(T);
      return CError::NoError(moduleName);
   }

   /**
    * Reads the std::pair<K, V> from buffer.
    *
    * @param val std::pair to be read.
    *
    * @retval NO_ERROR write successful
    * @retval 1 insufficient buffer size
    */
   template <typename K, typename V>
   CError read(std::pair<K, V> & val)
   {
      CError err = read(val.first);
      if (!err.isNoError())
         return err;
      err = read(val.second);
      if (!err.isNoError())
         return err;
      return CError::NoError(moduleName);
   }

   /**
    * Reads to container from buffer using passed InsertIterator.
    * Supports all container types, except map-like (std::map, std::multimap).
    * For map-like containers use readMap().
    *
    * Example:
    * @code
    * std::list<int> list;
    * reader->read(std::inserter(list, list.begin()));
    * @endcode
    *
    * @param result insert iterator used to insert data from buffer
    *
    * @retval NO_ERROR write successful
    * @retval 1 insufficient buffer size
    */
   template <typename InsertIter>
   CError readContainer(InsertIter result)
   {
      typedef typename InsertIter::container_type::value_type tVal;
      UInt32 size = 0;

      CError err = read(size);
      if (!err.isNoError())
         return err;

      for (UInt32 i = 0; i < size; ++i)
      {
         tVal val;
         err = read(val);
         if (!err.isNoError())
            return err;
         *result++ = val;
      }
      return CError::NoError(moduleName);
   }

   /**
    * Reads to map-like container from buffer using passed InsertIterator.
    * For other containers use readContainer()
    *
    * Example:
    * @code
    * std::map<std::string, int> map;
    * reader->read(std::inserter(map, map.begin()));
    * @endcode
    *
    * @param result insert iterator used to insert data from buffer
    *
    * @retval NO_ERROR write successful
    * @retval 1 insufficient buffer size
    */
   template <typename InsertIter>
   CError readMap(InsertIter result)
   {
      typedef typename InsertIter::container_type::key_type tKey;
      typedef typename InsertIter::container_type::mapped_type tVal;
      UInt32 size = 0;

      CError err = read(size);
      if (!err.isNoError())
         return err;

      for (UInt32 i = 0; i < size; ++i)
      {
         tKey key;
         tVal val;
         err = read(key);
         if (!err.isNoError())
            return err;
         err = read(val);
         if (!err.isNoError())
            return err;
         *result++ = std::make_pair(key, val);
      }
      return CError::NoError(moduleName);
   }

private:
   CBufferReader(CBufferReader const&);
   CBufferReader& operator=(CBufferReader const&);

private:
   UInt8 const* mpBuffer;  /// pointer to buffer with data
   UInt32 const mFullSize; /// maximal size of buffer
   UInt32 mUsedSize;       /// amount of already read data

   static char const* const moduleName;
};

template <> CError CBufferReader::read<AXIS::CUid>(AXIS::CUid & val);
template <> CError CBufferReader::read<std::string>(std::string & val);

}  // namespace Helpers
}  // namespace Ipc
}  // namespace ProfileManager
}  // namespace AXIS


#endif /* BUFFER_HELPERS_HPP_ */
