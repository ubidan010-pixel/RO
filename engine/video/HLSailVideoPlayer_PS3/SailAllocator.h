/* SCE CONFIDENTIAL
 * PlayStation(R)3 Programmer Tool Runtime Library 360.001
 * Copyright (C) 2010 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#ifndef __HLPLAYER_INTERNAL_ALLOCATOR_H__
#define __HLPLAYER_INTERNAL_ALLOCATOR_H__

#include <stdlib.h>
#include <string.h>
#include <cell/cell_fs.h>
#include <cell/gcm.h>

namespace cell {
namespace Sail {

class memallocator
{
public:
    memallocator(void) { }
    virtual ~memallocator(void) { }
    virtual void* Allocate(uint32_t size, uint32_t alignment=0)
    {
        return ITF::Memory::alignedMalloc(size, alignment);
    };

    virtual void Deallocate(void* pMemory)
    {
        ITF::Memory::alignedFree(pMemory);
    };

    virtual void* AllocateTexture(uint32_t size, uint32_t alignment=0)
    {
        const uint32_t align_mask = 0xfffff; // Meg mask
        size = (size + align_mask) & (~align_mask);
        void * texAddr = ITF::Memory::alignedMalloc(size, alignment);
        if (texAddr)
        {
            unsigned int mainBaseOffset;
            if (CELL_OK != cellGcmMapMainMemory(texAddr, size, &mainBaseOffset))
            {
                ITF::Memory::alignedFree(texAddr);
                texAddr = NULL;
            }
        }
        return texAddr;
    };

    virtual void DeallocateTexture(void* pMemory)
    {
        cellGcmUnmapEaIoAddress(pMemory);
        ITF::Memory::alignedFree(pMemory);
    };
};

} // namespace Sail
} // namespace cell

#endif //  __HLPLAYER_INTERNAL_ALLOCATOR_H__

