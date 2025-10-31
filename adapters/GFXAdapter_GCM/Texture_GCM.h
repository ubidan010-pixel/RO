#ifndef ITF_TEXTURE_GCM_H_
#define ITF_TEXTURE_GCM_H_

#include "GCM_API.h"

#include "VRAMRelocHdl.h"

// Texture remap mode

#define GCM_TEX_REMAP_ARGB \
            CELL_GCM_REMAP_MODE( \
                                CELL_GCM_TEXTURE_REMAP_ORDER_XYXY,\
                                CELL_GCM_TEXTURE_REMAP_FROM_A,\
                                CELL_GCM_TEXTURE_REMAP_FROM_R,\
                                CELL_GCM_TEXTURE_REMAP_FROM_G,\
                                CELL_GCM_TEXTURE_REMAP_FROM_B,\
                                CELL_GCM_TEXTURE_REMAP_REMAP,\
                                CELL_GCM_TEXTURE_REMAP_REMAP,\
                                CELL_GCM_TEXTURE_REMAP_REMAP,\
                                CELL_GCM_TEXTURE_REMAP_REMAP )
#define GCM_TEX_REMAP_ABGR \
            CELL_GCM_REMAP_MODE( \
                                CELL_GCM_TEXTURE_REMAP_ORDER_XYXY,\
                                CELL_GCM_TEXTURE_REMAP_FROM_A,\
                                CELL_GCM_TEXTURE_REMAP_FROM_B,\
                                CELL_GCM_TEXTURE_REMAP_FROM_G,\
                                CELL_GCM_TEXTURE_REMAP_FROM_R,\
                                CELL_GCM_TEXTURE_REMAP_REMAP,\
                                CELL_GCM_TEXTURE_REMAP_REMAP,\
                                CELL_GCM_TEXTURE_REMAP_REMAP,\
                                CELL_GCM_TEXTURE_REMAP_REMAP )
#define GCM_TEX_REMAP_DEPTH \
            CELL_GCM_REMAP_MODE( \
                                CELL_GCM_TEXTURE_REMAP_ORDER_XXXY,\
                                CELL_GCM_TEXTURE_REMAP_FROM_A,\
                                CELL_GCM_TEXTURE_REMAP_FROM_R,\
                                CELL_GCM_TEXTURE_REMAP_FROM_G,\
                                CELL_GCM_TEXTURE_REMAP_FROM_B,\
                                CELL_GCM_TEXTURE_REMAP_REMAP,\
                                CELL_GCM_TEXTURE_REMAP_REMAP,\
                                CELL_GCM_TEXTURE_REMAP_REMAP,\
                                CELL_GCM_TEXTURE_REMAP_REMAP )

// following macro are different flavor of remapping for the B8 (default) mode (to ARGB)
#define GCM_TEX_REMAP_B111 \
            CELL_GCM_REMAP_MODE( \
                                CELL_GCM_TEXTURE_REMAP_ORDER_XYXY,\
                                CELL_GCM_TEXTURE_REMAP_FROM_B,\
                                CELL_GCM_TEXTURE_REMAP_FROM_B,\
                                CELL_GCM_TEXTURE_REMAP_FROM_B,\
                                CELL_GCM_TEXTURE_REMAP_FROM_B,\
                                CELL_GCM_TEXTURE_REMAP_REMAP,\
                                CELL_GCM_TEXTURE_REMAP_ONE,\
                                CELL_GCM_TEXTURE_REMAP_ONE,\
                                CELL_GCM_TEXTURE_REMAP_ONE )

#define GCM_TEX_REMAP_1B11 \
            CELL_GCM_REMAP_MODE( \
                                CELL_GCM_TEXTURE_REMAP_ORDER_XYXY,\
                                CELL_GCM_TEXTURE_REMAP_FROM_B,\
                                CELL_GCM_TEXTURE_REMAP_FROM_B,\
                                CELL_GCM_TEXTURE_REMAP_FROM_B,\
                                CELL_GCM_TEXTURE_REMAP_FROM_B,\
                                CELL_GCM_TEXTURE_REMAP_ONE,\
                                CELL_GCM_TEXTURE_REMAP_REMAP,\
                                CELL_GCM_TEXTURE_REMAP_ONE,\
                                CELL_GCM_TEXTURE_REMAP_ONE )

