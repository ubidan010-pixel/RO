#ifndef _ITF_FIXEDARRAY_H_
#define _ITF_FIXEDARRAY_H_

#ifndef _ITF_CORETYPES_H_
#include "core/types.h"
#endif //_ITF_CORETYPES_H_

#ifndef _ITF_ARCHIVE_H_
#include "core/Archive.h"
#endif //_ITF_ARCHIVE_H_

#ifndef _ITF_CRC_H_
#include "core/crc.h"
#endif //_ITF_CRC_H_

namespace ITF
{

template <class T, u32 arraySize>
class FixedArray
{
public:

    FixedArray()
    {
        ITF_ASSERT(arraySize);

        m_elemCount = 0;
    }

    ~FixedArray()
    {
    }

    u32 capacity() const { return arraySize; }
    u32 size() const { return m_elemCount; }
    bbool empty() const { return m_elemCount == 0; }
    bbool full() const { return size() == capacity(); }
    void clear() { m_elemCount = 0; }

    bool operator==(const FixedArray& other) const
    {
       return size() == other.size() &&
              memcmp(m_data, other.m_data, size() * sizeof(T)) == 0;
    }

    T& operator [] ( u32 _index )
    {
        ITF_ASSERT_CRASH(_index < size(),"Accessing array out of bounds");

        return m_data[_index];
    }

    const T& operator [] ( u32 _index ) const
    {
        ITF_ASSERT_CRASH(_index < size(),"Accessing array out of bounds");

        return m_data[_index];
    }

    void push_back( const T& _data )
    {
        ITF_ASSERT_CRASH(size() < capacity(),"Pushing element into full array");

        m_data[m_elemCount++] = _data;
    }

    void eraseKeepOrder( u32 _index )
    {
        ITF_ASSERT_CRASH(_index < size(),"Accessing array out of bounds");

        if ( m_elemCount > 1 )
        {
            memmove(&m_data[_index], &m_data[_index + 1], sizeof(T) * (m_elemCount - _index - 1));
        }

        m_elemCount--;
    }

    void eraseNoOrder( u32 _index )
    {
        ITF_ASSERT_CRASH(_index < size(),"Accessing array out of bounds");

        m_elemCount--;

        if ( m_elemCount )
        {
            m_data[_index] = m_data[m_elemCount];
        }
    }

    u32 find( const T& _data) const
    {
        for (u32 i = 0; i < size(); i++)
        {
            if (m_data[i] == _data)
                return i;
        }

        return U32_INVALID;
    }

    u32 findStartIndex( const T& _data, u32 _startIndex  ) const
    {
        ITF_ASSERT_CRASH(_startIndex <= size(),"Accessing array out of bounds");

        for (u32 i = _startIndex; i < size(); i++)
        {
            if (m_data[i] == _data)
                return i;
        }

        return U32_INVALID;
    }

    void insert( const T& _data, u32 _atIndex )
    {
        ITF_ASSERT_CRASH(!full() && _atIndex <= size(),"Accessing array out of bounds or pushing element on an array that is full.");

        if (_atIndex == size())
        {
            push_back(_data);
            return;
        }

        memmove(&m_data[_atIndex+1], &m_data[_atIndex], sizeof(T) * (size() - _atIndex));
        m_elemCount++;
        m_data[_atIndex] = _data;
    }

    void resize( size_t _numElements )
    {
        ITF_ASSERT_CRASH(u32(_numElements)<=capacity(),"Resizing an array to a size bigger than its capacity.");
        m_elemCount = u32(_numElements);
    }

    ITF_INLINE void prefetch() const
    {
        #ifdef ITF_SUPPORT_FETCH
        const u32 memoryLen = arraySize*sizeof(T);
        for( u32 i = 0; i < memoryLen; i += ITF_CACHELINE )
            ITF_CACHEFETCH( i, m_data);
        #endif // ITF_SUPPORT_FETCH
    }

    FixedArray& operator=( const FixedArray& _src )
    {
        m_elemCount = _src.size();

        // I don't want to do a memcpy because I want to be able to use this Array with any type of structure or class
        for ( u32 i = 0; i < size(); i++ )
        {
            m_data[i] = _src[i];
        }

        return *this;
    }

    u32 computeCRC()
    {
        return CRC::compute32( (unsigned char*)m_data, sizeof(T) * m_elemCount);
    }

private:

    T                   m_data[arraySize];
    u32                 m_elemCount;
};

} // namespace ITF

#endif // _ITF_FIXEDARRAY_H_
