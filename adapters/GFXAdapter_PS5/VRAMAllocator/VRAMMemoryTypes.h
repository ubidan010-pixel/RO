#ifndef VRAMMEMORYTYPES_H_
#define VRAMMEMORYTYPES_H_

namespace ITF::Gnmp
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

    enum class VRAMMemoryBus : ux
    {
        DIRECT,
        Count,
    };

    enum VRAMResourceType : ux
    {
        READ_WRITE,
        RENDER_TARGET = READ_WRITE,
        READ_ONLY,
        RESOURCE = READ_ONLY,
        VRAMResourceType_Count,
    };

    // types relative to vram stuff
    typedef u8 * VRAMAddress;
    typedef u8 VRAMLocation;
    typedef u32 VRAMOffset;
    
    namespace VRAMAllocator
    {
        typedef u32 ChunkIdx;
        typedef u32 ChunkSize; // max size of a pool -> 4GiB 
    }

    typedef VRAMAllocator::ChunkSize VRAMChunkSize;

    // struct to get stats
    struct VRAMStats
    {
        u32 directFreeSize;
        u32 directTotalSize;
        u32 directBiggestBlockSize;
    };

} // ITF::Gnmp

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

#endif // VRAMMEMORYTYPES_H_
