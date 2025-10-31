#include "precompiled_GFXAdapter_WII.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////       implementation of various shapes drawing
///////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <revolution.h>
#include <revolution/sc.h>
#include <revolution/mtx.h>

#ifndef _ITF_WII_DRAWPRIM_H_
#include "adapters/GFXAdapter_WII/GFXAdapter_WII.h"
#endif //_ITF_WII_DRAWPRIM_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/Vec2d.h"
#endif //_ITF_VEC2D_H_
#ifndef _ITF_TEXTURE_H_
#include "engine/display/Texture.h"
#endif //_ITF_TEXTURE_H_
#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_FILE_H_
#include "core/file/File.h"
#endif //_ITF_FILE_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_GFXADAPTER_RENDERTARGET_H_
#include "engine/AdaptersInterfaces/GFXAdapter_RenderTarget.h"
#endif //_ITF_GFXADAPTER_RENDERTARGET_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_DDSFORMAT_H_
#include "engine/display/ddsformat.h"
#endif //_ITF_DDSFORMAT_H_

#ifndef _ITF_WARNINGMANAGER_H_
#include "engine/common/WarningManager.h"
#endif // _ITF_WARNINGMANAGER_H_

#ifndef _ITF_COLORINTEGER_H_
#include "core/ColorInteger.h"
#endif //_ITF_COLORINTEGER_H_

#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_H

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#include "core/memory/memory_WII.h"

#ifndef ITF_FINAL
#include <revolution/so/SOBasic.h>
#endif // ITF_FINAL

#if defined(_HEAP_DEBUG_) && !defined(ROBOT_COMPILATION)
#define FULL_MEM_DISPLAY
#endif // defined(_HEAP_DEBUG_) && !defined(ROBOT_COMPILATION)

