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


/**
 * \file CBuffer.hpp
 * This file contains declarations of the CBuffer class (used in sending data to the other side)
 * and some helper classes and functions.
 */


#ifndef CBUFFER_HPP__
#define CBUFFER_HPP__


#include "Types.hpp"
#include "BaseError.hpp"

namespace iviLink
{

namespace TypeTraits
{

/**
 * \namespace iviLink::TypeTraits
 * Types in this namespace are used to detect std::map-like types at compile time
 * and select appropriate method to fill collection with data.
 * 
 * Description of the technique used here:
 * http://en.wikipedia.org/wiki/Substitution_failure_is_not_an_error
 *
 * This code was a great help: 
 * http://nicola-bonelli-repo.googlecode.com/svn-history/r728/trunk/codes.cpp0x/type_traits.hpp
 */

/**
 * \struct SFINAE
 * Type is used to hold SFINAE-types with different size.
 */
struct SFINAE
{
   typedef char __one;
   typedef struct { char __arr[2]; } __two;
};

/**
 * \class has_mapped_type
 * \tparam type for checking
 * This trait is used to determine if type T has T::mapped_type type member.
 * This member is used in std::map, std::multimap.
 */ 
template <typename T> 
class has_mapped_type : public SFINAE
{
   template <typename C> static __one test(typename C::mapped_type *);
   template <typename C> static __two test(...);
public:
   enum { value = (sizeof(test<T>(0)) == sizeof(__one)) };
};


} // namespace TypeTraits

/**
 * \class CBuffer
 * Class is used to organize data buffers to send through iviLink.
 * Class provides methods for simple reading and writing of many standard data
 * types. Internal counters prevent buffer overflow  or reading the buffer after
 * its end. Example:
 * @code
 * bool result = true;
 * result = result && buf.write(1);
 * result = result && buf.write(2);
 * result = result && buf.write(std::string("hello, world!"));
 * if (!result) std::cerr << "buffer was too small";
 * @endcode
 *
 * All write() and read() methods don't store any information about type, so
 * the order of write() and read() calls with specific type must be the same.
 *
 * The set of types supported by read() and write() is limited
 * intentionally. It is possible (and much preferable in terms of C++) to write
 * just function templates. But we wanted to provide support only for tested basic types.
 */
class Buffer
{
public:

    /**
     * Create a new CBuffer with the data from given byte array inside.
     * @param pBuffer pointer to an array with data to be stored inside this CBuffer
     * @param size number of bytes from the array to be stored inside the newly created CBuffer
     * @param endianConvert if true - numeric data will be prepared for transfer
     *    through network. Otherwise, it will be saved in buffer intact.
     */
    Buffer(UInt8* pBuffer, UInt32 size, bool endianConvert = true);

    ~Buffer();

    /**
     * Copy constructor creates shallow copy of passed instance.
     * Take into account that copied object will have its own set of read and
     * write positions, but will use the same underling memory buffer as the original.
     * This can lead to rewriting data in case of simultanious access from both
     * objects.
     * @param CBuffer instance of CBuffer to be copied
     */
    Buffer(Buffer const&);

    /**
     * Assign operator creates a shallow copy of passed object to this object.
     * Take into account that copied object will have its own set of read and
     * write positions but will use the same underling memory buffer as the original.
     * This can lead to rewriting data in case of simultanious access from both
     * objects.
     */
    Buffer& operator=(Buffer const&);

    /**
     * Save numeric data in buffer.
     * Will change internal write counter, see getWritePosition().
     * @retval true write is successful
     * @retval false not enough space in buffer
     */
    bool write( Int8  value);
   
    /**
     * Save numeric data in buffer.
     * Will change internal write counter, see getWritePosition().
     * @retval true write is successful
     * @retval false not enough space in buffer
     */
    bool write(UInt8  value);
   
