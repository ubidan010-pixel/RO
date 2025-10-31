#ifndef ITF_SURFACE_GCM_H_
#define ITF_SURFACE_GCM_H_

#include "VRAMRelocHdl.h"

namespace ITF
{
    namespace GCM
    {
        struct Texture;

        struct Surface : public CellGcmSurface
        {
            Surface();
            ~Surface();
            OffsetPtrRelocHandle mrtVramHdl[CELL_GCM_MRT_MAXCOUNT];
            OffsetPtrRelocHandle depthVramHdl;
            u32 tileAreaIndex[CELL_GCM_MRT_MAXCOUNT]; // u32(-1) if not tiled
            u32 depthTileAreaIndex;
            u32 depthZCullSlot;
        };

        // Tools methods

        // Allocate a rendering surface and assign it a tile area
        // specify msaa -> 0 or 1 = no msaa, 2 = msaa 2x, 4 = msaa 4x rotated
        // Pass parameters width/height without aa size (not the 2x or 4x size)
        // _volatileSurface parameter is just there for correct allocation type
        Surface * CreateTiledSurface(u32 _width, u32 _height, u32 _bpp, u32 _msaa, u32 _gcmLocation = CELL_GCM_LOCATION_LOCAL, bool _volatileSurface = false);

        // This fct is doing the same as above, but for several surface sharing only one tile region.
        // Note that created surfaces have to be deleted together.
        void CreateTiledSurfaceGroup(Surface * surfArray[], u32 nbSurf, u32 _width, u32 _height, u32 _bpp, u32 _msaa, u32 _gcmLocation = CELL_GCM_LOCATION_LOCAL, bool _volatileSurface = false);

        // Allocate a rendering surface in swizzled format (so not tiled)
        // Width & height must be power of 2, only bpp==4 (A8R8G8B8) and bpp==2 (R5G6B5) allowed 
        // Width==1 => height==1
        Surface * CreateSwizzledSurface(u32 _width, u32 _height, u32 _bpp, u32 _gcmLocation = CELL_GCM_LOCATION_LOCAL, bool _volatileSurface = false);

        // Allocate a depth buffer and assign it to the given surface (internally it use the fct CreateTiledDepthBufferSurface)
        // _bpp -> 2 or 4 (Z16 or D24S8)
        // _tileBank is directly mapped to bank parameter of cellGcmSetTileInfo, should be 1..3
        // _disableComp can be set to true if the image tend to get too much high frequency 
        void AddTiledDepthBufferToSurface(u32 _bpp, Surface * _surface, u32 _msaa, bool _bindZCull = true, bool _disableComp = false, u32 _gcmLocation = CELL_GCM_LOCATION_LOCAL, u32 _tileBank = 1);

        // Allocate a depth buffer
        Surface * CreateTiledDepthBufferSurface(u32 _width, u32 _height, u32 _bpp, u32 _msaa, bool _bindZCull = true, bool _disableComp = false, u32 _gcmLocation = CELL_GCM_LOCATION_LOCAL, u32 _tileBank = 1);

        // Assign an already allocated depth buffer surface to the given surface
        void AttachDepthBufferToSurface(const Surface * depthBufferSurface, Surface * surface);

        // choose the best texture format to read in a surface area
        u8 ConvertSurfaceFormatToTextureFormat(u8 _surfaceFormat);

        // Create a texture object referencing the given depth buffer in the surface
        // set seenAsColor to true if you don't want sampling z-test or want to access the stencil value (format ARGB8 or X16)
        Texture * CreateDepthTextureFromSurface(const Surface * _surface, bool _seenAsColor = false);

        // Create a texture object referencing a mrt buffer in the surface
        void SetTextureFromSurface(Texture & _tex, const Surface * _surface, u32 _mrtIndex = 0);
        Texture * CreateTextureFromSurface(const Surface * _surface, u32 _mrtIndex = 0); // using the previous fct
    }
}

#endif // ITF_SURFACE_GCM_H_
