#ifdef ADAPTER_DRAWPATCH_DEGREE_TWO
// This part (ifdef ADAPTER_DRAWPATCH_DEGREE_TWO) is use as 2nd include !! Look down to understand
#ifdef USE_WGP
#if defined( USE_FLOAT2 )
            pvtxBuffer->wgp_write(*(Vec2d*)&TmpPoint);
#else
			pvtxBuffer->wgp_write(TmpPoint);
#endif
			pvtxBuffer->wgp_write(_z);
# ifdef PATCH32_USE_PCT
#  ifdef PATCH32_USE_COLOR
			pvtxBuffer->wgp_write(TmpColor);
#  else // PATCH32_USE_COLOR
			pvtxBuffer->wgp_write(_generalColor);
#  endif // PATCH32_USE_COLOR
# endif // PATCH32_USE_PCT
#if defined( USE_FLOAT2 )
			pvtxBuffer->wgp_write(*(Vec2d*)&TmpUV);
#else
			pvtxBuffer->wgp_write(TmpUV);
#endif
#else // USE_WGP
# ifdef PATCH32_USE_PCT
#  ifdef PATCH32_USE_COLOR
			write->setData(Vec3d(TmpPoint.m_x, TmpPoint.m_y, _z), TmpUV, TmpColor);
#  else // PATCH32_USE_COLOR
			write->setData(Vec3d(TmpPoint.m_x, TmpPoint.m_y, _z), TmpUV, _generalColor);
#  endif // PATCH32_USE_COLOR
# else // PATCH32_USE_PCT
			write->setData(Vec3d(TmpPoint.m_x, TmpPoint.m_y, _z), TmpUV);
# endif // PATCH32_USE_PCT
#endif // USE_WGP
		_pData->vertexCount++;
		write++;
// This part (ifdef ADAPTER_DRAWPATCH_DEGREE_TWO) is use as 2nd include !! Look down to understand
#else //ADAPTER_DRAWPATCH_DEGREE_TWO

#define ADAPTER_DRAWPATCH_DEGREE_TWO
// General code for DrawPatch
// Params
// Patch32Data *_pData, Texture *texture, Vec2d *PointsNS_, Color *_color, f32 _z, u32 _generalColor

// Modes: PATCH32_USE_PCT - PATCH32_USE_COLOR

#ifdef PATCH32_USE_PCT
VertexPCT	* __restrict write = _pData->vertexListPCT + _pData->vertexCount;
#else
VertexPT	* __restrict write = _pData->vertexListPT + _pData->vertexCount;
#endif

#ifdef SUPPORT_STATS_MANAGER
const void * writebegin = (const void*)write;
#endif


bbool ret = bfalse;
#if defined( USE_FLOAT2 )
	ps::Float2 TPoints_U[9];
	ps::Float2 TPoints_D[9];
	ps::Float2 TPointsUV_U[9];
	ps::Float2 TPointsUV_D[9];
	register ps::Float2 *TPoints;
	register ps::Float2 *TPointsUV;
	register ps::Float2 TmpPoint;
	register ps::Float2 TmpUV;
	const ps::Float2 *PointsNS_ = (ps::Float2 *)PointsNS;
	register ps::Float2 textureYratio = ps::MergeXX(ps::Load(1.0f),ps::Load(texture->getRatioXY()));
#define _ScaleAdd(xsa,xsb,xsf,xsd) xsa = ps::Add(ps::Mul(xsb,xsf),xsd)
#else
	Vec2d TPoints_U[9];
	Vec2d TPoints_D[9];
	Vec2d TPointsUV_U[9];
	Vec2d TPointsUV_D[9];
	Vec2d TmpPoint;
	Vec2d TmpUV;
	register Vec2d *TPoints = TPoints_U;
	register Vec2d *TPointsUV = TPointsUV_U;
	const Vec2d *PointsNS_ = PointsNS;
	const f32 textureYratio = texture->getRatioXY();
#define _ScaleAdd(xsa,xsb,xsf,xsd) Vec2d::ScaleAdd(&xsa,&xsb,xsf,&xsd)
#endif
#ifdef USE_WGP
	register ITF_VertexBuffer *pvtxBuffer = _pData->mesh->m_vtxBuffer;
