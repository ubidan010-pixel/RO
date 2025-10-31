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

#include "core/memory/memory_WII.h"

#ifndef _ITF_STATSMANAGER_H_
#include "engine/stats/statsManager.h"
#endif //_ITF_STATSMANAGER_H_

//#define DBG_STATIC_PNC3T
//#define DBG_CHECK_DRAW_VALIDITY

#define USE_DL

#ifdef VBMANAGER_USE_DYNAMICRING_VB
# define USE_RINGBUFFER_4_DYNMESH
#endif

#pragma relax_restrict on

namespace ITF
{

#ifndef ITF_FINAL
#pragma global_optimizer off
struct DrawSkipper
{
    DrawSkipper()
    {
        ITF_MemSet(this, 0, sizeof(DrawSkipper));
    }
    bbool bSkipDrawFluid;
    bbool bSkipDrawMeshPT;    
    bbool bSkipDrawEl_P;
    bbool bSkipDrawEl_PT;
    bbool bSkipDrawEl_PC;
    bbool bSkipDrawEl_PCT;
    bbool bSkipDrawEl_PCBT;
    bbool bSkipDrawEl_PNC3T;
    bbool bSkipDrawEl_unknown;
};

static DrawSkipper g_dbgDrawSkipper;
bbool dbg_skipmeshelement(const ITF_Mesh& _mesh)
{    
#ifndef VBMANAGER_USE_DYNAMICRING_VB
    ITF_VertexBuffer* pVertexBuffer = _mesh.m_vtxBuffer ? _mesh.m_vtxBuffer : _mesh.m_vtxBufferBuffered[_mesh.m_userBuffer];
#else
    ITF_VertexBuffer* pVertexBuffer = _mesh.m_vtxBuffer;
#endif // VBMANAGER_USE_DYNAMICRING_VB    

    switch( pVertexBuffer->m_vertexFormat )
    {
        case VertexFormat_P :
            return g_dbgDrawSkipper.bSkipDrawEl_P;
        case VertexFormat_PT :
            return g_dbgDrawSkipper.bSkipDrawEl_PT;
        case VertexFormat_PC :
            return g_dbgDrawSkipper.bSkipDrawEl_PC;
        case VertexFormat_PCT :
            return g_dbgDrawSkipper.bSkipDrawEl_PCT;
        case VertexFormat_PCBT :
            return g_dbgDrawSkipper.bSkipDrawEl_PCBT;
        case VertexFormat_PNC3T :
            return g_dbgDrawSkipper.bSkipDrawEl_PNC3T;
        default :
            return g_dbgDrawSkipper.bSkipDrawEl_unknown;
    }
}
#pragma global_optimizer reset
#endif // ITF_FINAL

#define COLOR_TO_GX(c)	((GXColor){ITFCOLOR_F32_TO_U8((c).getRed()), ITFCOLOR_F32_TO_U8((c).getGreen()), ITFCOLOR_F32_TO_U8((c).getBlue()), ITFCOLOR_F32_TO_U8((c).getAlpha())})
#define GX_WHITE		((GXColor){255,255,255,255})

void GFXAdapter_WII::setTev(Tev_Type _type)
{
    GXColorSrc  matSrc = GX_SRC_REG;

    m_CurrentTevType = _type;

	switch(m_CurrentTevType)
	{
		case TEV_TC:
		    matSrc = GX_SRC_VTX;
        case TEV_TA:
		    // Tev definition
		    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
            GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR0A0);

		    // Channel Color
			GXSetNumChans(1);
            GXSetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, matSrc, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);

            // RGB(A)
		    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
		    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
		    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

		    // Separate alpha + Fog
            GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP1);
	        GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_CPREV, GX_CC_C0,   GX_CC_A0,   GX_CC_ZERO);
		    GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD,  GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		    GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO,  GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
		    GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD,  GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    		break;

		case TEV_T:
		    // Tev definition
		    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
		    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);

		    // Channel Color
		    GXSetNumChans(0);
			GXSetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);

            // RGB(A) + Fog
	        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_TEXC, GX_CC_C0,   GX_CC_A0, GX_CC_ZERO);
		    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_TEXA, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
		    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

		    // Separate alpha
            GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP1);
		    GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_CPREV, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
		    GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD,  GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		    GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_TEXA,  GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
		    GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD,  GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    		break;

		case TEV_C:
		    // Tev definition
		    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);

		    // Channel Color
			GXSetNumChans(1);
		    GXSetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);

            // RGB(A) + Fog
	        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_RASC, GX_CC_C0, GX_CC_A0, GX_CC_ZERO);
		    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_RASA, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
		    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    		break;

        case TEV_AFX_2X:
		    // Tev definition
		    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
		    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP2, GX_COLOR_NULL);

            // Tex gens
            GXSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX1, GX_IDENTITY);

		    // Channel Color
		    GXSetNumChans(0);
			GXSetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);

            // RGB1
	        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_TEXC, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
		    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
		    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

		    // RGB2 x RGB1 x 2
		    GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO);
		    GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_2, GX_TRUE, GX_TEVPREV);
		    GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
		    GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            break;

        case TEV_AFX_BIGBLUR:
            // Tev definition
            GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL); 
            GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP0, GX_COLOR_NULL);
            GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD2, GX_TEXMAP0, GX_COLOR_NULL);
            GXSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD3, GX_TEXMAP0, GX_COLOR_NULL);

            // Tex gens
            GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
            GXSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1);
            GXSetTexCoordGen(GX_TEXCOORD2, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX2);
            GXSetTexCoordGen(GX_TEXCOORD3, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX3);

            // Channel Color
            GXSetNumChans(0);
            GXSetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);

            // Blur blending coeff : 1/2, 1/3, 1/4
            GXSetTevKColorSel(GX_TEVSTAGE1,  GX_TEV_KCSEL_1_2);
            GXSetTevKColorSel(GX_TEVSTAGE2,  GX_TEV_KCSEL_3_8); // ~1/3
            GXSetTevKColorSel(GX_TEVSTAGE3,  GX_TEV_KCSEL_1_4);

            GXSetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_1);
            GXSetTevKAlphaSel(GX_TEVSTAGE1, GX_TEV_KASEL_1);
            GXSetTevKAlphaSel(GX_TEVSTAGE2, GX_TEV_KASEL_3_8);
            GXSetTevKAlphaSel(GX_TEVSTAGE3, GX_TEV_KASEL_1);

            // Each TEV blends its texture with preceding TEV output
            GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_ONE, GX_CC_ZERO);
            GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_KONST);
            GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);    

            GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_CPREV, GX_CC_TEXC, GX_CC_KONST, GX_CC_ZERO);
            GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_KONST);
            GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

            GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_CPREV, GX_CC_TEXC, GX_CC_KONST, GX_CC_ZERO);
            GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_KONST);
            GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);     

            GXSetTevColorIn(GX_TEVSTAGE3, GX_CC_CPREV, GX_CC_TEXC, GX_CC_KONST, GX_CC_ZERO);
            GXSetTevColorOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            GXSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_KONST);
            GXSetTevAlphaOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            break;

        case TEV_AFX_GLOW_2X:
            // Add one last stage for Mul2X
            GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
            GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_TEXC, GX_CC_CPREV, GX_CC_CPREV);
            GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_2, GX_TRUE, GX_TEVPREV);
            GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
            GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        case TEV_AFX_GLOW_COPY:
            // Tev definition
            GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
            GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP2, GX_COLOR_NULL);

            // Tex gens
            GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
            GXSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX1, GX_IDENTITY);

            // Channel Color
            GXSetNumChans(1);
            GXSetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);

            // RGB1 = original scene
            GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_ONE, GX_CC_ZERO);
            GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_RASA);    
            GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);    

            // RGB1 + RGB2(= blurred texture) * GlowFactor
            GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_TEXC, GX_CC_APREV, GX_CC_CPREV);
            GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
            GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            break;

	    case TEV_REFRACTION:
		    // Tev definition
		    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
		    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
            GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD2, GX_TEXMAP2, GX_COLOR_NULL);
            GXSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD3, GX_TEXMAP3, GX_COLOR_NULL);

            // Indirect Tev definition
            GXSetIndTexOrder(GX_INDTEXSTAGE0, GX_TEXCOORD3, GX_TEXMAP4);

            // Tex gens
            GXSetTexCoordGen(GX_TEXCOORD2, GX_TG_MTX3x4, GX_TG_POS,  GX_TEXMTX1);
            GXSetTexCoordGen(GX_TEXCOORD3, GX_TG_MTX3x4, GX_TG_TEX1, GX_TEXMTX0);

		    // Channel Color
		    GXSetNumChans(0);
			GXSetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);

            // Diffuse
	        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_TEXC, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
		    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_TEXA, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
		    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

		    // Separate alpha for blend
		    GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_CPREV, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
		    GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD,  GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		    GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_TEXA,  GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
		    GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD,  GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

            // Indirect lookup + Blend Diffuse with frame buffer
            GXSetTevIndirect
            (
                GX_TEVSTAGE2,
                GX_INDTEXSTAGE0,
                GX_ITF_8,
                GX_ITB_ST,
                GX_ITM_0,
                GX_ITW_OFF,
                GX_ITW_OFF,
                GX_FALSE,
                GX_FALSE,
                GX_ITBA_OFF
            );
	        GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_TEXC,  GX_CC_CPREV, GX_CC_APREV, GX_CC_ZERO);
		    GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD,  GX_TB_ZERO,  GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		    GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_APREV, GX_CA_ZERO,  GX_CA_ZERO, GX_CA_ZERO);
		    GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD,  GX_TB_ZERO,  GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

            // Alpha final from refraction map 
            GXSetTevColorIn(GX_TEVSTAGE3, GX_CC_CPREV, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
            GXSetTevColorOp(GX_TEVSTAGE3, GX_TEV_ADD,  GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            GXSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_TEXA,  GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
            GXSetTevAlphaOp(GX_TEVSTAGE3, GX_TEV_ADD,  GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            break;

        default:
    		ITF_ASSERT_MSG(0, "Unknown TEV settings.");
    		break;
	}
}

void GFXAdapter_WII::finalizeTevSetting()
{
    // Global Color
    GXSetChanAmbColor(GX_COLOR0A0, COLOR_TO_GX(getGFXGlobalColor()));
    GXSetChanMatColor(GX_COLOR0A0, GX_WHITE);

    // Fog Color
    GXSetTevColor(GX_TEVREG0, COLOR_TO_GX(m_colorFog));

    u8 NumTevStages = 1;
    u8 NumTexGens   = 0;
	switch(m_CurrentTevType)
	{
		case TEV_TC:
		case TEV_TA:
		    if(m_Texture)
		    {
		        NumTexGens = 1;
		        if(useSrcAlphaForBlend(u_CurrentBlendingMode))
		        {
                    switch(m_Texture->m_pixFormat)
                    {
                        case Texture::PF_CMPA:
                            NumTevStages = 2;
                            NumTexGens++;
                            GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR0A0);
                            GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP1);
                		    break;
                    }
		        }
		    }

		    if((NumTevStages == 1) && isUseFog())
		    {
		        NumTevStages = 2;
                GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
                GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
            }
		    break;

		case TEV_T:
            if(m_Texture)
            {
                NumTexGens = 1;
                if(useSrcAlphaForBlend(u_CurrentBlendingMode))
                {
                    switch(m_Texture->m_pixFormat)
                    {
                        case Texture::PF_CMPA:
                            // Separate alpha
                            NumTevStages = 2;
                            NumTexGens++;
                		    break;
                    }
                }
            }
            break;

        case TEV_C:
            NumTevStages = 1;
            NumTexGens   = 0;
            break;

        case TEV_AFX_2X:
            NumTevStages = 2;
            NumTexGens   = 2;
            break;

        case TEV_AFX_BIGBLUR:
            NumTevStages = 4;
            NumTexGens   = 4;
            break;

        case TEV_AFX_GLOW_COPY:
            NumTevStages = 2;
            NumTexGens   = 2;
            break;

        case TEV_AFX_GLOW_2X:
            NumTevStages = 3;
            NumTexGens   = 2;
            break;

	    case TEV_REFRACTION:
            GXSetNumIndStages(1);
            NumTevStages = 4;
            NumTexGens   = 4;
		    if(m_Texture)
		    {
                switch(m_Texture->m_pixFormat)
                {
                    case Texture::PF_CMPA:
                        GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
                        GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP1);
            		    break;

                    default:
                        GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
                        GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
            		    break;
                }
		    }
            GXSetTevSwapMode(GX_TEVSTAGE3, GX_TEV_SWAP0, GX_TEV_SWAP2);
            break;

        default:
            break;
    }

    if(m_CurrentRTPass == RT_PASS_ALPHA)
    {
        // Put alpha in color
        GXTevStageID nextStageID = (GXTevStageID)((u8)GX_TEVSTAGE0 + NumTevStages);

        GXSetTevSwapMode(nextStageID, GX_TEV_SWAP0, GX_TEV_SWAP0);
        GXSetTevOrder(nextStageID, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
        GXSetTevColorIn(nextStageID, GX_CC_APREV, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
        GXSetTevColorOp(nextStageID, GX_TEV_ADD,  GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevAlphaIn(nextStageID, GX_CA_APREV, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
        GXSetTevAlphaOp(nextStageID, GX_TEV_ADD,  GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

        ++NumTevStages;
    }

    GXSetNumTevStages(NumTevStages);
    GXSetNumTexGens(NumTexGens);
}

//----------------------------------------------------------------------------//

#ifndef ITF_CONSOLE_FINAL
void GFXAdapter_WII::drawTriangle(const Vec2d& _p1, const Vec2d& _p2, const Vec2d& _p3, float _r, float _g, float _b, float _a, float _z)
{
#if ITF_DEBUG_LEVEL > 0
    setObjectMatrix(GMatrixIdentity);

    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);

    Color fcol(_a, _r, _g, _b);
    GXColor col = COLOR_TO_GX(fcol);
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetChanAmbColor(GX_COLOR0A0, col);
    GXSetChanMatColor(GX_COLOR0A0, col);

    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

    setAlphaBlend(GFX_BLEND_ALPHA);

    DrawBegin(GFX_TRIANGLES, 3);
    {
        GXPosition3f32(_p1.m_x, _p1.m_y, _z);
        GXPosition3f32(_p2.m_x, _p2.m_y, _z);
        GXPosition3f32(_p2.m_x, _p2.m_y, _z);
    }
    DrawEnd();
#endif // ITF_DEBUG_LEVEL > 0
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::drawTriangle(const Vertex& _v1, const Vertex& _v2, const Vertex& _v3, const Texture* _tex, bbool _showWireFrame)
{
	ITF_ASSERT_MSG(0, "GFXAdapter_WII::drawTriangle not implemented");
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::drawFlatMesh(const ITF_Mesh& _mesh)
{
#if ITF_DEBUG_LEVEL > 0
    setObjectMatrix(_mesh.getMatrix());

    /// set alpha blend.
    setAlphaBlend(GFX_BLEND_ALPHA);

    /// set Vertex format
    setTev(TEV_C);

    finalizeTevSetting();
    setVertexBuffer(_mesh.m_vtxBuffer);

    for (u32 i = 0; i < _mesh.m_ElementList.size(); i++)
    {
        if (!_mesh.m_ElementList[i].m_indexBuffer)
            drawVertexBuffer_internal(GFX_TRIANGLES, _mesh.m_vtxBuffer, _mesh.m_ElementList[i].m_startIndex, _mesh.m_ElementList[i].m_count);
        else
            drawIndexedVertexBuffer_internal(GFX_TRIANGLES, _mesh.m_vtxBuffer, _mesh.m_ElementList[i].m_indexBuffer, _mesh.m_ElementList[i].m_count);
    } 
#endif // ITF_DEBUG_LEVEL > 0
}
#endif // ITF_CONSOLE_FINAL

//----------------------------------------------------------------------------//

void GFXAdapter_WII::setVertexFormat(u32 _vformat)
{
    m_currentVertexFormat = _vformat;

    GXClearVtxDesc();

    switch(_vformat)
    {
    case VertexFormat_P:
        ITF_ASSERT(bfalse);
        break;

    case VertexFormat_PC:
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);

        GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XY, GX_F32, 0);
        GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);

        setTev(TEV_C);
        break;

    case VertexFormat_PT:
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);

        GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XY, GX_F32, 0);
        GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

        setTev(TEV_T);
        break;

    case VertexFormat_PTa:
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);

        GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XY, GX_F32, 0);
        GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

        setTev(TEV_TA);
        break;

    case VertexFormat_PCBT:
        GXSetVtxDesc(GX_VA_PNMTXIDX, GX_DIRECT);
    case VertexFormat_PCT:
    case VertexFormat_PNC3T:
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);

        GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XY, GX_F32, 0);
        GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
        GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

        setTev(TEV_TC);
        break;

    default:
        ITF_ASSERT_MSG(0, "Primitive not recognized");
        break;
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::DrawPrimitive(PRIMITIVETYPE _type, const void* _p_Vertex, u32 _NumberVertex)
{
    ITF_VertexBuffer localVBfr;
    localVBfr.m_vertexFormat    = m_currentVertexFormat;
    localVBfr.mp_VertexBuffer   = (u32)_p_Vertex;

    finalizeTevSetting();
    drawVertexBuffer_direct(_type, &localVBfr, 0, _NumberVertex);

    // avoid vb destruction by object destructor.
    localVBfr.mp_VertexBuffer   = 0;
}