namespace ITF
{
//----------------------------------------------------------------------------//

void GFXAdapter_WII::setShaderForFont(bbool bOutline)
{
    setGlobalColor(Color::white());
    disableFog();
    setTev(TEV_TC);
    setAlphaBlend(GFX_BLEND_ALPHA);
    setTextureAdressingMode(0, GFXTADDRESS_WRAP, GFXTADDRESS_WRAP);
}

OSFontHeader* GFXAdapter_WII::FontData = NULL;
    
//--- FONTS system -------------------------------------------------------------------------//
static void DrawCell(int x, int y, int xChar, int yChar, u32 _color, f32 _scale)
{
    s16 posLeft   = (s16) x;
    s16 posRight  = (s16) (posLeft + GFXAdapter_WII::FontData->cellWidth * _scale);
    s16 posTop    = (s16) y;
    s16 posBottom = (s16) (posTop  + GFXAdapter_WII::FontData->cellHeight * _scale);

    s16 texLeft   = (s16) xChar;
    s16 texRight  = (s16) (xChar + GFXAdapter_WII::FontData->cellWidth);
    s16 texTop    = (s16) yChar;
    s16 texBottom = (s16) (yChar + GFXAdapter_WII::FontData->cellHeight);

    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
        GXPosition3s16(posLeft , posTop   , 0);
        GXColor1u32(_color);
        GXTexCoord2s16(texLeft , texTop);

        GXPosition3s16(posRight, posTop   , 0);
        GXColor1u32(_color);
        GXTexCoord2s16(texRight, texTop);

        GXPosition3s16(posRight, posBottom, 0);
        GXColor1u32(_color);
        GXTexCoord2s16(texRight, texBottom);

        GXPosition3s16(posLeft , posBottom, 0);
        GXColor1u32(_color);
        GXTexCoord2s16(texLeft , texBottom);
    GXEnd();
}

static void LoadSheet(void* image)
{
    Mtx      mtx;
    GXTexObj texObj;

    // Set up and load texture object
    GXInitTexObj(&texObj,                           // obj
                 image,                             // image_ptr
                 GFXAdapter_WII::FontData->sheetWidth,              // weight
                 GFXAdapter_WII::FontData->sheetHeight,             // height
                 (GXTexFmt) GFXAdapter_WII::FontData->sheetFormat,  // format
                 GX_CLAMP,                          // wrap_s (don't care)
                 GX_CLAMP,                          // wrap_t (don't care)
                 GX_FALSE);                         // mipmap (don't care)

    GXInitTexObjLOD(&texObj,
                    GX_LINEAR,     // min_filt
                    GX_LINEAR,     // max_filt
                    0.0f,          // min_lod     (don't care)
                    0.0f,          // max_lod     (don't care)
                    0.0f,          // lod_bias    (don't care)
                    GX_DISABLE,    // bias_clamp  (don't care)
                    GX_FALSE,      // do_edge_lod (don't care)
                    GX_ANISO_1);   // max_aniso   (don't care)

    GXLoadTexObj(&texObj, GX_TEXMAP0);

    MTXScale(mtx, 1.0f / GFXAdapter_WII::FontData->sheetWidth,
                  1.0f / GFXAdapter_WII::FontData->sheetHeight,
                  1.0f);
    GXLoadTexMtxImm(mtx, GX_TEXMTX0, GX_MTX2x4);

    GXSetNumTexGens(1);
    GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
}

static s32 DrawChar(int x, int y, u16 code, u32 _color, f32 _scale)
{
    char  buffer[3];
    s32   width;
    void* image;
    s32   xChar;
    s32   yChar;

    if (code == 0)
    {
        return 0;
    }

    if (code < 256)
    {
        buffer[0] = (char) code;
        buffer[1] = '\0';
    }
    else
    {
        buffer[0] = (char) (code >> 8);
        buffer[1] = (char) (code & 0xff);
        buffer[2] = '\0';
    }

    OSGetFontTexture(buffer, &image, &xChar, &yChar, &width);
    LoadSheet(image);
    DrawCell(x, y, xChar, yChar, _color, _scale);

    GFX_ADAPTER_WII->resetUVAnim();

    return width;
}

static s32 DrawString(int x, int y, char* string, u32 _color, f32 _scale)
{
    void* image;
    s32   xChar,yChar,width;
    s32 cx = 0;

    OSGetFontTexture(string, &image, &xChar, &yChar, &width);
    LoadSheet(image);

    while (*string)
    {
        string = OSGetFontTexture(string, &image, &xChar, &yChar, &width);
        DrawCell(x + cx, y, xChar, yChar, _color, _scale);
        cx += width * _scale;
    }

    GFX_ADAPTER_WII->resetUVAnim();

    return cx;
}

#ifndef ITF_DISABLE_DEBUGINFO
void GFXAdapter_WII::drawDebugText()
{
#ifndef ITF_FINAL
    // safe Frame.
    if (getSafeFrameMode())
        drawSafeFrame();
#endif // ITF_FINAL

    renderContext_SetDefault();
    setAlphaBlend(GFX_BLEND_ALPHA);

    //----

#if ITF_DEBUG_LEVEL > 0
    // DBG primitives
    {
        init2DRender();
        setTev(TEV_C);
        finalizeTevSetting();

	    // Set up vertex descriptors
	    GXClearVtxDesc();
	    GXSetVtxDesc(GX_VA_POS,  GX_DIRECT);
	    GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
        GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
        GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);

        for(u32 it=0 ; it<m_Box2DList.size() ; ++it)
        {
            const Box2D& rect = m_Box2DList[it];
            DrawBegin(GFX_TRIANGLE_STRIP, 4);
            {
                for(u32 i=0 ; i<4 ; ++i)
                {
                    GXPosition3f32(rect.v[i].m_pos.m_x, rect.v[i].m_pos.m_y, rect.v[i].m_pos.m_z);
                    GXColor1u32(rect.v[i].m_color);
                }
            }
            DrawEnd();
        }
        m_Box2DList.clear();

        for(u32 it=0 ; it<m_Triangle2DList.size() ; ++it)
        {
            const Triangle2D& tri = m_Triangle2DList[it];
            DrawBegin(GFX_TRIANGLES, 3);
            {
                for(u32 i=0 ; i<3 ; ++i)
                {
                    GXPosition3f32(tri.v[i].m_pos.m_x, tri.v[i].m_pos.m_y, tri.v[i].m_pos.m_z);
                    GXColor1u32(tri.v[i].m_color);
                }
            }
            DrawEnd();
        }
        m_Triangle2DList.clear();
    }
#endif // ITF_DEBUG_LEVEL > 0