#endif
#ifdef PATCH32_USE_COLOR
u32   color_U[9];
u32   color_D[9];
u32   TmpColor;
#define BLENDCOLOR(a,b,f) ((((a & 0xff00ff) * f + (b & 0xff00ff) * (255-f)) & 0xff00ff00)>>8) + (((((a>>8) & 0xff00ff) * f + ((b>>8) & 0xff00ff) * (255-f)) & 0xff00ff00))

#endif


#ifndef PATCH32_IGNORE_MESHELEMENTS
ITF_ASSERT(_pData->mesh->m_ElementList.size() > 0);

ITF_MeshElement &elem = _pData->mesh->m_ElementList[0];
elem.m_material.m_textureDiffuse = texture->getID();

ITF_MeshElement &curElem                = _pData->mesh->m_ElementList[0];
curElem.m_count                         += _vdiv*(6*_hdiv);
#endif
/*
	sample : (4 x 4)

	LINE_U->	0--+1-+-2+--3  <= PointsNS_ == 4 control points (2 ext(0-3) + 2 tangents (1-2))
				|--+--+--+--|
				|--+--+--+--|
				|--+--+--+--|
	LINE_D->	4--+5-+-6+--7  
	
	HDIV x  VDIV 
	PointsNS_ + 8 = UV'S
*/


/**************************************************************************************************************************************/
/* 1 Evaluate upper & lower polynom */
/**************************************************************************************************************************************/
//          P² = (C-2B +A)µ² + (B - A)2µ + A
//          P³ = (-a +3b -3c +d)µ³ + (a -2b +c)3µ² + (b -a)3µ + a
TPoints_U[0] = PointsNS_[0];
TPoints_U[_hdiv] = PointsNS_[3];
TPoints_D[0] = PointsNS_[4];
TPoints_D[_hdiv] = PointsNS_[7];
#if defined( USE_FLOAT2 )
TPointsUV_U[0] = ps::Mul(PointsNS_[0+8],textureYratio);
TPointsUV_U[_hdiv] = ps::Mul(PointsNS_[3+8],textureYratio);
TPointsUV_D[0] = ps::Mul(PointsNS_[4+8],textureYratio);
TPointsUV_D[_hdiv] = ps::Mul(PointsNS_[7+8],textureYratio);
#else
TPointsUV_U[0] = PointsNS_[0+8];
TPointsUV_U[_hdiv] = PointsNS_[3+8];
TPointsUV_D[0] = PointsNS_[4+8];
TPointsUV_D[_hdiv] = PointsNS_[7+8];
TPointsUV_U[0].m_y*=textureYratio;
TPointsUV_D[0].m_y*=textureYratio;
TPointsUV_U[_hdiv].m_y*=textureYratio;
TPointsUV_D[_hdiv].m_y*=textureYratio;
#endif
#ifdef PATCH32_USE_COLOR
color_U[0] = _color[0].getAsU32();
color_U[_hdiv] = _color[1].getAsU32();
color_D[0] = _color[2].getAsU32();
color_D[_hdiv] = _color[3].getAsU32();
u32	*pcolor = color_U;
#endif