#define GCM_TEX_REMAP_11B1 \
            CELL_GCM_REMAP_MODE( \
                                CELL_GCM_TEXTURE_REMAP_ORDER_XYXY,\
                                CELL_GCM_TEXTURE_REMAP_FROM_B,\
                                CELL_GCM_TEXTURE_REMAP_FROM_B,\
                                CELL_GCM_TEXTURE_REMAP_FROM_B,\
                                CELL_GCM_TEXTURE_REMAP_FROM_B,\
                                CELL_GCM_TEXTURE_REMAP_ONE,\
                                CELL_GCM_TEXTURE_REMAP_ONE,\
                                CELL_GCM_TEXTURE_REMAP_REMAP,\
                                CELL_GCM_TEXTURE_REMAP_ONE )

#define GCM_TEX_REMAP_111B \
            CELL_GCM_REMAP_MODE( \
                                CELL_GCM_TEXTURE_REMAP_ORDER_XYXY,\
                                CELL_GCM_TEXTURE_REMAP_FROM_B,\
                                CELL_GCM_TEXTURE_REMAP_FROM_B,\
                                CELL_GCM_TEXTURE_REMAP_FROM_B,\
                                CELL_GCM_TEXTURE_REMAP_FROM_B,\
                                CELL_GCM_TEXTURE_REMAP_ONE,\
                                CELL_GCM_TEXTURE_REMAP_ONE,\
                                CELL_GCM_TEXTURE_REMAP_ONE,\
                                CELL_GCM_TEXTURE_REMAP_REMAP )

namespace ITF
{
    namespace GCM
    {
        struct Texture : public CellGcmTexture
        {
            Texture();
            OffsetPtrRelocHandle vramHdl;
            bool renderTarget;
            u8 antialias; // AA mode in case of render target
        };

        namespace TextureFormat
        {
            enum Enum
            {
                XRGB32 = CELL_GCM_TEXTURE_D8R8G8B8 | CELL_GCM_TEXTURE_SZ | CELL_GCM_TEXTURE_NR,
                ARGB32 = CELL_GCM_TEXTURE_A8R8G8B8 | CELL_GCM_TEXTURE_SZ | CELL_GCM_TEXTURE_NR,
                DXT1 = CELL_GCM_TEXTURE_COMPRESSED_DXT1 | CELL_GCM_TEXTURE_SZ | CELL_GCM_TEXTURE_NR,
                DXT1N = DXT1,
                DXT3 = CELL_GCM_TEXTURE_COMPRESSED_DXT23 | CELL_GCM_TEXTURE_SZ | CELL_GCM_TEXTURE_NR,
                DXT5 = CELL_GCM_TEXTURE_COMPRESSED_DXT45 | CELL_GCM_TEXTURE_SZ | CELL_GCM_TEXTURE_NR,
                D16Depth = CELL_GCM_TEXTURE_DEPTH16 | CELL_GCM_TEXTURE_SZ | CELL_GCM_TEXTURE_NR,
                D24S8Depth = CELL_GCM_TEXTURE_DEPTH24_D8 | CELL_GCM_TEXTURE_SZ | CELL_GCM_TEXTURE_NR,
                D24FS8Depth = D24S8Depth,

                // special
                A16B16G16R16 = CELL_GCM_TEXTURE_W16_Z16_Y16_X16_FLOAT | CELL_GCM_TEXTURE_SZ | CELL_GCM_TEXTURE_NR,

                A16B16G16R16F = CELL_GCM_TEXTURE_W16_Z16_Y16_X16_FLOAT | CELL_GCM_TEXTURE_SZ | CELL_GCM_TEXTURE_NR,
                A32B32G32R32F = CELL_GCM_TEXTURE_W32_Z32_Y32_X32_FLOAT | CELL_GCM_TEXTURE_SZ | CELL_GCM_TEXTURE_NR,
                L8 = CELL_GCM_TEXTURE_B8 | CELL_GCM_TEXTURE_SZ | CELL_GCM_TEXTURE_NR,
                A8 = L8,
                R32F = CELL_GCM_TEXTURE_X32_FLOAT | CELL_GCM_TEXTURE_SZ | CELL_GCM_TEXTURE_NR,

                // special
                R16F = CELL_GCM_TEXTURE_X32_FLOAT | CELL_GCM_TEXTURE_SZ | CELL_GCM_TEXTURE_NR,

                G16R16 = CELL_GCM_TEXTURE_Y16_X16 | CELL_GCM_TEXTURE_SZ | CELL_GCM_TEXTURE_NR,
                G16R16F = CELL_GCM_TEXTURE_Y16_X16_FLOAT | CELL_GCM_TEXTURE_SZ | CELL_GCM_TEXTURE_NR,
                R5G6B5 = CELL_GCM_TEXTURE_R5G6B5 | CELL_GCM_TEXTURE_SZ | CELL_GCM_TEXTURE_NR,
                A1R5G5B5 = CELL_GCM_TEXTURE_A1R5G5B5 | CELL_GCM_TEXTURE_SZ | CELL_GCM_TEXTURE_NR,

