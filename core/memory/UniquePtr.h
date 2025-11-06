#pragma once

#include <memory>
#include <type_traits>
#include "core/memory/memoryId.h"

namespace ITF
{
    template<typename T, typename D = std::default_delete<T>>
    using UniquePtr = std::unique_ptr<T, D>;


    template <::ITF::MemoryId::ITF_ALLOCATOR_IDS memId, typename T, typename... CtrArgs>
    inline
        std::enable_if_t< !std::is_array_v<T>, UniquePtr<T> >
        makeUnique(CtrArgs &&... _args)
    {
        return UniquePtr<T>(new (memId) T(std::forward<CtrArgs>(_args)...));
    }

    template <::ITF::MemoryId::ITF_ALLOCATOR_IDS memId, typename T>
    inline
        std::enable_if_t< std::is_array_v<T> && std::extent_v<T> == 0, UniquePtr<T> >
        makeUnique(size_t _size)
    {
        using Element = std::remove_extent_t<T>;
        return UniquePtr<T>(new (memId) Element[_size]());
    }

    template<::ITF::MemoryId::ITF_ALLOCATOR_IDS memId, class T, typename... CtrArgs>
    typename std::enable_if_t<std::extent<T>::value != 0, void>::type makeUnique(CtrArgs&&...) = delete;

    // Static cast for a unique ptr with the default deleter.
    // If you need to support a custom deleter, it needs to be done by hand (release, cast, construct).
    template <typename Derived, typename Base>
    UniquePtr<Derived> static_cast_unique_ptr(UniquePtr<Base> && p)
    {
        auto d = static_cast<Derived*>(p.release()); // release and static cast.
        return UniquePtr<Derived>(d); // build the other and move the deleter
    }
}
