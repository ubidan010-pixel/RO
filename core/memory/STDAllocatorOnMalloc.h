#pragma once

#include <stdlib.h>

namespace ITF
{

    // Use STDAllocatorOnMalloc wisely:
    // It's purpose is to not go through the ITF allocator, so to be out of any memory measurement.
    // Typically, use it in a memory measure tool to avoid reintrance.
    template <typename T>
    class STDAllocatorOnMalloc
    {
    public:

        using value_type = T;
        using size_type = size_t;

        using is_always_equal = std::true_type;

        constexpr STDAllocatorOnMalloc() noexcept {}
        constexpr STDAllocatorOnMalloc(const STDAllocatorOnMalloc&) noexcept = default;

        template <typename U>
        constexpr STDAllocatorOnMalloc(const STDAllocatorOnMalloc<U>&) noexcept {}

        constexpr bool operator == (const STDAllocatorOnMalloc&) const noexcept { return true; }
        constexpr bool operator != (const STDAllocatorOnMalloc&) const noexcept { return false; }

        T* allocate(size_t count)
        {
            // 0 size check to not be dependent on the ::malloc implementation for size = 0.
            return static_cast<T*>(count != 0 ? private_allocate(count * sizeof(value_type)) : nullptr);
        }

        void deallocate(T* ptr, size_t count)
        {
            private_deallocate(ptr);
        }

    private:
        void* private_allocate(size_t count) const
        {
            return ::malloc(count);
        }

        void private_deallocate(void* ptr) const
        {
            return ::free(ptr);
        }
    };

}