    /**
     * Save numeric data in buffer.
     * Will change internal write counter, see getWritePosition().
     * @retval true write is successful
     * @retval false not enough space in buffer
     */
    bool write( Int16 value);
   
    /**
     * Save numeric data in buffer.
     * Will change internal write counter, see getWritePosition().
     * @retval true write is successful
     * @retval false not enough space in buffer
     */
    bool write(UInt16 value);
   
    /**
     * Save numeric data in buffer.
     * Will change internal write counter, see getWritePosition().
     * @retval true write is successful
     * @retval false not enough space in buffer
     */
    bool write( Int32 value);
   
    /**
     * Save numeric data in buffer.
     * Will change internal write counter, see getWritePosition().
     * @retval true write is successful
     * @retval false not enough space in buffer
     */
    bool write(UInt32 value);
   
    /**
     * Save numeric data in buffer.
     * Will change internal write counter, see getWritePosition().
     * @retval true write is successful
     * @retval false not enough space in buffer
     */
    bool write( Int64 value);
   
    /**
     * Save numeric data in buffer.
     * Will change internal write counter, see getWritePosition().
     * @retval true write is successful
     * @retval false not enough space in buffer
     */
    bool write(UInt64 value);
   
    /**
     * Copy raw data into buffer. Analog of std::copy() or memcpy(). Size of
     * pointed raw data is not written to buffer.
     *
     * @param pBuffer pointer to raw data.
     *    If pBuffer points to somewhere in the memory that was used to initialize
     *    this instance of CBuffer, behavior is undefined.
     * @param size size of raw data in bytes
     *
     * @retval true write is successful
     * @retval false not enough space in buffer
     */
    bool write(void const* pBuffer, UInt32 size);

    /**
     * Write the std::string value into buffer. Size of string is written into
     * buffer with the string value.
     *
     * @param value string to be written
     *
     * @retval true write is successful
     * @retval false not enough space in buffer
     */
    bool write(std::string const& value);
   
    /**
     * Write the content of std::pair into buffer.
     * Types K and V must be writable using appropriate write() method,
     * i.e. there must be write() method for given type.
     *
     * @param value of pair
     *
     * @retval true write is successful
     * @retval false not enough space in buffer
     */
    template <typename K, typename V>
    bool write(std::pair<K, V> const& value)
    {
        if (!write(value.first))
        {
            return false;
        }
        if (!write(value.second))
        {
            return false;
        }
        return true;
    }

    /**
     * Write range of values to buffer.
     * Range [first, last) is specified by input iterators.
     *
     * Only std::list, std::vector, std::set, std::map have been tested.
     *
     * Example:
     * @code
     * std::list<std::string> stringList;
     * stringList.push_back("hello");
     * stringList.push_back("world");
     * buffer.write(stringList.begin(), stringList.end());
     * @endcode
     *
     * @param first start of the range
     * @param last end of the range
     *
     * @retval true write is successful
     * @retval false not enough space in buffer
     */
    template <typename InputIter>
    bool write(InputIter first, InputIter last)
    {
        UInt32 savedPos1 = getWritePosition();
        UInt32 size = 0;

        // Sort of hack - we reserve memory for size, but write real value
        // only after writing all elements. It allows to use input iterators
        // for containers with unknown size.
        if (!write(size))
        {
            return false;
        }

        for (; first != last; ++first, ++size)
        {
            if (!write(*first))
            {
                return false;
            }
        }

        UInt32 savedPos2 = getWritePosition();
        setWritePosition(savedPos1);
        if (!write(size))
        {
            return false;
        }
        setWritePosition(savedPos2);
        return true;
    }