//----------------------------------------------------------------------------//
void GFXAdapter_WII::setVertexDesc(const ITF_VertexBuffer* _vertexBuffer, bool _bIndex16, bool _dupUV)
{
	ITF_ASSERT(_vertexBuffer && _vertexBuffer->mp_VertexBuffer);

	GXClearVtxDesc();

	GXAttrType idxType = _bIndex16 ? GX_INDEX16 : GX_INDEX8;
	void* pVBdata = (u8*)_vertexBuffer->mp_VertexBuffer + _vertexBuffer->m_offset;

    switch(_vertexBuffer->m_vertexFormat)
    {
    case VertexFormat_P:
        ITF_ASSERT(bfalse);
        break;

    case VertexFormat_PC:
	{
		VertexPC* adr = (VertexPC*)pVBdata;

		GXSetVtxDesc(GX_VA_POS, idxType);
		GXSetVtxDesc(GX_VA_CLR0, idxType);

		GXSetArray(GX_VA_POS, (void*) &adr[0].m_pos, sizeof(VertexPC));
		GXSetArray(GX_VA_CLR0, (void*) &adr[0].m_color, sizeof(VertexPC));

		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
        break;
	}

    case VertexFormat_PT:
    case VertexFormat_PTa:
	{
		VertexPT* adr = (VertexPT*)pVBdata;

		GXSetVtxDesc(GX_VA_POS, idxType);
		GXSetVtxDesc(GX_VA_TEX0, idxType);

		GXSetArray(GX_VA_POS, (void*) &adr[0].m_pos, sizeof(VertexPT));
		GXSetArray(GX_VA_TEX0, (void*) &adr[0].m_uv, sizeof(VertexPT));

		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

        if(_dupUV)
        {
            GXSetVtxDesc(GX_VA_TEX1, idxType);
            GXSetArray(GX_VA_TEX1, (void*) &adr[0].m_uv, sizeof(VertexPT));
            GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_TEX_ST, GX_F32, 0);
        }
        break;
	}

    case VertexFormat_PCT:
	{
		VertexPCT* adr = (VertexPCT*)pVBdata;

		GXSetVtxDesc(GX_VA_POS, idxType);
		GXSetVtxDesc(GX_VA_CLR0, idxType);
		GXSetVtxDesc(GX_VA_TEX0, idxType);

		GXSetArray(GX_VA_POS, (void*) &adr[0].m_pos, sizeof(VertexPCT));
		GXSetArray(GX_VA_CLR0, (void*) &adr[0].m_color, sizeof(VertexPCT));
		GXSetArray(GX_VA_TEX0, (void*) &adr[0].m_uv, sizeof(VertexPCT));

		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

        if(_dupUV)
        {
            GXSetVtxDesc(GX_VA_TEX1, idxType);
            GXSetArray(GX_VA_TEX1, (void*) &adr[0].m_uv, sizeof(VertexPCT));
            GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_TEX_ST, GX_F32, 0);
        }
        break;
	}

    case VertexFormat_PCBT:
	{
		VertexPCBT* adr = (VertexPCBT*)pVBdata;

        GXSetVtxDesc(GX_VA_PNMTXIDX, GX_DIRECT);
		GXSetVtxDesc(GX_VA_POS, idxType);
		GXSetVtxDesc(GX_VA_CLR0, idxType);
		GXSetVtxDesc(GX_VA_TEX0, idxType);

		GXSetArray(GX_VA_POS, (void*) &adr[0].m_pos, sizeof(VertexPCBT));
		GXSetArray(GX_VA_CLR0, (void*) &adr[0].m_color, sizeof(VertexPCBT));
		GXSetArray(GX_VA_TEX0, (void*) &adr[0].m_uv, sizeof(VertexPCBT));

		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

        if(_dupUV)
        {
            GXSetVtxDesc(GX_VA_TEX1, idxType);
            GXSetArray(GX_VA_TEX1, (void*) &adr[0].m_uv, sizeof(VertexPCBT));
            GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_TEX_ST, GX_F32, 0);
        }
        break;
	}

    case VertexFormat_PNC3T:
	{
		VertexPNC3T* adr = (VertexPNC3T*)_vertexBuffer->mp_VertexBuffer;
#if defined(USE_RINGBUFFER_4_DYNMESH) || defined (DBG_STATIC_PNC3T)
		GXSetVtxDesc(GX_VA_POS, idxType);
#else
		GXSetVtxDesc(GX_VA_POS, GX_DIRECT);		
#endif
		GXSetVtxDesc(GX_VA_CLR0, idxType);
		GXSetVtxDesc(GX_VA_TEX0, idxType);
        // GXSetArray(GX_VA_POS) may be set in ::computeMeshElement
		GXSetArray(GX_VA_POS, (void*) &adr[0].m_pos, sizeof(VertexPNC3T));
		GXSetArray(GX_VA_CLR0, (void*) &adr[0].m_color, sizeof(VertexPNC3T));
		GXSetArray(GX_VA_TEX0, (void*) &adr[0].m_uv, sizeof(VertexPNC3T));

		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

        if(_dupUV)
        {
            GXSetVtxDesc(GX_VA_TEX1, idxType);
            GXSetArray(GX_VA_TEX1, (void*) &adr[0].m_uv, sizeof(VertexPNC3T));
            GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_TEX_ST, GX_F32, 0);
        }
        break;
	}

    default:
        ITF_ASSERT_MSG(0, "Primitive not recognized");
        break;
    }
}

//----------------------------------------------------------------------------//
inline GFXAdapter_WII::Tev_Type MeshType_TO_TevType(ITF_Mesh::MeshType _type, i32 _vertexFormat)
{
	switch(_type)
	{
	case ITF_Mesh::meshType_PT:
        switch(_vertexFormat)
        {
        case VertexFormat_PTa:
            return GFXAdapter_WII::TEV_TA;
            break;
        case VertexFormat_PT:
        default:
    		return GFXAdapter_WII::TEV_T;
        }
	    break;

	case ITF_Mesh::meshType_PTC:
	default:
        switch(_vertexFormat)
        {
        case VertexFormat_P:
            ITF_ASSERT(bfalse);
            break;
        case VertexFormat_PC:
            return GFXAdapter_WII::TEV_C;
            break;
        case VertexFormat_PT:
            return GFXAdapter_WII::TEV_T;
            break;
        case VertexFormat_PTa:
            return GFXAdapter_WII::TEV_TA;
            break;
        case VertexFormat_PCT:
        case VertexFormat_PCBT:
        case VertexFormat_PNC3T:
        default:
    		return GFXAdapter_WII::TEV_TC;
        }
		break;
	}

	return GFXAdapter_WII::TEV_TC;
}

//----------------------------------------------------------------------------//

struct MeshElementDisplayListRef
{
    ITF_IndexBuffer*          m_indexBuffer;
    u32                       m_startIndex;
    u32                       m_count;
    u8                        m_vtxAttrNb;
    const ITF_MeshElement*    m_meshElement; // not always set

    inline bool operator < (const MeshElementDisplayListRef& _ref) const
    {
        if(m_indexBuffer != _ref.m_indexBuffer)
            return m_indexBuffer < _ref.m_indexBuffer;
        if(m_count != _ref.m_count)
            return m_count < _ref.m_count;
        if(m_startIndex != _ref.m_startIndex)
            return m_startIndex < _ref.m_startIndex;
        if(m_vtxAttrNb != _ref.m_vtxAttrNb)
            return m_vtxAttrNb < _ref.m_vtxAttrNb;
        return m_meshElement < _ref.m_meshElement;
    }
};

#ifdef USE_DL
typedef ITF_MAP<MeshElementDisplayListRef, GFXDisplayList>  MeshElementDisplayListMap;
typedef MeshElementDisplayListMap::iterator MeshElementDisplayListMapIt;
static MeshElementDisplayListMap DisplayListMap_PC;
static MeshElementDisplayListMap DisplayListMap_PT;
static MeshElementDisplayListMap DisplayListMap_PCT;
static MeshElementDisplayListMap DisplayListMap_PCBT;
#if defined(USE_RINGBUFFER_4_DYNMESH) || defined (DBG_STATIC_PNC3T)
static MeshElementDisplayListMap DisplayListMap_PNC3T;
#endif // USE_RINGBUFFER_4_DYNMESH
#endif // USE_DL

//----------------------------------------------------------------------------//
void GFXAdapter_WII::CheckDeprecatedDL()
{
#ifdef USE_DL
    const u32 dTime = 500;
    OSTick curTick = OSGetTick();

    for(MeshElementDisplayListMapIt it = DisplayListMap_PC.begin(); it != DisplayListMap_PC.end(); ++it)
    {
        if(OSTicksToMilliseconds(OSDiffTick(curTick,it->second.m_usage)) > dTime)
        {
            Memory::alignedFree(it->second.m_ptr);
            DisplayListMap_PC.erase(it);
            break;
        }
     }

    for(MeshElementDisplayListMapIt it = DisplayListMap_PT.begin(); it != DisplayListMap_PT.end(); ++it)
    {
        if(OSTicksToMilliseconds(OSDiffTick(curTick,it->second.m_usage)) > dTime)
        {
            Memory::alignedFree(it->second.m_ptr);
            DisplayListMap_PT.erase(it);
            break;
        }
     }

    for(MeshElementDisplayListMapIt it = DisplayListMap_PCT.begin(); it != DisplayListMap_PCT.end(); ++it)
    {
        if(OSTicksToMilliseconds(OSDiffTick(curTick,it->second.m_usage)) > dTime)
        {
            Memory::alignedFree(it->second.m_ptr);
            DisplayListMap_PCT.erase(it);
            break;
        }
     }

    for(MeshElementDisplayListMapIt it = DisplayListMap_PCBT.begin(); it != DisplayListMap_PCBT.end(); ++it)
    {
        if(OSTicksToMilliseconds(OSDiffTick(curTick,it->second.m_usage)) > dTime)
        {
            Memory::alignedFree(it->second.m_ptr);
            DisplayListMap_PCBT.erase(it);
            break;
        }
     }

#if defined(USE_RINGBUFFER_4_DYNMESH) || defined(DBG_STATIC_PNC3T)
    for(MeshElementDisplayListMapIt it = DisplayListMap_PNC3T.begin(); it != DisplayListMap_PNC3T.end(); ++it)
    {
        if(OSTicksToMilliseconds(OSDiffTick(curTick,it->second.m_usage)) > dTime)
        {
            Memory::alignedFree(it->second.m_ptr);
            DisplayListMap_PNC3T.erase(it);
            break;
        }
     }
#endif // USE_RINGBUFFER_4_DYNMESH
#endif // USE_DL
}

