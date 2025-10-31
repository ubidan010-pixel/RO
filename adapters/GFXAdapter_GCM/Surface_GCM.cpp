#include "precompiled_GFXAdapter_GCM.h"
#include "VRAMAllocator/VRAMTools.h"
#include "Texture_GCM.h"
#include "Surface_GCM.h"

namespace ITF
{
    namespace GCM
    {
        Surface::Surface()
        {
            CellGcmSurface * gcmSurface = this;
            memset(gcmSurface, 0, sizeof(*gcmSurface)); // memset allowed as CellGcmSurface is a pod

            // bind the relocHdl to the CellGcmSurface structure
            for(u32 i = 0; i<CELL_GCM_MRT_MAXCOUNT; ++i)
            {
                mrtVramHdl[i].SetRelocPtr(&(colorOffset[i]), &(colorLocation[i]));
                tileAreaIndex[i] = VRAMAllocator::TileAreaAllocator::InvalidTileIndex;
            }
            depthVramHdl.SetRelocPtr(&depthOffset, &depthLocation);
            depthTileAreaIndex = VRAMAllocator::TileAreaAllocator::InvalidTileIndex;
            depthZCullSlot = VRAMAllocator::ZCullMemoryManager::InvalidRegionIndex;
        }

        Surface::~Surface()
        {
            for(u32 i = 0; i<CELL_GCM_MRT_MAXCOUNT; ++i)
            {
                if (tileAreaIndex[i] != VRAMAllocator::TileAreaAllocator::InvalidTileIndex)
                {
                    VRAMAllocator::TileAreaAllocator::get().releaseIndex(tileAreaIndex[i]);
                }
            }
            if (depthZCullSlot != VRAMAllocator::ZCullMemoryManager::InvalidRegionIndex)
            {
                VRAMAllocator::ZCullMemoryManager::get().unBindRegion(depthZCullSlot);
            }
            if (depthTileAreaIndex != VRAMAllocator::TileAreaAllocator::InvalidTileIndex)
            {
                VRAMAllocator::TileAreaAllocator::get().releaseIndex(depthTileAreaIndex);
            }
        }


        static const u32  TiledBufferAlignment			= 64*1024;
        static const u32  SwizzledBufferAlignment       = 128; // same as texture
        static const u32  TiledHeightAlignmentLocal			= 32; // 32 is enough as soon as we are in local memory
        static const u32  TiledHeightAlignmentMain			= 64; // 32 is enough as soon as we are in local memory
        static const u32  DepthBufferZCullWidthAlignment	= 64;
        static const u32  DepthBufferZCullHeightAlignment	= 64;
        // Tile compression offset -> incremented at each creation
        static u32		BaseTileCompOffset				= 0;