    /**
     * Obtain numeric data from buffer.
     * Will change internal read counter, see getReadPosition().
     * @retval true read is successful
     * @retval false not enough space in buffer
     */
    bool read( Int8  & value) const;
    /**
     * Obtain numeric data from buffer.
     * Will change internal read counter, see getReadPosition().
     * @retval true read is successful
     * @retval false not enough space in buffer
     */
    bool read(UInt8  & value) const;
    /**
     * Obtain numeric data from buffer.
     * Will change internal read counter, see getReadPosition().
     * @retval true read is successful
     * @retval false not enough space in buffer
     */
    bool read( Int16 & value) const;
    /**
     * Obtain numeric data from buffer.
     * Will change internal read counter, see getReadPosition().
     * @retval true read is successful
     * @retval false not enough space in buffer
     */
    bool read(UInt16 & value) const;
    /**
     * Obtain numeric data from buffer.
     * Will change internal read counter, see getReadPosition().
     * @retval true read is successful
     * @retval false not enough space in buffer
     */
    bool read( Int32 & value) const;
    /**
     * Obtain numeric data from buffer.
     * Will change internal read counter, see getReadPosition().
     * @retval true read is successful
     * @retval false not enough space in buffer
     */
    bool read(UInt32 & value) const;
    /**
     * Obtain numeric data from buffer.
     * Will change internal read counter, see getReadPosition().
     * @retval true read is successful
     * @retval false not enough space in buffer
     */
    bool read( Int64 & value) const;
    /**
     * Obtain numeric data from buffer.
     * Will change internal read counter, see getReadPosition().
     * @retval true read is successful
     * @retval false not enough space in buffer
     */
    bool read(UInt64 & value) const;

    /**
     * Copy raw data from CBuffer. Analog of std::copy() or memcpy().
     *
     * @param size size in bytes of raw data to be read
     * @param pBuffer pointer to place where raw data will be saved.
     *    Be aware about possible buffer overflow.
     *    If pBuffer points to somewhere in the memory that was used to initialize
     *    this instance of CBuffer, behavior is undefined.
     *
     * @retval true read is successful
     * @retval false not enough space in buffer
     */
    bool read(UInt32 size, void * pBuffer) const;

    /**
     * Read the std::string value from buffer. Size of string is read from
     * buffer with the string value. The string is changed using assign() method.
     *
     * @param value string to be read
     *
     * @retval true read is successful
     * @retval false not enough space in buffer
     */
    bool read(std::string & value) const;

    /**
     * Read the contents of a std::pair from buffer.
     * Types K and V must be readable using the appropriate read() method,
     * i.e. there must be a read() method for given type.
     *
     * @param value of pair
     *
     * @retval true write is successful
     * @retval false not enough space in buffer
     */
    template <typename K, typename V>
    bool read(std::pair<K, V> & val) const
    {
    if (!read(val.first))
    {
        return false;
    }
    if (!read(val.second))
    {
        return false;
    }
    return true;
   }

    /**
     * Read the range of values from buffer.
     * Passed InsertIterator is used to save data. Insert iterator for given
     * collection type can be obtained using std::inserter().
     * Currently, only std::list, std::vector, std::set, std::map are tested.
     *
     * Example:
     * @code
     * std::list<std::string> stringList;
     * buffer.read(std::inserter(stringList, stringList.begin()));
     * @endcode
     *
     * @param result insert iterator used to save data
     *
     * @retval true write is successful
     * @retval false not enough space in buffer
     */
    template <typename InsertIter>
    bool read(InsertIter result) const
    {
        // Here we using _readContainerSelector to determine if container is
        // std::map-like or not and select appropriate read() method.
        return _readContainerSelector< InsertIter,
                TypeTraits::has_mapped_type<typename InsertIter::container_type>::value
                > ::read(result, *this);
    }

    /**
     * Get buffer's capacity (maximum reading/writing position)
     * @return size of buffer in bytes.
     */
    UInt32 getSize() const;

    /**
     * Get buffer's read position as an offset in bytes from the beginning of the buffer.
     * This position is used by all read() methods.
     *
     * @return position in bytes
     */
    UInt32 getReadPosition() const;

