#include "precompiled_SystemAdapter_WII.h"

#ifndef _ITF_FILE_WII_H_
#include "core/file/wii/File_WII.h"
#endif //_ITF_FILE_WII_H_

#ifndef __MEM_MACROS_WII_H__
#include "core/memory/WII/MEM_macros_WII.h"
#endif //__MEM_MACROS_WII_H__

#ifndef _ITF_WARNINGMANAGER_H_
#include "engine/common/WarningManager.h"
#endif // _ITF_WARNINGMANAGER_H_

#ifndef _ITF_WII_DRAWPRIM_H_
#include "adapters/GFXAdapter_WII/GFXAdapter_WII.h"
#endif //_ITF_WII_DRAWPRIM_H_

#ifndef _ITF_INPUTADAPTER_TPL_WII_H_
#include "adapters/SystemAdapter_WII/SystemAdapter_TPL_WII.h"
#endif //_ITF_INPUTADAPTER_TPL_WII_H_

#ifndef _ITF_SYSTEMADAPTER_WII_H_
#include "adapters/SystemAdapter_WII/SystemAdapter_WII.h"
#endif //_ITF_SYSTEMADAPTER_WII_H_

namespace ITF
{
	bbool SystemAdapter_TPL::readAndAlloc(TPLPalettePtr* _pal, const String& _path, u32 * _readBytes)
	{		
		hwFile hFile; 
		//_pal = NULL;
		if (!hFile.open(_path.cStr(), 0))
		{
			ITF_ASSERT_MSG(0, "Could not open palette file");
			return bfalse;
		}
		
		unsigned long paletteSize = OSRoundUp32B(hFile.getLength());
		
		
		TPLPalette* p ; //= *_pal;
		
		
		p = (TPLPalettePtr)Memory::alignedMallocCategory(paletteSize, 32, MemoryId::mId_WiiSystem);
		//(*_pal) = (TPLPalettePtr)malloc(paletteSize);
		
		
		u32 readBytes = 0;
		if (!hFile.read(p, paletteSize, &readBytes))
		{
			ITF_ASSERT_MSG(0, "Could not read palette data");
			hFile.close();
			return bfalse;
		}

		hFile.close();
		if(_readBytes)
			*_readBytes = readBytes;

        DCFlushRange(p, paletteSize);

        *_pal = p;
		return btrue;
	}
	
	void SystemAdapter_TPL::unpack ( TPLPalettePtr _pal )
	{
		if(_pal->versionNumber != 2142000 )
		{			
			ITF_ASSERT_MSG(0, "Invalid version number for texture palette");
			return;
		}
		_pal->descriptorArray = (TPLDescriptorPtr)(((u32)(_pal->descriptorArray)) + ((u32)_pal));
		for (u16 i = 0; i < _pal->numDescriptors; i++ )
		{
			if(_pal->descriptorArray[i].textureHeader)
			{
				_pal->descriptorArray[i].textureHeader = (TPLHeaderPtr)(((u32)(_pal->descriptorArray[i].textureHeader)) + ((u32)_pal));	

				if(!(_pal->descriptorArray[i].textureHeader->unpacked))
				{
					_pal->descriptorArray[i].textureHeader->data = (Ptr)((u32)(_pal->descriptorArray[i].textureHeader->data) + (u32)_pal);
					_pal->descriptorArray[i].textureHeader->unpacked = 1;
				}
			}
			if(_pal->descriptorArray[i].CLUTHeader)
			{
				_pal->descriptorArray[i].CLUTHeader = (TPLClutHeaderPtr)((u32)(_pal->descriptorArray[i].CLUTHeader) + (u32)_pal);		

				if(!(_pal->descriptorArray[i].CLUTHeader->unpacked))
				{
					_pal->descriptorArray[i].CLUTHeader->data = (Ptr)((u32)(_pal->descriptorArray[i].CLUTHeader->data) + (u32)_pal);
					_pal->descriptorArray[i].CLUTHeader->unpacked = 1;
				}
			}
		}
    }

