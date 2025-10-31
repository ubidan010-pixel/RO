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
    void Frise::addIndex_Anim( MeshAnim_VertexList& _vertexTempList, MeshAnim_IndexList& _indexTempList, u16 _index ) const
    {
        _indexTempList.push_back(_index);
    }

    void Frise::setVtxAnimOnEdge( VertexAnim& _anim, const edgeFrieze& _edge, VertexPNC3T& _vtx ) const
    {      
        setVtxAnim( _anim, _vtx );

        if ( _anim.m_angleUsed)
        {           
            f32_CosSin( _anim.m_angle.ToRadians(), &_vtx.m_uv4.m_x, &_vtx.m_uv4.m_y);
        }
        else
        {                       
            f32_CosSin( atan2f( _edge.m_sightNormalized.m_y, _edge.m_sightNormalized.m_x ), &_vtx.m_uv4.m_x, &_vtx.m_uv4.m_y);
        }        
    }

    void Frise::setVtxAnimOnEdge( VertexAnim& _anim, Vec2d& _vector, VertexPNC3T& _vtx ) const
    {      
        setVtxAnim( _anim, _vtx );

        if ( _anim.m_angleUsed)
        {          
            f32_CosSin( _anim.m_angle.ToRadians(), &_vtx.m_uv4.m_x, &_vtx.m_uv4.m_y);
        }
        else
        {                     
            f32_CosSin( atan2f( _vector.m_y, _vector.m_x ), &_vtx.m_uv4.m_x, &_vtx.m_uv4.m_y);
        }        
    }

    void Frise::setVtxAnimOnCorner( VertexAnim& _anim, const edgeFrieze& _edge, VertexPNC3T& _vtx ) const
    {
        setVtxAnim( _anim, _vtx );

        if ( _anim.m_angleUsed)
        {        
            f32_CosSin( _anim.m_angle.ToRadians(), &_vtx.m_uv4.m_x, &_vtx.m_uv4.m_y);
        }
        else
        {      
            f32_CosSin( atan2f( -_edge.m_cornerNormal.m_x, _edge.m_cornerNormal.m_y ), &_vtx.m_uv4.m_x, &_vtx.m_uv4.m_y);
        } 
    }

    void Frise::setVtxAnim( VertexAnim& _anim, VertexPNC3T& _vtx ) const
    {
        _vtx.m_uv2.m_x = _anim.m_speedX;
        _vtx.m_uv2.m_y = _anim.m_speedY;
        _vtx.m_uv2.m_z = f32_Cos( m_recomputeData.m_animSyncXCur );
        _vtx.m_uv2.m_w = f32_Sin( m_recomputeData.m_animSyncYCur );

        _vtx.m_uv3.m_x = _anim.m_amplitudeX; 
        _vtx.m_uv3.m_y = _anim.m_amplitudeY;
        f32_CosSin( m_recomputeData.m_animSyncCur, &_vtx.m_uv3.m_z, &_vtx.m_uv3.m_w);

        m_recomputeData.m_animSyncCur += _anim.m_sync;
        m_recomputeData.m_animSyncXCur += _anim.m_syncX;
        m_recomputeData.m_animSyncYCur += _anim.m_syncY;
    }

    void Frise::copyVtxAnim( VertexPNC3T& _vtxTo, VertexPNC3T& _vtxFrom ) const
    {                
        _vtxTo.m_uv2 = _vtxFrom.m_uv2;                         
        _vtxTo.m_uv3 = _vtxFrom.m_uv3;     
        _vtxTo.m_uv4 = _vtxFrom.m_uv4;
    }

    void Frise::setVtxAnimNull( VertexPNC3T& _vtx ) const
    {
        _vtx.m_uv2.m_x = 0.f;
        _vtx.m_uv2.m_y = 0.f;
        _vtx.m_uv2.m_z = 0.f;
        _vtx.m_uv2.m_w = 0.f;

        _vtx.m_uv3.m_x = 0.f;
        _vtx.m_uv3.m_y = 0.f;
        _vtx.m_uv3.m_z = 0.f;
        _vtx.m_uv3.m_w = 0.f;

        _vtx.m_uv4.m_x = 0.f;
        _vtx.m_uv4.m_y = 0.f;
    }

    GFX_Vector4 Frise::getvParamsMeshAnim( const VertexAnim* _anim )
    {
        GFX_Vector4 vParams;

        f32_CosSin( (m_timeCur *_anim->m_globalSpeed) +m_animGlobalSync, &vParams.m_x, &vParams.m_y);
        f32_CosSin( m_animGlobalRotCur.ToRadians(), &vParams.m_z, &vParams.m_w);

        return vParams;
    }