void GFXAdapter_WII::ForceDeprecatedDL(const ITF_IndexBuffer* _IndexBuffer)
{
#ifdef USE_DL
    // Erase immediately DL that use this _IndexBuffer to avoid re-using them
    // LockRendering to free DL to be sure that are not in use

    for(MeshElementDisplayListMapIt it = DisplayListMap_PC.begin(); it != DisplayListMap_PC.end();)
    {
        if(it->first.m_indexBuffer == _IndexBuffer)
        {
            LockRendering();
                Memory::alignedFree(it->second.m_ptr);
                it = DisplayListMap_PC.erase(it);
            UnlockRendering();
        }
        else
            ++it;
    }

    for(MeshElementDisplayListMapIt it = DisplayListMap_PT.begin(); it != DisplayListMap_PT.end();)
    {
        if(it->first.m_indexBuffer == _IndexBuffer)
        {
            LockRendering();
                Memory::alignedFree(it->second.m_ptr);
                it = DisplayListMap_PT.erase(it);
            UnlockRendering();
        }
        else
            ++it;
    }

    for(MeshElementDisplayListMapIt it = DisplayListMap_PCT.begin(); it != DisplayListMap_PCT.end();)
    {
        if(it->first.m_indexBuffer == _IndexBuffer)
        {
            LockRendering();
                Memory::alignedFree(it->second.m_ptr);
                it = DisplayListMap_PCT.erase(it);
            UnlockRendering();
        }
        else
            ++it;
    }

    for(MeshElementDisplayListMapIt it = DisplayListMap_PCBT.begin(); it != DisplayListMap_PCBT.end();)
    {
        if(it->first.m_indexBuffer == _IndexBuffer)
        {
            LockRendering();
                Memory::alignedFree(it->second.m_ptr);
                it = DisplayListMap_PCBT.erase(it);
            UnlockRendering();
        }
        else
            ++it;
    }

#if defined(USE_RINGBUFFER_4_DYNMESH) || defined(DBG_STATIC_PNC3T)
    for(MeshElementDisplayListMapIt it = DisplayListMap_PNC3T.begin(); it != DisplayListMap_PNC3T.end();)
    {
        if(it->first.m_indexBuffer == _IndexBuffer)
        {
            LockRendering();
                Memory::alignedFree(it->second.m_ptr);
                it = DisplayListMap_PNC3T.erase(it);
            UnlockRendering();
        }
        else
            ++it;
    }
#endif // USE_RINGBUFFER_4_DYNMESH
#endif // USE_DL
}

//----------------------------------------------------------------------------//
GFXDisplayList *GFXAdapter_WII::FindOrCreateMeshElementDisplayList(ITF_VertexBuffer* _vertexBuffer, const ITF_MeshElement& _elem)
{
#ifdef USE_DL
#ifndef ITF_FINAL
    static bbool s_ForceDisableDL = bfalse;
    if(s_ForceDisableDL)
        return NULL;
#endif // ITF_FINAL

    MeshElementDisplayListMap *pmap = NULL;
    u8 vtxAttrNb;
    switch(_vertexBuffer->m_vertexFormat)
    {
        case VertexFormat_PC:
        	pmap = &DisplayListMap_PC;
        	vtxAttrNb = 2;
            break;
        case VertexFormat_PT:
        case VertexFormat_PTa:
        	pmap = &DisplayListMap_PT;
        	vtxAttrNb = 2;
            break;
        case VertexFormat_PCT:
        	pmap = &DisplayListMap_PCT;
        	vtxAttrNb = 3;
        	break;
        case VertexFormat_PCBT:
        	pmap = &DisplayListMap_PCBT;
        	vtxAttrNb = 4;
        	break;
#if defined(USE_RINGBUFFER_4_DYNMESH) || defined(DBG_STATIC_PNC3T)
        case VertexFormat_PNC3T:
        	pmap = &DisplayListMap_PNC3T;
        	vtxAttrNb = 3;
        	break;
#endif // USE_RINGBUFFER_4_DYNMESH

        default:
            return NULL;
    }
    if((_vertexBuffer->m_vertexFormat != VertexFormat_PC) && NeedDupUV())
        ++vtxAttrNb;

    MeshElementDisplayListRef refObj;
    refObj.m_indexBuffer    = _elem.m_indexBuffer;
    refObj.m_count          = _elem.m_count;
    refObj.m_startIndex     = _elem.m_startIndex;
    refObj.m_vtxAttrNb      = vtxAttrNb;
    if(_vertexBuffer->m_vertexFormat == VertexFormat_PCBT)
        refObj.m_meshElement = &_elem;
    else
        refObj.m_meshElement = NULL;

    MeshElementDisplayListMapIt it = pmap->find( refObj );
    if(it != pmap->end())
        return &it->second;

    // Generate display list

    // Check index size.
    bool bUseInd16 = false;
    if (!_elem.m_indexBuffer)
    {
        bUseInd16 = _elem.m_startIndex + _elem.m_count > 254;
    }
#if defined(USE_RINGBUFFER_4_DYNMESH) && !defined(DBG_STATIC_PNC3T)
    else if((_vertexBuffer->m_vertexFormat == VertexFormat_PNC3T) && (_elem.m_count>254))
    {
        bUseInd16 = true;
    }
#endif // defined(USE_RINGBUFFER_4_DYNMESH) && !defined(DBG_STATIC_PNC3T)
    else
    {
    	u16* pindex = (u16*)_elem.m_indexBuffer->mp_IndexBuffer;
    	for (u32 i = 0; i < _elem.m_count; i++)
    	{
    		if(pindex[i]>254)
    		{
    		    bUseInd16 = true;
    		    break;
    		}
    	}
    }

    u32 allocatedsize = vtxAttrNb * _elem.m_count * (bUseInd16?2:1);
    allocatedsize = OSRoundUp32B(allocatedsize + 96);
    MEM_M_PushExt(MEM_C_MEM2_ALLOC);
    void *allocatedpointer = Memory::alignedMallocCategory(allocatedsize, 32, MemoryId::mId_IndexBuffer);
    MEM_M_PopExt();
    DCInvalidateRange(allocatedpointer,allocatedsize);

    GXBeginDisplayList(allocatedpointer, allocatedsize);

    GXBegin(GX_TRIANGLES, GX_VTXFMT0, _elem.m_count);
    if (!_elem.m_indexBuffer)
    {
    	ITF_ASSERT(_vertexBuffer && _vertexBuffer->mp_VertexBuffer);

    	switch(_vertexBuffer->m_vertexFormat)
        {
        case VertexFormat_PC:
        	drawPC(_vertexBuffer, _elem.m_startIndex, _elem.m_count, bUseInd16);
            break;
        case VertexFormat_PT:
        case VertexFormat_PTa:
        	drawPT(_vertexBuffer, _elem.m_startIndex, _elem.m_count, bUseInd16, NeedDupUV());
            break;
        case VertexFormat_PCT:
        	drawPCT(_vertexBuffer, _elem.m_startIndex, _elem.m_count, bUseInd16, NeedDupUV());
        	break;
        case VertexFormat_PCBT:
        	drawPCBT(_vertexBuffer, _elem.m_startIndex, _elem.m_count, bUseInd16, NeedDupUV());
        	break;
        case VertexFormat_PNC3T:
        	drawPNC3T(_vertexBuffer, _elem.m_startIndex, _elem.m_count, bUseInd16, NeedDupUV());
        	break;
        default:
            ITF_ASSERT_MSG(0, "Primitive not recognized");
            break;
        }
    }
    else
    {
    	switch(_vertexBuffer->m_vertexFormat)
        {
        case VertexFormat_PC:
        	drawIndexedPC(_elem.m_indexBuffer, _vertexBuffer, _elem.m_count, bUseInd16);
            break;
        case VertexFormat_PT:
        case VertexFormat_PTa:
        	drawIndexedPT(_elem.m_indexBuffer, _vertexBuffer, _elem.m_count, bUseInd16, NeedDupUV());
            break;
        case VertexFormat_PCT:
        	drawIndexedPCT(_elem.m_indexBuffer, _vertexBuffer, _elem.m_count, bUseInd16, NeedDupUV());
            break;
        case VertexFormat_PCBT:
        	drawIndexedPCBT(_elem.m_indexBuffer, _vertexBuffer, _elem.m_count, bUseInd16, NeedDupUV());
        	break;
        case VertexFormat_PNC3T:
        	drawIndexedPNC3T(_elem.m_indexBuffer, _vertexBuffer, _elem.m_count, bUseInd16, NeedDupUV());
        	break;
        default:
            ITF_ASSERT_MSG(0, "Primitive not recognized");
            break;
        }
    }
    GXEnd();


    GFXDisplayList newElement;
    newElement.m_size = GXEndDisplayList();
    newElement.m_ptr = allocatedpointer;
    newElement.m_bUseInd16 = bUseInd16;
    DCFlushRangeNoSync(allocatedpointer, newElement.m_size);

    std::pair<MeshElementDisplayListMapIt,bool> res;
    MEM_M_PushExt(MEM_C_MEM1_ALLOC);
    res = pmap->insert(std::pair<MeshElementDisplayListRef, GFXDisplayList>(refObj, newElement));
    MEM_M_PopExt();

    return &res.first->second;
#else // USE_DL
    return NULL;
#endif // USE_DL
}

///----------------------------------------------------------------------------//
#ifdef USE_WGP
# define OVERLAY_USE_WGP
#endif
#ifdef OVERLAY_USE_WGP
static inline 
void Compute_frize_PNC3T_Pos_WGP(const VertexPNC3T *_v, const GFX_Vector4& _VecOverlay)
{
#if 1
	register ps::Float2 csTime, v1, v2, vA, vT0, vT1, vT2, vOut;
	register ps::Float2 _VecOverlaym_x = ps::Load((float*)&_VecOverlay.m_x);
	register ps::Float2 _VecOverlaym_z = ps::Load((float*)&_VecOverlay.m_z);

	//    Vec2d::Scale(&csTime, (Vec2d*)&_VecOverlay.m_x, _v->m_uv3.m_z);
	csTime = ps::Mul(_VecOverlaym_x,_v->m_uv3.m_z);
	
	//	Vec2d::Scale(&vA, (Vec2d*)&_VecOverlay.m_z, _v->m_uv4.m_x);
	vA = ps::Mul(_VecOverlaym_z,_v->m_uv4.m_x);
	
	//	Vec2d::Scale(&vT0,     (Vec2d*)&_VecOverlay.m_x, _v->m_uv3.m_w);
	vT0 = ps::Mul(_VecOverlaym_x,_v->m_uv3.m_w);
	
	//	Vec2d::Scale(&vT1, (Vec2d*)&_VecOverlay.m_z, _v->m_uv4.m_y);
	vT1 = ps::Mul(_VecOverlaym_z,_v->m_uv4.m_y);

	//  csTime.m_x -= vT0.m_y;
    //	csTime.m_y += vT0.m_x;

	csTime = ps::Add(csTime,ps::MergeYX(ps::Neg(vT0), vT0));

	//    vA.m_x -= vT1.m_y;
    //		vA.m_y += vT1.m_x;

	vA = ps::Add(vA,ps::MergeYX(ps::Neg(vT1), vT1));


	//Vec2d::MulAdd(&v1, &csTime, (Vec2d*)&_v->m_uv2.m_x, (Vec2d*)&_v->m_uv2.m_z);
	v1 = ps::Madd(csTime, ps::Load((float*)&_v->m_uv2.m_x), ps::Load((float*)&_v->m_uv2.m_z));

	//Vec2d::Mul(&v1, &v1, (Vec2d*)&_v->m_uv3.m_x);
	v1 = ps::Mul(v1,ps::Load((float*)&_v->m_uv3.m_x));

	//Vec2d::Scale(&vT2, &vA, v1.m_y);
	vT2 = ps::Mul(vA, ps::GetY(v1));
	
	//Vec2d::Scale(&v2, &vA, v1.m_x);
	v2 = ps::Mul(vA, ps::GetX(v1));
	
	//v2.m_x -= vT2.m_y;
    //v2.m_y += vT2.m_y;
	v2 = ps::Add(v2,ps::MergeYY(ps::Neg(vT2),vT2));
	
	//Vec2d::Add(&vOut, (Vec2d*)&_v->m_pos.m_x, &v2);
	vOut = ps::Add(ps::Load((float*)&_v->m_pos.m_x), v2);

//	GXWGFifo.f32 = ps::GetX(vOut);
//  GXWGFifo.f32 = ps::GetY(vOut);
    ps::Store(vOut, (float*)&GXWGFifo);
    GXWGFifo.f32 = _v->m_pos.m_z;
#else
	register Vec2d csTime, v1, v2, vA, vT0, vT1, vT2, vOut;
    register Vec2d vm11(-1.f,1.f);
    csTime = Vec2d::ScaleX((Vec2d*)&_VecOverlay.m_x, (Vec2d*)&_v->m_uv3.m_z);
    vA  = Vec2d::ScaleX((Vec2d*)&_VecOverlay.m_z, (Vec2d*)&_v->m_uv4.m_x);
    vT0 = Vec2d::ScaleY((Vec2d*)&_VecOverlay.m_x, (Vec2d*)&_v->m_uv3.m_z);
    Vec2d::SwapXY(&vT0, &vT0);
    Vec2d::Mul(&vT0, &vT0, &vm11);
    vT1 = Vec2d::ScaleY((Vec2d*)&_VecOverlay.m_z, (Vec2d*)&_v->m_uv4.m_x);
    Vec2d::SwapXY(&vT1, &vT1);
    Vec2d::Mul(&vT1, &vT1, &vm11);
    Vec2d::Add(&csTime, &csTime, &vT0);
    Vec2d::Add(&vA, &vA, &vT1);
    Vec2d::MulAdd(&v1, &csTime, (Vec2d*)&_v->m_uv2.m_x, (Vec2d*)&_v->m_uv2.m_z);
    Vec2d::Mul(&v1, &v1, (Vec2d*)&_v->m_uv3.m_x);
    vT2 = Vec2d::ScaleY(&vA, &v1);
    v2  = Vec2d::ScaleX(&vA, &v1);
    Vec2d::MergeYY(&vT2, &vT2, &vT2);
    Vec2d::Mul(&vT2, &vT2, &vm11);
    Vec2d::Add(&v2, &v2, &vT2);
    Vec2d::Add(&vOut, (Vec2d*)&_v->m_pos.m_x, &v2);
//    GXWGFifo.f32 = vOut.m_x;
//    GXWGFifo.f32 = vOut.m_y;
    *(Vec2d*)&GXWGFifo = vOut;
    GXWGFifo.f32 = _v->m_pos.m_z;
#endif
}
#endif // OVERLAY_USE_WGP