        // Allocate a rendering surface and assign it a tile area
        // specify msaa -> 0 or 1 = no msaa, 2 = msaa 2x, 4 = msaa 4x rotated
        // Pass parameters width/height without aa size (not the 2x or 4x size)
        // _volatileSurface parameter is just there for correct allocation type
        Surface * CreateTiledSurface(u32 _width, u32 _height, u32 _bpp, u32 _msaa, u32 _gcmLocation, bool _volatileSurface)
        {
            Surface * _surf[1];
            CreateTiledSurfaceGroup(_surf, 1, _width, _height, _bpp, _msaa, _gcmLocation, _volatileSurface);
            return _surf[0];

            u8 colorFormat;
            if(_bpp==4)
            {
                colorFormat = CELL_GCM_SURFACE_A8R8G8B8;
            }
            else if(_bpp==2)
            {
                colorFormat = CELL_GCM_SURFACE_R5G6B5;
            }
            else if(_bpp==8)
            {
                colorFormat = CELL_GCM_SURFACE_F_W16Z16Y16X16;
            }
            else
            {
                colorFormat = CELL_GCM_SURFACE_B8;
            }

            u32 msaaAdjustedBpp = ((_msaa==2||_msaa==4)?2:1)*_bpp;
            u32 tileEntry = VRAMAllocator::TileAreaAllocator::get().allocUnusedIndex();
            bool useTiling = tileEntry!=VRAMAllocator::TileAreaAllocator::InvalidTileIndex;

            Surface * surface = new Surface;
            surface->type = CELL_GCM_SURFACE_PITCH; // only possible value when using tiling
            // Antialias -> note that in 4X case, CELL_GCM_SURFACE_SQUARE_ROTATED_4 is prefered to CELL_GCM_SURFACE_SQUARE_CENTERED_4
            surface->antialias = _msaa==2?CELL_GCM_SURFACE_DIAGONAL_CENTERED_2:(_msaa==4?CELL_GCM_SURFACE_SQUARE_ROTATED_4:CELL_GCM_SURFACE_CENTER_1);
            surface->colorFormat = colorFormat;
            surface->colorTarget = CELL_GCM_SURFACE_TARGET_0; // no MRT
            surface->colorLocation[0] = CELL_GCM_LOCATION_LOCAL; // will be set by allocation anyway
            if (useTiling)
            {
                surface->colorPitch[0] = cellGcmGetTiledPitchSize(_width*msaaAdjustedBpp);
            }
            else
            {
                surface->colorPitch[0] = _width*msaaAdjustedBpp;
            }
            surface->tileAreaIndex[0] = tileEntry;
            surface->depthTileAreaIndex = u32(-1);
            surface->depthZCullSlot = u32(-1);
            for(int i=1; i<CELL_GCM_MRT_MAXCOUNT;++i)
            {
                surface->colorPitch[i] = 64;
                surface->tileAreaIndex[i] = u32(-1);
            }

            // no depth buffer
            surface->depthFormat = CELL_GCM_SURFACE_Z24S8;
            surface->depthLocation = 0;
            surface->depthPitch = 64;
            surface->depthVramHdl = InvalidRelocHandle;

            // windows
            surface->width = _width;
            surface->height = _height;
            surface->x = 0;
            surface->y = 0;

            // allocate area
            // height has to be 32 aligned when in local memory (64 in main memory)
            u32 tileHeightAlignment = (_gcmLocation == CELL_GCM_LOCATION_MAIN?TiledHeightAlignmentMain:TiledHeightAlignmentLocal);
            u32 effectiveHeight = GetAboveAlign(_height*(_msaa==4?2:1), tileHeightAlignment);
            // Surface size = pitch*height
            u32 oneSurfSize = surface->colorPitch[0] * effectiveHeight;
            // memory block size -> align on 64K
            u32 blockSize = GetAboveAlign(oneSurfSize, TiledBufferAlignment);
            if (_gcmLocation == CELL_GCM_LOCATION_MAIN)
                surface->mrtVramHdl[0].MainAllocAligned(blockSize, TiledBufferAlignment, _volatileSurface ? VRAM_Volatile : VRAM_RenderTarget);
            else
                surface->mrtVramHdl[0].LocalAllocAligned(blockSize, TiledBufferAlignment, _volatileSurface ? VRAM_Volatile : VRAM_RenderTarget);

            // set tile area
            if (useTiling)
            {			
                u16 base = 0;
                u8 compMode = _msaa==2?CELL_GCM_COMPMODE_C32_2X1:(_msaa==4?CELL_GCM_COMPMODE_C32_2X2:CELL_GCM_COMPMODE_DISABLED);
                if(compMode != CELL_GCM_COMPMODE_DISABLED)
                {
                    base = BaseTileCompOffset;
                    BaseTileCompOffset += blockSize/0x10000;
                }
                cellGcmSetTileInfo(tileEntry, surface->colorLocation[0],
                                   surface->colorOffset[0], blockSize, surface->colorPitch[0],
                                   compMode, base, 0);
                cellGcmBindTile(tileEntry);
            }
            return surface;
        }