    //----

    setOrthoView(0.f, (f32) getScreenWidth(), (f32) getScreenHeight(), 0.f);

    // Set TEV parameters
	GXSetNumTevStages(1);
	GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0,GX_DISABLE,GX_SRC_REG,GX_SRC_VTX,GX_LIGHT_NULL,GX_DF_NONE,GX_AF_NONE);

	// Set up vertex descriptors
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS,  GX_DIRECT);
	GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS,  GX_POS_XYZ, GX_S16, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST,  GX_S16, 0);

    //----

#if ITF_DEBUG_LEVEL > 0
    // DBG Texts
    {
	    GFX_RECT rct;
	    rct.left=30;
	    rct.right= getScreenWidth() - rct.left;
	    rct.top=20;
	    rct.bottom=rct.top+15;

	    ITF_VECTOR<DBGText>::iterator it = m_DBGTexts.begin();
	    ITF_VECTOR<DBGText>::iterator end = m_DBGTexts.end();
	    for (; it != end; ++it)
	    {
    	    Color ccolor(1.f, it->m_r, it->m_g, it->m_b);

		    if ((*it).m_x < 0.0f && (*it).m_y < 0.0f)
		    {
			    rct.top += 15;
			    rct.bottom = rct.top+15;
			    StringConverter str((*it).m_text);
			    DrawString((f32)rct.left, (f32)rct.top, (char*)str.getChar(), ccolor.getAsU32(), 0.5f);
		    }
		    else
		    {
			    GFX_RECT localRect;
			    localRect.left=(i32)((*it).m_x);
			    localRect.right=getScreenWidth();
			    localRect.top=(i32)((*it).m_y);
			    localRect.bottom=rct.top+15;
			    StringConverter str((*it).m_text);
			    DrawString((f32)localRect.left, (f32)localRect.top, (char*)str.getChar(), ccolor.getAsU32(), 0.5f);
		    }
	    }

        m_DBGTexts.clear();
    }
