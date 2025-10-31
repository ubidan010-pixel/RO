#include "precompiled_GFXAdapter_GCM.h"
#include "core/math/BitTweak.h"
#include "VRAMAllocator/VRAMTools.h"
#include "Texture_GCM.h"
#include "GTF/gtfloader.h"
#include "engine/compression/EdgeUncompress_PS3.h"

namespace ITF
{
    namespace GCM
    {
        static const u32 TextureAlignment = 128;

        Texture::Texture()
            : renderTarget(0)
            , antialias(CELL_GCM_SURFACE_CENTER_1)
        {
            CellGcmTexture * gcmTex = this;
            memset(gcmTex, 0, sizeof(*gcmTex)); // memset as CellGcmTexture is a pod
            vramHdl.SetRelocPtr(&offset, &location); // bind the relocHdl to the CellGcmTexture structure
        }

        // returns byte per pixel for given format
        f32 GetBytePerPixelFactor(u8 _gcmTexFormat)
        {
            // remove additional flag
            _gcmTexFormat = _gcmTexFormat & (~(CELL_GCM_TEXTURE_SZ|CELL_GCM_TEXTURE_LN|CELL_GCM_TEXTURE_NR|CELL_GCM_TEXTURE_UN));
            float f = 1.f;
            switch(_gcmTexFormat)
            {
            case CELL_GCM_TEXTURE_D8R8G8B8:
            case CELL_GCM_TEXTURE_A8R8G8B8:
                f = 4.f; break;
            case CELL_GCM_TEXTURE_COMPRESSED_DXT1:
                f = 0.5f; break;
            case CELL_GCM_TEXTURE_COMPRESSED_DXT23:
            case CELL_GCM_TEXTURE_COMPRESSED_DXT45:
                f = 1.f; break;
            case CELL_GCM_TEXTURE_DEPTH24_D8:
            case CELL_GCM_TEXTURE_DEPTH24_D8_FLOAT:
                f = 4.f; break;
            case CELL_GCM_TEXTURE_W16_Z16_Y16_X16_FLOAT:
                f = 8.f; break;
            case CELL_GCM_TEXTURE_W32_Z32_Y32_X32_FLOAT:
                f = 16.f; break;
            case CELL_GCM_TEXTURE_B8:
                f = 1.f; break;
            case CELL_GCM_TEXTURE_X32_FLOAT:
                f = 4.f; break;
            case CELL_GCM_TEXTURE_Y16_X16:
            case CELL_GCM_TEXTURE_Y16_X16_FLOAT:
                f = 4.f; break;
            case CELL_GCM_TEXTURE_R5G6B5:
            case CELL_GCM_TEXTURE_A1R5G5B5:
            case CELL_GCM_TEXTURE_DEPTH16:
                f = 2.f; break;
            default:
                ITF_ASSERT_MSG(0, "Unknown texture format, cannot recover BytePerPixel.\n");
            }
            return f;
        }

        u32 GetPitch(u8 _gcmTexFormat, u32 _width, bool _isRenderTarget)
        {
            f32 bpp = GetBytePerPixelFactor(_gcmTexFormat);
            u32 pitch = u32(ceil(f32(_width) * bpp));

            if(_isRenderTarget)
            {
                // need to align the pitch to 64 for the rebuild surface from tex
                return GetAboveAlign(pitch, 64u);
            }
            else if(_gcmTexFormat & CELL_GCM_TEXTURE_LN)
            {
                return pitch;
            }
            else // swizzle format -> pitch is 0
            {
                // assume all swizzled tex when not render target (faster to read)
                ITF_ASSERT(_isRenderTarget == false);
                return 0;
            }
        }

        u32 GetNbBytes(u8 _gcmFormat, u32 _width, u32 _height, u32 nbMips)
        {
            float bpp = GetBytePerPixelFactor(_gcmFormat);
            u32 bytes = 0;
            
            u32 mipWidth = _width;
            u32 mipHeight = _height;
            for(int i=0;
                i<nbMips;
                    ++i,
                    mipWidth = mipWidth >> 1,
                    mipHeight = mipHeight >> 1
                    )
            {
                u32 adjustedWidth = mipWidth;
                u32 adjustedHeight = mipHeight;

                // linear -> add padding on width
                if (_gcmFormat & CELL_GCM_TEXTURE_LN)
                {
                    adjustedWidth = _width;
                }
                // DXT -> align on 4x4 blocks (and min size is 4)
                if (    (_gcmFormat & CELL_GCM_TEXTURE_COMPRESSED_DXT1)
                    ||  (_gcmFormat & CELL_GCM_TEXTURE_COMPRESSED_DXT23)
                    ||  (_gcmFormat & CELL_GCM_TEXTURE_COMPRESSED_DXT45))
                {
                    adjustedWidth = GetAboveAlign(std::max(adjustedWidth, 4u), 4u);
                    adjustedHeight = GetAboveAlign(std::max(adjustedHeight, 4u), 4u);
                }
                bytes += u32(ceil(f32(adjustedWidth)*f32(adjustedHeight)*bpp));
            }
            return bytes;
        }

