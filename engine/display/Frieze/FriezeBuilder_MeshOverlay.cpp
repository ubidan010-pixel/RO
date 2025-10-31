#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

#ifndef _ITF_FRIEZESTATS_H_
#include "engine/display/Frieze/FriezeStats.h"
#endif // _ITF_FRIEZESTATS_H_

namespace ITF
{
    void Frise::clearMeshOverlayData()
    {       
        if ( !m_pMeshOverlayData )
            return;

        if ( m_pMeshOverlayData->m_mesh.m_vtxBuffer )
        {
            GFX_ADAPTER->removePrimitiveFromRenderLists(GFX_ITF_MESH, (void*)&m_pMeshOverlayData->m_mesh);
            FreeVB(m_pMeshOverlayData->m_mesh.m_vtxBuffer);
        }

        for (u32 i = 0; i < m_pMeshOverlayData->m_mesh.m_ElementList.size(); i++)
        {
            if(m_pMeshOverlayData->m_mesh.m_ElementList[i].m_indexBuffer) 
                FreeIB(m_pMeshOverlayData->m_mesh.m_ElementList[i].m_indexBuffer);
        }

        SF_DEL(m_pMeshOverlayData->m_mesh.m_pMatrixOverlay);
        SF_DEL(m_pMeshOverlayData);
    }

    void Frise::buildMesh_Overlay( MeshOverlay_VertexList* _vertexTempList, MeshOverlay_IndexList* _indexTempList, const Matrix44* _pMatrixTransfo )
    {        
        if ( !_vertexTempList || !_indexTempList )
            return;

        if ( _vertexTempList->size() >= 3 )
        {
            const FriseConfig* config = getConfig();
            if ( !config )
                return;

            // new alloc
            m_pMeshOverlayData = newAlloc(mId_Frieze, MeshOverlayData);
            m_pMeshOverlayData->m_mesh.m_pMatrixOverlay = newAlloc(mId_Frieze, GMatrix44);

            const u32 vtxCount = _vertexTempList->size();
            const u32 idCount = _indexTempList->size();

            // set aabb
            for(u32 id = 0; id< idCount; ++id)
            {
                const Vec3d& pos = (*_vertexTempList)[ (*_indexTempList)[id]].m_pos;

                m_pMeshOverlayData->m_AABB.grow( pos );

#ifdef ITF_WINDOWS
                EditorWireframeMesh.push_back( pos );
#endif // ITF_WINDOWS
            }            

            if ( _pMatrixTransfo )
                m_pMeshOverlayData->m_mesh.setMatrix(*_pMatrixTransfo);
            
            // copy vertexTempList
            m_pMeshOverlayData->m_mesh.m_vtxBuffer = GFX_ADAPTER->createVertexBuffer
                (
                vtxCount,
                VertexFormat_PCBT,
                sizeof(VertexPCBT),
                bfalse,
                VB_T_FRIEZEOVERLAY
                );

            VertexPCBT    *pdata = 0;
            GFX_ADAPTER->getVertexBufferManager().LockVertexBuffer(m_pMeshOverlayData->m_mesh.m_vtxBuffer,(void **) &pdata);
            ITF_MemcpyWriteCombined(pdata, _vertexTempList->getDataBuffer(), sizeof(VertexPCBT)*vtxCount);
            GFX_ADAPTER->getVertexBufferManager().UnlockVertexBuffer(m_pMeshOverlayData->m_mesh.m_vtxBuffer);

            FRIEZESTAT_ADD_VB( m_pMeshOverlayData->m_mesh.m_vtxBuffer, getRef(), m_pMeshOverlayData->m_mesh.m_vtxBuffer->m_size );

            // copy element                  
            m_pMeshOverlayData->m_mesh.addElement();

            ITF_MeshElement& elem = m_pMeshOverlayData->m_mesh.m_ElementList[0];                
            setMeshElement( elem, config, idCount, (u32)getFirstNoFillTextureIndex( config ) );
            elem.m_material.m_matType = GFX_MAT_FRIEZEOVERLAY;

            u16* pindexelem;
            elem.m_indexBuffer->Lock((void**)&pindexelem);
            ITF_MemcpyWriteCombined(pindexelem , _indexTempList->getDataBuffer(), sizeof(u16) *idCount );
            elem.m_indexBuffer->Unlock();                   
        }

        SF_DEL(_indexTempList);
        SF_DEL(_vertexTempList);
    }
   
