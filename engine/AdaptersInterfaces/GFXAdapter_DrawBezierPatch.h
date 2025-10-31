
// General code for DrawBezierPatch
// Params
// Patch32Data *_pData, Texture *texture, const Vec2d *PointsNS, const f32* _widths, const f32* _widths2, const f32* _alphas, const Color *_color, u32 _generalColor, f32 _z

// Modes: PATCH32_USE_PCT - PATCH32_USE_COLOR

#ifdef PATCH32_USE_PCT
VertexPCT	* __restrict write = _pData->vertexListPCT + _pData->vertexCount;
#else // PATCH32_USE_PCT
VertexPT	* __restrict write = _pData->vertexListPT + _pData->vertexCount;
#endif // PATCH32_USE_PCT

#ifndef PATCH32_IGNORE_MESHELEMENTS
ITF_ASSERT(_pData->mesh->m_ElementList.size() > 0);
ITF_MeshElement &elem = _pData->mesh->m_ElementList[0];
elem.m_material.m_textureDiffuse = texture->getID();
elem.m_count        += _vdiv*(6*_hdiv);
#endif // PATCH32_IGNORE_MESHELEMENTS

const f32 UStep = f32_Inv((f32)_hdiv);
const f32 VStep = f32_Inv((f32)_vdiv);

const f32 lenUV   = _widths[3] - _widths[2];
const f32 limB    = _widths2[1];//_alphas[1];
const f32 limA    = _widths2[0];//_alphas[0];

Vec2d pos;
Vec2d uv;
Vec2d tgt;
Vec2d perp[2];

Vec2d TmpPoint;
Vec2d TmpUV;
f32 TmpS;

#ifdef PATCH32_USE_COLOR
ColorInteger   TmpColor;

const f32 f32_InvA0     = f32_Inv(_alphas[0]);
const f32 f32_InvA1     = f32_Inv(1.f - _alphas[1]);
const f32 f32_InvDiffA  = f32_Inv(_alphas[1] - _alphas[0]);
#endif // PATCH32_USE_COLOR

#ifdef USE_WGP
ITF_VertexBuffer *pvtxBuffer = _pData->mesh->m_vtxBuffer;
#endif

