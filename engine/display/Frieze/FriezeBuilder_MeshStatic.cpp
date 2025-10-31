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

#include "core/types.h"

namespace ITF
{

    void Frise::clearMeshStaticData()
    {        
        if ( !m_pMeshStaticData )
            return;

        if ( m_pMeshStaticData->m_mesh.m_vtxBuffer )
        {
            GFX_ADAPTER->removePrimitiveFromRenderLists(GFX_ITF_MESH, (void*)&m_pMeshStaticData->m_mesh);
            FreeVB( m_pMeshStaticData->m_mesh.m_vtxBuffer);
        }

        for (u32 i = 0; i < m_pMeshStaticData->m_mesh.m_ElementList.size(); i++)
        {
            if( m_pMeshStaticData->m_mesh.m_ElementList[i].m_indexBuffer) 
                FreeIB( m_pMeshStaticData->m_mesh.m_ElementList[i].m_indexBuffer);
        }

        SF_DEL(m_pMeshStaticData);
    }

    void Frise::buildMesh_Static( const MeshStatic_VertexList& _vertexTempList, const MeshStatic_IndexList* _indexTempList, const Matrix44* _pMatrixTransfo )
    {
        if ( _vertexTempList.size() >= 3 )
        {
            const FriseConfig* config = getConfig();
            if ( !config )
                return;

            m_pMeshStaticData = newAlloc(mId_Frieze, MeshStaticData);

            // set mesh aabb
            for(u32 indexTempListIndex = 0; indexTempListIndex < m_recomputeData.m_MeshElementTable_ElemCount[MeshElementRouter::MeshElementTable_Static]; ++indexTempListIndex)
            {
                for(u32 iIndex = 0; iIndex < _indexTempList[indexTempListIndex].size(); ++iIndex)
                {
                    const Vec3d& pos = _vertexTempList[_indexTempList[indexTempListIndex][iIndex]].m_pos;

                    m_pMeshStaticData->m_AABB.grow(pos);

#ifdef ITF_WINDOWS
                    EditorWireframeMesh.push_back( pos );
#endif // ITF_WINDOWS
                }
            }


            if ( _pMatrixTransfo )
                m_pMeshStaticData->m_mesh.setMatrix(*_pMatrixTransfo);

            const u32 vtxCount = _vertexTempList.size();

            // copy vertexTempList
            m_pMeshStaticData->m_mesh.m_vtxBuffer = GFX_ADAPTER->createVertexBuffer
                (
                vtxCount,
                VertexFormat_PCT,
                sizeof(VertexPCT),
                bfalse,
                VB_T_FRIEZESTATIC
                );

            VertexPCT    *pdata = 0;
            GFX_ADAPTER->getVertexBufferManager().LockVertexBuffer(m_pMeshStaticData->m_mesh.m_vtxBuffer,(void **) &pdata);
            ITF_MemcpyWriteCombined(pdata, _vertexTempList.getDataBuffer(), sizeof(VertexPCT) *vtxCount);
            GFX_ADAPTER->getVertexBufferManager().UnlockVertexBuffer(m_pMeshStaticData->m_mesh.m_vtxBuffer);

            FRIEZESTAT_ADD_VB(m_pMeshStaticData->m_mesh.m_vtxBuffer, getRef(), m_pMeshStaticData->m_mesh.m_vtxBuffer->m_size );

            // copy all element       
            i32 textureConfigCount = (i32)config->m_textureConfigs.size();
            for ( i32 i = 0; i < textureConfigCount ; i++ )
            {
                u8 routingFlags = MeshElementRouter::MeshElementTable_FlagDefault;
                if ( i == config->m_fillFriseTextureConfigIndex )
                {
                    routingFlags = MeshElementRouter::MeshElementTable_FlagFill;
                }

                const MeshElementRouter* elemRouter = getRoutingElement(routingFlags, i);
                if (!elemRouter)
                    continue;

                const MeshStatic_IndexList& indexTable = _indexTempList[elemRouter->m_indexInTable];

                u32 indexTableSize = indexTable.size();
                if ( indexTableSize == 0 )
                    continue;

                m_pMeshStaticData->m_mesh.addElement();

                ITF_MeshElement& elem = m_pMeshStaticData->m_mesh.m_ElementList[m_pMeshStaticData->m_mesh.m_ElementList.size()-1];
                setMeshElement( elem, config, indexTableSize, (u32)i );

                u16* pindexelem;
                elem.m_indexBuffer->Lock((void**)&pindexelem);
                ITF_MemcpyWriteCombined(pindexelem ,&indexTable[0], sizeof(u16) * indexTableSize );
                elem.m_indexBuffer->Unlock();

                m_pMeshStaticData->m_elemConfig_IndexList.push_back(i);
            }
        }
    }