    /**
     * Set read position as an offset in bytes from the beginning of the buffer.
     * This position is used by all read() methods.
     *
     * @param pos new read position.
     *
     * @retval true success
     * @retval false buffer size is less than or equal to given position, position is
     *    left unchanged
     */
    bool setReadPosition(UInt32 pos) const;

    /**
     * Get write position as an offset in bytes from the beginning of the buffer.
     * This position is used by all write() methods.
     *
     * @return position in bytes
     */
    UInt32 getWritePosition() const;

    /**
     * Set write position as an offset in bytes from the beginning of the buffer.
     * This position is used by all write() methods.
     *
     * @param pos new write position.
     *
     * @retval true success
     * @retval false buffer size is less than or equal to given position, position is
     *    left unchanged
     */
    bool setWritePosition(UInt32 pos);

    /**
    * Get raw data stored in buffer
    * @return pointer to array with data
    */
    UInt8* getBuffer() const;

private:
 
    /**
     * Generic write method used to store numeric types.
     * @param val value
     * @retval true write is successful
     * @retval false not enough space in buffer
     */
    template <typename T> bool write_internal(T val);

    /**
     * Generic read method used to obtain numeric types.
     * @param val value
     * @retval true read is successful
     * @retval false not enough space in buffer
     */
    template <typename T> bool read_internal(T& val) const;

    /**
     * Helper type used to select in compile-time, which method will be used to
     * read into container.
     */
    template <typename InsertIter, bool isMapType>
    struct _readContainerSelector;

    /**
     * Will be used to read into std::map, e.t.c.
     */
    template <typename InsertIter>
    struct _readContainerSelector<InsertIter, true>
    {
        static bool read(InsertIter result, Buffer const& buf)
        {
            return buf._readMap(result);
        }
    };

    /**
     * Will be used to read into std::vector, std::list, std::set, e.t.c.
     */
    template <typename InsertIter>
    struct _readContainerSelector<InsertIter, false>
    {
        static bool read(InsertIter result, Buffer const& buf)
        {
            return buf._readContainer(result);
        }
    };

    /**
     * Read data into insert iterator created for non-std::map-like container.
     * @param result insert iterator
     * @retval true read is successful
     * @retval false not enough space in buffer
     */
    template <typename InsertIter>
    bool _readContainer(InsertIter result) const
    {
        typedef typename InsertIter::container_type::value_type tVal;
        UInt32 size = 0;

        if (!read(size))
        {
            return false;
        }

        for (UInt32 i = 0; i < size; ++i)
        {
            tVal val;
            if  (!read(val))
            {
                return false;
            }
            *result++ = val;
        }
        return true;
    }

    /**
     * Reads data into insert iterator created for std::map-like container.
     * @param result insert iterator
     * @retval true read is successful
     * @retval false not enough space in buffer
     */
    template <typename InsertIter>
    bool _readMap(InsertIter result) const
    {
        typedef typename InsertIter::container_type::key_type tKey;
        typedef typename InsertIter::container_type::mapped_type tVal;
        UInt32 size = 0;

        if (!read(size))
        {
            return false;
        }

        for (UInt32 i = 0; i < size; ++i)
        {
            std::pair<tKey, tVal> pair;
            if (!read(pair))
            {
                return false;
            }
            *result++ = pair;
        }
        return true;
    }

private:

    /// Size of the memory buffer
    UInt32 mSize;

    /// Reading position
    mutable UInt32 mReadPos;

    /// Writing position
    UInt32 mWritePos;

    /// Pointer to memory buffer
    UInt8* mpBuffer;

    /// If true - data must be prepared for network transfer
    bool mEndianConvert;
};


/**
 * Version of class CBuffer with memory buffer contained inside instance.
 * Size of the buffer is set by SIZE value.
 */
template <size_t SIZE>
class CSBuffer : public Buffer
{
public:
    CSBuffer()
        : Buffer(mBuffer, SIZE)
    {
    }

private:
    UInt8 mBuffer[SIZE];
};

}  // namespace iviLink


#endif //  CBUFFER_HPP__
