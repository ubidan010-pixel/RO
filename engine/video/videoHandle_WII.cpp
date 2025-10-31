#include "precompiled_engine.h"

#include <revolution/sc.h>

#include "engine/video/videoHandle.h"

#ifndef _ITF_WII_DRAWPRIM_H_
#include "adapters/GFXAdapter_WII/GFXAdapter_WII.h"
#endif //_ITF_WII_DRAWPRIM_H_

#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#ifndef _ITF_TRCManagerAdapter_H_
#include "engine/AdaptersInterfaces/TRCManager_Adapter.h"
#endif //_ITF_TRCManagerAdapter_H_

#ifndef _ITF_SYSTEMADAPTER_WII_H_
#include "adapters/SystemAdapter_WII/SystemAdapter_WII.h"
#endif //_ITF_SYSTEMADAPTER_WII_H_

#ifndef _ITF_GFXADAPTER_RENDERTARGET_H_
#include "engine/AdaptersInterfaces/GFXAdapter_RenderTarget.h"
#endif // _ITF_GFXADAPTER_RENDERTARGET_H_

RADDEFFUNC void *   radmalloc(U32);
RADDEFFUNC void     radfree(void *);

namespace  ITF
{
#ifdef USE_ITF_IO
	static videoHandle *	g_CurrentVideo = NULL;    
#endif // USE_ITF_IO

/*
 =======================================================================================================================
    Bink allocation
 =======================================================================================================================
 */
static void *RADLINK BINK_CALLBACK_Alloc(U32 size)
{
    return Memory::alignedMallocCategory(sizeof(u8) * size, 32, MemoryId::mId_GfxAdapter);
}

/*
 =======================================================================================================================
    Bink Free
 =======================================================================================================================
 */
static void RADLINK BINK_CALLBACK_Free(void *Adr)
{
    Memory::alignedFree(Adr);
}

/*
 =======================================================================================================================
    Manage Endianess
 =======================================================================================================================
 */
static void BINK_SwapEndianess(void *dst, u32 size)
{
    if(size)
    {
        u32 u32_Size = size / sizeof(u32);
        u32 *pdest = (u32 *)dst;
        for(u32 i=0 ; i<u32_Size ; ++i)
            pdest[i] = (u32)__lwbrx(&pdest[i],0);
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static U32 nextpow2(U32 x)
{
    return 1 << (32 - __cntlzw(x));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void fill_offset_buffer(U8 * buffer, U32 sourcewidth, U32 offwidth)
{
    U32 finalwidth = sourcewidth / 8;

    S32 srcx = 0;
    S32 srcy = 0;

    for(S32 y=0 ; y<4 ; ++y)
    {
        for(S32 x=0 ; x<finalwidth ; ++x)
        {
            U32 texoffset   = (y * offwidth + x) * 2;
            S32 xoffset     = srcx - x;
            S32 yoffset     = srcy - y;

            buffer[texoffset  ] = (U8)(xoffset + 128);
            buffer[texoffset+1] = (U8)(yoffset + 128);

            if(++srcy == 4)
            {
                srcy = 0;
                ++srcx;
            }
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void swizzle_offsets(U8 * swizz, U8 const * offsets, U32 sourcewidth, U32 sourceheight, U32 bytesperpixel)
{
    // Only works (tested, anyways) for 8 and 16 bit formats for the moment.
    // Texture tiling format is specified in the GX programmer's
    // guide, Appendix D, as of this writing.

    // Not super efficient, but it doesn't really have to be.

    U32 pitch       = sourcewidth * bytesperpixel;
    U32 tilesw      = pitch / 8;
    U32 tilesh      = sourceheight / 4;
    U32 swizzoffset = 0;

    for(U32 tiley=0 ; tiley<tilesh ; ++tiley)
    {
        for(U32 tilex=0 ; tilex<tilesw ; ++tilex)
        {
            U32 x            = tilex * 8;
            U32 y            = tiley * 4;
            U32 sourceoffset = y * pitch + x;

            for(U32 row=0 ; row<4 ; ++row)
            {
                U32 * dest = (U32*)(swizz   + swizzoffset  + (row * 8    ));
                U32 * src  = (U32*)(offsets + sourceoffset + (row * pitch));

                dest[0] = src[0];
                dest[1] = src[1];
            }

            swizzoffset += 32;
        }
    }
}

/*
 =======================================================================================================================
 @brief		Create a texture while allocating the memory ourselves. 
 =======================================================================================================================
 */
static S32 make_texture
    (
        U32             pitch,
        U32             height,
        GXTexFmt        format,
        GXTexWrapMode   type,
        GXTexObj *      texture,
        void *          ptr
    )
{
    GXInitTexObj
    (
        texture,
        ptr,
        (U16)pitch,
        (U16)height,
        format,
        type,
        type,
        GX_FALSE
    );
    GXInitTexObjLOD
    (
        texture,
        GX_NEAR,
        GX_NEAR,
        0.f,
        0.f,
        0.f,
        GX_FALSE,
        0,
        GX_ANISO_1
    );
    return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static U32 divisible128(U32 val)
{
    return (val + 127) & ~127;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static U32 swizz_div(U32 sourcewidth)
{
    // The width of the offset texture is the source/8, but it must be
    // padded out to both the next power of 2.  (Also the next
    // multiple of 4, for very small textures.)

    u32 offwidth = ((sourcewidth / 8) + 3) & ~3;

    if(offwidth & (offwidth - 1))
    {
        // must be a power of 2
        offwidth = nextpow2(offwidth);
    }

    return offwidth;
}

/*
 =======================================================================================================================
 @brief		Create a texture object for the inverse swizzle pattern
 =======================================================================================================================
 */
static U32 make_swizzler
    (
        void        *   data,
        GXTexObj    *   obj,
        U32         *   out_swidth,
        U32         *   out_sheight,
        U32             sourcewidth
    )
{
    U32 offwidth = swizz_div(sourcewidth);

    *out_swidth  = offwidth;
    *out_sheight = 4;

    U32 offsize = divisible128(offwidth * 2 * 4);

    // Fill the buffer, and swizzle the result
    U8 buffer[256 * 4 * 2];
    fill_offset_buffer(buffer, sourcewidth, offwidth);
    swizzle_offsets((U8*)data, buffer, offwidth, 4, 2);

    DCFlushRange(data, offsize);

    make_texture(offwidth, 4, GX_TF_IA8, GX_REPEAT, obj, data);

    return offsize;
}

/*
 =======================================================================================================================
 @brief		Make the two deswizzler textures (which are only around 512 bytes long)
 =======================================================================================================================
 */
static void do_deswizzlers(BINKTEXTURESET * set_textures, U8 * data)
{
    data += 
        make_swizzler
        (
            data,
            &set_textures->YAdeswizzle, 
            &set_textures->YAdeswizzle_width,
            &set_textures->YAdeswizzle_height,
            set_textures->bink_buffers.YABufferWidth
        );
    data += 
        make_swizzler
        (
            data,
            &set_textures->cRcBdeswizzle, 
            &set_textures->cRcBdeswizzle_width,
            &set_textures->cRcBdeswizzle_height,
            set_textures->bink_buffers.cRcBBufferWidth
        );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void calc_layout
    (
        BINKFRAMEBUFFERS    *   buffers,
        U32                 *   cRoffset,
        U32                 *   cBoffset,
        U32                 *   Aoffset,
        U32                 *   framesize,
        U32                 *   swizzlesize,
        U32                 *   YApitch,
        U32                 *   cRcBpitch
    )
{
	U32 ywidth  	= (buffers->Frames[0].YPlane.Allocate ) ? buffers->YABufferWidth   : 0;
	U32 awidth 		= (buffers->Frames[0].APlane.Allocate ) ? buffers->YABufferWidth   : 0;
	U32 crcbwidth 	= (buffers->Frames[0].cRPlane.Allocate) ? buffers->cRcBBufferWidth : 0;
	U32 yaheight 	= buffers->YABufferHeight;
	U32 crcbheight 	= buffers->cRcBBufferHeight;

    U32 ysize, asize, crcbsize;
    ysize       = divisible128(ywidth    * yaheight);
    asize       = divisible128(awidth    * yaheight);
    crcbsize    = divisible128(crcbwidth * crcbheight);

    *swizzlesize =  divisible128(swizz_div(buffers->YABufferWidth  ) * 2 * 4) +
                    divisible128(swizz_div(buffers->cRcBBufferWidth) * 2 * 4);

    *Aoffset    = ysize;
    *cRoffset   = ysize + asize;
    *cBoffset   = ysize + asize + crcbsize;
    *framesize  = ysize + asize + 2 * crcbsize;
    *YApitch    = ywidth | awidth; // they will be the same, or one will be zero
    *cRcBpitch  = crcbwidth;
}

/*
 =======================================================================================================================
 @brief		Free the textures that we allocated.   
 =======================================================================================================================
 */
void Free_Bink_textures(videoHandle::BINKSET * _set)
{
    if(_set->mb_UseAFXBfr)
    {
        _set->m_TextureSet.base_ptr = 0;
        GFX_ADAPTER_WII->unlockAFXBuffer();
        _set->mb_UseAFXBfr = bfalse;
    }

    if(_set->m_TextureSet.base_ptr)
    {
        radfree(_set->m_TextureSet.base_ptr);
        _set->m_TextureSet.base_ptr = 0;
    }

    if(_set->m_RT)
    {
        _set->m_RT->getTexture()->m_adapterimplementationData = 0;
        GFX_ADAPTER->deleteRenderTarget(_set->m_RT);
        _set->m_RT = NULL;
    }
}

/*
 =======================================================================================================================
 @brief		Allocate 2 sets of textures for Bink to decompress into... 
 =======================================================================================================================
 */
S32 Create_Bink_textures(HBINK _bink, videoHandle::BINKSET * _set)
{
    BINKTEXTURESET * set_textures = &_set->m_TextureSet;

    U32 cRoffset;
    U32 cBoffset;
    U32 Aoffset;
    U32 framesize;
    U32 swizzlesize;
    U32 YApitch;
    U32 cRcBpitch;
    calc_layout
    (
        &set_textures->bink_buffers,
        &cRoffset,
        &cBoffset,
        &Aoffset,
        &framesize,
        &swizzlesize,
        &YApitch,
        &cRcBpitch
    );

    /* Videos have to be smaller than 1024 for the dependent texture read to work */
    if(YApitch > 1024)
        return 0;

    f32 w = (f32) _bink->Width;
    f32 h = (f32) _bink->Height;

	/* alloc memory */
	U32 bfrSize = framesize * set_textures->bink_buffers.TotalFrames + swizzlesize + 127;
    U32 texSize = GXGetTexBufferSize(w, h, GFX_ADAPTER_WII->PixFormat_TO_GXTexFmt(Texture::PF_RGBA), GX_FALSE, 0);
    U8 * data = GFX_ADAPTER_WII->lockAFXBuffer(bfrSize+texSize);
    if(data)
    {
        _set->mb_UseAFXBfr = btrue;
    }
    else
    {
        _set->mb_UseAFXBfr = bfalse;
        data = (U8*) radmalloc(bfrSize+texSize);
    }

    if(!data)
        goto fail;

    set_textures->base_ptr = data;

    // RT
    _set->m_RT = GFX_ADAPTER->getNewRenderTarget();
    _set->m_RT->m_area[0] = 0;
    _set->m_RT->m_area[1] = 0;
    _set->m_RT->m_area[2] = w;
    _set->m_RT->m_area[3] = h;
    {
        Texture* p_Tex = _set->m_RT->getTexture();
        p_Tex->m_adapterimplementationData = (u32) data;
        GFX_ADAPTER_WII->initializeDynamicTexture(p_Tex, w, h, Texture::PF_RGBA);
        data += texSize;
    }

    /* Align the start of the block */
    data = (U8*)((((U32) data) + 127) & ~127);

    /* Create textures */
    for(int i=0 ; i<set_textures->bink_buffers.TotalFrames ; ++i)
    {
        if(set_textures->bink_buffers.Frames[i].YPlane.Allocate)
        {
            set_textures->bink_buffers.Frames[i].YPlane.Buffer = data;
            set_textures->bink_buffers.Frames[i].YPlane.BufferPitch = YApitch;
	  
            // create Y plane
            if(!make_texture(   YApitch,
                                set_textures->bink_buffers.YABufferHeight,
                                GX_TF_I8,
                                GX_CLAMP,
                                &set_textures->textures[i].Ytexture,
                                data))
                goto fail;
        }
    	                                          
        if(set_textures->bink_buffers.Frames[i].cRPlane.Allocate)
        {
            set_textures->bink_buffers.Frames[i].cRPlane.Buffer = data + cRoffset;
            set_textures->bink_buffers.Frames[i].cRPlane.BufferPitch = cRcBpitch;
    	  
            // create cR plane
            if(!make_texture(   cRcBpitch,
                                set_textures->bink_buffers.cRcBBufferHeight,
                                GX_TF_I8,
                                GX_CLAMP,
                                &set_textures->textures[i].cRtexture,
                                data + cRoffset))
                goto fail;
        }

        if(set_textures->bink_buffers.Frames[i].cBPlane.Allocate)
        {
            set_textures->bink_buffers.Frames[i].cBPlane.Buffer = data + cBoffset;
            set_textures->bink_buffers.Frames[i].cBPlane.BufferPitch = cRcBpitch;

            // create cB plane
            if(!make_texture(   cRcBpitch,
                                set_textures->bink_buffers.cRcBBufferHeight,
                                GX_TF_I8,
                                GX_CLAMP,
                                &set_textures->textures[i].cBtexture,
                                data + cBoffset))
                goto fail;
        }

        if(set_textures->bink_buffers.Frames[i].APlane.Allocate)
        {
            set_textures->bink_buffers.Frames[i].APlane.Buffer = data + Aoffset;
            set_textures->bink_buffers.Frames[i].APlane.BufferPitch = YApitch;
    	  
            // create alpha plane
            if(!make_texture(   YApitch,
                                set_textures->bink_buffers.YABufferHeight,
                                GX_TF_I8,
                                GX_CLAMP,
                                &set_textures->textures[i].Atexture,
                                data + Aoffset))
                goto fail;
        }  

        data += framesize;
    }

	set_textures->framesize = framesize;
	do_deswizzlers(set_textures, data);

	return 1;

fail:

	Free_Bink_textures(_set);
	return 0;
}

/*
 =======================================================================================================================
 @brief		Draw our textures onto the screen with our vertex and pixel shaders.
 =======================================================================================================================
 */
void Draw_Bink_textures
    (
        BINKTEXTURESET *    set_textures,
        const f32           _binkRealWidth,
        const f32           _binkRealHeight,
        const f32           width,
        const f32           height
    )
{
    // Constants
    static const GXColorS10 reg0color = { -102, 0, -128, 154 };
    static const GXColor color0 = {   0,   0, 255, 100 };
    static const GXColor color1 = { 204,   0,   0, 208 };
    static const GXColor color2 = { 255,   0, 255, 128 };

    static const f32 indMtx[2][3] =
    {
        8.0f / 16.0f, 0.0f, 0.0f,
        0.0f, 1.0f / 16.0f, 0.0f
    };

    static f32 ATTRIBUTE_ALIGN(32) yswiz_texmtx[2][4] = 
    {
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f
    };

    static f32 ATTRIBUTE_ALIGN(32) crcbswiz_texmtx[2][4] =
    {
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f
    };

    static f32 ATTRIBUTE_ALIGN(32) ya_texmtx[2][4] =
    {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f
    };

    static f32 ATTRIBUTE_ALIGN(32) crcb_texmtx[2][4] =
    {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f
    };

    int doalpha = set_textures->bink_buffers.Frames[0].APlane.Buffer ? 1 : 0;

    // mark these textures as being in use and drop a sentinal in the GPU buffer
    //   that we can see to know when we are in use

    //set_textures->drawing[set_textures->bink_buffers.FrameNum] = 1;
    //GXSetDrawSync(0x4444 + set_textures->bink_buffers.FrameNum);

    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS,  GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT); // Note that we pass the same texcoord in twice, to
    GXSetVtxDesc(GX_VA_TEX1, GX_DIRECT); // get around the automatic scaling that takes place

	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS,  GX_POS_XY, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_TEX_ST, GX_F32, 0);

    GXLoadTexObj(&set_textures->textures[ set_textures->bink_buffers.FrameNum ].Ytexture,  GX_TEXMAP0);
    GXLoadTexObj(&set_textures->YAdeswizzle,  GX_TEXMAP1);
    GXLoadTexObj(&set_textures->cRcBdeswizzle, GX_TEXMAP2);
    GXLoadTexObj(&set_textures->textures[ set_textures->bink_buffers.FrameNum ].cRtexture,  GX_TEXMAP3);
    GXLoadTexObj(&set_textures->textures[ set_textures->bink_buffers.FrameNum ].cBtexture,  GX_TEXMAP4);

    // If there is alpha present
    if(doalpha)
        GXLoadTexObj(&set_textures->textures[ set_textures->bink_buffers.FrameNum ].Atexture,  GX_TEXMAP5);

    GXSetNumChans(0);
    GXSetNumTexGens(4);
    GXSetNumTevStages(4);
    GXSetNumIndStages(3);

    GXSetIndTexOrder(GX_INDTEXSTAGE0, GX_TEXCOORD1, GX_TEXMAP1); // y swiz
    GXSetIndTexOrder(GX_INDTEXSTAGE1, GX_TEXCOORD2, GX_TEXMAP2); // uv swiz

    // No alpha blending
    if(doalpha)
        GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    else
        GXSetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);

    // First stage: cB
    {
        GXSetTevIndirect(   GX_TEVSTAGE0,
                            GX_INDTEXSTAGE1,
                            GX_ITF_8,
                            GX_ITB_STU,
                            GX_ITM_0,
                            GX_ITW_OFF, GX_ITW_OFF,
                            GX_FALSE, GX_FALSE,
                            GX_ITBA_OFF );

        GXSetTevOrder( GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP4, GX_COLOR_NULL );

        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST, GX_CC_C0);
        GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_KONST, GX_CA_A0);
        GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);

        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        GXSetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_K0_A);
        GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    }

    // Stage 2: cR
    {
        // cR
        GXSetTevIndirect(   GX_TEVSTAGE1,
                            GX_INDTEXSTAGE1,
                            GX_ITF_8,
                            GX_ITB_STU,
                            GX_ITM_0,
                            GX_ITW_OFF, GX_ITW_OFF,
                            GX_FALSE, GX_FALSE,
                            GX_ITBA_OFF );

        GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP3, GX_COLOR_NULL);

        GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST, GX_CC_CPREV);
        GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_2, GX_FALSE, GX_TEVPREV);
        GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_TEXA, GX_CA_KONST, GX_CA_APREV);
        GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);

        GXSetTevKColorSel(GX_TEVSTAGE1, GX_TEV_KCSEL_K1);
        GXSetTevKAlphaSel(GX_TEVSTAGE1, GX_TEV_KASEL_K1_A);
        GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
    }

    // Stage 3: Y
    {
        // y
        GXSetTevIndirect(   GX_TEVSTAGE2,
                            GX_INDTEXSTAGE0,
                            GX_ITF_8,
                            GX_ITB_STU,
                            GX_ITM_0,
                            GX_ITW_OFF, GX_ITW_OFF,
                            GX_FALSE, GX_FALSE,
                            GX_ITBA_OFF );

        GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD3, GX_TEXMAP0, GX_COLOR_NULL);

        GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_TEXC, GX_CC_ONE, GX_CC_CPREV);
        GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_TEXA, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

        GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
    }
	
    if(doalpha)
    {
        GXSetTevIndirect(   GX_TEVSTAGE3,
                            GX_INDTEXSTAGE0,
                            GX_ITF_8,
                            GX_ITB_STU,
                            GX_ITM_0,
                            GX_ITW_OFF, GX_ITW_OFF,
                            GX_FALSE, GX_FALSE,
                            GX_ITBA_OFF );

        GXSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD3, GX_TEXMAP5, GX_COLOR_NULL);

        GXSetTevColorIn(GX_TEVSTAGE3, GX_CC_APREV, GX_CC_CPREV, GX_CC_KONST, GX_CC_ZERO);
        GXSetTevColorOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_TEXA, GX_CA_KONST, GX_CA_ZERO);
        GXSetTevAlphaOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

        GXSetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K2);
        GXSetTevKAlphaSel(GX_TEVSTAGE3, GX_TEV_KASEL_K2_A);
        GXSetTevSwapMode(GX_TEVSTAGE3, GX_TEV_SWAP0, GX_TEV_SWAP0);
    }
    else
    {
        GXSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);

        GXSetTevColorIn(GX_TEVSTAGE3, GX_CC_APREV, GX_CC_CPREV, GX_CC_KONST, GX_CC_ZERO);
        GXSetTevColorOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_KONST, GX_CA_KONST, GX_CA_ZERO);
        GXSetTevAlphaOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

        GXSetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K2);
        GXSetTevKAlphaSel(GX_TEVSTAGE3, GX_TEV_KASEL_K2_A);
        GXSetTevSwapMode(GX_TEVSTAGE3, GX_TEV_SWAP0, GX_TEV_SWAP0);
    }

    GXSetTevColorS10(GX_TEVREG0, reg0color);
    GXSetTevKColor(GX_KCOLOR0, color0);
    GXSetTevKColor(GX_KCOLOR1, color1);
    GXSetTevKColor(GX_KCOLOR2, color2);

    // Setup the indirect matrix
    // You can't actually store 1.0 in this matrix, so we use the
    // bias exponent to get 8 and 1 in the locations where we need
    // them

    GXSetIndTexMtx(GX_ITM_0, indMtx, 4);

    // Setup texcoords ...
    {
        f32 crcbDiffW = (_binkRealWidth / 2.0f) - (f32)set_textures->bink_buffers.cRcBBufferWidth;
        f32 crcbDiffH = (_binkRealHeight / 2.0f) - (f32)set_textures->bink_buffers.cRcBBufferHeight;

        f32 yaDiffW = _binkRealWidth - (f32)set_textures->bink_buffers.YABufferWidth;
        f32 yaDiffH = _binkRealHeight - (f32)set_textures->bink_buffers.YABufferHeight;

        // adjust for the pow2 expansion
        f32 yaswizzletexU = ( (f32) ( set_textures->bink_buffers.YABufferWidth / 8 ) ) / ( (f32) set_textures->YAdeswizzle_width );
        f32 yaSwizzletexV = ( (f32) set_textures->bink_buffers.YABufferHeight ) / ( (f32) set_textures->YAdeswizzle_height );
        yswiz_texmtx[0][0] = yaswizzletexU + (yaDiffW / ((f32)set_textures->YAdeswizzle_width));
        yswiz_texmtx[1][1] = yaSwizzletexV + (yaDiffH / ((f32) set_textures->YAdeswizzle_height));

        f32 crcbswizzletexU = ( (f32) ( set_textures->bink_buffers.cRcBBufferWidth / 8 ) ) / ( (f32) set_textures->cRcBdeswizzle_width );
        f32 crcbswizzletexV = ( (f32) set_textures->bink_buffers.cRcBBufferHeight ) / ( (f32) set_textures->cRcBdeswizzle_height );
        crcbswiz_texmtx[0][0] = crcbswizzletexU + (crcbDiffW / ((f32)set_textures->cRcBdeswizzle_width));
        crcbswiz_texmtx[1][1] = crcbswizzletexV + (crcbDiffH / ((f32)set_textures->cRcBdeswizzle_height));

        crcb_texmtx[0][0] = 1.f + (crcbDiffW / ((f32)set_textures->bink_buffers.cRcBBufferWidth));
        crcb_texmtx[1][1] = 1.f + (crcbDiffH / ((f32)set_textures->bink_buffers.cRcBBufferHeight));

        ya_texmtx[0][0] = 1.f + (yaDiffW / ((f32)set_textures->bink_buffers.YABufferWidth));
        ya_texmtx[1][1] = 1.f + (yaDiffH / ((f32) set_textures->bink_buffers.YABufferHeight));

        GXLoadTexMtxImm(crcb_texmtx, GX_TEXMTX0, GX_MTX2x4);
        GXLoadTexMtxImm(yswiz_texmtx, GX_TEXMTX1, GX_MTX2x4);
        GXLoadTexMtxImm(crcbswiz_texmtx, GX_TEXMTX2, GX_MTX2x4);
        GXLoadTexMtxImm(ya_texmtx, GX_TEXMTX3, GX_MTX2x4);

        GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
        GXSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1);
        GXSetTexCoordGen(GX_TEXCOORD2, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX2);
        GXSetTexCoordGen(GX_TEXCOORD3, GX_TG_MTX2x4, GX_TG_TEX1, GX_TEXMTX3);
    }

    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    {
        GXPosition2f32(0.f,   0.f   ); GXTexCoord2f32(0.0f, 1.0f); GXTexCoord2f32(0.0f, 1.0f);
        GXPosition2f32(0.f,   height); GXTexCoord2f32(0.0f, 0.0f); GXTexCoord2f32(0.0f, 0.0f);
        GXPosition2f32(width, height); GXTexCoord2f32(1.0f, 0.0f); GXTexCoord2f32(1.0f, 0.0f);
        GXPosition2f32(width, 0.f   ); GXTexCoord2f32(1.0f, 1.0f); GXTexCoord2f32(1.0f, 1.0f);
    }
    GXEnd();
}