#endif // ITF_DEBUG_LEVEL > 0

	//----

    if(getDisplayDebugStats())
    {
#ifndef ITF_FINAL
        // Min/Max
        #define ADD_MEM_SIZE(v, s) \
            (v).m_x = ((v).m_x == 0.f) ? (s) : f32_Min((v).m_x, (s)); \
            (v).m_y = ((v).m_y == 0.f) ? (s) : f32_Max((v).m_y, (s));
        #define GET_MEM_MIN(v)  ((v).m_x)
        #define GET_MEM_MAX(v)  ((v).m_y)

        static Vec2d memTEX(Vec2d::Zero);
        static Vec2d memSND(Vec2d::Zero);
        static Vec2d memRETAIL(Vec2d::Zero);
        static bbool lastFrameWasLoading = bfalse;

        if(lastFrameWasLoading && !GAMEMANAGER->isLoadingVisible())
        {
            // Reset at end of loading
            memTEX      = Vec2d::Zero;
            memSND      = Vec2d::Zero;
            memRETAIL   = Vec2d::Zero;
        }
        lastFrameWasLoading = GAMEMANAGER && GAMEMANAGER->isConfigurationLoaded() && GAMEMANAGER->isLoadingVisible();
#endif // ITF_FINAL

        // Opaque Background
        setAlphaBlend(GFX_BLEND_COPY);

        char bfr[128];

        u32 ypos = getScreenHeight() - (FontData->cellHeight * 2) - 20;
        u32 xpos = 30;

        const f32 hScaleY   = 1.0f;
        const f32 hScaleY2  = hScaleY * 0.5f;
        const f32 lScaleY   = 0.6f;

        const f32 oToMo     = 1.f / (1024.f * 1024.f);

        const u32 colorGOOD   = COLOR_GREEN;
        const u32 colorWARN   = COLOR_RED|COLOR_GREEN;
        const u32 colorERROR  = COLOR_RED;

        #define COLOR_LIMIT_INF(v,l1,l2)    (((v) < (l2)) ? colorERROR : (((v) < (l1)) ? colorWARN : colorGOOD))
        #define COLOR_LIMIT_SUP(v,l1,l2)    (((v) > (l1)) ? colorERROR : (((v) > (l2)) ? colorWARN : colorGOOD))

#if !defined(ITF_FINAL) && defined(ITF_CATEGORY_MEMORY_ENABLE)
        {
            // MEM Textures
            const f32 texLimit1  = 20.f;
            const f32 texLimit2  = texLimit1 - (texLimit1 * 0.01f); // 1%
            const f32 texmemfree = memoryStatsManager::get().getCategoryCurrentAllocated(MemoryId::mId_Textures) * oToMo;
            ADD_MEM_SIZE(memTEX, texmemfree);
            #ifdef ROBOT_COMPILATION
                sprintf(bfr, "MEM(cur/min/max Mo): TEX %0.02f/%0.02f/%0.02f", texmemfree, GET_MEM_MIN(memTEX), GET_MEM_MAX(memTEX));
            #else // ROBOT_COMPILATION
                sprintf(bfr, "MEM: TEX %0.02f Mo", texmemfree);
            #endif // ROBOT_COMPILATION
            xpos += DrawString(xpos, ypos, bfr, COLOR_LIMIT_SUP(texmemfree,texLimit1,texLimit2), hScaleY2);

            // MEM Sounds
            const f32 sndLimit1  = 6.f;
            const f32 sndLimit2  = sndLimit1 - (sndLimit1 * 0.01f); // 1%
            const f32 sndmemData = memoryStatsManager::get().getCategoryCurrentAllocated(MemoryId::mId_AudioData) * oToMo;
            const f32 sndmemEngine = memoryStatsManager::get().getCategoryCurrentAllocated(MemoryId::mId_AudioEngine) * oToMo;
            const f32 sndmemStream = memoryStatsManager::get().getCategoryCurrentAllocated(MemoryId::mId_AudioStreamBuffers) * oToMo;
            ADD_MEM_SIZE(memSND, sndmemData);
            #ifdef ROBOT_COMPILATION
                sprintf(bfr, " | SND %0.02f/%0.02f/%0.02f", sndmemData, GET_MEM_MIN(memSND), GET_MEM_MAX(memSND));
            #else // ROBOT_COMPILATION
                sprintf(bfr, " | SND D: %0.02f Mo S: %0.02f Mo E: %0.02f Mo", sndmemData, sndmemStream, sndmemEngine);
            #endif // ROBOT_COMPILATION
            xpos += DrawString(xpos, ypos, bfr, COLOR_LIMIT_SUP(sndmemData,sndLimit1,sndLimit2), hScaleY2);

            ypos += (FontData->cellHeight * hScaleY2);
        }
#endif // !defined(ITF_FINAL) && defined(ITF_CATEGORY_MEMORY_ENABLE)

        xpos = 30;

#ifdef _HEAP_SIZE_DEBUG_
        {
            const f32 limit1 = 8.f;
            const f32 limit2 = 3.f;

            // MEM Free
#ifdef FULL_MEM_DISPLAY
            const f32 memfree1 = MEM_GetFreeSize(MEM_Heap1) * oToMo;
            const f32 memfree2 = MEM_GetFreeSize(MEM_Heap2) * oToMo;
            const f32 memfree = memfree1 + memfree2;
            sprintf(bfr, "MEM Free: %0.02f + %0.02f Mo", memfree1, memfree2);
            xpos += DrawString(xpos, ypos, bfr, COLOR_LIMIT_INF(memfree,limit1,limit2), lScaleY);
#endif // FULL_MEM_DISPLAY

            // Retail Free
            const f32 systemMem  = (f32)MEM_GetSystemPhysicalMemory() * oToMo;
            const f32 retailfree = ((f32)MEM_GetTotalRetailPhysicalMemory() - (f32)MEM_M_AllocSize()) * oToMo - systemMem;
#ifdef FULL_MEM_DISPLAY
            sprintf(bfr, " (retail: %0.02f | sys: %0.02f)", retailfree, systemMem);
#else // FULL_MEM_DISPLAY
            #ifdef ROBOT_COMPILATION
                #ifndef ITF_FINAL
                    ADD_MEM_SIZE(memRETAIL, retailfree);
                    sprintf(bfr, "MEM Free(cur/min/max Mo): %0.02f/%0.02f/%0.02f", retailfree, GET_MEM_MIN(memRETAIL), GET_MEM_MAX(memRETAIL));
                #else // ITF_FINAL
                    sprintf(bfr, "MEM Free: %0.02f Mo", retailfree);
                #endif // ITF_FINAL
            #else // ROBOT_COMPILATION
                sprintf(bfr, "MEM Free: %0.02f Mo (sys: %0.02f)", retailfree, systemMem);
            #endif // ROBOT_COMPILATION
#endif // FULL_MEM_DISPLAY
            xpos += DrawString(xpos, ypos, bfr, COLOR_LIMIT_INF(retailfree,limit1,limit2), lScaleY);

            ypos += (FontData->cellHeight * lScaleY);
        }
#endif // _HEAP_SIZE_DEBUG_

        xpos = 30;

        if(getfPs() > 0.0f)
	    {
            u32 ypos2 = ypos;

	        const f32 limit1 = getMonitorRefreshRate() - (getMonitorRefreshRate() * 0.01f);  //  1%
	        const f32 limit2 = getMonitorRefreshRate() - (getMonitorRefreshRate() * 0.10f);  // 10%

            // CPU
#ifndef ITF_FINAL
            extern volatile f32 gf32_DBG_gpu;
            f32 curFps = f32_Min(getfPsNoVsync(), gf32_DBG_gpu);
#else // ITF_FINAL
            f32 curFps = getfPs();
#endif // ITF_FINAL
            sprintf(bfr, "%0.02f", curFps);
            xpos += DrawString(xpos, ypos2, bfr, COLOR_LIMIT_INF(curFps,limit1,limit2), hScaleY);
            sprintf(bfr, " fps  ", curFps);
            DrawString(xpos, ypos2, bfr, COLOR_LIMIT_INF(curFps,limit1,limit2), hScaleY2);
            sprintf(bfr, " %dHz", (u32)getMonitorRefreshRate());
            xpos += DrawString(xpos, ypos2+(FontData->cellHeight * hScaleY2), bfr, COLOR_LIMIT_INF(curFps,limit1,limit2), hScaleY2);

#ifndef ITF_FINAL
            // CPU without Vsync
            curFps = getfPsNoVsync();
            sprintf(bfr, " CPU: %0.02f ms  ", 1000.f/curFps);
            DrawString(xpos, ypos2, bfr, COLOR_LIMIT_INF(curFps,limit1,limit2), hScaleY2);
            ypos2 += (FontData->cellHeight * hScaleY2);

            // GPU
            sprintf(bfr, " GPU: %0.02f ms  ", 1000.f/gf32_DBG_gpu);
            xpos += DrawString(xpos, ypos2, bfr, COLOR_LIMIT_INF(gf32_DBG_gpu,limit1,limit2), hScaleY2);

            // Overflow
            {
		        const u32 ocount = GXGetOverflowCount();
                static OSTick tlastMax = 0;
                static u32 ocountmax = 0;
                OSTick tick = OSGetTick();
                if(OSTicksToMilliseconds(OSDiffTick(tick,tlastMax)) > 1250) // reset max overflow.
                {
                    tlastMax = tick;
                    ocountmax = 0;
                }
                ocountmax = ocountmax < ocount ? ocount : ocountmax;
                if(ocountmax)
                {
		            sprintf(bfr, " / Overflowcount = %u  ", ocountmax);
		            xpos += DrawString(xpos, ypos2, bfr, colorERROR, hScaleY2);
		            GXResetOverflowCount();
                }
            }
#endif // ITF_FINAL
        }

        // Extra info
        if(!m_extraInfo.isEmpty())
        {
            sprintf(bfr, "(%ls)", m_extraInfo.cStr());
            DrawString(xpos, ypos, bfr, COLOR_GREEN, hScaleY2);
        }
        ypos += (FontData->cellHeight * hScaleY2);

#ifndef ITF_FINAL
        #ifdef SUPPORT_STATS_MANAGER
        {
            sprintf(bfr, "Actors:% 3d/%d", StatsManager::getptr()->getActorUpdate(), StatsManager::getptr()->getActor());
            xpos += DrawString(xpos, ypos, bfr, COLOR_LIMIT_SUP(StatsManager::getptr()->getActorUpdate(),140,100), hScaleY2);
        }
        #endif

        u32 addr = SOGetHostID();
        if(addr)
        {
            sprintf(bfr," IP:%u.%u.%u.%u", (addr>>24)&0xff, (addr>>16)&0xff,(addr>>8)&0xff, addr&0xff);
            xpos += DrawString(xpos, ypos, bfr, COLOR_GREEN, hScaleY2);
        }
#endif // ITF_FINAL

        // Version
        if(!m_engineDataVersion.isEmpty())
        {
            sprintf(bfr, " (%ls)", m_engineDataVersion.cStr());
            DrawString(xpos, ypos, bfr, COLOR_GREEN, hScaleY2);
        }
    }
}
#endif // ITF_DISABLE_DEBUGINFO

