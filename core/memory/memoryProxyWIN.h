#ifndef _ITF_MEMORYPROXYWIN_H_
#define _ITF_MEMORYPROXYWIN_H_

#pragma once

namespace ITF
{

class MemoryProxy
{

public:

    static u32 getITFAlignmentPhysicalFromAttribute(u32 _platformAttribute)
    {
         return MEM_ALIGNMENT_4;
    }

    static ITF_INLINE u32 getITFAttributeProtectFlagsFromX360(u32 _protectX360)
    {
        u32 res = 0;
        return res;
    }

    static u32 getAlignmentPhysicalFromITF(u32 _platformITF)
    {
        return MEM_ALIGNMENT_16;
    }

    static ITF_INLINE u32 getITFAttributeProtectFlags(u32 _protect)
    {
        u32 res = 0;
        return res;
    }

    static ITF_INLINE u32 getAttributeProtectFlagsFromITF(uPtr _memory, u32 _protectITF)
    {
        u32 protect = 0;
        return protect;
    }

    static void* memoryHeapAllocFunction(size_t _size,size_t _align,u32 _protectFlagsITF)
    {
         return VirtualAlloc(NULL,_size, MEM_COMMIT,_protectFlagsITF);
    }


    static void* memoryPhysicalAllocFunction(size_t _size,size_t _align,u32 _protectFlagsITF)
    {
        ITF_ASSERT(0);
        return NULL;  
    }

    static void memoryPhysicalFreeFunction(void* _ptr)
    {
        ITF_ASSERT(0);
    }

};

}

#endif //_ITF_MEMORYPROXYWIN_H_