#ifdef USE_ITF_IO
/*
 =======================================================================================================================
    Bink IO
 =======================================================================================================================
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
U32 RADLINK BINK_CALLBACK_GetBufferSize(struct BINKIO PTR4 *Binkio, U32 Size)
{
	return Size * 4;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
U32 RADLINK BINK_CALLBACK_Read(struct BINKIO *Binkio, void *Dest, U32 Size)
{
	ITF_ASSERT(g_CurrentVideo);

	u32 sizeRead;
	if(g_CurrentVideo->m_BinkFile->read(Dest, Size, &sizeRead))
	{
        BINK_SwapEndianess(Dest, sizeRead);
		return sizeRead;
	}

    g_CurrentVideo->setReadError(1);
	return 0;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
U32 RADLINK BINK_CALLBACK_ReadHeader(struct BINKIO PTR4 *Bnkio, S32 Offset, void PTR4 *Dest, U32 Size)
{
	return BINK_CALLBACK_Read(Bnkio, Dest, Size);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
U32 RADLINK BINK_CALLBACK_ReadFrame(struct BINKIO PTR4 *Bnkio, U32 Framenum, S32 origofs, void PTR4 *Dest, U32 Size)
{
	return BINK_CALLBACK_Read(Bnkio, Dest, Size);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void RADLINK BINK_CALLBACK_SetInfo(struct BINKIO PTR4 *Bnkio, void PTR4 *Buf, U32 Size, U32 FileSize, U32 simulate)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
U32 RADLINK BINK_CALLBACK_Idle(struct BINKIO PTR4 *Bnkio)
{
    OSYieldThread();
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void RADLINK BINK_CALLBACK_Close(struct BINKIO PTR4 *Binkio)
{
	ITF_ASSERT(g_CurrentVideo);
	g_CurrentVideo->stop();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
S32 RADLINK BINK_CALLBACK_CreateBinkStream(struct BINKIO PTR4 *Binkio, const char PTR4 *name, U32 flags)
{
	ITF_ASSERT(g_CurrentVideo);

	// Set callbacks
	Binkio->GetBufferSize	= BINK_CALLBACK_GetBufferSize;
	Binkio->ReadHeader		= BINK_CALLBACK_ReadHeader;
	Binkio->ReadFrame		= BINK_CALLBACK_ReadFrame;
	Binkio->SetInfo			= BINK_CALLBACK_SetInfo;
	Binkio->Idle			= BINK_CALLBACK_Idle;
	Binkio->Close			= BINK_CALLBACK_Close;

	// Close file if already opened
	BINK_CALLBACK_Close(Binkio);

	// Open file
	g_CurrentVideo->m_BinkFile = FILEMANAGER->openFile(name, ITF_FILE_ATTR_READ);
	if(!g_CurrentVideo->m_BinkFile)
	{
		BinkSetError("Problem while opening file.");
		return 0;
	}

	return 1;
}
#endif // USE_ITF_IO

//------------------------------------------------------------------------------------------------------------------------------------------------------

void videoHandle::destroy()
{
    close();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------

bbool videoHandle::open(const String& _filename)
{
    close();
    
#ifdef USE_ITF_IO
	ITF_ASSERT(!g_CurrentVideo);
	g_CurrentVideo = this;
#endif // USE_ITF_IO

	RADARAMCALLBACKS aram_callbacks = { BINK_CALLBACK_Alloc, BINK_CALLBACK_Free };
	BinkSetMemory(BINK_CALLBACK_Alloc, BINK_CALLBACK_Free);
	BinkSoundUseAX(&aram_callbacks);

    m_lastVideoOpened = _filename;
    StringConverter fullName(_filename+"_16x9_WII.bik");

    if(isPlayedFromMemory())
    {
        File *pFile = FILEMANAGER->openFile(fullName.getChar(), ITF_FILE_ATTR_READ);
        if(pFile)
        {
            u32 fileSize = (u32)pFile->getLength();
            ITF_ASSERT(!m_pMemory);
            m_pMemory = (u8 *) Memory::alignedMallocCategory(sizeof(u8) * fileSize, 32, MemoryId::mId_Temporary);
            if (m_pMemory == NULL)
            {
                ITF_ASSERT_MSG(0,"no enough memory to play the movie");
                return bfalse;
            }

            u32 sizeRead = 0;
            pFile->read(m_pMemory, fileSize, &sizeRead);
            FILEMANAGER->closeFile(pFile);

            BINK_SwapEndianess(m_pMemory, sizeRead);
            m_Bink = BinkOpen((const char *)m_pMemory, BINKNOFRAMEBUFFERS | BINKALPHA | BINKFROMMEMORY);
        }
    }
    else
    {
#ifdef USE_ITF_IO
	    BinkSetIO(BINK_CALLBACK_CreateBinkStream);				
        m_Bink = BinkOpen(fullName.getChar(), BINKNOFRAMEBUFFERS | BINKALPHA | BINKIOPROCESSOR);
#else // USE_ITF_IO
        m_Bink = BinkOpen(fullName.getChar(), BINKNOFRAMEBUFFERS | BINKALPHA);
#endif // USE_ITF_IO
    }

    if(!m_Bink)
    {
        ITF_WARNING(NULL, bfalse, "Video %s not valid (error=%s)", fullName.getChar(), BinkGetError());
		return bfalse;
    }

    if(m_lastFrameNumAfterIOError)
    {
        BinkGoto(m_Bink, m_lastFrameNumAfterIOError, BINKGOTOQUICKSOUND);
        m_lastFrameNumAfterIOError = 0;
    }
    
    BinkSetWillLoop(m_Bink,0);
    BinkGetFrameBuffersInfo(m_Bink, &m_Set.m_TextureSet.bink_buffers);

    if(Create_Bink_textures(m_Bink, &m_Set))
    {
        BinkRegisterFrameBuffers(m_Bink, &m_Set.m_TextureSet.bink_buffers);
        // start decompress first frame
        BinkDoFrame(m_Bink);
    }
    else
    {
        ITF_WARNING(NULL, bfalse, "Can't create textures for Video %s", fullName.getChar());
		close();
		return bfalse;
    }
    
    m_opened  = btrue;
    m_stopped = bfalse;
    m_isPaused = bfalse;
    m_oneFramePlayed = bfalse;

    //compute total time and fps
    BINKSUMMARY summary;
    BinkGetSummary(m_Bink,&summary);

    m_fps				 = summary.FrameRate / (f32) summary.FrameRateDiv; 
    m_totalTimeInSeconds = (f32)(summary.TotalFrames / m_fps);

#ifndef ITF_FINAL
    LOG(">>VideoHandle :%s  total time:%0.02f seconds", fullName.getChar(), m_totalTimeInSeconds);
#endif //ITF_FINALs*/

    SYSTEM_ADAPTER_WII->disableHomeButton();
    m_SaveScreenProtectionState = GFX_ADAPTER->setScreenProtection(bfalse);

    return btrue;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------