#ifdef ITF_WII
    bbool Frise::isVtxAnim( VertexPNC3T& _vtx) const
    {
        if ( _vtx.m_uv3.m_x != 0.f || _vtx.m_uv3.m_y != 0.f )
            return btrue;

        return bfalse;
    }

    u16 Frise::getIndexLastWithoutVtxAnim( u16 _indexStop, MeshAnim_VertexList& _vertexTempList) const
    {
        for ( u16 i = _indexStop; i >0; i-- )
        {
            if ( !isVtxAnim( _vertexTempList[i]) )
            {
                return i;                
            }
        }

        return 1;
    }

    void Frise::reorderVertexAndIndexListByAnim( MeshAnim_VertexList& _vertexTempList, MeshAnim_IndexList* _indexTempList, u32 _indexTempListSize) const
    {
        const u16 vertexListCount = (u16)_vertexTempList.size();        
        u16 indexVtxEndWithoutAnim = getIndexLastWithoutVtxAnim( vertexListCount -1, _vertexTempList);

        u16* newIndexList = newAlloc(mId_Frieze, u16[vertexListCount]);
        for ( u16 i = 0; i<vertexListCount; i++ )
            newIndexList[i] = i;

        // reorder vertexList
        for ( u16 i = 0; i<indexVtxEndWithoutAnim; i++)
        {            
            if ( isVtxAnim( _vertexTempList[i]) )
            {              
                // switch vertex
                VertexPNC3T vtxCopy = _vertexTempList[indexVtxEndWithoutAnim];
                _vertexTempList[indexVtxEndWithoutAnim] = _vertexTempList[i];
                _vertexTempList[i] = vtxCopy;

                // update newIndexList
                newIndexList[indexVtxEndWithoutAnim] = i;
                newIndexList[i] = indexVtxEndWithoutAnim;                

                indexVtxEndWithoutAnim = getIndexLastWithoutVtxAnim( indexVtxEndWithoutAnim -1, _vertexTempList);
            }
        }

        // update indexList
        for (u32 index=0; index<_indexTempListSize; index++)
        {
            MeshAnim_IndexList &currentIndexList = _indexTempList[index];
            for ( u32 i = 0; i<currentIndexList.size(); i++ )
            {
                currentIndexList[i] = newIndexList[ currentIndexList[i]];
            }
        }

        delete[] newIndexList;

        // register first vtx index anim
        _vertexTempList[0].m_uv2.m_x = (f32)indexVtxEndWithoutAnim +1.f;
    }