        void CreateTiledSurfaceGroup(Surface * _surfArray[], u32 _nbSurf, u32 _width, u32 _height, u32 _bpp, u32 _msaa, u32 _gcmLocation, bool _volatileSurface)
        {
            u8 colorFormat;
            if(_bpp==4)
            {
                colorFormat = CELL_GCM_SURFACE_A8R8G8B8;
            }
            else if(_bpp==2)
            {
                colorFormat = CELL_GCM_SURFACE_R5G6B5;
            }
            else if(_bpp==8)
            {
                colorFormat = CELL_GCM_SURFACE_F_W16Z16Y16X16;
            }
            else
            {
                colorFormat = CELL_GCM_SURFACE_B8;
            }

            u32 msaaAdjustedBpp = ((_msaa==2||_msaa==4)?2:1)*_bpp;
            u32 tileEntry = VRAMAllocator::TileAreaAllocator::get().allocUnusedIndex();
            bool useTiling = tileEntry!=VRAMAllocator::TileAreaAllocator::InvalidTileIndex;

            Surface surface;
            surface.type = CELL_GCM_SURFACE_PITCH; // only possible value when using tiling
            // Antialias -> note that in 4X case, CELL_GCM_SURFACE_SQUARE_ROTATED_4 is prefered to CELL_GCM_SURFACE_SQUARE_CENTERED_4
            surface.antialias = _msaa==2?CELL_GCM_SURFACE_DIAGONAL_CENTERED_2:(_msaa==4?CELL_GCM_SURFACE_SQUARE_ROTATED_4:CELL_GCM_SURFACE_CENTER_1);
            surface.colorFormat = colorFormat;
            surface.colorTarget = CELL_GCM_SURFACE_TARGET_0; // no MRT
            surface.colorLocation[0] = _gcmLocation; // will be set by allocation anyway
            if (useTiling)
            {
                surface.colorPitch[0] = cellGcmGetTiledPitchSize(_width*msaaAdjustedBpp);
            }
            else
            {
                surface.colorPitch[0] = GetAboveAlign(_width*msaaAdjustedBpp, 64u);
            }
            surface.tileAreaIndex[0] = tileEntry;
            surface.depthTileAreaIndex = u32(-1);
            surface.depthZCullSlot = u32(-1);
            for(int i=1; i<CELL_GCM_MRT_MAXCOUNT;++i)
            {
                surface.colorPitch[i] = 64;
                surface.tileAreaIndex[i] = u32(-1);
            }

            // no depth buffer
            surface.depthFormat = CELL_GCM_SURFACE_Z24S8;
            surface.depthLocation = 0;
            surface.depthPitch = 64;
            surface.depthVramHdl = InvalidRelocHandle;

            // windows
            surface.width = _width;
            surface.height = _height;
            surface.x = 0;
            surface.y = 0;
            ITF_ASSERT(_width!=1u || _height==1u); // constraint

            // The surface is used as a reference for all the surfaces in the array. 
            // The vram reloc hdl is referencing the same block, with an offset.

            // allocate area
            // height has to be 32 aligned when in local memory (64 in main memory)
            u32 tileHeightAlignment = (_gcmLocation == CELL_GCM_LOCATION_MAIN?TiledHeightAlignmentMain:TiledHeightAlignmentLocal);
            u32 effectiveHeight = GetAboveAlign(_height*(_msaa==4?2:1), tileHeightAlignment);
            // Surface size = pitch*height
            u32 oneSurfSize = surface.colorPitch[0] * effectiveHeight;
            if (_nbSurf>1)
            {
                // As we want to put several surfaces, need to align on pitch*8 and 64K
                u32 pitchRelativeAlignment = u32(surface.colorPitch[0]) * 8u;
                u32 alignedBelowSurfSize = (oneSurfSize / pitchRelativeAlignment) * pitchRelativeAlignment;
                if (alignedBelowSurfSize != oneSurfSize) // not already aligned -> just align now
                {
                    oneSurfSize = alignedBelowSurfSize + pitchRelativeAlignment;
                }
                // now looping adding pitch*8 until aligned on 64K
                while(GetAboveAlign(oneSurfSize, 64u*1024u) != oneSurfSize)
                {
                    oneSurfSize += pitchRelativeAlignment;
                } 
            }
            // memory block size -> align on 64K
            u32 blockSize = GetAboveAlign(oneSurfSize * _nbSurf, TiledBufferAlignment);
            if (_gcmLocation == CELL_GCM_LOCATION_MAIN)
                surface.mrtVramHdl[0].MainAllocAligned(blockSize, TiledBufferAlignment, _volatileSurface ? VRAM_Volatile : VRAM_RenderTarget);
            else
                surface.mrtVramHdl[0].LocalAllocAligned(blockSize, TiledBufferAlignment, _volatileSurface ? VRAM_Volatile : VRAM_RenderTarget);
            // assign all surfaces
            for(u32 i = 0; i<_nbSurf; ++i)
            {
                _surfArray[i] = new Surface;
                (*_surfArray[i]) = surface;
                VRAMAllocator::TileAreaAllocator::get().incRefIndex(_surfArray[i]->tileAreaIndex[0]);
                _surfArray[i]->mrtVramHdl[0] += i*oneSurfSize; // offset
            }

            // finally bind tile area
            if (useTiling)
            {
                u16 base = 0;
                u8 compMode = _msaa==2?CELL_GCM_COMPMODE_C32_2X1:(_msaa==4?CELL_GCM_COMPMODE_C32_2X2:CELL_GCM_COMPMODE_DISABLED);
                if(compMode != CELL_GCM_COMPMODE_DISABLED)
                {
                    base = BaseTileCompOffset;
                    BaseTileCompOffset += blockSize/0x10000;
                }
                cellGcmSetTileInfo(tileEntry, surface.colorLocation[0],
                    surface.colorOffset[0], blockSize, surface.colorPitch[0],
                    compMode, base, 0);
                cellGcmBindTile(tileEntry);
            }
        }