f32 u = 0.f;
for(u32 iu = 0; iu <= _hdiv; ++iu, u = f32_Min(1.f,u+UStep))
{
    const f32 UCarre    = u * u;
    const f32 UCube     = UCarre * u;

    const f32 UInv      = 1.f - u;
    const f32 UInvCarre = UInv * UInv;
    const f32 UInvCube  = UInvCarre * UInv;
    
    const f32 UUInv2    = 2.f * u * UInv;
    
    const f32 C1        = 3.f * u * UInvCarre;
    const f32 C2        = 3.f * UCarre * UInv;
    const f32 C3        = UInvCarre - UUInv2;
    const f32 C4        = UUInv2 - UCarre;

    // Pos
    Vec2d::Scale(&pos, &PointsNS[0], UInvCube);
    Vec2d::ScaleAdd(&pos, &PointsNS[1], C1, &pos);
    Vec2d::ScaleAdd(&pos, &PointsNS[2], C2, &pos);
    Vec2d::ScaleAdd(&pos, &PointsNS[3], UCube, &pos);
    
    // Tangeant
    Vec2d::Scale(&tgt, &PointsNS[0], -UInvCarre);
    Vec2d::ScaleAdd(&tgt, &PointsNS[1], C3, &tgt);
    Vec2d::ScaleAdd(&tgt, &PointsNS[2], C4, &tgt);
    Vec2d::ScaleAdd(&tgt, &PointsNS[3], UCarre, &tgt);
    tgt.normalize();

    if(u < limA)
    {
		const f32 TmpR = u * f32_Inv(limA);
		TmpS  =  _widths2[2] + ((_widths[0] - _widths2[2]) * TmpR);
    }
    else if(u > limB)
    {
 		const f32 TmpR = (u - limB) * f32_Inv(1.0f - limB);
		TmpS  =  _widths[1] + ((_widths2[3] - _widths[1]) * TmpR);
    }
    else
    {
		const f32 TmpR = (u - limA) * f32_Inv(limB - limA);
		TmpS =  _widths[0] + ((_widths[1] - _widths[0]) * TmpR);
    }

    // Perpendicular
    perp[0].m_x = - tgt.m_y;
    perp[0].m_y =   tgt.m_x;
    perp[0] *= TmpS;

    // UV
    Vec2d::Scale(&uv, &PointsNS[4], UInvCube);
    Vec2d::ScaleAdd(&uv, &PointsNS[5], C1, &uv);
    Vec2d::ScaleAdd(&uv, &PointsNS[6], C2, &uv);
    Vec2d::ScaleAdd(&uv, &PointsNS[7], UCube, &uv);    
    // Tangeant
    Vec2d::Scale(&tgt, &PointsNS[4], -UInvCarre);
    Vec2d::ScaleAdd(&tgt, &PointsNS[5], C3, &tgt);
    Vec2d::ScaleAdd(&tgt, &PointsNS[6], C4, &tgt);
    Vec2d::ScaleAdd(&tgt, &PointsNS[7], UCarre, &tgt);    
    tgt.normalize();
    // Perpendicular
    perp[1].m_x = - tgt.m_y;
    perp[1].m_y =   tgt.m_x;
    perp[1] *= (_widths[2] + lenUV*u);

    // Color
#ifdef PATCH32_USE_COLOR
    if(u < _alphas[0])
    {
        TmpColor     = _color[0];
        TmpColor.m_rgba.m_a = ITFCOLOR_F32_TO_U8(_alphas[2] + ((TmpColor.getF32Alpha() - _alphas[2]) * u * f32_InvA0));
    }
    else if(u > _alphas[1])
    {
        TmpColor      = _color[1];
        f32 alpha     = TmpColor.getF32Alpha();
        alpha        += ((_alphas[3] - alpha) * (u - _alphas[1]) * f32_InvA1);
        TmpColor.m_rgba.m_a = ITFCOLOR_F32_TO_U8(alpha);
    }
    else
    {
        TmpColor = ColorInteger::Interpolate(_color[0], _color[1], (u - _alphas[0]) * f32_InvDiffA);
    }
#endif // PATCH32_USE_COLOR
       
    f32 v = 0.f;
    for(u32 iv=0; iv <= _vdiv; ++iv, v+=VStep)
    {
        // Extrude Position
        Vec2d::ScaleAdd(&TmpPoint, &perp[0], v - 0.5f, &pos);
        
        // Extrude UV
        Vec2d::ScaleAdd(&TmpUV,    &perp[1], 0.5f - v, &uv);

#ifndef VBMANAGER_USE_DYNAMICRING_VB
        ITF_ASSERT(_pData->vertexCount < (_pData->mesh->m_vtxBuffer ? _pData->mesh->m_vtxBuffer : _pData->mesh->m_vtxBufferBuffered[_pData->mesh->m_userBuffer])->m_nVertex);
#else // VBMANAGER_USE_DYNAMICRING_VB
        ITF_ASSERT(_pData->vertexCount < _pData->mesh->m_vtxBuffer->m_nVertex);
#endif // VBMANAGER_USE_DYNAMICRING_VB

#ifdef USE_WGP
        pvtxBuffer->wgp_write(TmpPoint);
        pvtxBuffer->wgp_write(_z);
# ifdef PATCH32_USE_PCT
        #ifdef PATCH32_USE_COLOR
        pvtxBuffer->wgp_write(TmpColor.getAsU32());
        #else // PATCH32_USE_COLOR
        pvtxBuffer->wgp_write(_generalColor);
        #endif // PATCH32_USE_COLOR
# endif // PATCH32_USE_PCT
        pvtxBuffer->wgp_write(TmpUV);
#else // USE_WGP
        write->setData
        (
            Vec3d(TmpPoint.m_x, TmpPoint.m_y, _z)
          , TmpUV
# ifdef PATCH32_USE_PCT
          #ifdef PATCH32_USE_COLOR
          , TmpColor.getAsU32()
          #else // PATCH32_USE_COLOR
          , _generalColor
          #endif // PATCH32_USE_COLOR
# endif // PATCH32_USE_PCT
        );
#endif // USE_WGP
        write++;
        _pData->vertexCount++;
    }
}

#ifdef SUPPORT_STATS_MANAGER
StatsManager::getptr()->drawBezierPatchVtxCntInc(_pData->vertexCount);
#endif

#ifndef PATCH32_DO_NOT_RETURN
return bfalse;
#endif