    void Frise::buildVB_Static_CornerRounderBlending( MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList& _indexTempList, CornerRounder& _corner ) const
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

            VertexPCT& vtxCorner = _vertexTempList[*_corner.m_indexVtxCorner];
            setVtxPos( pos, vtxCorner.m_pos, _corner.m_zVtxCorner);
            vtxCorner.m_uv = uvCur;
            vtxCorner.setColor( _corner.m_colorCorner);

            //  add vertex to create a triangle
            _indexTempList.push_back(*_corner.m_indexVtxCorner);

            // add two index for next triangle
            _indexTempList.push_back(*_corner.m_indexVtxCenter);
            _indexTempList.push_back(*_corner.m_indexVtxCorner);

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

    void Frise::buildVB_Static_CornerRounder( MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList& _indexTempList, CornerRounder& _corner ) const
    {
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

            VertexPCT& vtxCorner = _vertexTempList[*_corner.m_indexVtxCorner];
            setVtxPos( _corner.m_startPos, vtxCorner.m_pos, _corner.m_zVtxCorner);
            vtxCorner.m_uv = *_corner.m_uvCorner;
            vtxCorner.setColor( _corner.m_colorCorner);

            //  add vertex to create a triangle
            _indexTempList.push_back(*_corner.m_indexVtxCorner);

            // add two index for next triangle
            _indexTempList.push_back(*_corner.m_indexVtxCenter);
            _indexTempList.push_back(*_corner.m_indexVtxCorner);

            *_corner.m_indexVtxCorner += 1;
        }