        // GetMemSize return the size in bytes taken by a GCM texture (1D, 2D, 3D, cubemap, ...)
        u32 GetMemSize(Texture * tex)
        {
            u32 xSize = tex->width;
            u32 ySize = tex->height;
            f32 bpp =  GetBytePerPixelFactor(tex->format);

            if ( (tex->pitch !=0)  && (tex->pitch != (xSize*bpp)) )
            {
                // must add padding to the texture 
                u32 padding = tex->pitch - (xSize*u32(bpp));
                padding	= (u32)(f32(padding)/bpp);
                xSize += padding;
            }

            int size = GetNbBytes(tex->format, xSize, ySize, u32(tex->mipmap));

            if (tex->cubemap==CELL_GCM_TRUE)
            {
                size *= 6;
            }
            else if (tex->dimension == CELL_GCM_TEXTURE_DIMENSION_3)
            {
                size *= tex->depth;
            }
            return size;
        }

        static inline TextureFormat::Enum ConvertToLinear(TextureFormat::Enum format)
        {
            return (TextureFormat::Enum)((format & (~CELL_GCM_TEXTURE_SZ))|CELL_GCM_TEXTURE_LN);
        }

        static inline bool IsB8(TextureFormat::Enum format)
        {
            int nativeFormat = (format & ~(CELL_GCM_TEXTURE_SZ | CELL_GCM_TEXTURE_LN | CELL_GCM_TEXTURE_NR | CELL_GCM_TEXTURE_UN));
            return (nativeFormat == CELL_GCM_TEXTURE_B8);
        }

        Texture * CreateTexture(u32 _width, u32 _height, u32 _depth, u32 _mipLevels, TextureFormat::Enum _format, bool _renderTarget, bool _allocInSystemRam, bool _cubeMap, u8 _dimension)
        {
            // for render targets, be sure to not be swizzled
            if (_renderTarget)
            {
                _format = ConvertToLinear(_format);
            }

            // DXT textures are pow2 or linear (DXT*Linear)
            ITF_ASSERT(_format != TextureFormat::DXT1 || (IsPow2(_width) && IsPow2(_height)));
            ITF_ASSERT(_format != TextureFormat::DXT3 || (IsPow2(_width) && IsPow2(_height)));
            ITF_ASSERT(_format != TextureFormat::DXT5 || (IsPow2(_width) && IsPow2(_height)));
            ITF_ASSERT(!(IsPow2(_width) && IsPow2(_height)) || (_format & CELL_GCM_TEXTURE_SZ)==0);

            // tweak mipmap level
            if (_mipLevels == 0)
            {
                _mipLevels = std::min(std::max(GetLog2(_width), GetLog2(_height)), 13u); //biggest mipmap maximized at 13
            }

            // allocate header
            Texture * tex = new Texture;
            tex->format			= _format;
            tex->mipmap			= _mipLevels;
            tex->dimension		= _dimension;
            tex->cubemap		= u8(_cubeMap);
            tex->remap			= IsB8(_format) ? GCM_TEX_REMAP_B111 : GCM_TEX_REMAP_ARGB; // B8 -> A8
            tex->width			= _width;
            tex->height			= _height;
            tex->depth			= _depth;
            tex->renderTarget	= _renderTarget;
            tex->pitch			= GetPitch(_format, _width, _renderTarget);
            tex->antialias      = CELL_GCM_SURFACE_CENTER_1;

            u32 blockSize = GetAboveAlign(GetMemSize(tex), 16u);

            if (_allocInSystemRam)
            {
                tex->vramHdl.MainAllocAligned(blockSize, TextureAlignment, _renderTarget?VRAM_RenderTarget:VRAM_Texture); 
            }
            else
            {
                // let the policy decide...
                tex->vramHdl.AllocAligned(blockSize, TextureAlignment, _renderTarget?VRAM_RenderTarget:VRAM_Texture);
            }

            if (tex->vramHdl.GetAddress()==0)
            {
                delete tex;
                return 0;
            }
            return tex;
        }

        void MemCopyToVram(u8 * _destData, const u8 * _srcData, ux _size)
        {
            if (_size<100*1024)
            {
                memcpy(_destData, _srcData, _size);
            }
            else
            {
                // use the memcopy option of EdgeZLib
                EdgeUnCompress::copyBuffer(_destData, _size, _srcData);
            }
        }

        Texture * CreateTextureFromGTFRawData(u8 * _rawData)
        {
            CellGtfFileHeader header;
            int res  = cellGtfReadFileHeaderFromMemory(_rawData, &header);
            ITF_ASSERT(res>=0);
            ITF_ASSERT(header.numTexture==1);
            CellGtfTextureAttribute attr;
            Texture * tex = new Texture;
            res = cellGtfReadTextureAttributeFromMemory((const uint8_t*) _rawData, 0, &attr, tex);
            ITF_ASSERT(res>=0);

            int textureSize = attr.textureSize;
            const u8* rawTextureData = _rawData + attr.offsetToTex;
            // Should not have a linear texture here, but jump over it; one possibilty is non-power of two textures...
            int textureSizeAligned = BitTweak::GetAboveAlign(textureSize, TextureAlignment);
            tex->vramHdl.AllocAligned(textureSizeAligned, TextureAlignment, VRAM_Texture); // update the CellGcmTexture members
            tex->renderTarget = false;
            tex->antialias = CELL_GCM_SURFACE_CENTER_1;

            // Now copy the texture to VRAM (slow 4GB/s path)
            MemCopyToVram(tex->vramHdl.GetAddress(), rawTextureData, textureSizeAligned);

            return tex;
        }
    }
}