        Surface * CreateSwizzledSurface(u32 _width, u32 _height, u32 _bpp, u32 _gcmLocation, bool _volatileSurface)
        {
            ITF_ASSERT(IsPow2(_width));
            ITF_ASSERT(IsPow2(_height));
            u8 colorFormat;
            if(_bpp==4)
            {
                colorFormat = CELL_GCM_SURFACE_A8R8G8B8;
            }
            else
            {
                ITF_ASSERT(_bpp == 2);
                colorFormat = CELL_GCM_SURFACE_R5G6B5;
            }

            Surface * surface = new Surface;
            surface->type = CELL_GCM_SURFACE_SWIZZLE; // only possible value when using tiling
            surface->antialias = CELL_GCM_SURFACE_CENTER_1;
            surface->colorFormat = colorFormat;
            surface->colorTarget = CELL_GCM_SURFACE_TARGET_0; // no MRT
            surface->colorLocation[0] = _gcmLocation; // will be set by allocation anyway
            surface->colorPitch[0] = GetAboveAlign(_width*_bpp, 64u); // minimal pitch -> 64
            for(int i=1; i<CELL_GCM_MRT_MAXCOUNT;++i)
            {
                surface->colorPitch[i] = 64;
            }

            // no depth buffer
            surface->depthFormat = CELL_GCM_SURFACE_Z24S8;
            surface->depthLocation = 0;
            surface->depthPitch = 64;
            surface->depthVramHdl = InvalidRelocHandle;

            // windows
            surface->width = _width;
            surface->height = _height;
            surface->x = 0;
            surface->y = 0;
            ITF_ASSERT(_width!=1u || _height==1u); // constraint

            // allocate area
            // memory block size -> align size and offset on 128 bytes (same as texture)
            u32 blockSize = GetAboveAlign(surface->colorPitch[0] * _height, SwizzledBufferAlignment);
            if (_gcmLocation == CELL_GCM_LOCATION_MAIN)
                surface->mrtVramHdl[0].MainAllocAligned(blockSize, SwizzledBufferAlignment, _volatileSurface ? VRAM_Volatile : VRAM_RenderTarget);
            else
                surface->mrtVramHdl[0].LocalAllocAligned(blockSize, SwizzledBufferAlignment, _volatileSurface ? VRAM_Volatile : VRAM_RenderTarget);
            return surface;
        }

