#pragma once

#include "DX12/Helpers.h"

namespace ITF::DX12
{
    enum VramMemoryCategory
    {
        VRAM_Unknown = 0,   // U
        VRAM_RenderTarget,  // R
        VRAM_DepthBuffer,   // D
        VRAM_Texture,       // T
        VRAM_VertexBuffer,  // V
        VRAM_IndexBuffer,   // I
        VRAM_Shader,        // S
        VRAM_Volatile,      // O
        VRAM_Last           // count
    };

    enum class VRAMResourceType : ux
    {
        RESOURCE_UPLOAD, // CPU writable resource (upload heap)
        RESOURCE, // GPU only resource (default heap)
        Count,
    };

    // types relative to vram stuff
    typedef u8 * VRAMAddress;
    typedef u8 VRAMLocation;
    typedef u32 VRAMOffset;
    
    namespace VRAMAllocator
    {
        typedef u32 ChunkIdx;
        typedef u32 ChunkSize; // max size of a pool -> 4GiB

        // Simple information of memory block, a memory pool, an offset, and a size.
        struct GPUStorage
        {
            ComPtr<ID3D12Heap> memoryPool = nullptr;
            VRAMOffset offset = 0;
            ChunkSize size = 0;
        };
    }

    typedef VRAMAllocator::ChunkSize VRAMChunkSize;

    // struct to get stats
    struct VRAMStats
    {
        u32 directFreeSize;
        u32 directTotalSize;
        u32 directBiggestBlockSize;
    };

} // ITF::DX12

#ifndef ITF_RETAIL
    #define VRAM_PRINT_INFO
#endif
#if !defined(ITF_DISABLE_DEBUGINFO) || !defined(ITF_RETAIL)
    #define VRAM_ALLOC_COUNT
#endif


#if defined(ITF_DEBUG) && defined(ASSERT_ENABLED)
    #define VRAM_ALLOC_ASSERT ITF_ASSERT
    #define VRAM_ALLOC_ASSERT_ENABLED 1
#else
    #define VRAM_ALLOC_ASSERT(...)
#endif

#ifndef ITF_RETAIL
    #define VRAM_ALLOC_TRACE(...)    fprintf(stdout, ##  __VA_ARGS__)
    #define VRAM_ALLOC_TRACE_ENABLED 1
#else
    #define VRAM_ALLOC_TRACE(...)
#endif