static inline void Compute_frize_PNC3T_Pos(Vec3d * restrict _vout, const VertexPNC3T *_v, const GFX_Vector4& _VecOverlay)
{
    f32 cosTime = _VecOverlay.m_x * _v->m_uv3.m_z - _VecOverlay.m_y * _v->m_uv3.m_w;
    f32 sinTime = _VecOverlay.m_y * _v->m_uv3.m_z + _VecOverlay.m_x * _v->m_uv3.m_w;
                     
    f32 x1 = (cosTime * _v->m_uv2.m_x + _v->m_uv2.m_z) * _v->m_uv3.m_x;
    f32 y1 = (sinTime * _v->m_uv2.m_y + _v->m_uv2.m_w) * _v->m_uv3.m_y;

    f32 cosAngle = _v->m_uv4.m_x * _VecOverlay.m_z - _v->m_uv4.m_y * _VecOverlay.m_w;
    f32 sinAngle = _v->m_uv4.m_x * _VecOverlay.m_w + _v->m_uv4.m_y * _VecOverlay.m_z;

    f32 x2 = x1 * cosAngle - y1 * sinAngle;
    f32 y2 = x1 * sinAngle + y1 * cosAngle;

    // /!\ _vout has been initialized with _v->m_pos
    _vout->m_x += x2;
    _vout->m_y += y2;
}

///----------------------------------------------------------------------------//
void GFXAdapter_WII::computeMeshElement(const ITF_VertexBuffer* _vertexBuffer, const ITF_Mesh& _mesh, const ITF_MeshElement& _elem)
{
#if defined(USE_RINGBUFFER_4_DYNMESH) && !defined(DBG_STATIC_PNC3T)
    if(_vertexBuffer->m_vertexFormat == VertexFormat_PNC3T)
    {
        Vec3d *pdataOut;
        const VertexPNC3T * restrict pdataIn = (const VertexPNC3T*)_vertexBuffer->mp_VertexBuffer;
        u16 idxStartAnimated = pdataIn[0].m_uv2.m_x;

        #ifdef SUPPORT_STATS_MANAGER
        StatsManager::getptr()->drawPNC3TVtxCntInc(_elem.m_count);
        #endif

        ITF_VertexBuffer vbuffer;
#if 1
        ITF_ASSERT(_elem.m_indexBuffer); // check if next code must be compiled.
#else
        if (!_elem.m_indexBuffer)
        {
            pdataIn += _elem.m_startIndex;
            getVertexBufferManager().LockDynamicRingVB(_elem.m_count * sizeof(Vec3d), &vbuffer, (void**)&pdataOut);
            Vec3d *p = pdataOut;
            for(u32 i = 0; i < _elem.m_count; ++i)
            {
                *p = pdataIn->m_pos;
                Compute_frize_PNC3T_Pos(p++, pdataIn++, m_VecOverlay);
            }
            getVertexBufferManager().UnlockDynamicRingVB(&vbuffer);
        }
        else
#endif
        {
            const u16* pindex = (u16*)_elem.m_indexBuffer->mp_IndexBuffer;
            #ifndef OVERLAY_USE_WGP
            // +96 for 3 dbcz instructions while clearing output pos.
            const u32 padAdd = 96;
            #else
            const u32 padAdd = 0;
            #endif
            getVertexBufferManager().LockDynamicRingVB(_elem.m_count * sizeof(Vec3d) + padAdd, &vbuffer, (void**)&pdataOut);
            #ifdef OVERLAY_USE_WGP
            const int cnt = (int)_elem.m_count;
            int i = 0;
            u16 idx;
            #if defined( ITF_PSI )
            register float* dst = (float*)&GXWGFifo;
            #endif
            while(i < cnt)
            {
                idx = pindex[i++];
                const VertexPNC3T * pv = &pdataIn[idx];
                if(idx >= idxStartAnimated)
                    Compute_frize_PNC3T_Pos_WGP(pv, m_VecOverlay);
                else
                {
                    #if defined( ITF_PSI )
		            register __vec2x32float__ f_p0;
		            register float* src = (float*)&pv->m_pos.m_x;
		            asm	{		
			            psq_l     f_p0,  0(src), 0, 0;
			            psq_st    f_p0,  0(dst), 0, 0;
		            }
                    #else
                    GXWGFifo.f32 = pv->m_pos.m_x;
                    GXWGFifo.f32 = pv->m_pos.m_y;
                    #endif
                    GXWGFifo.f32 = pv->m_pos.m_z;
                }
            }
            vbuffer.m_wgpWrite = 12 * cnt;
            #else // OVERLAY_USE_WGP
            Vec3d * restrict p = pdataOut;
            const int cnt = (int)_elem.m_count;
            int i = 0;
            u16 idx;
            while(i < cnt)
            {
                if((u32)p == OSRoundDown32B(p))
                {
                    __dcbz((void*)p, 0);
                    __dcbz((void*)p,32);
                    __dcbz((void*)p,64);
                }

                idx = pindex[i++];
                *p = pdataIn[idx].m_pos;
                if(idx >= idxStartAnimated)
                    Compute_frize_PNC3T_Pos(p, &pdataIn[idx], m_VecOverlay);                    
                p++;
            }
            #endif // OVERLAY_USE_WGP
            getVertexBufferManager().UnlockDynamicRingVB(&vbuffer);
        }
        vbuffer.mp_VertexBuffer = NULL; // avoid vb destruction by object destructor.
        GXSetArray(GX_VA_POS, (void*) pdataOut, sizeof(Vec3d));
    } // _vertexBuffer->m_vertexFormat == VertexFormat_PNC3T
    else if(_vertexBuffer->m_vertexFormat == VertexFormat_PCBT)
    {
        if(_mesh.m_pMatrixOverlay)
        {
            GMatrix44 mat = mg_ObjMatrix;

            mat.fa16[12] = mg_ObjMatrix.fa16[12] + _mesh.m_pMatrixOverlay->fa16[0];
            mat.fa16[13] = mg_ObjMatrix.fa16[13] + _mesh.m_pMatrixOverlay->fa16[1];
            setObjectMatrix(mat, GX_PNMTX1);

            mat.fa16[12] = mg_ObjMatrix.fa16[12] + _mesh.m_pMatrixOverlay->fa16[2];
            mat.fa16[13] = mg_ObjMatrix.fa16[13] + _mesh.m_pMatrixOverlay->fa16[3];
            setObjectMatrix(mat, GX_PNMTX2);

            mat.fa16[12] = mg_ObjMatrix.fa16[12] + _mesh.m_pMatrixOverlay->fa16[4];
            mat.fa16[13] = mg_ObjMatrix.fa16[13] + _mesh.m_pMatrixOverlay->fa16[5];
            setObjectMatrix(mat, GX_PNMTX3);

            mat.fa16[12] = mg_ObjMatrix.fa16[12] + _mesh.m_pMatrixOverlay->fa16[6];
            mat.fa16[13] = mg_ObjMatrix.fa16[13] + _mesh.m_pMatrixOverlay->fa16[7];
            setObjectMatrix(mat, GX_PNMTX4);

            mat.fa16[12] = mg_ObjMatrix.fa16[12] + _mesh.m_pMatrixOverlay->fa16[8];
            mat.fa16[13] = mg_ObjMatrix.fa16[13] + _mesh.m_pMatrixOverlay->fa16[9];
            setObjectMatrix(mat, GX_PNMTX5);

            mat.fa16[12] = mg_ObjMatrix.fa16[12] + _mesh.m_pMatrixOverlay->fa16[10];
            mat.fa16[13] = mg_ObjMatrix.fa16[13] + _mesh.m_pMatrixOverlay->fa16[11];
            setObjectMatrix(mat, GX_PNMTX6);

            mat.fa16[12] = mg_ObjMatrix.fa16[12] + _mesh.m_pMatrixOverlay->fa16[12];
            mat.fa16[13] = mg_ObjMatrix.fa16[13] + _mesh.m_pMatrixOverlay->fa16[13];
            setObjectMatrix(mat, GX_PNMTX7);

            mat.fa16[12] = mg_ObjMatrix.fa16[12] + _mesh.m_pMatrixOverlay->fa16[14];
            mat.fa16[13] = mg_ObjMatrix.fa16[13] + _mesh.m_pMatrixOverlay->fa16[15];
            setObjectMatrix(mat, GX_PNMTX8);
        }
    }
#endif // USE_RINGBUFFER_4_DYNMESH    
}
//----------------------------------------------------------------------------//
void GFXAdapter_WII::drawMeshElement(const ITF_Mesh& _mesh, ITF_VertexBuffer* _vertexBuffer, const ITF_MeshElement& _elem)
{
    ITF_ASSERT(_vertexBuffer && _vertexBuffer->mp_VertexBuffer);

    finalizeTevSetting();

    #ifdef SUPPORT_STATS_MANAGER
    StatsManager::getptr()->drawVtxCntInc(_elem.m_count);
    #endif

#ifdef USE_DL
    GFXDisplayList *dl = FindOrCreateMeshElementDisplayList(_vertexBuffer, _elem);
    if(dl)
    {
        setVertexDesc(_vertexBuffer, dl->m_bUseInd16, NeedDupUV());
        computeMeshElement(_vertexBuffer, _mesh, _elem);
        PPCSync();

        GXCallDisplayList(dl->m_ptr, dl->m_size);
        dl->m_usage = OSGetTick();
        return;
    }
    else
#endif // USE_DL
    {
        setVertexBuffer(_vertexBuffer);
        computeMeshElement(_vertexBuffer, _mesh, _elem);
    }

    if (!_elem.m_indexBuffer)
        drawVertexBuffer_internal(GFX_TRIANGLES, _vertexBuffer, _elem.m_startIndex, _elem.m_count);
    else
        drawIndexedVertexBuffer_internal(GFX_TRIANGLES, _vertexBuffer, _elem.m_indexBuffer, _elem.m_count);
}