if (_hdiv == 2) // Often
{
//	p = (a + 3 b + 3 c + d) / 8.0f
#if defined( USE_FLOAT2 )
	TPoints_U[1] = ps::Mul((PointsNS_[0] + ps::Mul(PointsNS_[1] + PointsNS_[2],3.0f) + PointsNS_[3]) , (1.0f / 8.0f));
	TPoints_D[1] = ps::Mul((PointsNS_[4] + ps::Mul(PointsNS_[5] + PointsNS_[6],3.0f) + PointsNS_[7]) , (1.0f / 8.0f));
	TPointsUV_U[1] = ps::Mul(ps::Mul((PointsNS_[8] + ps::Mul((PointsNS_[9] + PointsNS_[10]),3.0f) + PointsNS_[11]) , (1.0f / 8.0f)),textureYratio);
	TPointsUV_D[1] = ps::Mul(ps::Mul((PointsNS_[12] + ps::Mul((PointsNS_[13] + PointsNS_[14]),3.0f) + PointsNS_[15]) , (1.0f / 8.0f)),textureYratio);
#else
	TPoints_U[1] = (PointsNS_[0] + 3.0f * (PointsNS_[1] + PointsNS_[2]) + PointsNS_[3]) * (1.0f / 8.0f);
	TPoints_D[1] = (PointsNS_[4] + 3.0f * (PointsNS_[5] + PointsNS_[6]) + PointsNS_[7]) * (1.0f / 8.0f);
	TPointsUV_U[1] = (PointsNS_[8] + 3.0f * (PointsNS_[9] + PointsNS_[10]) + PointsNS_[11]) * (1.0f / 8.0f);
	TPointsUV_D[1] = (PointsNS_[12] + 3.0f * (PointsNS_[13] + PointsNS_[14]) + PointsNS_[15]) * (1.0f / 8.0f);
	TPointsUV_U[1].m_y*=textureYratio;
	TPointsUV_D[1].m_y*=textureYratio;
#endif
#ifdef PATCH32_USE_COLOR
	color_U[1] = ((color_U[0]>>1) & 0x7f7f7f7f) + ((color_U[2]>>1) & 0x7f7f7f7f);
	color_D[1] = ((color_D[0]>>1) & 0x7f7f7f7f) + ((color_D[2]>>1) & 0x7f7f7f7f);
#endif

} else
if (_hdiv > 1)
{
	const f32 UVStep = f32_Inv((f32)_hdiv);
#if defined( USE_FLOAT2 )
	register ps::Float2 PolyNom0,PolyNom1,PolyNom2,PolyNom3,PolyNom4,PolyNom5,PolyNom6,PolyNom7;
#else
	register Vec2d PolyNom0,PolyNom1,PolyNom2,PolyNom3,PolyNom4,PolyNom5,PolyNom6,PolyNom7;
#endif
#ifdef PATCH32_USE_COLOR
	pcolor = color_U;
#endif

/**************************************************************************************************************************************/
	/* Build Up */
//          P² = (C-2B +A)µ² + (B - A)2µ + A
//          P³ = (-a +3b -3c +d)µ³ + (a -2b +c)3µ² + (b -a)3µ + a
	TPoints = TPoints_U+1;;
	TPointsUV = TPointsUV_U+1;
	for (int k = 0 ; k < 2 ; k ++)
	{
		register f32 UVBase = 0;
		PolyNom0 = PointsNS_[0];
		PolyNom3 = (PointsNS_[1] - PointsNS_[2])*3.0f + PointsNS_[3] - PointsNS_[0];
		PolyNom2 = (PointsNS_[0] - PointsNS_[1]*2.0f + PointsNS_[2])*3.0f;
		PolyNom1 = (PointsNS_[1] - PointsNS_[0])*3.0f;
		PolyNom4 = PointsNS_[0+8];
		PolyNom7 = (PointsNS_[1+8] - PointsNS_[2+8])*3.0f + PointsNS_[3+8] - PointsNS_[0+8];
		PolyNom6 = (PointsNS_[0+8] - PointsNS_[1+8]*2.0f + PointsNS_[2+8])*3.0f;
		PolyNom5 = (PointsNS_[1+8] - PointsNS_[0+8])*3.0f;

		for (u32 i = 1 ; i < _hdiv ; i ++)
		{
			UVBase += UVStep;
			register f32 UVStepSquare = UVBase * UVBase;
			register f32 UVStepSquare3 = UVStepSquare*UVBase;
			_ScaleAdd(TmpPoint, PolyNom1, UVBase, PolyNom0);
			_ScaleAdd(TmpPoint, PolyNom2, UVStepSquare, TmpPoint);
			_ScaleAdd(TmpPoint, PolyNom3, UVStepSquare3, TmpPoint);
			_ScaleAdd(TmpUV,    PolyNom5, UVBase, PolyNom4);        
			_ScaleAdd(TmpUV,    PolyNom6, UVStepSquare, TmpUV);
			_ScaleAdd(TmpUV,    PolyNom7, UVStepSquare3, TmpUV);
#if defined( USE_FLOAT2 )
			TmpUV = ps::Mul(TmpUV,textureYratio);
#else
			TmpUV.m_y*=textureYratio;
#endif
			*(TPoints++) = TmpPoint;
			*(TPointsUV++) = TmpUV;
#ifdef PATCH32_USE_COLOR
			pcolor[i]= BLENDCOLOR(pcolor[_hdiv],pcolor[0],(u32)(UVBase * 256.0f));
#endif

		}
		/* Build Down */
		TPoints = TPoints_D+1;
		TPointsUV = TPointsUV_D+1;
		PointsNS_+=4;
#ifdef PATCH32_USE_COLOR
		pcolor = color_D;
#endif
	}
}