void videoHandle::close()
{
    // Don't change the flag when an IO error occurred
    if(!m_wasInterruptedFromAnIOError)
        m_stopped = btrue;

    m_opened  = bfalse;
    m_isPaused = bfalse;
    if(m_Bink)
    {
        BinkClose(m_Bink);
        SYSTEM_ADAPTER_WII->enableHomeButton();
        GFX_ADAPTER->setScreenProtection(m_SaveScreenProtectionState);
        m_Bink = 0;
    }

#ifdef USE_ITF_IO
    if(m_BinkFile)
    {
        m_BinkFile = FILEMANAGER->closeFile(g_CurrentVideo->m_BinkFile);
    }
    g_CurrentVideo = NULL;
#endif // USE_ITF_IO

	Free_Bink_textures(&m_Set);

    if(m_pMemory)
    {
        Memory::alignedFree(m_pMemory);
        m_pMemory = NULL;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

bbool videoHandle::update(bbool _ignoreIOError)
{
    if(m_wasInterruptedFromAnIOError)
    {    
        if(!_ignoreIOError && TRC_ADAPTER->isAFatalErrorRunning())
            return bfalse;

        //if(!isStopped())
        //    close();
        
        //if(open(m_lastVideoOpened))
            //m_wasInterruptedFromAnIOError = bfalse;
        
        // We are handling our Handle, so we can just resume the video (done by the DVDError thread)
        m_wasInterruptedFromAnIOError = bfalse;

        return bfalse; 
    }

    if(isStopped() || !m_Bink || m_isPaused)
        return bfalse;
 
#ifdef USE_ITF_IO
	ITF_ASSERT(g_CurrentVideo == this);
#endif // USE_ITF_IO

    // IO error ?
    if(handleError())
        return bfalse;

    // Synchro ?
	if(BinkWait(m_Bink))
        return bfalse;

    // decompress a frame
    BinkDoFrame(m_Bink);

    // skip frames if we need to...
    while ( BinkShouldSkip(m_Bink) )
    {
        BinkNextFrame(m_Bink);
        if(handleError())return bfalse; 

        BinkDoFrame(m_Bink);
        if(handleError())return bfalse; 

        m_oneFramePlayed = btrue;
    }

    // Stop video if the last frame is reached
    if((m_Bink->FrameNum == m_Bink->Frames) && !m_loop)
    {
        stop();
        return btrue;
    }

    // Move to next frame
    BinkNextFrame( m_Bink );
    handleError();

    return btrue;
}

bbool videoHandle::handleError()
{
    // Not openend ?
    if(!m_Bink)
        return bfalse;

    if(m_wasInterruptedFromAnIOError)
        return btrue; 

    // No error detected by the lib
    if(!m_Bink->ReadError)
        return bfalse;
    
    // Error happened, close the bink file
    onIOError();
    return btrue;
}

void videoHandle::onIOError()
{
    if(isStopped() || !m_Bink)
        return;

    pause();

    m_lastFrameNumAfterIOError = m_Bink->FrameNum;
    m_wasInterruptedFromAnIOError = btrue;
    m_oneFramePlayed = bfalse;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------

void videoHandle::render()
{
    if(isStopped() || !m_Bink)
        return;

#ifdef USE_ITF_IO
	ITF_ASSERT(g_CurrentVideo == this);
#endif // USE_ITF_IO

    GFX_ADAPTER->syncGPU();

    const f32 binkW = (f32) m_Bink->Width;
    const f32 binkH = (f32) m_Bink->Height;

    GFX_ADAPTER->clear(GFX_CLEAR_COLOR|GFX_CLEAR_DEFERRED, COLOR_ZERO);
    GFX_ADAPTER->setCullMode(GFX_CULL_NONE);
    GFX_ADAPTER->depthMask(0);
    GFX_ADAPTER->depthTest(0);

    GFX_ADAPTER->startImpostorRendering(m_Set.m_RT, 0.f, 0.f, binkW, binkH, bfalse);
    {
        Mtx modelview;
        MTXIdentity(modelview);
        GXLoadPosMtxImm(modelview, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);

        Draw_Bink_textures(&m_Set.m_TextureSet, binkW, binkH, binkW, binkH);

        GXSetNumIndStages(0);
        GXSetTevDirect(GX_TEVSTAGE0);
        GXSetTevDirect(GX_TEVSTAGE1);
        GXSetTevDirect(GX_TEVSTAGE2);
        GXSetTevDirect(GX_TEVSTAGE3);
    }
    GFX_ADAPTER->stopImpostorRendering(m_Set.m_RT);

    //----

    GFX_ADAPTER->init2DRender();
    GFX_ADAPTER->setGlobalColor(Color::white());
    GFX_ADAPTER_WII->disableFog();
    GFX_ADAPTER_WII->resetUVAnim();

    GFX_ADAPTER->setTextureAdressingMode(0, GFXTADDRESS_CLAMP, GFXTADDRESS_CLAMP);
    GFX_ADAPTER->SetTexture(0, m_Set.m_RT->getTexture());

    GFX_ADAPTER->setAlphaBlend(GFX_BLEND_COPY);
    GFX_ADAPTER->drawScreenQuad(0.f, 0.f, (f32)GFX_ADAPTER->getScreenWidth(), (f32)GFX_ADAPTER->getScreenHeight(), 0.f, COLOR_WHITE);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------

bbool videoHandle::getCurrentTime(f32& _timeInSeconds)
{
    if(m_Bink)
    {
        BINKREALTIME realtime;
        BinkGetRealtime(m_Bink, &realtime, 0);
        f32 time = (f32)realtime.FrameNum / m_fps; //switch to seconds
        _timeInSeconds = time;
        return btrue;
    }

    return bfalse;
}

void videoHandle::pause()
{
    if(m_isPaused)return;
    
    m_isPaused = btrue;

    BinkPause(m_Bink, 1);
}

void videoHandle::resume()
{
    if(!m_isPaused)return;

    m_isPaused = bfalse;

    BinkPause(m_Bink, 0);
}

#ifdef USE_ITF_IO
videoHandle* videoHandle::getVideoHandle()
{
    return g_CurrentVideo;
}

bbool videoHandle::isVideoPlaying()
{
    if(g_CurrentVideo) return (!g_CurrentVideo->isStopped() && g_CurrentVideo->m_oneFramePlayed);
    return bfalse;
}

#endif //USE_ITF_IO

//------------------------------------------------------------------------------------------------------------------------------------------------------
} // namespace  ITF