    void SystemAdapter_TPL::draw(TPLPalettePtr _pal, Vec2d _pos, Vec2d _size, u8 _alpha, bbool _drawStripe)
    {
        const f32 W = GFX_ADAPTER_WII->getRealScreenWidth();
        const f32 H = GFX_ADAPTER_WII->getRealScreenHeight();

        const f32 fTop    = _pos.m_y;
        const f32 fBottom = fTop + _size.m_y;
        const f32 fLeft   = _pos.m_x;
        const f32 fRight  = fLeft + _size.m_x;

//         if (SCGetAspectRatio() == SC_ASPECT_RATIO_16x9)
//         {
//         //    float fCorrection = 812.f / 608.f;
//         //    fLeft /= fCorrection;
//         //    fRight /= fCorrection;
//         }		

        GXTexObj texObj;
        TPLGetGXTexObjFromPalette(_pal, &texObj, 0);
        GXLoadTexObj(&texObj,GX_TEXMAP0);		
        GXSetBlendMode(GX_BM_BLEND,GX_BL_SRCALPHA,GX_BL_INVSRCALPHA,GX_LO_NOOP);

        f32 pm[GX_PROJECTION_SZ];
        GXGetProjectionv(pm);
        Mtx44 ortho;
        MTXOrtho(ortho, 0.0f, H, 0.0f, W, 0.0f, 100.0f);
        GXSetProjection(ortho,GX_ORTHOGRAPHIC);

        GFX_RECT saveViewport;
        GFX_ADAPTER->getViewport(&saveViewport);

        GFX_RECT Viewport;
        Viewport.left   = 0;
        Viewport.top    = 0;
        Viewport.right  = (u32)W;
        Viewport.bottom = (u32)H;
        GFX_ADAPTER->setupViewport(&Viewport);

        GXVtxDescList attr_list[GX_MAX_VTXDESCLIST_SZ];		
        GXGetVtxDescv(attr_list);

        GXCullMode mode;
        GXGetCullMode(&mode);
        GXSetCullMode(GX_CULL_NONE);

        Mtx id;
        MTXIdentity(id);
        GXLoadPosMtxImm(id,GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);

        GXSetZMode(GX_FALSE,GX_LEQUAL,GX_FALSE);

        GXSetNumChans(1);
        GXSetChanCtrl(GX_COLOR0A0,GX_DISABLE,GX_SRC_REG,GX_SRC_VTX,GX_LIGHT_NULL,GX_DF_NONE,GX_AF_NONE);

        GXSetColorUpdate(GX_TRUE);
        GXSetAlphaUpdate(GX_TRUE);

        for(u32 i=GX_TEVSTAGE0 ; i<8 ; ++i)
        {
            GXSetTevSwapMode((GXTevStageID)i, GX_TEV_SWAP0, GX_TEV_SWAP0);
            GXSetTevDirect((GXTevStageID)i);
        }

        GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_NEVER, 0);
        GXSetNumTevStages(1);

        GFX_ADAPTER_WII->resetUVAnim();
        GXSetTevOrder(GX_TEVSTAGE0,GX_TEXCOORD0,GX_TEXMAP0,GX_COLOR0A0);
        GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);		
        GXSetNumTexGens(1);

        GXClearVtxDesc();
        GXSetVtxDesc(GX_VA_POS,GX_DIRECT);
        GXSetVtxDesc(GX_VA_CLR0,GX_DIRECT);	
        GXSetVtxDesc(GX_VA_TEX0,GX_DIRECT);

        GXSetVtxAttrFmt(GX_VTXFMT0,GX_VA_POS,GX_POS_XY,GX_F32,0);
        GXSetVtxAttrFmt(GX_VTXFMT0,GX_VA_CLR0,GX_CLR_RGBA,GX_RGBA8,0);
        GXSetVtxAttrFmt(GX_VTXFMT0,GX_VA_TEX0,GX_TEX_ST,GX_F32,0);

        GXBegin(GX_QUADS,GX_VTXFMT0,4);		
            GXPosition2f32(fLeft,fTop);
            GXColor4u8(255,255,255,_alpha);
            GXTexCoord2f32(0,0);

            GXPosition2f32(fRight,fTop);		
            GXColor4u8(255,255,255,_alpha);
            GXTexCoord2f32(1,0);

            GXPosition2f32(fRight,fBottom);
            GXColor4u8(255,255,255,_alpha);
            GXTexCoord2f32(1,1);

            GXPosition2f32(fLeft,fBottom);
            GXColor4u8(255,255,255,_alpha);
            GXTexCoord2f32(0,1);
        GXEnd();

        if(SCGetAspectRatio() == SC_ASPECT_RATIO_16x9 && _drawStripe)
        {              
            GFX_ADAPTER_WII->drawScreenQuad(0.0f, 0.0f, fLeft, H, 0.0f, Color(_alpha/255.0f, 1.0f, 1.0f, 1.0f).getAsU32(), btrue);
            GFX_ADAPTER_WII->drawScreenQuad(fRight, 0.0f, W - fRight, H, 0.0f, Color(_alpha/255.0f, 1.0f, 1.0f, 1.0f).getAsU32(), btrue);
        }

        GFX_ADAPTER->setupViewport(&saveViewport);
        GXSetProjectionv(pm);

        GXSetCullMode(mode);
        GXSetZMode(GX_TRUE,GX_LEQUAL,GX_TRUE);
        GXSetVtxDescv(attr_list);		
        GXSetNumTexGens(1);

        GXSetBlendMode(GX_BM_NONE,GX_BL_ONE,GX_BL_ZERO,GX_LO_NOOP);
    }

} // namespace ITF