        void AddTiledDepthBufferToSurface(u32 _bpp, Surface * _surface, u32 _msaa, bool _bindZCull, bool _disableComp, u32 _gcmLocation, u32 _tileBank)
        {
            u32 width = (_msaa==2||_msaa==4?2:1) * _surface->width;
            u32 height = (_msaa==4?2:1) * _surface->height;

            u32 tileEntry = VRAMAllocator::TileAreaAllocator::get().allocUnusedIndex();
            bool useTiling = tileEntry!=VRAMAllocator::TileAreaAllocator::InvalidTileIndex;
            _surface->depthTileAreaIndex = tileEntry;
            _surface->depthFormat = _bpp==2?CELL_GCM_SURFACE_Z16:CELL_GCM_SURFACE_Z24S8;
            _surface->depthLocation = _gcmLocation;
            _surface->depthPitch = useTiling?cellGcmGetTiledPitchSize(width*_bpp):width*_bpp;

            u32 tileHeightAlignment = (_gcmLocation == CELL_GCM_LOCATION_MAIN?TiledHeightAlignmentMain:TiledHeightAlignmentLocal);
            u32 depthBlockSize = GetAboveAlign(_surface->depthPitch * GetAboveAlign(height, tileHeightAlignment), TiledBufferAlignment);
            _surface->depthVramHdl.AllocAligned(depthBlockSize, TiledBufferAlignment, VRAM_DepthBuffer);

            // set tile area
            // it can actually be faster to just use CELL_GCM_COMPMODE_Z32_SEPSTENCIL
            if (useTiling)
            {
                u8 compMode = CELL_GCM_COMPMODE_Z32_SEPSTENCIL;
                u16	base = 0;

                if (_bpp==2)
                {
                    compMode = CELL_GCM_COMPMODE_DISABLED;
                }
                else if (_surface->antialias == CELL_GCM_SURFACE_DIAGONAL_CENTERED_2)
                {
                    compMode = CELL_GCM_COMPMODE_Z32_SEPSTENCIL_DIAGONAL;		
                }
                else if (_surface->antialias == CELL_GCM_SURFACE_SQUARE_ROTATED_4)
                {
                    compMode = CELL_GCM_COMPMODE_Z32_SEPSTENCIL_ROTATED;
                }
                else if (_surface->antialias == CELL_GCM_SURFACE_SQUARE_CENTERED_4
                    || _surface->antialias == CELL_GCM_SURFACE_CENTER_1)
                {
                    compMode = CELL_GCM_COMPMODE_Z32_SEPSTENCIL_REGULAR;
                }

                if(_disableComp)
                {
                    compMode = CELL_GCM_COMPMODE_DISABLED;
                }

                if(compMode != CELL_GCM_COMPMODE_DISABLED)
                {
                    base = BaseTileCompOffset;
                    BaseTileCompOffset += depthBlockSize/0x10000;
                }
                cellGcmSetTileInfo( tileEntry, _surface->depthLocation,
                                    _surface->depthOffset, depthBlockSize, _surface->depthPitch,
                                    compMode, base, _tileBank);
                cellGcmBindTile(tileEntry);
            }

            // check zBuffer size for alignment (cant bind a buffer in main memory)
            if(_bindZCull && _surface->depthLocation == CELL_GCM_LOCATION_LOCAL)
            {
                VRAMAllocator::ZCullMemoryManager::get().bindRegion(_surface->depthOffset, _surface->width, _surface->height, _surface->depthFormat, _surface->antialias);
            }
        }