        // update uv
        _corner.m_uvCorner->m_x += uvXStep;
    }

    void Frise::buildVB_Static_CornerSquare( MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList& _indexTempList, CornerRounder& _corner ) const
    {
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

        VertexPCT& vtxCorner = _vertexTempList[*_corner.m_indexVtxCorner];
        setVtxPos( _corner.m_cornerPos, vtxCorner.m_pos, _corner.m_zVtxCorner);
        vtxCorner.m_uv = *_corner.m_uvCorner;
        vtxCorner.setColor( _corner.m_colorCorner);

        //  add vertex to create a triangle
        _indexTempList.push_back(*_corner.m_indexVtxCorner);

        // add two index for next triangle
        _indexTempList.push_back(*_corner.m_indexVtxCenter);
        _indexTempList.push_back(*_corner.m_indexVtxCorner);

        *_corner.m_indexVtxCorner += 1;

        // update uv
        _corner.m_uvCorner->m_x += uvRightCorner;
    }

    void Frise::buildVB_Static_Corner( MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList& _indexTempList, CornerRounder& _corner ) const
    { 
        if ( _corner.m_edgeCur->m_cornerAngle == 0.f )
            return;

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
        }

        _indexTempList.push_back(*_corner.m_indexVtxCenter);
        _indexTempList.push_back(*_corner.m_indexVtxCorner);

        if ( _corner.m_config->m_smoothVisual )
        {
            if ( isHeightEqual( *_corner.m_edgeLast, *_corner.m_edgeCur))
            {
                buildVB_Static_CornerRounder( _vertexTempList, _indexTempList, _corner );
            }
            else
            {
                buildVB_Static_CornerRounderBlending( _vertexTempList, _indexTempList, _corner );
            }
        }
        else
        {
            buildVB_Static_CornerSquare( _vertexTempList, _indexTempList, _corner );
        }

        // add last vertex to create last triangle
        VertexPCT& vtx = _vertexTempList[*_corner.m_indexVtxCorner];
        setVtxPos( _corner.m_stopPos, vtx.m_pos, _corner.m_zVtxCorner);
        vtx.m_uv = *_corner.m_uvCorner;
        vtx.setColor(_corner.m_colorCorner);

        _indexTempList.push_back(*_corner.m_indexVtxCorner);
    }

    void Frise::buildVB_Static_EdgeRunCorner( FriseConfig* config, ITF_VECTOR<edgeFrieze>& _edgeList, const edgeRun& _edgeRun, MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList& _indexTempList, u32 _vtxColorUp, u32 _vtxColorDown, u16& _indexVtxUp, u16& _indexVtxDown, Vec2d& _uvUp, Vec2d& _uvDown, f32& _uvUpXOffSet, f32& _uvDownXOffSet, f32 _uvXsign ) const
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
                    buildVB_Static_addVtxOnStartPosEdge( config, *corner.m_edgeLast, _vertexTempList, _vtxColorUp, _vtxColorDown, _indexVtxUp, _indexVtxDown, _uvUp, _uvDown );
                }

                // update quad uv 
                updateUvs( _edgeRun, corner.m_edgeLast, _uvUp, _uvDown, _uvXsign, _uvUpXOffSet, _uvDownXOffSet );

                VertexPCT& vtxDown = _vertexTempList.incrSize(2);                
                VertexPCT& vtxUp = _vertexTempList[_vertexTempList.size() -1];

                if ( isEdgeWithHoleVisual(*corner.m_edgeCur))
                {
                    Vec2d posDown, posUp;
                    setPosStopOnEdgeWithHoleVisual( config, *corner.m_edgeLast, posDown, posUp, corner.m_edgeCur->m_idPoint);
                    setVtxPosDown( posDown, vtxDown.m_pos);
                    setVtxPosUp( posUp, vtxUp.m_pos);
                }
                else
                {
                    setVtxBorderRightPosDown( *corner.m_edgeLast, vtxDown.m_pos);
                    setVtxBorderRightPosUp( *corner.m_edgeLast, vtxUp.m_pos);
                }
                
                vtxDown.m_uv = _uvDown;
                vtxDown.setColor(_vtxColorDown);
                
                vtxUp.m_uv = _uvUp;
                vtxUp.setColor(_vtxColorUp);

                addIndex_Static_Quad( _vertexTempList, _indexTempList, _indexVtxDown, _indexVtxUp, corner.m_edgeCur->m_cornerAngle <0.f/*flipEdge(_uvDownXOffSet-_uvUpXOffSet)*/ );

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
                buildVB_Static_addVtxOnStopPosEdge( *corner.m_edgeLast, _vertexTempList, _vtxColorUp, _vtxColorDown, _indexVtxUp, _indexVtxDown, _uvUp, _uvDown );
            }

            buildVB_Static_Corner( _vertexTempList, _indexTempList, corner);

            corner.m_edgeLast = corner.m_edgeCur;
        }

        if ( isEdgeWithHoleVisual(*corner.m_edgeLast))
        {
            updateUvs( _edgeRun, corner.m_edgeLast, _uvUp, _uvDown, _uvXsign, _uvUpXOffSet, _uvDownXOffSet );
        }
    }

    void Frise::buildVB_Static_addVtxOnStartPosEdge( const FriseConfig* _config, const edgeFrieze& _edge, MeshStatic_VertexList& _vertexTempList, u32 _vtxColorUp, u32 _vtxColorDown, u16& _indexVtxUp, u16& _indexVtxDown, Vec2d& _uvUp, Vec2d& _uvDown, edgeFrieze* _edgeLast ) const
    {
        m_recomputeData.m_edgeRunStartBuildVB = btrue;

        _indexVtxDown = (u16)_vertexTempList.size();
        _indexVtxUp = _indexVtxDown +1;
        VertexPCT& vtxDown = _vertexTempList.incrSize(2);            
        VertexPCT& vtxUp = _vertexTempList[_indexVtxUp];

        if ( _edgeLast && isEdgeWithHoleVisual(*_edgeLast) )
        {
            Vec2d posDown, posUp;
            setPosStartOnEdgeWithHoleVisual( _config, _edge, posDown, posUp);

            setVtxPosDown( posDown, vtxDown.m_pos);
            setVtxPosUp( posUp, vtxUp.m_pos);
        }
        else
        {
            setVtxBorderLeftPosDown( _edge, vtxDown.m_pos);
            setVtxBorderLeftPosUp( _edge, vtxUp.m_pos);
        }
        
        vtxDown.m_uv = _uvDown;
        vtxDown.setColor( _vtxColorDown);
        
        vtxUp.m_uv = _uvUp;
        vtxUp.setColor(_vtxColorUp);
    }

    void Frise::buildVB_Static_addVtxOnStopPosEdge( const edgeFrieze& edge, MeshStatic_VertexList& _vertexTempList, u32 _vtxColorUp, u32 _vtxColorDown, u16& _indexVtxUp, u16& _indexVtxDown, Vec2d& _uvUp, Vec2d& _uvDown ) const
    {
        m_recomputeData.m_edgeRunStartBuildVB = btrue;

        _indexVtxDown = (u16)_vertexTempList.size();
        _indexVtxUp = _indexVtxDown +1;
        VertexPCT& vtxDown = _vertexTempList.incrSize(2);            
        VertexPCT& vtxUp = _vertexTempList[_indexVtxUp];

        setVtxBorderRightPosDown( edge, vtxDown.m_pos);
        vtxDown.m_uv = _uvDown;
        vtxDown.setColor( _vtxColorDown);

        setVtxBorderRightPosUp( edge, vtxUp.m_pos);
        vtxUp.m_uv = _uvUp;
        vtxUp.setColor( _vtxColorUp);
    }

    void Frise::buildVB_Static_EdgeRun( FriseConfig* config, ITF_VECTOR<edgeFrieze>& _edgeList, const edgeRun& _edgeRun, MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList& _indexTempList, u32 _vtxColorUp, u32 _vtxColorDown, u32 _vtxColorUpExtremity, u32 _vtxColorDownExtremity, u16& _indexVtxUp, u16& _indexVtxDown, Vec2d& _uvUp, Vec2d& _uvDown, f32 _uvXsign ) const
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
                        VertexPCT& vtxDown = _vertexTempList.incrSize(2);            
                        VertexPCT& vtxUp = _vertexTempList[_indexVtxUp];
                        
                        Vec2d posDown, posUp;
                        setPosStartOnEdgeWithHoleVisual( config, *edgeCur, posDown, posUp);
                        
                        setVtxPosDown( posDown, vtxDown.m_pos);
                        vtxDown.m_uv = _uvDown;
                        vtxDown.setColor( _vtxColorDown);

                        setVtxPosUp( posUp, vtxUp.m_pos);
                        vtxUp.m_uv = _uvUp;
                        vtxUp.setColor( _vtxColorUp);
                    }
                }

                if ( !build )
                    buildVB_Static_addVtxOnStartPosEdge( config, *edgeCur, _vertexTempList, _vtxColorUpExtremity, _vtxColorDownExtremity, _indexVtxUp, _indexVtxDown, _uvUp, _uvDown );
            }
        }

        //  corner rounder buid way
        buildVB_Static_EdgeRunCorner( config, _edgeList, _edgeRun, _vertexTempList, _indexTempList, _vtxColorUp, _vtxColorDown, _indexVtxUp, _indexVtxDown, _uvUp, _uvDown, uvUpXOffSet, uvDownXOffSet, _uvXsign );

        // build quad edge last
        const u32 idEdgeStop = ( _edgeRun.m_idEdgeStart + _edgeRun.m_edgeCount +m_recomputeData.m_edgeListCount -1) % m_recomputeData.m_edgeListCount;
        const edgeFrieze* edgeStop = &_edgeList[idEdgeStop];

        if ( !edgeStop->m_snap && !isEdgeWithHoleVisual(*edgeStop))
        {
            if ( !m_recomputeData.m_edgeRunStartBuildVB)
            {
                edgeFrieze* edgeLast = NULL;

                if ( idEdgeStop >0 || m_pointsList.isLooping() )
                {
                    u32 idEdgeLast = ( idEdgeStop +m_recomputeData.m_edgeListCount -1) % m_recomputeData.m_edgeListCount;
                    edgeLast = &_edgeList[idEdgeLast];
                }

                buildVB_Static_addVtxOnStartPosEdge( config, *edgeStop, _vertexTempList, _vtxColorUp, _vtxColorDown, _indexVtxUp, _indexVtxDown, _uvUp, _uvDown, edgeLast );
            }

            Vec2d posDown(edgeStop->m_points[2]);
            Vec2d posUp = posDown +getEdgeBorderRight( *edgeStop);
            
            if ( idEdgeStop < m_recomputeData.m_edgeListCount -1 || m_pointsList.isLooping() )
            {
                u32 idEdgeNext = ( idEdgeStop +1) % m_recomputeData.m_edgeListCount;
                const edgeFrieze& edgeNext = _edgeList[idEdgeNext];
                if ( isEdgeWithHoleVisual(edgeNext))
                {
                    setPosStopOnEdgeWithHoleVisual( config, *edgeStop, posDown, posUp, edgeNext.m_idPoint);
                }
            }
            VertexPCT& vtxDown = _vertexTempList.incrSize(2);            
            VertexPCT& vtxUp = _vertexTempList[_vertexTempList.size()-1];

            // update uv 
            f32 uvXQuadOffSet = edgeStop->m_normUv * _edgeRun.m_coeff *_uvXsign;
            _uvUp.m_x += uvXQuadOffSet + uvUpXOffSet;
            _uvDown.m_x += uvXQuadOffSet + uvDownXOffSet;

            setVtxPosDown( posDown, vtxDown.m_pos);
            vtxDown.m_uv = _uvDown;
            vtxDown.setColor(_vtxColorDownExtremity); 

            setVtxPosUp( posUp, vtxUp.m_pos);
            vtxUp.m_uv = _uvUp;
            vtxUp.setColor(_vtxColorUpExtremity);

            addIndex_Static_Quad( _vertexTempList, _indexTempList, _indexVtxDown, _indexVtxUp, flipEdge(uvUpXOffSet-uvDownXOffSet) );
        }
    }

    void Frise::addIndex_Static_Quad( MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList& _indexTempList, u16& _indexVtxDown, u16& _indexVtxUp, bbool _flip ) const
    {
        /*
        Flip     !Flip  
        1__3     1__3              
        |\ |     | /|   
        |_\|     |/_|   
        0  2     0  2   
        */

        u16 _vertexCount = (u16)_vertexTempList.size();

        _indexTempList.push_back(_indexVtxDown);
        _indexTempList.push_back(_indexVtxUp);
        
        setFlipQuadWithAlphaBorder( _flip, _vertexTempList[_indexVtxDown].m_color, _vertexTempList[_vertexCount -2].m_color);

        if ( _flip )
        {
            _indexVtxDown = _vertexCount -2;
            _indexTempList.push_back(_indexVtxDown);
            _indexTempList.push_back(_indexVtxUp);

            _indexVtxUp = _vertexCount -1;
            _indexTempList.push_back(_indexVtxUp);
            _indexTempList.push_back(_indexVtxDown);
        }
        else
        {
            _indexVtxUp = _vertexCount -1;
            _indexTempList.push_back(_indexVtxUp);
            _indexTempList.push_back(_indexVtxDown);
            _indexTempList.push_back(_indexVtxUp);

            _indexVtxDown = _vertexCount -2;
            _indexTempList.push_back(_indexVtxDown);
        }
    }

    void Frise::buildVB_Static_Quad( MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList& _indexTempList, Vec2d* _pos, Vec2d* _uv, u32* _color, bbool _flip ) const
    {
        /*
        v1...v3
        .    .
        .    .
        v0...v2
        */

        u16 indexVtxDown = (u16)_vertexTempList.size();
        u16 indexVtxUp = indexVtxDown +1;
        _vertexTempList.incrSize(4); 

        VertexPCT& vtx0 = _vertexTempList[ indexVtxDown ];
        setVtxPosDown( _pos[0], vtx0.m_pos);
        vtx0.setColor(_color[0]);
        vtx0.m_uv = _uv[0];            

        VertexPCT& vtx1 = _vertexTempList[ indexVtxUp ];
        setVtxPosUp( _pos[1], vtx1.m_pos);
        vtx1.setColor(_color[1]);
        vtx1.m_uv = _uv[1];

        VertexPCT& vtx2 = _vertexTempList[ indexVtxDown +2];
        setVtxPosDown( _pos[2], vtx2.m_pos);
        vtx2.setColor(_color[2]);
        vtx2.m_uv = _uv[2];

        VertexPCT& vtx3 = _vertexTempList[ indexVtxUp +2];
        setVtxPosUp( _pos[3], vtx3.m_pos);
        vtx3.setColor(_color[3]);
        vtx3.m_uv = _uv[3];

        addIndex_Static_Quad( _vertexTempList, _indexTempList, indexVtxDown, indexVtxUp, _flip );
    }

} // namespace ITF