#endif //ITF_WII

    void Frise::clearMeshAnimData()
    {        
        if ( !m_pMeshAnimData )
            return;

        if (m_pMeshAnimData->m_mesh.m_vtxBuffer)
        {
            GFX_ADAPTER->removePrimitiveFromRenderLists(GFX_ITF_MESH, (void*)&m_pMeshAnimData->m_mesh);
            FreeVB(m_pMeshAnimData->m_mesh.m_vtxBuffer);
        }

        for (u32 i = 0; i < m_pMeshAnimData->m_mesh.m_ElementList.size(); i++)
        {
            if(m_pMeshAnimData->m_mesh.m_ElementList[i].m_indexBuffer) 
                FreeIB(m_pMeshAnimData->m_mesh.m_ElementList[i].m_indexBuffer);
        }

        SF_DEL(m_pMeshAnimData);
    }

    void Frise::buildMesh_Anim( MeshAnim_VertexList& _vertexTempList, MeshAnim_IndexList* _indexTempList, const Matrix44* _pMatrixTransfo )
    {      
        if ( _vertexTempList.size() >= 3 )
        {
            FriseConfig* config = getConfig();
            if ( !config )
                return;

            m_pMeshAnimData = newAlloc(mId_Frieze, MeshAnimData);

#ifdef ITF_WII
            reorderVertexAndIndexListByAnim( _vertexTempList, _indexTempList, m_recomputeData.m_MeshElementTable_ElemCount[MeshElementRouter::MeshElementTable_Dynamic]);
#endif //ITF_WII

            // set aabb
            for(u32 indexTempListIndex = 0; indexTempListIndex < m_recomputeData.m_MeshElementTable_ElemCount[MeshElementRouter::MeshElementTable_Dynamic]; ++indexTempListIndex)
            {
                for(u32 iIndex = 0; iIndex < _indexTempList[indexTempListIndex].size(); ++iIndex)
                {
                    const Vec3d& pos = _vertexTempList[_indexTempList[indexTempListIndex][iIndex]].m_pos;

                    m_pMeshAnimData->m_AABB.grow( pos );
#ifdef ITF_WINDOWS
                    EditorWireframeMesh.push_back( pos );
#endif // ITF_WINDOWS
                }
            }

            if ( _pMatrixTransfo )
                m_pMeshAnimData->m_mesh.setMatrix(*_pMatrixTransfo);

            const u32 vtxCount = _vertexTempList.size();

            // copy vertexTempList
            m_pMeshAnimData->m_mesh.m_vtxBuffer = GFX_ADAPTER->createVertexBuffer
                (
                vtxCount,
                VertexFormat_PNC3T,
                sizeof(VertexPNC3T),
                bfalse,
                VB_T_FRIEZEANIM
                );

            VertexPNC3T    *pdata = 0;
            GFX_ADAPTER->getVertexBufferManager().LockVertexBuffer(m_pMeshAnimData->m_mesh.m_vtxBuffer,(void **) &pdata);
            ITF_MemcpyWriteCombined(pdata, _vertexTempList.getDataBuffer(), sizeof(VertexPNC3T) *vtxCount);
            GFX_ADAPTER->getVertexBufferManager().UnlockVertexBuffer(m_pMeshAnimData->m_mesh.m_vtxBuffer);

            FRIEZESTAT_ADD_VB(m_pMeshAnimData->m_mesh.m_vtxBuffer, getRef(), m_pMeshAnimData->m_mesh.m_vtxBuffer->m_size );

            // copy all element       
            u32 textureConfigCount = config->m_textureConfigs.size();
            for (u32 i = 0; i < textureConfigCount ; i++ )
            {            
                const MeshElementRouter* elemRouter = getRoutingElement(MeshElementRouter::MeshElementTable_FlagAnimated, i);
                if (elemRouter)
                {   // we have a default mesh elem
                    ITF_ASSERT(elemRouter->m_tableID == MeshElementRouter::MeshElementTable_Dynamic);

                    MeshAnim_IndexList& indexTable = _indexTempList[elemRouter->m_indexInTable];

                    u32 indexTableSize = indexTable.size();
                    if ( indexTableSize == 0 )
                        continue;

                    m_pMeshAnimData->m_mesh.addElement();

                    ITF_MeshElement& elem = m_pMeshAnimData->m_mesh.m_ElementList[m_pMeshAnimData->m_mesh.m_ElementList.size()-1];                
                    setMeshElement( elem, config, indexTableSize, i );
                    elem.m_material.m_matType = GFX_MAT_FRIEZEANIM;

                    u16* pindexelem;
                    elem.m_indexBuffer->Lock((void**)&pindexelem);
                    ITF_MemcpyWriteCombined(pindexelem ,&indexTable[0], sizeof(u16) * indexTableSize );
                    elem.m_indexBuffer->Unlock();
                    
                    m_pMeshAnimData->m_elemConfig_IndexList.push_back(i);
                }
            }

            ITF_ASSERT( m_pMeshAnimData->m_mesh.m_ElementList.size() == 1); // to match with renderFrieze() mesh anim
        }
    }

    void Frise::buildVB_Anim_CornerRounderBlending( MeshAnim_VertexList& _vertexTempList, MeshAnim_IndexList& _indexTempList, CornerRounder& _corner ) const
    {
        u16 indexVtxStart = *_corner.m_indexVtxCorner;            

        // corner's edge norm 
        f32 angle = abs( _corner.m_edgeCur->m_cornerAngle );        
        u32 stepCount = getCornerStepNb( angle, _corner.m_config->m_smoothFactorVisual, getCoeffScaleStepFanToBlend( _corner.m_edgeCur, _corner.m_edgeLast) );  
        f32 timeStep = 1.f /(f32)stepCount;

        // bezier curve        
        Vec2d p0 = _corner.m_startPos;     
        Vec2d p3 = _corner.m_stopPos; 
        Vec2d p1 = p0; 
        Vec2d p2 = p3; 

        buildOutline_initPosCornerRounderBlending( _corner.m_edgeLast, _corner.m_edgeCur, p1, p2);

#ifdef DEVELOPER_JAY_FRIEZE
        GFX_ADAPTER->drawDBGLine( transformPos(p0), transformPos(p1), Color::red(), 0.f, 5.f, getDepth() );
        GFX_ADAPTER->drawDBGLine( transformPos(p1), transformPos(p2), Color::red(), 0.f, 5.f, getDepth() );
        GFX_ADAPTER->drawDBGLine( transformPos(p2), transformPos(p3), Color::red(), 0.f, 5.f, getDepth() );
#endif

        Vec2d posLast = p0;
        f32 uvCornerTotal = 0.f;
        Vec2d uvCur = *_corner.m_uvCorner;
        f32 coeffUv = getStepCornerCoeff( _corner.m_edgeRun->m_coeff, angle ) *(*_corner.m_uvXsign);

        *_corner.m_indexVtxCorner = (u16)_vertexTempList.size();
        u16 indexVtxCorner = *_corner.m_indexVtxCorner -1;
        _vertexTempList.incrSize( Max( 1u, stepCount ) );

        for ( u32 i = 1; i < stepCount; i++ )
        {
            f32 time = (f32)i * timeStep;
            f32 timeInv = 1.f - time;

            Vec2d pos =   ( p0 *timeInv *timeInv *timeInv ) 
                + ( p1 *3.f *time *timeInv *timeInv ) 
                + ( p2 *3.f *time *time *timeInv ) 
                + ( p3 *time *time *time );

            // update uv
            f32 uvToAdd = ( pos -posLast).norm() *coeffUv;
            uvCur.m_x += uvToAdd;
            uvCornerTotal += uvToAdd;

            VertexPNC3T& vtxCorner = _vertexTempList[*_corner.m_indexVtxCorner];
            setVtxPos( pos, vtxCorner.m_pos, _corner.m_zVtxCorner);
            vtxCorner.setUv( uvCur);
            vtxCorner.setColor( _corner.m_colorCorner);
            copyVtxAnim(vtxCorner, _vertexTempList[indexVtxStart]);

            //  add vertex to create a triangle
            addIndex_Anim( _vertexTempList, _indexTempList, *_corner.m_indexVtxCorner );

            // add two index for next triangle
            addIndex_Anim( _vertexTempList, _indexTempList, *_corner.m_indexVtxCenter );
            addIndex_Anim( _vertexTempList, _indexTempList, *_corner.m_indexVtxCorner );

            *_corner.m_indexVtxCorner += 1;

            posLast = pos;
        }

        uvCornerTotal += ( p3 -posLast).norm() *coeffUv;

        // set uvs corner with offset
        f32 offset = uvCornerTotal *0.5f;

        // update uv quad edge last
        updateEdgeCornerUvs( _corner, _vertexTempList[indexVtxStart].m_uv.m_x, offset, offset);

        if ( _corner.m_edgeCur->m_snap && *_corner.m_edgeCountCur == _corner.m_edgeRun->m_edgeCount -1)
            offset *= 2.f;

        // update uv corner
        if ( !_corner.m_edgeLast->m_snap)
        {
            for ( u32 i = 1; i < stepCount; i++ )
            {
                _vertexTempList[indexVtxCorner +i ].m_uv.m_x -= offset;
            }
        }

        // update uv for next vtx
        _corner.m_uvCorner->m_x += uvCornerTotal;
    }

    void Frise::buildVB_Anim_CornerRounder( MeshAnim_VertexList& _vertexTempList, MeshAnim_IndexList& _indexTempList, CornerRounder& _corner ) const
    {
        u16 vtxStartId = *_corner.m_indexVtxCorner;

        u32 stepNb;
        f32 stepCornerAngle;  
        f32 uvXStep;                
        f32 arcBy2;
        setCornerRounderDatas( _corner.m_config, _corner.m_edgeCur, _corner.m_edgeRun->m_coeff, stepNb, stepCornerAngle, uvXStep, arcBy2, *_corner.m_uvXsign );

        // update uvs
        updateEdgeCornerUvs( _corner, _vertexTempList[*_corner.m_indexVtxCorner].m_uv.m_x, arcBy2, arcBy2);

        *_corner.m_indexVtxCorner = (u16)_vertexTempList.size();
        _vertexTempList.incrSize( Max( 1u, stepNb ) );

        f32 rotCos, rotSin;
        f32_CosSin( -stepCornerAngle *getSign(_corner.m_edgeCur->m_cornerAngle), &rotCos, &rotSin);

        for ( u32 i = 1; i < stepNb; i++ )
        {
            // update uv
            _corner.m_uvCorner->m_x += uvXStep;
            // update rotation
            _corner.m_startPos = _corner.m_startPos.RotateAroundCS( _corner.m_centerPos, rotCos, rotSin );

            VertexPNC3T& vtxCorner = _vertexTempList[*_corner.m_indexVtxCorner];
            setVtxPos( _corner.m_startPos, vtxCorner.m_pos, _corner.m_zVtxCorner);
            vtxCorner.setUv( *_corner.m_uvCorner);
            vtxCorner.setColor( _corner.m_colorCorner);
            copyVtxAnim(vtxCorner, _vertexTempList[vtxStartId]);

            //  add vertex to create a triangle
            addIndex_Anim( _vertexTempList, _indexTempList, *_corner.m_indexVtxCorner );

            // add two index for next triangle
            addIndex_Anim( _vertexTempList, _indexTempList, *_corner.m_indexVtxCenter );
            addIndex_Anim( _vertexTempList, _indexTempList, *_corner.m_indexVtxCorner );

            *_corner.m_indexVtxCorner += 1;
        }

        // update uv
        _corner.m_uvCorner->m_x += uvXStep;
    }

    void Frise::buildVB_Anim_CornerSquare( MeshAnim_VertexList& _vertexTempList, MeshAnim_IndexList& _indexTempList, CornerRounder& _corner ) const
    {
        u16 indexVtxStart = *_corner.m_indexVtxCorner; 

        f32 uvLeftCorner = getCornerSquareArcBy2( _corner.m_edgeCur->m_cornerAngle, _corner.m_edgeRun->m_coeff, _corner.m_startPos - _corner.m_cornerPos, *_corner.m_uvXsign );
        f32 uvRightCorner = uvLeftCorner;

        if ( !isHeightEqual( *_corner.m_edgeLast, *_corner.m_edgeCur) )
        {
            uvRightCorner = getCornerSquareArcBy2( _corner.m_edgeCur->m_cornerAngle, _corner.m_edgeRun->m_coeff, _corner.m_stopPos - _corner.m_cornerPos, *_corner.m_uvXsign );
        }    

        // update uvs
        updateEdgeCornerUvs( _corner, _vertexTempList[*_corner.m_indexVtxCorner].m_uv.m_x, uvLeftCorner, uvRightCorner);

        *_corner.m_indexVtxCorner = (u16)_vertexTempList.size();
        _vertexTempList.incrSize( 2 );

        // update uv
        _corner.m_uvCorner->m_x += uvLeftCorner;

        VertexPNC3T& vtxCorner = _vertexTempList[*_corner.m_indexVtxCorner];
        setVtxPos( _corner.m_cornerPos, vtxCorner.m_pos, _corner.m_zVtxCorner);
        vtxCorner.setUv( *_corner.m_uvCorner);
        vtxCorner.setColor( _corner.m_colorCorner);
        copyVtxAnim(vtxCorner, _vertexTempList[indexVtxStart]);

        //  add vertex to create a triangle
        addIndex_Anim( _vertexTempList, _indexTempList, *_corner.m_indexVtxCorner );

        // add two index for next triangle
        addIndex_Anim( _vertexTempList, _indexTempList, *_corner.m_indexVtxCenter );
        addIndex_Anim( _vertexTempList, _indexTempList, *_corner.m_indexVtxCorner );

        *_corner.m_indexVtxCorner += 1;

        // update uv
        _corner.m_uvCorner->m_x += uvRightCorner;
    }

    void Frise::buildVB_Anim_Corner( MeshAnim_VertexList& _vertexTempList, MeshAnim_IndexList& _indexTempList, CornerRounder& _corner ) const
    { 
        if ( _corner.m_edgeCur->m_cornerAngle == 0.f )
            return;

        u16 indexVtxAnimToCopy;

        if ( _corner.m_edgeCur->m_cornerAngle > 0.f )
        {
            _corner.m_indexVtxCenter = _corner.m_indexVtxDown;
            _corner.m_indexVtxCorner = _corner.m_indexVtxUp;
            _corner.m_zVtxCorner = m_recomputeData.m_zVtxUp;

            _corner.m_startPos = _corner.m_edgeLast->m_points[3];
            _corner.m_centerPos = _corner.m_edgeCur->m_interDown;
            _corner.m_stopPos = _corner.m_edgeCur->m_points[1];
            _corner.m_cornerPos = _corner.m_edgeCur->m_interUp;

            _corner.m_uvCorner = _corner.m_uvUp;
            _corner.m_uvXoffsetCorner = _corner.m_uvUpXOffSet;

            _corner.m_colorCorner = _vertexTempList[*_corner.m_indexVtxUp].m_color;
            indexVtxAnimToCopy = *_corner.m_indexVtxUp;
        }
        else
        {
            _corner.m_indexVtxCenter = _corner.m_indexVtxUp;
            _corner.m_indexVtxCorner = _corner.m_indexVtxDown;
            _corner.m_zVtxCorner = m_recomputeData.m_zVtxDown;

            _corner.m_startPos = _corner.m_edgeLast->m_points[2];
            _corner.m_centerPos = _corner.m_edgeCur->m_interUp;
            _corner.m_stopPos = _corner.m_edgeCur->m_points[0];
            _corner.m_cornerPos = _corner.m_edgeCur->m_interDown;

            _corner.m_uvCorner = _corner.m_uvDown;
            _corner.m_uvXoffsetCorner = _corner.m_uvDownXOffSet;

            _corner.m_colorCorner = _vertexTempList[*_corner.m_indexVtxDown].m_color;
            indexVtxAnimToCopy = *_corner.m_indexVtxDown;
        }

        addIndex_Anim( _vertexTempList, _indexTempList, *_corner.m_indexVtxCenter );
        addIndex_Anim( _vertexTempList, _indexTempList, *_corner.m_indexVtxCorner );

        if ( _corner.m_config->m_smoothVisual )
        {
            if ( isHeightEqual( *_corner.m_edgeLast, *_corner.m_edgeCur))
            {
                buildVB_Anim_CornerRounder( _vertexTempList, _indexTempList, _corner );
            }
            else
            {
                buildVB_Anim_CornerRounderBlending( _vertexTempList, _indexTempList, _corner );
            }
        }
        else
        {
            buildVB_Anim_CornerSquare( _vertexTempList, _indexTempList, _corner );
        }

        // add last vertex to create last triangle
        VertexPNC3T& vtx = _vertexTempList[*_corner.m_indexVtxCorner];
        setVtxPos( _corner.m_stopPos, vtx.m_pos, _corner.m_zVtxCorner);
        vtx.setUv( *_corner.m_uvCorner);
        vtx.setColor(_corner.m_colorCorner);
        copyVtxAnim(vtx, _vertexTempList[indexVtxAnimToCopy]);

        addIndex_Anim( _vertexTempList, _indexTempList, *_corner.m_indexVtxCorner );
    }

    void Frise::buildVB_Anim_EdgeRunCorner( FriseConfig* config, ITF_VECTOR<edgeFrieze>& _edgeList, const edgeRun& _edgeRun, MeshAnim_VertexList& _vertexTempList, MeshAnim_IndexList& _indexTempList, u32 _vtxColorUp, u32 _vtxColorDown, u16& _indexVtxUp, u16& _indexVtxDown, Vec2d& _uvUp, Vec2d& _uvDown, f32& _uvUpXOffSet, f32& _uvDownXOffSet, f32 _uvXsign ) const
    {
        u32 edgeCount = 1;

        CornerRounder corner;
        corner.m_edgeCur = NULL;
        corner.m_edgeLast = &_edgeList[_edgeRun.m_idEdgeStart];
        corner.m_config = config;
        corner.m_edgeRun = &_edgeRun;    
        corner.m_indexVtxDown = &_indexVtxDown;
        corner.m_indexVtxUp = &_indexVtxUp;
        corner.m_uvXsign = &_uvXsign;
        corner.m_uvDown = &_uvDown;
        corner.m_uvUp = &_uvUp;
        corner.m_uvDownXOffSet = &_uvDownXOffSet;
        corner.m_uvUpXOffSet = &_uvUpXOffSet;
        corner.m_edgeCountCur = &edgeCount;

        //  edgeRun's corners
        for (; edgeCount < _edgeRun.m_edgeCount; edgeCount++ )
        {
            u32 idEdge = ( _edgeRun.m_idEdgeStart + edgeCount ) % m_recomputeData.m_edgeListCount;

            corner.m_edgeCur = &_edgeList[idEdge];

            if ( isEdgeWithHoleVisual(*corner.m_edgeLast))
            {
                m_recomputeData.m_edgeRunStartBuildVB = bfalse;

                // update uv 
                updateUvs( _edgeRun, corner.m_edgeLast, _uvUp, _uvDown, _uvXsign, _uvUpXOffSet, _uvDownXOffSet );

                corner.m_edgeLast = corner.m_edgeCur;
                continue;
            }

            // build quad edge last
            if ( !corner.m_edgeLast->m_snap )
            {       
                if ( !m_recomputeData.m_edgeRunStartBuildVB )
                {
                    buildVB_Anim_addVtxOnStartPosEdge( config->m_vtxAnim, *corner.m_edgeLast, _vertexTempList, _vtxColorUp, _vtxColorDown, _indexVtxUp, _indexVtxDown, _uvUp, _uvDown );
                }

                // update quad uv 
                updateUvs( _edgeRun, corner.m_edgeLast, _uvUp, _uvDown, _uvXsign, _uvUpXOffSet, _uvDownXOffSet );

                VertexPNC3T& vtxDown = _vertexTempList.incrSize(2);                
                VertexPNC3T& vtxUp = _vertexTempList[_vertexTempList.size() -1];

                setVtxBorderRightPosDown( *corner.m_edgeLast, vtxDown.m_pos);
                vtxDown.setUv( _uvDown);
                vtxDown.setColor(_vtxColorDown);
                setVtxAnimNull(vtxDown);

                setVtxBorderRightPosUp( *corner.m_edgeLast, vtxUp.m_pos);
                vtxUp.setUv( _uvUp);
                vtxUp.setColor(_vtxColorUp);
                setVtxAnimOnCorner( config->m_vtxAnim, *corner.m_edgeCur, vtxUp);

                addIndex_Anim_Quad( _vertexTempList, _indexTempList, _indexVtxDown, _indexVtxUp, corner.m_edgeCur->m_cornerAngle <0.f );

                // reset uvXoffset
                _uvUpXOffSet = 0.f;
                _uvDownXOffSet = 0.f;
            } // end build quad edge last

            //  build corner's edge cur
            if ( isEdgeWithHoleVisual(*corner.m_edgeCur))
            {
                m_recomputeData.m_edgeRunStartBuildVB = bfalse;
                corner.m_edgeLast = corner.m_edgeCur;
                continue;
            }

            if ( !m_recomputeData.m_edgeRunStartBuildVB)
            {
                buildVB_Anim_addVtxOnStopPosEdge( config->m_vtxAnim, *corner.m_edgeLast, _vertexTempList, _vtxColorUp, _vtxColorDown, _indexVtxUp, _indexVtxDown, _uvUp, _uvDown );
            }

            buildVB_Anim_Corner( _vertexTempList, _indexTempList, corner);

            corner.m_edgeLast = corner.m_edgeCur;
        }

        if ( isEdgeWithHoleVisual(*corner.m_edgeLast))
        {
            updateUvs( _edgeRun, corner.m_edgeLast, _uvUp, _uvDown, _uvXsign, _uvUpXOffSet, _uvDownXOffSet );
        }
    }

    void Frise::buildVB_Anim_addVtxOnStartPosEdge( VertexAnim& _anim, const edgeFrieze& edge, MeshAnim_VertexList& _vertexTempList, u32 _vtxColorUp, u32 _vtxColorDown, u16& _indexVtxUp, u16& _indexVtxDown, Vec2d& _uvUp, Vec2d& _uvDown ) const
    {
        m_recomputeData.m_edgeRunStartBuildVB = btrue;

        _indexVtxDown = (u16)_vertexTempList.size();
        _indexVtxUp = _indexVtxDown +1;
        VertexPNC3T& vtxDown = _vertexTempList.incrSize(2);            
        VertexPNC3T& vtxUp = _vertexTempList[_indexVtxUp];

        setVtxBorderLeftPosDown( edge, vtxDown.m_pos);
        vtxDown.setUv( _uvDown);
        vtxDown.setColor( _vtxColorDown);
        setVtxAnimNull(vtxDown);

        setVtxBorderLeftPosUp( edge, vtxUp.m_pos);
        vtxUp.setUv( _uvUp);
        vtxUp.setColor(_vtxColorUp);
        setVtxAnimOnEdge(_anim, edge, vtxUp);
    }

    void Frise::buildVB_Anim_addVtxOnStopPosEdge( VertexAnim& _anim, const edgeFrieze& edge, MeshAnim_VertexList& _vertexTempList, u32 _vtxColorUp, u32 _vtxColorDown, u16& _indexVtxUp, u16& _indexVtxDown, Vec2d& _uvUp, Vec2d& _uvDown ) const
    {
        m_recomputeData.m_edgeRunStartBuildVB = btrue;

        _indexVtxDown = (u16)_vertexTempList.size();
        _indexVtxUp = _indexVtxDown +1;
        VertexPNC3T& vtxDown = _vertexTempList.incrSize(2);            
        VertexPNC3T& vtxUp = _vertexTempList[_indexVtxUp];

        setVtxBorderRightPosDown( edge, vtxDown.m_pos);
        vtxDown.setUv( _uvDown);
        vtxDown.setColor( _vtxColorDown);
        setVtxAnimNull(vtxDown);

        setVtxBorderRightPosUp( edge, vtxUp.m_pos);
        vtxUp.setUv( _uvUp);
        vtxUp.setColor( _vtxColorUp);
        setVtxAnimOnEdge(_anim, edge, vtxUp);
    }

    void Frise::buildVB_Anim_EdgeRun( FriseConfig* config, ITF_VECTOR<edgeFrieze>& _edgeList, const edgeRun& _edgeRun, MeshAnim_VertexList& _vertexTempList, MeshAnim_IndexList& _indexTempList, u32 _vtxColorUp, u32 _vtxColorDown, u32 _vtxColorUpExtremity, u32 _vtxColorDownExtremity, u16& _indexVtxUp, u16& _indexVtxDown, Vec2d& _uvUp, Vec2d& _uvDown, f32 _uvXsign ) const
    {
        edgeFrieze* edgeCur;        
        f32 uvUpXOffSet = 0.f;
        f32 uvDownXOffSet = 0.f;   
        m_recomputeData.m_edgeRunStartBuildVB = bfalse;

        //  start edge
        edgeCur = &_edgeList[_edgeRun.m_idEdgeStart];

        // left vertexs'edge start if it's not snaped
        if ( !isEdgeWithHoleVisual(*edgeCur))
        {
            if ( !edgeCur->m_snap )
            {    
                bbool build = bfalse;

                // hole visual
                if ( _edgeRun.m_idEdgeStart > 0 || m_pointsList.isLooping())
                {
                    u32 idEgdeLast = ( _edgeRun.m_idEdgeStart -1 +m_recomputeData.m_edgeListCount) % m_recomputeData.m_edgeListCount;
                    edgeFrieze* edgeLast = &_edgeList[idEgdeLast];

                    if ( isEdgeWithHoleVisual(*edgeLast) )
                    {
                        m_recomputeData.m_edgeRunStartBuildVB = btrue;
                        build = btrue;

                        _indexVtxDown = (u16)_vertexTempList.size();
                        _indexVtxUp = _indexVtxDown +1;
                        VertexPNC3T& vtxDown = _vertexTempList.incrSize(2);            
                        VertexPNC3T& vtxUp = _vertexTempList[_indexVtxUp];

                        Vec2d pos = edgeCur->m_points[0] -edgeCur->m_sightNormalized *config->m_visualOffset *m_recomputeData.m_heightScale;
                        setVtxPosDown( pos, vtxDown.m_pos);
                        vtxDown.setUv( _uvDown);
                        vtxDown.setColor( _vtxColorDown);
                        setVtxAnimNull(vtxDown);

                        pos +=  getEdgeBorderLeft(*edgeCur);
                        setVtxPosUp( pos, vtxUp.m_pos);
                        vtxUp.setUv( _uvUp);
                        vtxUp.setColor( _vtxColorUp);
                        setVtxAnimOnEdge( config->m_vtxAnim, *edgeCur, vtxUp);
                    }
                }

                if ( !build )
                    buildVB_Anim_addVtxOnStartPosEdge( config->m_vtxAnim, *edgeCur, _vertexTempList, _vtxColorUpExtremity, _vtxColorDownExtremity, _indexVtxUp, _indexVtxDown, _uvUp, _uvDown );
            }
        }

        //  corner rounder buid way
        buildVB_Anim_EdgeRunCorner( config, _edgeList, _edgeRun, _vertexTempList, _indexTempList, _vtxColorUp, _vtxColorDown, _indexVtxUp, _indexVtxDown, _uvUp, _uvDown, uvUpXOffSet, uvDownXOffSet, _uvXsign );

        // build quad edge last
        u32 idEdgeStop = ( _edgeRun.m_idEdgeStart + _edgeRun.m_edgeCount +m_recomputeData.m_edgeListCount -1) % m_recomputeData.m_edgeListCount;
        edgeFrieze* edgeStop = &_edgeList[idEdgeStop];

        if ( !edgeStop->m_snap && !isEdgeWithHoleVisual(*edgeStop))
        {
            if ( !m_recomputeData.m_edgeRunStartBuildVB)
            {
                buildVB_Anim_addVtxOnStartPosEdge( config->m_vtxAnim, *edgeStop, _vertexTempList, _vtxColorUp, _vtxColorDown, _indexVtxUp, _indexVtxDown, _uvUp, _uvDown  );
            }

            Vec2d holeOffset(0.f,0.f);
            if ( idEdgeStop < m_recomputeData.m_edgeListCount -1 || m_pointsList.isLooping() )
            {
                u32 idEdgeNext = ( idEdgeStop +1) % m_recomputeData.m_edgeListCount;
                if ( isEdgeWithHoleVisual(_edgeList[idEdgeNext]))
                {
                    holeOffset = edgeStop->m_sightNormalized * config->m_visualOffset *m_recomputeData.m_heightScale;
                }
            }
            VertexPNC3T& vtxDown = _vertexTempList.incrSize(2);            
            VertexPNC3T& vtxUp = _vertexTempList[_vertexTempList.size()-1];

            // update uv 
            f32 uvXQuadOffSet = edgeStop->m_normUv * _edgeRun.m_coeff *_uvXsign;
            _uvUp.m_x += uvXQuadOffSet + uvUpXOffSet;
            _uvDown.m_x += uvXQuadOffSet + uvDownXOffSet;

            Vec2d pos = edgeStop->m_points[2] + holeOffset;
            setVtxPosDown( pos, vtxDown.m_pos);
            vtxDown.setUv( _uvDown);
            vtxDown.setColor(_vtxColorDownExtremity); 
            setVtxAnimNull(vtxDown);

            pos += getEdgeBorderRight( *edgeStop);
            setVtxPosUp( pos, vtxUp.m_pos);
            vtxUp.setUv( _uvUp);
            vtxUp.setColor(_vtxColorUpExtremity);
            setVtxAnimOnEdge( config->m_vtxAnim, *edgeStop, vtxUp);

            addIndex_Anim_Quad( _vertexTempList, _indexTempList, _indexVtxDown, _indexVtxUp, flipEdge(uvUpXOffSet-uvDownXOffSet) );
        }
    }

    void Frise::addIndex_Anim_Quad( MeshAnim_VertexList& _vertexTempList, MeshAnim_IndexList& _indexTempList, u16& _indexVtxDown, u16& _indexVtxUp, bbool _flip ) const
    {
        /*
        Flip     !Flip  
        1__3     1__3              
        |\ |     | /|   
        |_\|     |/_|   
        0  2     0  2   
        */

        u16 _vertexCount = (u16)_vertexTempList.size();

        addIndex_Anim(_vertexTempList, _indexTempList, _indexVtxDown );
        addIndex_Anim(_vertexTempList, _indexTempList, _indexVtxUp );

        setFlipQuadWithAlphaBorder( _flip, _vertexTempList[_indexVtxDown].m_color, _vertexTempList[_vertexCount -2].m_color);

        if ( _flip )
        {
            _indexVtxDown = _vertexCount -2;
            addIndex_Anim(_vertexTempList, _indexTempList, _indexVtxDown );
            addIndex_Anim(_vertexTempList, _indexTempList, _indexVtxUp );

            _indexVtxUp = _vertexCount -1;
            addIndex_Anim(_vertexTempList, _indexTempList, _indexVtxUp );
            addIndex_Anim(_vertexTempList, _indexTempList, _indexVtxDown );
        }
        else
        {
            _indexVtxUp = _vertexCount -1;
            addIndex_Anim(_vertexTempList, _indexTempList, _indexVtxUp );
            addIndex_Anim(_vertexTempList, _indexTempList, _indexVtxDown );
            addIndex_Anim(_vertexTempList, _indexTempList, _indexVtxUp );

            _indexVtxDown = _vertexCount -2;
            addIndex_Anim(_vertexTempList, _indexTempList, _indexVtxDown );
        }
    }

    void Frise::buildVB_Anim_Quad( VertexAnim& _anim, MeshAnim_VertexList& _vertexTempList, MeshAnim_IndexList& _indexTempList, Vec2d* _pos, Vec2d* _uv, u32* _color, bbool _flip, bbool _vtxAnimStart, bbool _vtxAnimStop ) const
    {
        /*
        v1...v3
        .    .
        .    .
        v0...v2
        */

        Vec2d vector = _pos[2] -_pos[0];
        vector.normalize();

        u16 indexVtxDown = (u16)_vertexTempList.size();
        u16 indexVtxUp = indexVtxDown +1;
        _vertexTempList.incrSize(4); 

        VertexPNC3T& vtx0 = _vertexTempList[ indexVtxDown ];
        setVtxPosDown( _pos[0], vtx0.m_pos);
        vtx0.setColor(_color[0]);
        vtx0.setUv( _uv[0]);   
        setVtxAnimNull(vtx0);

        VertexPNC3T& vtx1 = _vertexTempList[ indexVtxUp ];
        setVtxPosUp( _pos[1], vtx1.m_pos);
        vtx1.setColor(_color[1]);
        vtx1.setUv( _uv[1]);
        if ( _vtxAnimStart )
            setVtxAnimOnEdge( _anim, vector, vtx1);
        else
            setVtxAnimNull( vtx1);

        VertexPNC3T& vtx2 = _vertexTempList[ indexVtxDown +2];
        setVtxPosDown( _pos[2], vtx2.m_pos);
        vtx2.setColor(_color[2]);
        vtx2.setUv( _uv[2]);
        setVtxAnimNull(vtx2);

        VertexPNC3T& vtx3 = _vertexTempList[ indexVtxUp +2];
        setVtxPosUp( _pos[3], vtx3.m_pos);
        vtx3.setColor(_color[3]);
        vtx3.setUv( _uv[3]);
        if ( _vtxAnimStop )
            setVtxAnimOnEdge( _anim, vector, vtx3);
        else
            setVtxAnimNull( vtx3);

        addIndex_Anim_Quad( _vertexTempList, _indexTempList, indexVtxDown, indexVtxUp, _flip );
    }

} // namespace ITF
