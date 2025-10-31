#pragma once

#include "core/memory/memory.h"

namespace ITF
{

    template <typename T, MemoryId::ITF_ALLOCATOR_IDS tag = MemoryId::mId_Unknow>
    class STDAllocatorOnITFMemory
    {
    public:

        using value_type = T;
        using size_type = size_t;

        using is_always_equal = std::true_type;

        constexpr STDAllocatorOnITFMemory() noexcept {}
        constexpr STDAllocatorOnITFMemory(const STDAllocatorOnITFMemory&) noexcept = default;

        // Allow copy construction from any tag
        template <typename U, MemoryId::ITF_ALLOCATOR_IDS otherTag>
        constexpr STDAllocatorOnITFMemory(const STDAllocatorOnITFMemory<U, otherTag>& other) noexcept {}

        constexpr bool operator == (const STDAllocatorOnITFMemory&) const noexcept { return true; }
        constexpr bool operator != (const STDAllocatorOnITFMemory&) const noexcept { return false; }

        // rebind, keep the same tag
        template <typename U>
        struct rebind
        {
            using other = STDAllocatorOnITFMemory<U, tag>;
        };

        T* allocate(size_t count)
        {
            return static_cast<T*>(ITF::Memory::mallocCategory(count * sizeof(T), tag));
        }
        void deallocate(T* ptr, size_t count)
        {
            ITF::Memory::free(ptr);
        }
    };

}
