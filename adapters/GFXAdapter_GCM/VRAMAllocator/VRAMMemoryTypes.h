#ifndef VRAMMEMORYTYPES_H_
#define VRAMMEMORYTYPES_H_

namespace ITF
{
namespace GCM
{
    // Value is on *3 bits* [0..7]
    enum VramMemoryType
    {
        VRAM_Unknown = 0,   // U
        VRAM_RenderTarget,  // R
        VRAM_DepthBuffer,   // D
        VRAM_Texture,       // T
        VRAM_VertexBuffer,  // V
        VRAM_IndexBuffer,   // I
        VRAM_Shader,        // S
        VRAM_Volatile,      // O
        VRAM_Last           // max
    };

    // types relative to vram stuff
    typedef u8 * VRAMAddress;
    typedef u8 VRAMLocation;
    typedef u32 VRAMOffset;
    
    namespace VRAMAllocator
    {
        typedef u32 ChunkIdx;
        typedef u32 ChunkSize;
    }

    typedef VRAMAllocator::ChunkSize VRAMChunkSize;

    // struct to get stats
    struct VRAMStats
    {
        u32 vramFreeSize;
        u32 vramTotalSize;
        u32 mainFreeSize;
        u32 mainTotalSize;
        u32 vramBiggestBlockSize;
        u32 mainBiggestBlockSize;
    };

} // GCM
} // ITF

#ifndef ITF_FINAL
    #define VRAM_PRINT_INFO
#endif
#ifndef ITF_DISABLE_DEBUGINFO
    #define VRAM_ALLOC_COUNT
#endif

#ifdef ITF_DEBUG
    #define VRAM_ALLOC_ASSERT ITF_ASSERT
#else
    #define VRAM_ALLOC_ASSERT(...)
#endif

#ifndef ITF_FINAL
    #define VRAM_ALLOC_TRACE(...)    fprintf(stdout, ##  __VA_ARGS__)
#else
    #define VRAM_ALLOC_TRACE(...)
#endif

#endif // VRAMMEMORYTYPES_H_