                XRGB32Linear = CELL_GCM_TEXTURE_D8R8G8B8 | CELL_GCM_TEXTURE_LN | CELL_GCM_TEXTURE_NR,
                ARGB32Linear = CELL_GCM_TEXTURE_A8R8G8B8 | CELL_GCM_TEXTURE_LN | CELL_GCM_TEXTURE_NR,
                DXT1Linear = CELL_GCM_TEXTURE_COMPRESSED_DXT1 | CELL_GCM_TEXTURE_LN | CELL_GCM_TEXTURE_NR,
                DXT3Linear = CELL_GCM_TEXTURE_COMPRESSED_DXT23 | CELL_GCM_TEXTURE_LN | CELL_GCM_TEXTURE_NR,
                DXT5Linear = CELL_GCM_TEXTURE_COMPRESSED_DXT45 | CELL_GCM_TEXTURE_LN | CELL_GCM_TEXTURE_NR,
                D16DepthLinear = CELL_GCM_TEXTURE_DEPTH16 | CELL_GCM_TEXTURE_LN | CELL_GCM_TEXTURE_NR,
                D24S8DepthLinear = CELL_GCM_TEXTURE_DEPTH24_D8 | CELL_GCM_TEXTURE_LN | CELL_GCM_TEXTURE_NR,
                D24FS8DepthLinear = D24S8DepthLinear,

                // special
                A16B16G16R16Linear = CELL_GCM_TEXTURE_W16_Z16_Y16_X16_FLOAT | CELL_GCM_TEXTURE_LN | CELL_GCM_TEXTURE_NR,

                A16B16G16R16FLinear = CELL_GCM_TEXTURE_W16_Z16_Y16_X16_FLOAT | CELL_GCM_TEXTURE_LN | CELL_GCM_TEXTURE_NR,
                A32B32G32R32FLinear = CELL_GCM_TEXTURE_W32_Z32_Y32_X32_FLOAT | CELL_GCM_TEXTURE_LN | CELL_GCM_TEXTURE_NR,
                L8Linear = CELL_GCM_TEXTURE_B8 | CELL_GCM_TEXTURE_LN | CELL_GCM_TEXTURE_NR,
                A8Linear = L8Linear,
                R32FLinear = CELL_GCM_TEXTURE_X32_FLOAT | CELL_GCM_TEXTURE_LN | CELL_GCM_TEXTURE_NR,

                // special
                R16FLinear = CELL_GCM_TEXTURE_X32_FLOAT | CELL_GCM_TEXTURE_LN | CELL_GCM_TEXTURE_NR,

                G16R16Linear = CELL_GCM_TEXTURE_Y16_X16 | CELL_GCM_TEXTURE_LN | CELL_GCM_TEXTURE_NR,
                G16R16FLinear = CELL_GCM_TEXTURE_Y16_X16_FLOAT | CELL_GCM_TEXTURE_LN | CELL_GCM_TEXTURE_NR,
                R5G6B5Linear = CELL_GCM_TEXTURE_R5G6B5 | CELL_GCM_TEXTURE_LN | CELL_GCM_TEXTURE_NR,
                A1R5G5B5Linear = CELL_GCM_TEXTURE_A1R5G5B5 | CELL_GCM_TEXTURE_LN | CELL_GCM_TEXTURE_NR,

                D16_ATI = D16Depth,
                D24_ATI = D24S8Depth,
            };
        }
        
        // tools fcts
        f32 GetBytePerPixelFactor(u8 _gcmTexFormat); // can also be called with TextureFormat::Enum

        // GetPitch return the pitch for the full TexFormat, to assign to CellGcmTexture::pitch
        // ie. return 0 for swizzled textures
        u32 GetPitch(u8 _gcmTexFormat, u32 _width, bool _isRenderTarget);
        // GetNbBytes return the size in bytes for a 2D texture of specific format
        u32 GetNbBytes(u8 _gcmFormat, u32 _width, u32 _height, u32 nbMips);
        // GetMemSize return the size in bytes taken by a GCM texture (1D, 2D, 3D, cubemap, ...)
        u32 GetMemSize(Texture * _tex);
        // Create a texture
        Texture * CreateTexture(u32 _width, u32 _height, u32 _depth, u32 _mipLevels, TextureFormat::Enum _format, bool _renderTarget = false, bool _allocInSystemRam = false, bool _cubeMap = false, u8 _dimension = 2);

        // Load texture from raw data of a gtf file
        Texture * CreateTextureFromGTFRawData(u8 * _rawData);
    }
}

#endif // ITF_TEXTURE_GCM_H_