///----------------------------------------------------------------------------//
/// Message Box.
///----------------------------------------------------------------------------//

void GFXAdapter_WII::DisplayMessageBox(const String& _title, const String& _msg, u32 _type)
{
	if(!Rmode)
		return;
	
	startRendering();
	clear(GFX_CLEAR_COLOR, COLOR_ZERO);
	disableFog();

	//----
	
    setOrthoView(0.f, (f32) getScreenWidth(), (f32) getScreenHeight(), 0.f);
    renderContext_SetDefault();

	// Set TEV parameters to "REPLACE COLOR"
	GXSetNumTevStages(1);
	GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0,GX_DISABLE,GX_SRC_REG,GX_SRC_VTX,GX_LIGHT_NULL,GX_DF_NONE,GX_AF_NONE);

    setAlphaBlend(GFX_BLEND_ALPHA);

	// Set up vertex descriptors
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS,  GX_DIRECT);
	GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS,  GX_POS_XYZ, GX_S16, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST,  GX_S16, 0);

	//----

	// Title
	{
		// Make panel alive.
		OSTick ti = OSGetTick();
		static OSTick lastTick = 0;
		static f32 f32_t = 0.f;	
		const f32 siz = 0.75f + 0.25f * f32_Abs(f32_Cos(f32_Modulo(f32_t,MTH_2PI)));
		if(OSTicksToMilliseconds(OSDiffTick(ti,lastTick)) > 32*siz)
		{
			lastTick = ti;
			f32_t += 0.1f;
		}

		StringConverter str_title(_title);
		const u32 col = (_type & ITF_MSG_ICONERROR) ? ITFCOLOR_TO_U32(255,255,64,64)
							: ((_type & ITF_MSG_ICONWARNING) ? ITFCOLOR_TO_U32(255,255,255,64)
								: ((_type & ITF_MSG_ICONQUESTION) ? ITFCOLOR_TO_U32(255,64,255,64)
									: COLOR_WHITE));
		DrawString(360, 50, (char*)str_title.getChar(), col, siz);
	}

	// Message
	{
		const u32 col = COLOR_WHITE;
		const f32 siz = 0.6f;
		
		char str_msg[512];
		u32 len = _msg.getLen();
		u32 max_len = 90;
		u32 s = 0;
		u32 y = 100;

		_msg.charCopyToBuffer(str_msg, 512);

		while(s<len && s <= 512-max_len)
		{
			char c = str_msg[s + max_len];
			str_msg[s + max_len] = '\0';

			DrawString(30, y, str_msg + s, col, siz);

			str_msg[s + max_len] = c;

			s += max_len;
			y += 20;
		}
	}

	// Choice
	{
		const u32 col = ITFCOLOR_TO_U32(255,64,64,255);
		const f32 siz = 0.7f;

		if(_type & ITF_MSG_YESNOCANCEL)
		{
			DrawString(300, getScreenHeight()-110, "Press \"1\" or \"A\" to debug.", col, siz);
			DrawString(300, getScreenHeight()-90, "Press \"2\" or \"+\" to ignore once.", col, siz);
			DrawString(300, getScreenHeight()-70, "Press \"-\" or \"B\" to ignore always.", col, siz);
		}
		else if(_type & ITF_MSG_YESNO)
		{
			DrawString(300, getScreenHeight()-110, "Press \"1\" or \"A\" for yes.", col, siz);
			DrawString(300, getScreenHeight()-90, "Press \"2\" or \"X\" for no.", col, siz);
		}
		else if(_type & ITF_MSG_OK)
		{
			DrawString(300, getScreenHeight()-110, "Press \"1\" or \"A\" to continue.", col, siz);
		}
	}

	//----

	endRendering();
	present();
}

