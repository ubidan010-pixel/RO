#pragma once

#include "core/types.h"
#include "core/memory/slotallocator.h"

namespace ITF
{
    class MemoryProxy
    {

    public:

        static u32 getITFAlignmentPhysicalFromAttribute(u32 _platformAttribute)
        {
             return MEM_ALIGNMENT_4;
        }

        static u32 getITFAttributeProtectFlagsFromX360(u32 _protectX360)
        {
            u32 res = 0;
            return res;
        }

        static u32 getAlignmentPhysicalFromITF(u32 _platformITF)
        {
            return MEM_ALIGNMENT_16;
        }

        static u32 getITFAttributeProtectFlags(u32 _protect)
        {
            u32 res = 0;
            return res;
        }


        static u32 getAttributeProtectFlagsFromITF(u32 _memory,u32 _protectITF)
        {
            u32 protect = 0;
            return protect;
        }

        static void* memoryHeapAllocFunction(size_t _size,size_t _align,u32 _protectFlagsITF)
        {
            ITF_ASSERT_CRASH(0, "Not implemented");
            return nullptr;
        }

        static void* memoryPhysicalAllocFunction(size_t _size,size_t _align,u32 _protectFlagsITF)
        {
            ITF_ASSERT_CRASH(0, "Not implemented");
            return nullptr;  
        }

        static void memoryPhysicalFreeFunction(void* _ptr)
        {
            ITF_ASSERT_CRASH(0, "Not implemented");
        }
    };
}