/**************************************************************************************************************************************/
/* Push first line */
/**************************************************************************************************************************************/
	TPoints = TPoints_U;
	TPointsUV = TPointsUV_U;
	for (u32 i = 0 ; i < _hdiv + 1 ; i ++)
	{
		TmpPoint = *(TPoints++);
		TmpUV = *(TPointsUV++);
#ifdef PATCH32_USE_COLOR
		TmpColor = color_U[i];
#endif
#include "engine/AdaptersInterfaces/GFXAdapter_DrawPatch.h" //ADAPTER_DRAWPATCH_DEGREE_TWO
	}

/**************************************************************************************************************************************/
/* push intermediate lines */ 
/**************************************************************************************************************************************/
if (_vdiv > 1)
{
	register float fVMUL  = f32_Inv((f32)_vdiv);
	register float Interp = 0.0f;
	for (u32 j = 1 ; j < _vdiv ; j ++)
	{
		Interp += fVMUL;
		register float OmInterp = 1.0f - Interp;
		for (u32 i = 0 ; i < _hdiv + 1 ; i ++)
		{
#if defined( USE_FLOAT2 )
			TmpPoint = ps::Mul(TPoints_U[i],OmInterp) + ps::Mul(TPoints_D[i],Interp);
			TmpUV = ps::Mul(TPointsUV_U[i],OmInterp) + ps::Mul(TPointsUV_D[i],Interp);
#else
			TmpPoint = (TPoints_U[i]*OmInterp) + (TPoints_D[i]*Interp);
			TmpUV = (TPointsUV_U[i]*OmInterp) + (TPointsUV_D[i]*Interp);
#endif
#ifdef PATCH32_USE_COLOR
			TmpColor = BLENDCOLOR(color_D[i],color_U[i],(u32)(Interp * 255.0f));
#endif

#include "engine/AdaptersInterfaces/GFXAdapter_DrawPatch.h" ////ADAPTER_DRAWPATCH_DEGREE_TWO
		}
	}
}
/**************************************************************************************************************************************/
/* push last line */
/**************************************************************************************************************************************/
	TPoints = TPoints_D;
	TPointsUV = TPointsUV_D;
	for (u32 i = 0 ; i < _hdiv + 1 ; i ++)
	{
		TmpPoint = *(TPoints++);
		TmpUV = *(TPointsUV++);
#ifdef PATCH32_USE_COLOR
		TmpColor = color_D[i];
#endif
#include "engine/AdaptersInterfaces/GFXAdapter_DrawPatch.h" //ADAPTER_DRAWPATCH_DEGREE_TWO
	}

#ifdef SUPPORT_STATS_MANAGER
uPtr vtxCnt = uPtr(write) - uPtr(writebegin);
#ifdef PATCH32_USE_PCT
vtxCnt /= sizeof(VertexPCT);
#else
vtxCnt /= sizeof(VertexPT);
#endif
StatsManager::getptr()->drawPatchVtxCntInc(u32(vtxCnt));
#endif

#ifndef PATCH32_DO_NOT_RETURN
return ret;
#else
(void)ret;
#endif

#undef ADAPTER_DRAWPATCH_DEGREE_TWO

#endif // ADAPTER_DRAWPATCH_DEGREE_TWO