        Surface * CreateTiledDepthBufferSurface(u32 _width, u32 _height, u32 _bpp, u32 _msaa, bool _bindZCull, bool _disableComp, u32 _gcmLocation, u32 _tileBank)
        {
            Surface * surface = new Surface();
            surface->type = CELL_GCM_SURFACE_PITCH;
            surface->antialias = _msaa==2?CELL_GCM_SURFACE_DIAGONAL_CENTERED_2:(_msaa==4?CELL_GCM_SURFACE_SQUARE_CENTERED_4:CELL_GCM_SURFACE_CENTER_1);
            surface->colorTarget = CELL_GCM_SURFACE_TARGET_NONE;
            surface->colorFormat = CELL_GCM_SURFACE_A8R8G8B8;
            for(int i=0; i<CELL_GCM_MRT_MAXCOUNT;++i)
            {
                surface->colorPitch[i] = 64;
                surface->tileAreaIndex[i] = -1;
            }
            // windows
            surface->width = _width;
            surface->height = _height;  
            surface->x = 0;
            surface->y = 0;

            // call the previous fct
            AddTiledDepthBufferToSurface(_bpp, surface, _msaa, _bindZCull, _disableComp, _gcmLocation, _tileBank);

            return surface;
        }

        void AttachDepthBufferToSurface(const Surface * _depthBufferSurface, Surface * _surface)
        {
            _surface->depthFormat = _depthBufferSurface->depthFormat;
            _surface->depthLocation = _depthBufferSurface->depthLocation;
            _surface->depthPitch = _depthBufferSurface->depthPitch;

            _surface->depthVramHdl = _depthBufferSurface->depthVramHdl;
            _surface->depthTileAreaIndex = _depthBufferSurface->depthTileAreaIndex;

            // we could add check there
        }


        u8 ConvertSurfaceFormatToTextureFormat(u8 _surfaceFormat)
        {
            switch (_surfaceFormat)
            {
            case CELL_GCM_SURFACE_X1R5G5B5_Z1R5G5B5:
            case CELL_GCM_SURFACE_X1R5G5B5_O1R5G5B5:
                return CELL_GCM_TEXTURE_A1R5G5B5;
            case CELL_GCM_SURFACE_R5G6B5:
                return CELL_GCM_TEXTURE_R5G6B5;
            case CELL_GCM_SURFACE_X8R8G8B8_Z8R8G8B8:
            case CELL_GCM_SURFACE_X8R8G8B8_O8R8G8B8:
            case CELL_GCM_SURFACE_A8R8G8B8:
                return CELL_GCM_TEXTURE_A8R8G8B8;
            case CELL_GCM_SURFACE_B8:
                return CELL_GCM_TEXTURE_B8;
            case CELL_GCM_SURFACE_G8B8:
                return CELL_GCM_TEXTURE_G8B8;
            case CELL_GCM_SURFACE_F_W16Z16Y16X16:
                return CELL_GCM_TEXTURE_W16_Z16_Y16_X16_FLOAT;
            case CELL_GCM_SURFACE_F_W32Z32Y32X32:
                return CELL_GCM_TEXTURE_W32_Z32_Y32_X32_FLOAT;
            case CELL_GCM_SURFACE_F_X32:
                return CELL_GCM_TEXTURE_X32_FLOAT;
            case CELL_GCM_SURFACE_X8B8G8R8_Z8B8G8R8 :
            case CELL_GCM_SURFACE_X8B8G8R8_O8B8G8R8 :
            case CELL_GCM_SURFACE_A8B8G8R8 :
                return CELL_GCM_TEXTURE_A8R8G8B8; // Those are the ABGR surfaces. Swapping has to be done in texture input cross or shader
            default:
                return CELL_GCM_TEXTURE_A8R8G8B8; // nothing left, but lets be safe
            }
        }