struct LineStr
{
    const char *st, *en;
    s16 sz, tabCount;
};

void GFXAdapter_WII::DisplayMessageBox(const String& _msgString, u32 _maxcharline)
{
    f32 _f32_x = 0.f;
    f32 _f32_y = 0.f;
    bool _b_CenteredX = true;
    bool _b_CenteredY = true;

    void        *image;
    s32         x, y, yinc, tabSize, xChar, yChar, width, lastBegSize, spaceWidth, tabWidth;
    const char  *lastBegWord;
    char        cbuf[2] = " ";
    LineStr     lineTab[64];
    LineStr     *plineTab = lineTab;    
    f32         scaleX = 1.f;
    f32         scaleY = 1.f;
    f32         scaledScrWidth, scaledScrHeight;

	if(!Rmode)
		return;

	clear(GFX_CLEAR_COLOR, COLOR_ZERO);
	startRendering();
	disableFog();
    setResolution_WII((SCGetAspectRatio() == SC_ASPECT_RATIO_16x9) ? 812 : 608, 456, SCGetAspectRatio());

	//----

    scaledScrWidth  = (f32) getScreenWidth()  / scaleX;
    scaledScrHeight = (f32) getScreenHeight() / scaleY;

    setOrthoView(0.f, scaledScrWidth, scaledScrHeight, 0.f);
    renderContext_SetDefault();
    resetUVAnim();

	// Set TEV parameters to "REPLACE COLOR"
	GXSetNumTevStages(1);
	GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0,GX_DISABLE,GX_SRC_REG,GX_SRC_VTX,GX_LIGHT_NULL,GX_DF_NONE,GX_AF_NONE);

    setAlphaBlend(GFX_BLEND_ALPHA);

	// Set up vertex descriptors
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS,  GX_DIRECT);
	GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS,  GX_POS_XYZ, GX_S16, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST,  GX_S16, 0);

	char _msg[512];
	_msgString.charCopyToBuffer(_msg, sizeof(_msg));

    ITF_MemSet(lineTab, 0, sizeof(lineTab));    
    lastBegWord  = 0;
    lastBegSize  = 0;
    plineTab->st = _msg;
    plineTab->sz = _f32_x;
    
    OSGetFontTexture(cbuf, &image, &xChar, &yChar, &spaceWidth);
    *cbuf = '\t';
    OSGetFontTexture(cbuf, &image, &xChar, &yChar, &tabWidth); 
       
    /* prepare lines */
    char* pMsg = (char*)_msg;
    while(*pMsg && ((pMsg - _msg) < (u16)_maxcharline))
    {
        switch(*pMsg)
        {
            case '\n':
            case '\r':
            {
                plineTab->en = pMsg;
                plineTab++;
                /* init next line */
                plineTab->st = pMsg+1; 
                plineTab->sz = _f32_x;
                lastBegWord = 0;
                lastBegSize = 0;
                break;
            }
            /* char wrap */
            case '\t':
                plineTab->tabCount++;
            case '(':
            case '[':
            case ' ':
            case ':':
            case '.':
            case ',':
            {
                lastBegWord = pMsg;
                lastBegSize = plineTab->sz; // contain line size less lastBegWord size.
            }
            default:
                OSGetFontTexture(pMsg, &image, &xChar, &yChar, &width);
                plineTab->sz += width;
                const char *pNextChar;
                if(lastBegWord && (plineTab->sz > scaledScrWidth))
                {
                    switch(*lastBegWord)
                    {
                        /* skip char */
                        case ' ':
                            plineTab->en = lastBegWord;
                            pNextChar = lastBegWord;
                            break;
                        /* put char on next line */
                        case '\t':
                            plineTab->tabCount--;
                        case '(':
                        case '[':
                            plineTab->en = lastBegWord;
                            pNextChar = lastBegWord-1;
                            break;
                        /* keep char on line */
                        default :
                            pNextChar = OSGetFontTexture(lastBegWord, &image, &xChar, &yChar, &width);
                            lastBegSize += width;
                            plineTab->en = pNextChar;
                            lastBegWord = pNextChar;
                            break;
                    }
                    plineTab->sz = lastBegSize;
                    plineTab++;
                    /* init next line */
                    plineTab->st = pNextChar;
                    pMsg         = (char*)lastBegWord;
                    plineTab->sz = _f32_x;
                    lastBegWord  = 0;
                    lastBegSize  = 0;
                }
                break;
        }
        pMsg = OSGetFontTexture(pMsg, &image, &xChar, &yChar, &width);
    }
    plineTab->en = pMsg;

    y = _f32_y * scaledScrHeight;
    if(_b_CenteredY)
    {
        y = FontData->cellHeight * (plineTab-lineTab + 1);
        y = (scaledScrHeight - y) / 2;
    }
        
    yinc = FontData->cellHeight;

    OSGetFontTexture(" ", &image, &xChar, &yChar, &width);
    LoadSheet(image);

    /* handle lines */
    plineTab = lineTab;
    while(plineTab->sz)
    {
        /* strip ending spaces */
        while( (plineTab->en > plineTab->st) && (*(plineTab->en-1) == ' ') )
        {
            plineTab->sz -= spaceWidth;
            plineTab->en--;
        }

        /* strip starting spaces */
        while( (plineTab->st < plineTab->en) && (*plineTab->st == ' ') )
        {
            plineTab->sz -= spaceWidth;
            plineTab->st++;
        }
        
        /* Check line position */
        if(_b_CenteredX)
            x = (scaledScrWidth - plineTab->sz) / 2;
        else
            x = _f32_x;
        if(plineTab->tabCount)
        {
            tabSize = tabWidth + (x*2 / plineTab->tabCount);
            x = _f32_x;
        }
        else
            tabSize = tabWidth;

        /* render line */
        // This will work with japanese
        char* pLine = (char*)plineTab->st;
        while(pLine != plineTab->en)
        {
        	if(*pLine == '\t')
        	{
        		width = tabSize;
        		pLine = OSGetFontTexture(pLine, &image, &xChar, &yChar, &width);
        	}
        	else
        	{
	            pLine = OSGetFontTexture(pLine, &image, &xChar, &yChar, &width);                        
	            DrawCell(x, y, xChar, yChar, 0xffffffff, 1.f);
	        }
            x += width;
        }
        
        y += yinc;

        plineTab++;
    }    

	endRendering();
	present();

    setResolution_WII(getScreenWidth(), getScreenHeight(), SC_ASPECT_RATIO_16x9);
}

} // namespace ITF