    void Frise::buildVB_Overlay_Quad( MeshOverlay_VertexList* _vertexTempList, MeshOverlay_IndexList* _indexTempList, Vec2d* _pos, Vec2d* _uv, u32* _color, u32 _idAnim ) const
    {
        /*
        v1...v3
        .    .
        .    .
        v0...v2
        */

        u16 indexVtxDown = (u16)_vertexTempList->size();
        u16 indexVtxUp = indexVtxDown +1;
        _vertexTempList->incrSize(4); 

        VertexPCBT& vtx0 = (*_vertexTempList)[ indexVtxDown ];
        vtx0.setData( _pos[0], _color[0], 0, _uv[0]);

        VertexPCBT& vtx1 = (*_vertexTempList)[ indexVtxUp ];
        vtx1.setData( _pos[1], _color[1], _idAnim, _uv[1]);

        VertexPCBT& vtx2 = (*_vertexTempList)[ indexVtxDown +2];
        vtx2.setData( _pos[2], _color[2], 0, _uv[2]);

        VertexPCBT& vtx3 = (*_vertexTempList)[ indexVtxUp +2];
        vtx3.setData( _pos[3], _color[3], _idAnim, _uv[3]);

        // add index
        _indexTempList->push_back(indexVtxDown);
        _indexTempList->push_back(indexVtxUp);
        _indexTempList->push_back(indexVtxDown +2);

        _indexTempList->push_back(indexVtxUp);  
        _indexTempList->push_back(indexVtxUp +2);
        _indexTempList->push_back(indexVtxDown +2);        
    }

    void Frise::setMeshOverlayDataMatrix( const VertexAnim* _anim )
    {
        GFX_Vector4 vParams;

        f32_CosSin( (m_timeCur *_anim->m_globalSpeed) +m_animGlobalSync, &vParams.m_x, &vParams.m_y);
        f32_CosSin( m_animGlobalRotCur.ToRadians(), &vParams.m_z, &vParams.m_w);

        for ( u32 i=0; i<8; i++)
        {
            const f32 cosTime = vParams.m_x * m_pMeshOverlayData->m_sync[i].m_x - vParams.m_y * m_pMeshOverlayData->m_sync[i].m_y;                  
            const f32 sinTime = vParams.m_y * m_pMeshOverlayData->m_sync[i].m_x + vParams.m_x * m_pMeshOverlayData->m_sync[i].m_y;

            const f32 x1 = (cosTime * _anim->m_speedX + m_pMeshOverlayData->m_syncVtx[i].m_x) * _anim->m_amplitudeX;
            const f32 y1 = (sinTime * _anim->m_speedY + m_pMeshOverlayData->m_syncVtx[i].m_y) * _anim->m_amplitudeY;

            const f32 cosAngle = m_pMeshOverlayData->m_angle.m_x * vParams.m_z - m_pMeshOverlayData->m_angle.m_y * vParams.m_w;
            const f32 sinAngle = m_pMeshOverlayData->m_angle.m_x * vParams.m_w + m_pMeshOverlayData->m_angle.m_y * vParams.m_z;

            const u32 idMatrix = i * 2;
            m_pMeshOverlayData->m_mesh.m_pMatrixOverlay->fa16[idMatrix] = x1 * cosAngle - y1 * sinAngle;
            m_pMeshOverlayData->m_mesh.m_pMatrixOverlay->fa16[idMatrix +1] = x1 * sinAngle + y1 * cosAngle;
        }
    }
} // namespace ITF