        // set seenAsColor to true if you don't want sampling z-test or want to access the stencil value (set mode ARGB or X16)
        Texture * CreateDepthTextureFromSurface(const Surface * _surface, bool _seenAsColor)
        {
            ITF_ASSERT(_surface!=0); // ? no depth buffer in this surface
            ITF_ASSERT(_surface->depthVramHdl.GetAddress()!=0); // ? no depth buffer in this surface
            Texture * tex = new Texture();

            tex->vramHdl = _surface->depthVramHdl;
            if (_seenAsColor)
            {
                tex->format = _surface->depthFormat==CELL_GCM_SURFACE_Z16?CELL_GCM_TEXTURE_X16:CELL_GCM_TEXTURE_A8R8G8B8;
            }
            else
            {
                tex->format = _surface->depthFormat==CELL_GCM_SURFACE_Z16?CELL_GCM_TEXTURE_DEPTH16:CELL_GCM_TEXTURE_DEPTH24_D8;
            }
            tex->format |= CELL_GCM_TEXTURE_LN | CELL_GCM_TEXTURE_NR; // linear as probably in a tile area
            tex->mipmap = 1;
            tex->dimension = CELL_GCM_TEXTURE_DIMENSION_2;
            tex->cubemap = CELL_GCM_FALSE;
            tex->remap = GCM_TEX_REMAP_ARGB;

            tex->width = _surface->width;
            tex->height = _surface->height;
            tex->depth = 1;
            tex->location = _surface->depthLocation;
            tex->pitch = _surface->depthPitch; // hmmm, maybe a problem here because of excessive tiled pitch

            tex->renderTarget = true;
            tex->antialias = _surface->antialias;

            return tex;
        }

        void SetTextureFromSurface(Texture & _tex, const Surface * _surface, u32 _mrtIndex)
        {
            _tex.vramHdl = _surface->mrtVramHdl[_mrtIndex];
            _tex.format = ConvertSurfaceFormatToTextureFormat(_surface->colorFormat);
            if (_surface->type == CELL_GCM_SURFACE_PITCH)
            {
                ITF_ASSERT(_surface->tileAreaIndex[_mrtIndex] != VRAMAllocator::TileAreaAllocator::InvalidTileIndex);
                _tex.format |= CELL_GCM_TEXTURE_LN | CELL_GCM_TEXTURE_NR; // linear as in a tile area
            }
            else
            {
                // if swizzled -> not tiled
                ITF_ASSERT(_surface->tileAreaIndex[_mrtIndex] == VRAMAllocator::TileAreaAllocator::InvalidTileIndex);
                ITF_ASSERT(IsPow2(_surface->width) && IsPow2(_surface->height));
                _tex.format |= CELL_GCM_TEXTURE_SZ | CELL_GCM_TEXTURE_NR; // swizzled
            }
            _tex.mipmap = 1;
            _tex.dimension = CELL_GCM_TEXTURE_DIMENSION_2;
            _tex.cubemap = CELL_GCM_FALSE;

            if (_surface->colorFormat == CELL_GCM_SURFACE_X8B8G8R8_Z8B8G8R8
                || _surface->colorFormat == CELL_GCM_SURFACE_X8B8G8R8_O8B8G8R8
                || _surface->colorFormat == CELL_GCM_SURFACE_A8B8G8R8)
            {
                // special remap
                _tex.remap = GCM_TEX_REMAP_ABGR;
            }
            else
            {
                _tex.remap = GCM_TEX_REMAP_ARGB;
            }

            _tex.width = _surface->width;
            _tex.height = _surface->height;
            _tex.depth = 1;
            _tex.location = _surface->colorLocation[_mrtIndex];
            _tex.pitch = _surface->colorPitch[_mrtIndex]; // hmmm, maybe a problem here because of excessive tiled pitch

            _tex.renderTarget = true;
            _tex.antialias = _surface->antialias;
        }

        Texture * CreateTextureFromSurface(const Surface * _surface, u32 _mrtIndex)
        {
            Texture * tex = new Texture();
            SetTextureFromSurface(*tex, _surface, _mrtIndex);
            return tex;
        }
    }
}
