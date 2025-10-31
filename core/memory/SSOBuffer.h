#pragma once

#include "core/types.h"
#include <algorithm>

namespace ITF
{

    // Simple low level class to handle Small Size Optimization buffers:
    // - Allocation is done only when the requested size > InternalSize, before that
    // - The buffer is not initialized (user's responsibility).
    // - Moveable, but keep in mind that moving can touch uninitialized memory (undefined behavior).
    template <typename T, ux InternalSize>
    class SSOBuffer
    {
    public:
        SSOBuffer() = default;
        explicit SSOBuffer(ux _size) { alloc(_size); }

        // movable but not copyable
        SSOBuffer(SSOBuffer && _other)
        {
            if (_other.isUsingInternalBuffer())
            {
                std::copy(_other.m_internal, _other.m_internal + ITF_ARRAY_SIZE(m_internal), m_internal);
            }
            else
            {
                m_data = _other.m_data;
            }
            _other.m_data = _other.m_internal;
        }

        SSOBuffer & operator = (SSOBuffer && _other)
        {
            clear();
            if (_other.isUsingInternalBuffer())
            {
                std::copy(_other.m_internal, _other.m_internal + ITF_ARRAY_SIZE(m_internal), m_internal);
            }
            else
            {
                m_data = _other.m_data;
            }
            _other.m_data = _other.m_internal;
            return *this;
        }

        // we don't know how to copy (no size)
        SSOBuffer(const SSOBuffer & _other) = delete;
        SSOBuffer & operator = (const SSOBuffer & _other) = delete;

        ~SSOBuffer() { clear(); }

        void alloc(ux _size)
        {
            clear();
            if (_size>ITF_ARRAY_SIZE(m_internal))
                m_data = new (MemoryId::mId_Allocator) T[_size];
        }

        void clear()
        {
            if (!isUsingInternalBuffer())
            {
                delete[] m_data;
                m_data = nullptr;
            }
        }

        bool isUsingInternalBuffer() const { return m_data == m_internal; }
        T * get() { return m_data; }
        const T * get() const { return m_data; }

    private:
        T * m_data = m_internal;
        T m_internal[InternalSize];
    };

}