//----------------------------------------------------------------------------//
void GFXAdapter_WII::drawMesh(ITF_Mesh& _mesh, bbool _showWireFrame)
{
#ifndef VBMANAGER_USE_DYNAMICRING_VB
    ITF_VertexBuffer* pVertexBuffer = _mesh.m_vtxBuffer ? _mesh.m_vtxBuffer : _mesh.m_vtxBufferBuffered[_mesh.m_userBuffer];
#else
    ITF_VertexBuffer* pVertexBuffer = _mesh.m_vtxBuffer;
#endif // VBMANAGER_USE_DYNAMICRING_VB
	if(!pVertexBuffer || !pVertexBuffer->mp_VertexBuffer) return;

    ITF_ASSERT(!pVertexBuffer->m_lockedsize);
    
    #ifndef ITF_FINAL
    if(dbg_skipmeshelement(_mesh))
        return;
    #endif    

    #ifdef SUPPORT_STATS_MANAGER
    switch( pVertexBuffer->m_vertexFormat )
    {
        case VertexFormat_P :     StatsManager::getptr()->drawMeshPInc();break;
        case VertexFormat_PT :    StatsManager::getptr()->drawMeshPTInc();break;
        case VertexFormat_PC :    StatsManager::getptr()->drawMeshPCInc();break;
        case VertexFormat_PCT :   StatsManager::getptr()->drawMeshPCTInc();break;
        case VertexFormat_PCBT :  StatsManager::getptr()->drawMeshPCBTInc();break;
        case VertexFormat_PNC3T : StatsManager::getptr()->drawMeshPNC3TInc();break;
        default : StatsManager::getptr()->drawMeshUnknownInc();break;
    }
    #endif // SUPPORT_STATS_MANAGER

    Tev_Type tevType = MeshType_TO_TevType(_mesh.m_type, pVertexBuffer->m_vertexFormat);
	setTev(tevType);
	setObjectMatrix(_mesh.getMatrix());

    for (u32 i = 0; i < _mesh.m_ElementList.size(); i++)
    {
        ITF_MeshElement& elem = _mesh.m_ElementList[i];

        if(!elem.m_count)
            continue;

        if (!elem.m_material.m_textureDiffuse.isValidResourceId())
            continue;

        if (elem.m_material.m_uvAnim.m_uvFlagAnim)
        {
            computeUVAnim(&elem.m_material.m_uvAnim);
            setUVAnim(elem.m_material.m_uvAnim);
        }
        else
        {
            resetUVAnim();
        }

        setGfxMaterial(elem.m_material);
        drawMeshElement(_mesh, pVertexBuffer, elem);
        unsetGfxMaterial(elem.m_material, tevType);
    }

    resetUVAnim();
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::drawMeshPT(const ITF_Mesh& _mesh, bbool _showWireFrame)
{
#ifndef ITF_FINAL
#pragma global_optimizer off
    if(g_dbgDrawSkipper.bSkipDrawMeshPT)
        return;
#pragma global_optimizer reset
#endif // ITF_FINAL

#ifndef VBMANAGER_USE_DYNAMICRING_VB
    ITF_VertexBuffer* pVertexBuffer = _mesh.m_vtxBuffer ? _mesh.m_vtxBuffer : _mesh.m_vtxBufferBuffered[_mesh.m_userBuffer];
#else
    ITF_VertexBuffer* pVertexBuffer = _mesh.m_vtxBuffer;
#endif // VBMANAGER_USE_DYNAMICRING_VB

	if(!pVertexBuffer || !pVertexBuffer->mp_VertexBuffer)
	    return;

    Tev_Type tevType = MeshType_TO_TevType(ITF_Mesh::meshType_PT, pVertexBuffer->m_vertexFormat);
	setTev(tevType);
	setObjectMatrix(_mesh.getMatrix());

    for (u32 i = 0; i < _mesh.m_ElementList.size(); i++)
    {
        const ITF_MeshElement& elem = _mesh.m_ElementList[i];

        if(!elem.m_count)
            continue;

        if (!elem.m_material.m_textureDiffuse.isValidResourceId())
            continue;

        resetUVAnim();

        setGfxMaterial(elem.m_material);
        drawMeshElement(_mesh, pVertexBuffer, elem);
        unsetGfxMaterial(elem.m_material, tevType);
    }

    resetUVAnim();
}

//----------------------------------------------------------------------------//

#ifndef ITF_CONSOLE_FINAL
void GFXAdapter_WII::drawLine(const Vec2d& _p1, const Vec2d& _p2, float _r, float _g, float _b, float _size, float _z)
{
	ITF_ASSERT_MSG(0, "GFXAdapter_WII::drawLine not implemented");
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::draw2dLine(const Vec2d& _p1, const Vec2d& _p2, float _size, u32 _color)
{
#if ITF_DEBUG_LEVEL > 0
    const f32 screenHeight = (f32)getScreenHeight();

    Vec2d p1(_p1.m_x, screenHeight - _p1.m_y);
    Vec2d p2(_p2.m_x, screenHeight - _p2.m_y);

    Vec2d dir = (p2 - p1).normalize();
    Vec2d perp = dir.getPerpendicular();
    perp *= _size * 0.5f;

    Box2D box;

    box.v[0].m_pos = (p1 + perp).to3d();
    box.v[1].m_pos = (p2 + perp).to3d();
    box.v[2].m_pos = (p1 - perp).to3d();
    box.v[3].m_pos = (p2 - perp).to3d();

    box.v[0].m_color = _color;
    box.v[1].m_color = _color;
    box.v[2].m_color = _color;
    box.v[3].m_color = _color;

    m_Box2DList.push_back(box);
#endif // ITF_DEBUG_LEVEL > 0
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::draw2dTriangle(const Vec2d& _p1, const Vec2d& _p2, const Vec2d& _p3, u32 _color, i32 _rank)
{
#if ITF_DEBUG_LEVEL > 0
    const f32 p1y = (f32)getScreenHeight() - _p1.m_y;
    const f32 p2y = (f32)getScreenHeight() - _p2.m_y;
    const f32 p3y = (f32)getScreenHeight() - _p3.m_y;

    Triangle2D tri;

    tri.v[0].m_pos.m_x  = _p1.m_x;
    tri.v[0].m_pos.m_y  = p1y;
    tri.v[1].m_pos.m_x  = _p2.m_x;
    tri.v[1].m_pos.m_y  = p2y;
    tri.v[2].m_pos.m_x  = _p3.m_x;
    tri.v[2].m_pos.m_y  = p3y;

    tri.v[0].m_color = _color;
    tri.v[1].m_color = _color;
    tri.v[2].m_color = _color;

    tri.m_rank = _rank;

    m_Triangle2DList.push_back(tri);
#endif // ITF_DEBUG_LEVEL > 0
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::draw2dBox(const Vec2d& _p, f32 _width, f32 _height, u32 _color0, u32 _color1, u32 _color2, u32 _color3, i32 _rank)
{
#if ITF_DEBUG_LEVEL > 0
    if (hasHideFlags(HideFlag_2DBox))
        return;

    /// strip:
    /// 0 -- 2
    /// |    |
    /// 1 -- 3

    const f32 py = (f32)getScreenHeight() - _p.m_y - _height;

    Box2D box;

    box.v[0].m_pos.m_x = _p.m_x;
    box.v[0].m_pos.m_y = py;
    box.v[1].m_pos.m_x = _p.m_x;
    box.v[1].m_pos.m_y = py + _height;
    box.v[2].m_pos.m_x = _p.m_x + _width;
    box.v[2].m_pos.m_y = py;
    box.v[3].m_pos.m_x = _p.m_x + _width;
    box.v[3].m_pos.m_y = py + _height;

    box.v[0].m_color = _color0;
    box.v[1].m_color = _color1;
    box.v[2].m_color = _color2;
    box.v[3].m_color = _color3;
    box.m_rank = _rank;

    m_Box2DList.push_back(box);
#endif // ITF_DEBUG_LEVEL > 0
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::drawCircle(float _x, float _y, float r, float _r, float _g, float _b, float _z, u32 _segsCount)
{
	ITF_ASSERT_MSG(0, "GFXAdapter_WII::drawCircle not implemented");
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::drawBox(const Vec2d& _pos, f32 _angle, const Vec2d& _extent, f32 _r, f32 _g, f32 _b, float _z, bbool _fill)
{
	ITF_ASSERT_MSG(0, "GFXAdapter_WII::drawBox not implemented");
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::drawSphere(float _x, float _y, float _z, float r, int lats, int longs, int _wireFrame, float _r, float _g, float _b)
{
	ITF_ASSERT_MSG(0, "GFXAdapter_WII::drawSphere not implemented");
}
#endif // ITF_CONSOLE_FINAL

///----------------------------------------------------------------------------//
/// Draw Vertex/IndexBuffer.
///----------------------------------------------------------------------------//

void GFXAdapter_WII::drawPCT(ITF_VertexBuffer* _vertexBuffer, u32 _st, u32 _nb, bool _bIndex16, bool _dupUV)
{
	ITF_ASSERT(_vertexBuffer && _vertexBuffer->mp_VertexBuffer);

    if(_dupUV)
    {
        if(_bIndex16)
	    {
    	    for (u32 i = 0; i < _nb; i++)
    	    {
    		    GXPosition1x16(i + _st);
    		    GXColor1x16(i + _st);
    		    GXTexCoord1x16(i + _st);
                GXTexCoord1x16(i + _st);
    	    }
        }
        else
        {
    	    for (u32 i = 0; i < _nb; i++)
    	    {
    		    GXPosition1x8(i + _st);
    		    GXColor1x8(i + _st);
    		    GXTexCoord1x8(i + _st);
                GXTexCoord1x8(i + _st);
    	    }
        }
    }
    else
    {
        if(_bIndex16)
        {
            for (u32 i = 0; i < _nb; i++)
            {
                GXPosition1x16(i + _st);
                GXColor1x16(i + _st);
                GXTexCoord1x16(i + _st);
            }
        }
        else
        {
            for (u32 i = 0; i < _nb; i++)
            {
                GXPosition1x8(i + _st);
                GXColor1x8(i + _st);
                GXTexCoord1x8(i + _st);
            }
        }
    }
}

//----------------------------------------------------------------------------//
void GFXAdapter_WII::DrawPCT(ITF_VertexBuffer* _vertexBuffer, u32 _st, u32 _nb)
{
    drawPCT(_vertexBuffer, _st, _nb, true, NeedDupUV());
}

//----------------------------------------------------------------------------//
void GFXAdapter_WII::drawPC(ITF_VertexBuffer* _vertexBuffer, u32 _st, u32 _nb, bool _bIndex16)
{
	ITF_ASSERT(_vertexBuffer && _vertexBuffer->mp_VertexBuffer);

	if(_bIndex16)
	{
    	for (u32 i = 0; i < _nb; i++)
    	{
    		GXPosition1x16(i + _st);
    		GXColor1x16(i + _st);
    	}
    }
    else
    {
    	for (u32 i = 0; i < _nb; i++)
    	{
    		GXPosition1x8(i + _st);
    		GXColor1x8(i + _st);
    	}

    }
}

//----------------------------------------------------------------------------//
void GFXAdapter_WII::DrawPC(ITF_VertexBuffer* _vertexBuffer, u32 _st, u32 _nb)
{
    drawPC(_vertexBuffer, _st, _nb, true);
}

//----------------------------------------------------------------------------//
void GFXAdapter_WII::drawPT(ITF_VertexBuffer* _vertexBuffer, u32 _st, u32 _nb, bool _bIndex16, bool _dupUV)
{
	ITF_ASSERT(_vertexBuffer && _vertexBuffer->mp_VertexBuffer);

    if(_dupUV)
    {
	    if(_bIndex16)
	    {
    	    for (u32 i = 0; i < _nb; i++)
    	    {
    		    GXPosition1x16(i + _st);
    		    GXTexCoord1x16(i + _st);
                GXTexCoord1x16(i + _st);
    	    }
        }
        else
        {
    	    for (u32 i = 0; i < _nb; i++)
    	    {
    		    GXPosition1x8(i + _st);
    		    GXTexCoord1x8(i + _st);
                GXTexCoord1x8(i + _st);
    	    }
        }
    }
    else
    {
        if(_bIndex16)
        {
            for (u32 i = 0; i < _nb; i++)
            {
                GXPosition1x16(i + _st);
                GXTexCoord1x16(i + _st);
            }
        }
        else
        {
            for (u32 i = 0; i < _nb; i++)
            {
                GXPosition1x8(i + _st);
                GXTexCoord1x8(i + _st);
            }
        }
    }
}

//----------------------------------------------------------------------------//
void GFXAdapter_WII::DrawPT(ITF_VertexBuffer* _vertexBuffer, u32 _st, u32 _nb)
{
    drawPT(_vertexBuffer, _st, _nb, true, NeedDupUV());
}

//----------------------------------------------------------------------------//
void GFXAdapter_WII::drawPNC3T(ITF_VertexBuffer* _vertexBuffer, u32 _st, u32 _nb, bool _bIndex16, bool _dupUV)
{
	ITF_ASSERT(_vertexBuffer && _vertexBuffer->mp_VertexBuffer);

#if defined( USE_RINGBUFFER_4_DYNMESH ) || defined(DBG_STATIC_PNC3T)
    if(_dupUV)
    {
        if(_bIndex16)
        {
            for (u32 i = 0; i < _nb; i++)
            {
        	    GXPosition1x16(i + _st);
        	    GXColor1x16(i + _st);
        	    GXTexCoord1x16(i + _st);
                GXTexCoord1x16(i + _st);
            }
        }
        else
        {
            for (u32 i = 0; i < _nb; i++)
            {
        	    GXPosition1x8(i + _st);
        	    GXColor1x8(i + _st);
        	    GXTexCoord1x8(i + _st);
                GXTexCoord1x8(i + _st);
            }
        }
	}	
    else
    {
        if(_bIndex16)
        {
            for (u32 i = 0; i < _nb; i++)
            {
                GXPosition1x16(i + _st);
                GXColor1x16(i + _st);
                GXTexCoord1x16(i + _st);
            }
        }
        else
        {
            for (u32 i = 0; i < _nb; i++)
            {
                GXPosition1x8(i + _st);
                GXColor1x8(i + _st);
                GXTexCoord1x8(i + _st);
            }
        }
    }
#else
	VertexPNC3T* pVertex = (VertexPNC3T*)_vertexBuffer->mp_VertexBuffer;
    if(_dupUV)
    {
        if(_bIndex16)
        {
            for (u32 i = 0; i < _nb; i++)
            {
                Send_frize_PNC3T_Pos(pVertex[i + _st]);

        	    GXColor1x16(i + _st);
        	    GXTexCoord1x16(i + _st);
                GXTexCoord1x16(i + _st);
            }
        }
        else
        {
            for (u32 i = 0; i < _nb; i++)
            {
                Send_frize_PNC3T_Pos(pVertex[i + _st]);

        	    GXColor1x8(i + _st);
        	    GXTexCoord1x8(i + _st);
                GXTexCoord1x8(i + _st);
            }
        }
	}
    else
    {
        if(_bIndex16)
        {
            for (u32 i = 0; i < _nb; i++)
            {
                Send_frize_PNC3T_Pos(pVertex[i + _st]);

                GXColor1x16(i + _st);
                GXTexCoord1x16(i + _st);
            }
        }
        else
        {
            for (u32 i = 0; i < _nb; i++)
            {
                Send_frize_PNC3T_Pos(pVertex[i + _st]);

                GXColor1x8(i + _st);
                GXTexCoord1x8(i + _st);
            }
        }
    }
#endif
}

//----------------------------------------------------------------------------//
void GFXAdapter_WII::DrawPNC3T(ITF_VertexBuffer* _vertexBuffer, u32 _st, u32 _nb)
{
    drawPNC3T(_vertexBuffer, _st, _nb, true, NeedDupUV());
}

inline u8 IndexToMtxID(DWORD _idx)
{
    return (u8)(_idx*3);
//    return (u8) 0;
}

//----------------------------------------------------------------------------//
void GFXAdapter_WII::drawPCBT(ITF_VertexBuffer* _vertexBuffer, u32 _st, u32 _nb, bool _bIndex16, bool _dupUV)
{
	ITF_ASSERT(_vertexBuffer && _vertexBuffer->mp_VertexBuffer);

    VertexPCBT* pVertex = (VertexPCBT*)_vertexBuffer->mp_VertexBuffer;

    if(_dupUV)
    {
	    if(_bIndex16)
	    {
    	    for (u32 i = 0; i < _nb; i++)
    	    {
                GXMatrixIndex1x8(IndexToMtxID(pVertex[i + _st].m_blendindices));
    		    GXPosition1x16(i + _st);
    		    GXColor1x16(i + _st);
    		    GXTexCoord1x16(i + _st);
                GXTexCoord1x16(i + _st);
    	    }
	    }
        else
        {
    	    for (u32 i = 0; i < _nb; i++)
    	    {
                GXMatrixIndex1x8(IndexToMtxID(pVertex[i + _st].m_blendindices));
    		    GXPosition1x8(i + _st);
    		    GXColor1x8(i + _st);
    		    GXTexCoord1x8(i + _st);
                GXTexCoord1x8(i + _st);
    	    }
        }
    }
    else
    {
        if(_bIndex16)
        {
            for (u32 i = 0; i < _nb; i++)
            {
                GXMatrixIndex1x8(IndexToMtxID(pVertex[i + _st].m_blendindices));
                GXPosition1x16(i + _st);
                GXColor1x16(i + _st);
                GXTexCoord1x16(i + _st);
            }
        }
        else
        {
            for (u32 i = 0; i < _nb; i++)
            {
                GXMatrixIndex1x8(IndexToMtxID(pVertex[i + _st].m_blendindices));
                GXPosition1x8(i + _st);
                GXColor1x8(i + _st);
                GXTexCoord1x8(i + _st);
            }
        }
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::DrawPCBT(ITF_VertexBuffer* _vertexBuffer, u32 _st, u32 _nb)
{
    drawPCBT(_vertexBuffer, _st, _nb, true, NeedDupUV());
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::drawVertexBuffer_internal(u32 _type, ITF_VertexBuffer* _vertexBuffer, u32 _vertexStart, u32 _vertexNumber)
{
	ITF_ASSERT(_vertexBuffer && _vertexBuffer->mp_VertexBuffer);

    DrawBegin(_type, _vertexNumber);
    {
    	switch(_vertexBuffer->m_vertexFormat)
        {
        case VertexFormat_PC:
        	DrawPC(_vertexBuffer, _vertexStart, _vertexNumber);
            break;
        case VertexFormat_PT:
        case VertexFormat_PTa:
        	DrawPT(_vertexBuffer, _vertexStart, _vertexNumber);
            break;
        case VertexFormat_PCT:
        	DrawPCT(_vertexBuffer, _vertexStart, _vertexNumber);
        	break;
        case VertexFormat_PCBT:
        	DrawPCBT(_vertexBuffer, _vertexStart, _vertexNumber);
        	break;
        case VertexFormat_PNC3T:
        	DrawPNC3T(_vertexBuffer, _vertexStart, _vertexNumber);
        	break;
        default:
            ITF_ASSERT_MSG(0, "Primitive not recognized");
            break;
        }
    }
    DrawEnd();
}

void GFXAdapter_WII::DrawVertexBuffer(u32 _type, ITF_VertexBuffer* _vertexBuffer, u32 _vertexStart, u32 _vertexNumber)
{
    finalizeTevSetting();    
    setVertexBuffer(_vertexBuffer);
    drawVertexBuffer_internal(_type, _vertexBuffer, _vertexStart, _vertexNumber);
}

///----------------------------------------------------------------------------//

void GFXAdapter_WII::drawPCT_direct(ITF_VertexBuffer* _vertexBuffer, u32 _st, u32 _nb)
{
    ITF_ASSERT(_vertexBuffer && _vertexBuffer->mp_VertexBuffer);

    VertexPCT* vb = ((VertexPCT *) _vertexBuffer->mp_VertexBuffer) + _st;

    for (u32 i = 0; i < _nb; ++i)
    {
        GXPosition2f32(vb[i].m_pos.m_x, vb[i].m_pos.m_y);
        GXColor1u32(vb[i].m_color);
        GXTexCoord2f32(vb[i].m_uv.m_x, vb[i].m_uv.m_y);
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::drawPC_direct(ITF_VertexBuffer* _vertexBuffer, u32 _st, u32 _nb)
{
    ITF_ASSERT(_vertexBuffer && _vertexBuffer->mp_VertexBuffer);

    VertexPC* vb = ((VertexPC *) _vertexBuffer->mp_VertexBuffer) + _st;

    for (u32 i = 0; i < _nb; ++i)
    {
        GXPosition2f32(vb[i].m_pos.m_x, vb[i].m_pos.m_y);
        GXColor1u32(vb[i].m_color);
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::drawPT_direct(ITF_VertexBuffer* _vertexBuffer, u32 _st, u32 _nb)
{
    ITF_ASSERT(_vertexBuffer && _vertexBuffer->mp_VertexBuffer);

    VertexPT* vb = ((VertexPT *) _vertexBuffer->mp_VertexBuffer) + _st;

    for (u32 i = 0; i < _nb; ++i)
    {
        GXPosition2f32(vb[i].m_pos.m_x, vb[i].m_pos.m_y);
        GXTexCoord2f32(vb[i].m_uv.m_x, vb[i].m_uv.m_y);
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::drawPNC3T_direct(ITF_VertexBuffer* _vertexBuffer, u32 _st, u32 _nb)
{
    ITF_ASSERT(_vertexBuffer && _vertexBuffer->mp_VertexBuffer);

    VertexPNC3T* vb = ((VertexPNC3T *) _vertexBuffer->mp_VertexBuffer) + _st;

    for (u32 i = 0; i < _nb; ++i)
    {
        GXPosition2f32(vb[i].m_pos.m_x, vb[i].m_pos.m_y);
        GXColor1u32(vb[i].m_color);
        GXTexCoord2f32(vb[i].m_uv.m_x, vb[i].m_uv.m_y);
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::drawPCBT_direct(ITF_VertexBuffer* _vertexBuffer, u32 _st, u32 _nb)
{
    ITF_ASSERT(_vertexBuffer && _vertexBuffer->mp_VertexBuffer);

    VertexPCBT* vb = ((VertexPCBT *) _vertexBuffer->mp_VertexBuffer) + _st;

    for (u32 i = 0; i < _nb; ++i)
    {
        GXMatrixIndex1x8(IndexToMtxID(vb[i].m_blendindices));
        GXPosition2f32(vb[i].m_pos.m_x, vb[i].m_pos.m_y);
        GXColor1u32(vb[i].m_color);
        GXTexCoord2f32(vb[i].m_uv.m_x, vb[i].m_uv.m_y);
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::drawVertexBuffer_direct(u32 _type, ITF_VertexBuffer* _vertexBuffer, u32 _vertexStart, u32 _vertexNumber)
{
    ITF_ASSERT(_vertexBuffer && _vertexBuffer->mp_VertexBuffer);

    DrawBegin(_type, _vertexNumber);
    {
        switch(_vertexBuffer->m_vertexFormat)
        {
        case VertexFormat_PC:
            drawPC_direct(_vertexBuffer, _vertexStart, _vertexNumber);
            break;
        case VertexFormat_PT:
        case VertexFormat_PTa:
            drawPT_direct(_vertexBuffer, _vertexStart, _vertexNumber);
            break;
        case VertexFormat_PCT:
            drawPCT_direct(_vertexBuffer, _vertexStart, _vertexNumber);
            break;
        case VertexFormat_PCBT:
            drawPCBT_direct(_vertexBuffer, _vertexStart, _vertexNumber);
            break;
        case VertexFormat_PNC3T:
            drawPNC3T_direct(_vertexBuffer, _vertexStart, _vertexNumber);
            break;
        default:
            ITF_ASSERT_MSG(0, "Primitive not recognized");
            break;
        }
    }
    DrawEnd();
}

///----------------------------------------------------------------------------//

void GFXAdapter_WII::drawIndexedPCT(ITF_IndexBuffer* _indexBuffer, ITF_VertexBuffer* _vertexBuffer, u32 _indexNumber, bool _bIndex16, bool _dupUV)
{
	ITF_ASSERT(_vertexBuffer && _vertexBuffer->mp_VertexBuffer);
	ITF_ASSERT(_indexBuffer && _indexBuffer->mp_IndexBuffer);

	u16* pindex = (u16*)_indexBuffer->mp_IndexBuffer;
	u16 indx = 0;

    if(_dupUV)
    {
        if(_bIndex16)
        {
    	    for (u32 i = 0; i < _indexNumber; i++)
    	    {
    		    indx = pindex[i];
    		    GXPosition1x16(indx);
    		    GXColor1x16(indx);
    		    GXTexCoord1x16(indx);
                GXTexCoord1x16(indx);
    	    }
        }
        else
        {
    	    for (u32 i = 0; i < _indexNumber; i++)
    	    {
    		    indx = pindex[i];
    		    GXPosition1x8(indx);
    		    GXColor1x8(indx);
    		    GXTexCoord1x8(indx);
                GXTexCoord1x8(indx);
    	    }
        }
    }
    else
    {
        if(_bIndex16)
        {
            for (u32 i = 0; i < _indexNumber; i++)
            {
                indx = pindex[i];
                GXPosition1x16(indx);
                GXColor1x16(indx);
                GXTexCoord1x16(indx);
            }
        }
        else
        {
            for (u32 i = 0; i < _indexNumber; i++)
            {
                indx = pindex[i];
                GXPosition1x8(indx);
                GXColor1x8(indx);
                GXTexCoord1x8(indx);
            }
        }
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::DrawIndexedPCT(ITF_IndexBuffer* _indexBuffer, ITF_VertexBuffer* _vertexBuffer, u32 _indexNumber)
{
    drawIndexedPCT(_indexBuffer, _vertexBuffer, _indexNumber, true, NeedDupUV());
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::drawIndexedPC(ITF_IndexBuffer* _indexBuffer, ITF_VertexBuffer* _vertexBuffer, u32 _indexNumber, bool _bIndex16)
{
	ITF_ASSERT(_vertexBuffer && _vertexBuffer->mp_VertexBuffer);
	ITF_ASSERT(_indexBuffer && _indexBuffer->mp_IndexBuffer);

	u16* pindex = (u16*)_indexBuffer->mp_IndexBuffer;
	u16 indx = 0;

    if(_bIndex16)
    {
    	for (u32 i = 0; i < _indexNumber; i++)
    	{
    		indx = pindex[i];
    		GXPosition1x16(indx);
    		GXColor1x16(indx);
    	}
    }
    else
    {
    	for (u32 i = 0; i < _indexNumber; i++)
    	{
    		indx = pindex[i];
    		GXPosition1x8(indx);
    		GXColor1x8(indx);
    	}
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::DrawIndexedPC(ITF_IndexBuffer* _indexBuffer, ITF_VertexBuffer* _vertexBuffer, u32 _indexNumber)
{
    drawIndexedPC(_indexBuffer, _vertexBuffer, _indexNumber, true);
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::drawIndexedPT(ITF_IndexBuffer* _indexBuffer, ITF_VertexBuffer* _vertexBuffer, u32 _indexNumber, bool _bIndex16, bool _dupUV)
{
	ITF_ASSERT(_vertexBuffer && _vertexBuffer->mp_VertexBuffer);
	ITF_ASSERT(_indexBuffer && _indexBuffer->mp_IndexBuffer);

	u16* pindex = (u16*)_indexBuffer->mp_IndexBuffer;
	u16 indx = 0;

    if(_dupUV)
    {
        if(_bIndex16)
        {
    	    for (u32 i = 0; i < _indexNumber; i++)
    	    {
    		    indx = pindex[i];
    		    GXPosition1x16(indx);
    		    GXTexCoord1x16(indx);
                GXTexCoord1x16(indx);
    	    }
        }
        else
        {
    	    for (u32 i = 0; i < _indexNumber; i++)
    	    {
    		    indx = pindex[i];
    		    GXPosition1x8(indx);
    		    GXTexCoord1x8(indx);
                GXTexCoord1x8(indx);
    	    }
        }
    }
    else
    {
        if(_bIndex16)
        {
            for (u32 i = 0; i < _indexNumber; i++)
            {
                indx = pindex[i];
                GXPosition1x16(indx);
                GXTexCoord1x16(indx);
            }
        }
        else
        {
            for (u32 i = 0; i < _indexNumber; i++)
            {
                indx = pindex[i];
                GXPosition1x8(indx);
                GXTexCoord1x8(indx);
            }
        }
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::DrawIndexedPT(ITF_IndexBuffer* _indexBuffer, ITF_VertexBuffer* _vertexBuffer, u32 _indexNumber)
{
    drawIndexedPT(_indexBuffer, _vertexBuffer, _indexNumber, true, NeedDupUV());
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::drawIndexedPNC3T(ITF_IndexBuffer* _indexBuffer, ITF_VertexBuffer* _vertexBuffer, u32 _indexNumber, bool _bIndex16, bool _dupUV)
{
	ITF_ASSERT(_vertexBuffer && _vertexBuffer->mp_VertexBuffer);
	ITF_ASSERT(_indexBuffer && _indexBuffer->mp_IndexBuffer);

	u16* pindex = (u16*)_indexBuffer->mp_IndexBuffer;
	u16 indx = 0;

#if defined(USE_RINGBUFFER_4_DYNMESH) || defined(DBG_STATIC_PNC3T)
    if(_dupUV)
    {
        if(_bIndex16)
        {
            for (u32 i = 0; i < _indexNumber; i++)
            {
                indx = pindex[i];

                #if defined(DBG_STATIC_PNC3T)
		        GXPosition1x16(indx);
                #else    		
		        GXPosition1x16(i);
                #endif    		
		        GXColor1x16(indx);
		        GXTexCoord1x16(indx);
                GXTexCoord1x16(indx);
            }
	    }
	    else
	    {
            for (u32 i = 0; i < _indexNumber; i++)
            {
                indx = pindex[i];

                #if defined(DBG_STATIC_PNC3T)		
		        GXPosition1x8(indx);
                #else
		        GXPosition1x8(i);
                #endif    		
		        GXColor1x8(indx);
		        GXTexCoord1x8(indx);
		        GXTexCoord1x8(indx);
            }
	    }
    }
    else
    {
        if(_bIndex16)
        {
            for (u32 i = 0; i < _indexNumber; i++)
            {
                indx = pindex[i];

                #if defined(DBG_STATIC_PNC3T)
                GXPosition1x16(indx);
                #else    		
                GXPosition1x16(i);
                #endif    		
                GXColor1x16(indx);
                GXTexCoord1x16(indx);
            }
        }
        else
        {
            for (u32 i = 0; i < _indexNumber; i++)
            {
                indx = pindex[i];

                #if defined(DBG_STATIC_PNC3T)		
                GXPosition1x8(indx);
                #else
                GXPosition1x8(i);
                #endif    		
                GXColor1x8(indx);
                GXTexCoord1x8(indx);
            }
        }
    }
#else // USE_RINGBUFFER_4_DYNMESH
	VertexPNC3T* pVertex = (VertexPNC3T*)_vertexBuffer->mp_VertexBuffer;

    if(_dupUV)
    {
        if(_bIndex16)
        {
            for (u32 i = 0; i < _indexNumber; i++)
            {
                indx = pindex[i];

                Send_frize_PNC3T_Pos(pVertex[indx]);

    		    GXColor1x16(indx);
    		    GXTexCoord1x16(indx);
                GXTexCoord1x16(indx);
            }
		}
		else
		{
            for (u32 i = 0; i < _indexNumber; i++)
            {
                indx = pindex[i];

                Send_frize_PNC3T_Pos(pVertex[indx]);

    		    GXColor1x8(indx);
    		    GXTexCoord1x8(indx);
                GXTexCoord1x8(indx);
            }
		}
	}
    else
    {
        if(_bIndex16)
        {
            for (u32 i = 0; i < _indexNumber; i++)
            {
                indx = pindex[i];

                Send_frize_PNC3T_Pos(pVertex[indx]);

                GXColor1x16(indx);
                GXTexCoord1x16(indx);
            }
        }
        else
        {
            for (u32 i = 0; i < _indexNumber; i++)
            {
                indx = pindex[i];

                Send_frize_PNC3T_Pos(pVertex[indx]);

                GXColor1x8(indx);
                GXTexCoord1x8(indx);
            }
        }
    }
#endif // USE_RINGBUFFER_4_DYNMESH
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::DrawIndexedPNC3T(ITF_IndexBuffer* _indexBuffer, ITF_VertexBuffer* _vertexBuffer, u32 _indexNumber)
{
    drawIndexedPNC3T(_indexBuffer, _vertexBuffer, _indexNumber, true, NeedDupUV());
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::drawIndexedPCBT(ITF_IndexBuffer* _indexBuffer, ITF_VertexBuffer* _vertexBuffer, u32 _indexNumber, bool _bIndex16, bool _dupUV)
{
	ITF_ASSERT(_vertexBuffer && _vertexBuffer->mp_VertexBuffer);
	ITF_ASSERT(_indexBuffer && _indexBuffer->mp_IndexBuffer);

	u16* pindex = (u16*)_indexBuffer->mp_IndexBuffer;
	u16 indx = 0;
    VertexPCBT* pVertex = (VertexPCBT*)_vertexBuffer->mp_VertexBuffer;

    if(_dupUV)
    {
        if(_bIndex16)
        {
    	    for (u32 i = 0; i < _indexNumber; i++)
    	    {
    		    indx = pindex[i];
                GXMatrixIndex1x8(IndexToMtxID(pVertex[indx].m_blendindices));
    		    GXPosition1x16(indx);
    		    GXColor1x16(indx);
    		    GXTexCoord1x16(indx);
                GXTexCoord1x16(indx);
    	    }
        }
        else
        {
    	    for (u32 i = 0; i < _indexNumber; i++)
    	    {
    		    indx = pindex[i];
                GXMatrixIndex1x8(IndexToMtxID(pVertex[indx].m_blendindices));
    		    GXPosition1x8(indx);
    		    GXColor1x8(indx);
    		    GXTexCoord1x8(indx);
                GXTexCoord1x8(indx);
    	    }
        }
    }
    else
    {
        if(_bIndex16)
        {
            for (u32 i = 0; i < _indexNumber; i++)
            {
                indx = pindex[i];
                GXMatrixIndex1x8(IndexToMtxID(pVertex[indx].m_blendindices));
                GXPosition1x16(indx);
                GXColor1x16(indx);
                GXTexCoord1x16(indx);
            }
        }
        else
        {
            for (u32 i = 0; i < _indexNumber; i++)
            {
                indx = pindex[i];
                GXMatrixIndex1x8(IndexToMtxID(pVertex[indx].m_blendindices));
                GXPosition1x8(indx);
                GXColor1x8(indx);
                GXTexCoord1x8(indx);
            }
        }
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::DrawIndexedPCBT(ITF_IndexBuffer* _indexBuffer, ITF_VertexBuffer* _vertexBuffer, u32 _indexNumber)
{
    drawIndexedPCBT(_indexBuffer, _vertexBuffer, _indexNumber, true, NeedDupUV());
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::drawIndexedVertexBuffer_internal(u32 _type, ITF_VertexBuffer* _vertexBuffer, ITF_IndexBuffer* _indexBuffer, u32 _indexNumber)
{
	ITF_ASSERT(_vertexBuffer && _vertexBuffer->mp_VertexBuffer);

	if(!_indexBuffer || !_indexBuffer->mp_IndexBuffer) return;

    DrawBegin(_type, _indexNumber);
    {
    	switch(_vertexBuffer->m_vertexFormat)
        {
        case VertexFormat_PC:
        	DrawIndexedPC(_indexBuffer, _vertexBuffer, _indexNumber);
            break;
        case VertexFormat_PT:
        case VertexFormat_PTa:
        	DrawIndexedPT(_indexBuffer, _vertexBuffer, _indexNumber);
            break;
        case VertexFormat_PCT:
        	DrawIndexedPCT(_indexBuffer, _vertexBuffer, _indexNumber);
            break;
        case VertexFormat_PCBT:
        	DrawIndexedPCBT(_indexBuffer, _vertexBuffer, _indexNumber);
        	break;
        case VertexFormat_PNC3T:
        	DrawIndexedPNC3T(_indexBuffer, _vertexBuffer, _indexNumber);
            break;
        default:
            ITF_ASSERT_MSG(0, "Primitive not recognized");
            break;
        }
    }
    DrawEnd();
}

void GFXAdapter_WII::DrawIndexedVertexBuffer(u32 _type, ITF_VertexBuffer* _vertexBuffer, ITF_IndexBuffer* _indexBuffer, u32 _indexNumber)
{
    finalizeTevSetting();    
    setVertexBuffer(_vertexBuffer);
    drawIndexedVertexBuffer_internal(_type, _vertexBuffer, _indexBuffer, _indexNumber);
}

///----------------------------------------------------------------------------//

void GFXAdapter_WII::DrawBegin(u32 _type, u32 _nverts)
{
    GXPrimitive prim = GX_TRIANGLES;
    switch(_type)
    {
        default:
        case GFX_TRIANGLES:
            prim = GX_TRIANGLES;
            break;
        case GFX_POINTS:
            prim = GX_POINTS;
            break;
        case GFX_LINES:
            prim = GX_LINES;
            break;
        case GFX_LINE_STRIP:
            prim = GX_LINESTRIP;
            break;
        case GFX_TRIANGLE_STRIP:
            prim = GX_TRIANGLESTRIP;
            break;
        case GFX_TRIANGLE_FAN:
            prim = GX_TRIANGLEFAN;
            break;
        case GFX_QUADS:
            prim = GX_QUADS;
            break;
    }

    PPCSync();
    GXBegin(prim, GX_VTXFMT0, _nverts);
}

void GFXAdapter_WII::DrawEnd()
{
    GXEnd();
#ifdef DBG_CHECK_DRAW_VALIDITY
    syncGPU();
#endif // DBG_CHECK_DRAW_VALIDITY
}

void GFXAdapter_WII::Send_frize_PNC3T_Pos(const VertexPNC3T &_v) const
{
    Vec3d v = _v.m_pos;
    Compute_frize_PNC3T_Pos(&v, &_v, m_VecOverlay);
	GXPosition3f32(v.m_x, v.m_y, v.m_z);
}

///----------------------------------------------------------------------------//
/// Shader Fluid.
///----------------------------------------------------------------------------//

void GFXAdapter_WII::drawFluid(GMatrix44* _matrix, Texture* _texture, const FixedArray<u32, GFXADAPTER_FLUID_COLORTAB_ENTRY_COUNT> &_colorTab,
                              const Vec2d _heightTab[], const u32 _heightTabSize, const FixedArray<Vec2d, 4> &_uvTab, u32 mode, f32 _delta1, f32 _delta2, f32 _stepCount)
{
#ifndef ITF_FINAL
#pragma global_optimizer off
    if(g_dbgDrawSkipper.bSkipDrawFluid)
        return;
#pragma global_optimizer reset        
#endif // ITF_FINAL

#ifdef VBMANAGER_USE_DYNAMICRING_VB
    u32 hdiv = MESH_DEFAULT_HDIV;
    u32 vdiv = MESH_DEFAULT_VDIV;

    ITF_IndexBuffer *  indexBufferIn  = getIndexBuffer(hdiv, vdiv);
    ITF_VertexBuffer * vertexBufferIn = getVertexBuffer(hdiv, vdiv);
	ITF_ASSERT(vertexBufferIn && indexBufferIn);
	ITF_ASSERT(vertexBufferIn->mp_VertexBuffer && indexBufferIn->mp_IndexBuffer);
    ITF_ASSERT(vertexBufferIn->m_vertexFormat == VertexFormat_PT);

    //----

    if(_texture)
    {
        setTextureAdressingMode(0, GFXTADDRESS_WRAP, GFXTADDRESS_WRAP);
        SetTextureBind(0, (unsigned int)_texture);
    }

    renderContext_Set();

    setTev(TEV_TC);
    setObjectMatrix(*_matrix);
    resetUVAnim();
    finalizeTevSetting();       	    
    
    int heightTabSize   = _heightTabSize;
    int vLen            = heightTabSize - 1;
    
    VertexPCT *vertexOut;
    
    ITF_MeshElement meshElement;
    meshElement.m_indexBuffer = indexBufferIn;
    meshElement.m_startIndex = 0;
    meshElement.m_count =  hdiv * vLen * 6;
    
    ITF_VertexBuffer dynVTXbuffer;
    dynVTXbuffer.m_vertexFormat = VertexFormat_PCT;
    dynVTXbuffer.m_nVertex = (hdiv+1)*(vLen+1);
    dynVTXbuffer.m_structVertexSize = sizeof(VertexPCT);
    getVertexBufferManager().LockDynamicRingVB(dynVTXbuffer.m_nVertex * dynVTXbuffer.m_structVertexSize, &dynVTXbuffer, (void**)&vertexOut);

    // Compute vertices
    VertexPCT * __restrict pout = vertexOut;
    const VertexPT  * pin = (VertexPT*)((u8*)vertexBufferIn->mp_VertexBuffer + vertexBufferIn->m_offset);
    
    const f32 hStepRatio    = f32_Inv((f32)hdiv);
    const f32 vStepRatio    = f32_Inv((f32)vLen);

    const f32 deltaStepPos = (_delta2 - _delta1) * vStepRatio * _stepCount;
    f32 deltaPos = _delta1 * _stepCount;

    const Vec2d deltaStepUV0 = (_uvTab[3]-_uvTab[1]) * vStepRatio;
    const Vec2d deltaStepUV1 = (_uvTab[2]-_uvTab[0]) * vStepRatio;
    Vec2d deltaUV0 = _uvTab[1];    
    Vec2d deltaUV1 = _uvTab[0];
        
    const Vec2d *pHeight = &_heightTab[0];
    const u32 *pColTable = &_colorTab[0];
    f32 xRatio = 0.f;
    Vec2d deltaUVIn;
    Vec2d posAdd;
    u32 tmpColor[2];

    if(mode)
    {
        for(int j = 0; j <= vLen; ++j, deltaPos += deltaStepPos, xRatio += vStepRatio, ++pHeight)
        {
            posAdd.set(deltaPos, - pHeight->m_y);

            Vec2d::Sub  (&deltaUVIn, &deltaUV1, &deltaUV0);
            Vec2d::Scale(&deltaUVIn, &deltaUVIn, hStepRatio);
            Vec2d uv = deltaUV0;

            int idx = 4 * (int)pHeight->m_x;
            bool bColInterpolate;
            if((pColTable[idx+2] == pColTable[idx]) && (pColTable[idx+3] == pColTable[idx+1]))
            {
                bColInterpolate = false;
            }
            else
            {
                bColInterpolate = true;
                ITFCOLOR_BLEND(tmpColor  , pColTable+idx  , pColTable+idx+2, xRatio);
                ITFCOLOR_BLEND(tmpColor+1, pColTable+idx+1, pColTable+idx+3, xRatio);
            }
            
            f32 yRatio = 1.f;
            if(bColInterpolate)
            {
                for(int i = 0; i <= hdiv; ++i, ++pout, ++pin, yRatio -= hStepRatio)
                {
                    //pout->m_pos = pin->m_pos + posAdd*yRatio;
                    pout->m_pos.m_x = pin->m_pos.m_x;
                    Vec2d::ScaleAdd((Vec2d*)&pout->m_pos.m_y, &posAdd, yRatio, (Vec2d*)&pin->m_pos.m_y);

                    pout->m_uv  = uv;
                    ITFCOLOR_BLEND(&pout->m_color, tmpColor+1, tmpColor, yRatio);
                                                        
                    Vec2d::Add(&uv, &uv, &deltaUVIn);
                }
            }
            else
            {
                for(int i = 0; i <= hdiv; ++i, ++pout, ++pin, yRatio -= hStepRatio)
                {
                    //pout->m_pos = pin->m_pos + posAdd*yRatio;
                    pout->m_pos.m_x = pin->m_pos.m_x;
                    Vec2d::ScaleAdd((Vec2d*)&pout->m_pos.m_y, &posAdd, yRatio, (Vec2d*)&pin->m_pos.m_y);

                    pout->m_uv  = uv;
                    pout->m_color = pColTable[idx];
                                                        
                    Vec2d::Add(&uv, &uv, &deltaUVIn);
                }
            }
            
            Vec2d::Add(&deltaUV0, &deltaUV0, &deltaStepUV0);
            Vec2d::Add(&deltaUV1, &deltaUV1, &deltaStepUV1);
        }
    }
    else
    {
        for(int j = 0; j <= vLen; ++j, deltaPos += deltaStepPos, xRatio += vStepRatio, ++pHeight)
        {
            posAdd.set(- pHeight->m_y, deltaPos);

            Vec2d::Sub  (&deltaUVIn, &deltaUV1, &deltaUV0);
            Vec2d::Scale(&deltaUVIn, &deltaUVIn, hStepRatio);
            Vec2d uv = deltaUV0;

            int idx = 4 * (int)pHeight->m_x;
            bool bColInterpolate;
            if((pColTable[idx+2] == pColTable[idx]) && (pColTable[idx+3] == pColTable[idx+1]))
            {
                bColInterpolate = false;
            }
            else
            {
                bColInterpolate = true;
                ITFCOLOR_BLEND(tmpColor  , pColTable+idx  , pColTable+idx+2, xRatio);
                ITFCOLOR_BLEND(tmpColor+1, pColTable+idx+1, pColTable+idx+2, xRatio);
            }
            
            f32 yRatio = 1.f;
            if(bColInterpolate)
            {
                for(int i = 0; i <= hdiv; ++i, ++pout, ++pin, yRatio -= hStepRatio)
                {
                    //pout->m_pos = pin->m_pos + posAdd*yRatio;
                    Vec2d::ScaleAdd((Vec2d*)&pout->m_pos.m_x, &posAdd, yRatio, (Vec2d*)&pin->m_pos.m_x);
                    pout->m_pos.m_z = pin->m_pos.m_z;

                    pout->m_uv  = uv;
                    ITFCOLOR_BLEND(&pout->m_color, tmpColor+1, tmpColor, yRatio);
                                                        
                    Vec2d::Add(&uv, &uv, &deltaUVIn);
                }
            }
            else
            {
                for(int i = 0; i <= hdiv; ++i, ++pout, ++pin, yRatio -= hStepRatio)
                {
                    //pout->m_pos = pin->m_pos + posAdd*yRatio;
                    Vec2d::ScaleAdd((Vec2d*)&pout->m_pos.m_x, &posAdd, yRatio, (Vec2d*)&pin->m_pos.m_x);
                    pout->m_pos.m_z = pin->m_pos.m_z;

                    pout->m_uv  = uv;
                    pout->m_color = pColTable[idx];
                                                        
                    Vec2d::Add(&uv, &uv, &deltaUVIn);
                }
            }
            
            Vec2d::Add(&deltaUV0, &deltaUV0, &deltaStepUV0);
            Vec2d::Add(&deltaUV1, &deltaUV1, &deltaStepUV1);
        }
    }
    
    // Render
    getVertexBufferManager().UnlockDynamicRingVB(&dynVTXbuffer);    

    #ifdef SUPPORT_STATS_MANAGER
    StatsManager::getptr()->drawVtxCntInc(meshElement.m_count);
    #endif

#ifdef USE_DL
    GFXDisplayList *dl = FindOrCreateMeshElementDisplayList(&dynVTXbuffer, meshElement);
    if(dl)
    {
        setVertexDesc(&dynVTXbuffer, dl->m_bUseInd16, NeedDupUV());
        PPCSync();

        GXCallDisplayList(dl->m_ptr, dl->m_size);
        dl->m_usage = OSGetTick();
    }
    else
#endif // USE_DL
    {
        setVertexBuffer(&dynVTXbuffer);
        drawIndexedVertexBuffer_internal(GFX_TRIANGLES, &dynVTXbuffer, meshElement.m_indexBuffer, meshElement.m_count);
    }

    dynVTXbuffer.mp_VertexBuffer = NULL; // avoid vb destruction by object destructor.
#else // VBMANAGER_USE_DYNAMICRING_VB
    ITF_ASSERT_MSG(0, "GFXAdapter_WII::drawFluid without dynamic vb ring buffer not implemented");
#endif
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::drawScreenQuad(f32 _px, f32 _py, f32 _width, f32 _height, f32 _z, u32 _color, bbool _noTex)
{
    drawScreenQuadInternal(_px, _py, _width, _height, _z, _color, _noTex);
}

void GFXAdapter_WII::drawScreenQuadInternal(f32 _px, f32 _py, f32 _width, f32 _height, f32 _z, u32 _color, bbool _noTex, Tev_Type _type)
{
	Mtx		modelview;
	MTXIdentity(modelview);
	GXLoadPosMtxImm(modelview, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);

	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);

	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);

    if(_noTex)
	{
        setTev(TEV_C);
		finalizeTevSetting();

		DrawBegin(GFX_QUADS, 4);
        {
			GXPosition3f32(_px, _py, _z);
			GXColor1u32(_color);

			GXPosition3f32(_px, _py + _height, _z);
			GXColor1u32(_color);

			GXPosition3f32(_px + _width, _py + _height, _z);
			GXColor1u32(_color);

			GXPosition3f32(_px + _width, _py, _z);
			GXColor1u32(_color);
        }
		DrawEnd();

        return;
    }

    switch(_type)
    {
    case TEV_AFX_2X:
    case TEV_AFX_GLOW_COPY:
    case TEV_AFX_GLOW_2X:
        {
            // 2 coords in case textures used have different sizes

            setTev(_type);
		    finalizeTevSetting();

		    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
		    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
            GXSetVtxDesc(GX_VA_TEX1, GX_DIRECT);
            GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_TEX_ST, GX_F32, 0);

		    DrawBegin(GFX_QUADS, 4);
            {
			    GXPosition3f32(_px, _py, _z);
			    GXColor1u32(_color);
			    GXTexCoord2f32(0.f, 1.0f);
                GXTexCoord2f32(0.f, 1.0f);

			    GXPosition3f32(_px, _py + _height, _z);
			    GXColor1u32(_color);
			    GXTexCoord2f32(0.f, 0.f);
                GXTexCoord2f32(0.f, 0.f);

			    GXPosition3f32(_px + _width, _py + _height, _z);
			    GXColor1u32(_color);
			    GXTexCoord2f32(1.0f, 0.0f);
                GXTexCoord2f32(1.0f, 0.0f);

			    GXPosition3f32(_px + _width, _py, _z);
			    GXColor1u32(_color);
			    GXTexCoord2f32(1.0f, 1.0f);
                GXTexCoord2f32(1.0f, 1.0f);
            }
		    DrawEnd();
        }
        break;
    
    default:
        {
            setTev(_type);
            finalizeTevSetting();

            GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
            GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

            DrawBegin(GFX_QUADS, 4);
            {
                GXPosition3f32(_px, _py, _z);
                GXColor1u32(_color);
                GXTexCoord2f32(0.f, 1.0f);

                GXPosition3f32(_px, _py + _height, _z);
                GXColor1u32(_color);
                GXTexCoord2f32(0.f, 0.f);

                GXPosition3f32(_px + _width, _py + _height, _z);
                GXColor1u32(_color);
                GXTexCoord2f32(1.0f, 0.0f);

                GXPosition3f32(_px + _width, _py, _z);
                GXColor1u32(_color);
                GXTexCoord2f32(1.0f, 1.0f);
            }
            DrawEnd();
        }
        break;
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::drawQuad2D(f32 _px, f32 _py, f32 _width, f32 _height, f32 _z, u32 _color, const Texture* _tex, const Vec2d* _uvStart,  const Vec2d* _uvEnd)
{
	Mtx		modelview;
	MTXIdentity(modelview);
	GXLoadPosMtxImm(modelview, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);

    /*
        ------->  +X
        |
        |
        |
        |
        +Y
    */

    const f32 py = (f32)getScreenHeight() - _py - _height;
    Vec2d uvStart, uvEnd;
    if (_uvStart)
        uvStart = *_uvStart;
    else
        uvStart = Vec2d(0.f, 0.f);
    if (_uvEnd)
        uvEnd = *_uvEnd;
    else
        uvEnd = Vec2d(1.f, 1.f);

	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);

	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);

    setAlphaBlend(GFX_BLEND_ALPHA);

    if(_tex)
    {
		SetTextureBind(0, (unsigned int)_tex);

        setTev(TEV_TC);
		finalizeTevSetting();

		GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

		DrawBegin(GFX_QUADS, 4);
        {
			GXPosition3f32(_px, py, _z);
			GXColor1u32(_color);
			GXTexCoord2f32(uvStart.m_x, uvEnd.m_y);

			GXPosition3f32(_px, py + _height, _z);
			GXColor1u32(_color);
			GXTexCoord2f32(uvStart.m_x, uvStart.m_y);

			GXPosition3f32(_px + _width, py + _height, _z);
			GXColor1u32(_color);
			GXTexCoord2f32(uvEnd.m_x, uvStart.m_y);

			GXPosition3f32(_px + _width, py, _z);
			GXColor1u32(_color);
			GXTexCoord2f32(uvEnd.m_x, uvEnd.m_y);
        }
		DrawEnd();
	}
	else
	{
        setTev(TEV_C);
		finalizeTevSetting();

		DrawBegin(GFX_QUADS, 4);
        {
			GXPosition3f32(_px, py, _z);
			GXColor1u32(_color);

			GXPosition3f32(_px, py + _height, _z);
			GXColor1u32(_color);

			GXPosition3f32(_px + _width, py + _height, _z);
			GXColor1u32(_color);

			GXPosition3f32(_px + _width, py, _z);
			GXColor1u32(_color);
        }
		DrawEnd();
	}
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::draw3DQuad(GFX_3DQUAD* _t_Quad)
{
    f32 sx = _t_Quad->m_size.m_x * 0.5f;
    f32 sy = _t_Quad->m_size.m_y * 0.5f;

    Matrix44 M;
    M.setIdentity();
    M.setTranslation(Vec3d(_t_Quad->m_pos.m_x, _t_Quad->m_pos.m_y, _t_Quad->m_pos.m_z));

    if(!_t_Quad->m_rotation.IsEqual(Vec3d::Zero, MTH_EPSILON))
    {
        //reorient dir
        Matrix44 rotZ;
        Matrix44 rotX;
        Matrix44 rotY;

        M44_setMatrixRotationX(&rotX, _t_Quad->m_rotation.m_x);
        M44_setMatrixRotationY(&rotY, _t_Quad->m_rotation.m_y);
        M44_setMatrixRotationZ(&rotZ, _t_Quad->m_rotation.m_z);

        M = rotX * rotY * rotZ * M;
    }

    setObjectMatrix(GMatrix44(M));
    setAlphaBlend(_t_Quad->m_blendMode);

    setTev(TEV_TC);
    finalizeTevSetting();

    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);

    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

    DrawBegin(GFX_QUADS, 4);
    {
        GXPosition3f32(-sx, -sy, 0.f);
        GXColor1u32(_t_Quad->m_color);
        GXTexCoord2f32(0.0f, 1.0f);

        GXPosition3f32(-sx, sy, 0.f);
        GXColor1u32(_t_Quad->m_color);
        GXTexCoord2f32(0.0f, 0.0f);

        GXPosition3f32(sx, sy, 0.f);
        GXColor1u32(_t_Quad->m_color);
        GXTexCoord2f32(1.0f, 0.0f);

        GXPosition3f32(sx, -sy, 0.f);
        GXColor1u32(_t_Quad->m_color);
        GXTexCoord2f32(1.0f, 1.0f);
    }
    DrawEnd();
}
} // namespace ITF
