#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

#ifndef _ITF_NGON2D_H_
#include "Core/math/Ngon2d.h"
#endif // _ITF_NGON2D_H_

namespace ITF
{
    static f32 edgeFill_sqrNormMin = 0.0001f;
    static f32 edgeFill_distMaxToSkipParallelEdge = 0.01f; 

    bbool Frise::buildFilling( const ITF_VECTOR<edgeFrieze>& _edgeList, const ITF_VECTOR<edgeRun>& _edgeRunList, MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList* _indexTempList ) const
    {
        FriseConfig* config = getConfig();

        if ( !config || config->m_fillFriseTextureConfigIndex == -1 )
            return bfalse;

        if ( _edgeRunList.size() == 0 )
            return bfalse;

        SafeArray<Vec2d> filling;
        buildFilling_Points( config, filling, _edgeList, _edgeRunList );

        const MeshElementRouter* elemRouter = getRoutingElement(MeshElementRouter::MeshElementTable_FlagFill, config->m_fillFriseTextureConfigIndex);
        ITF_ASSERT(elemRouter);
        ITF_ASSERT(elemRouter->m_tableID == MeshElementRouter::MeshElementTable_Static);

        return buildFilling_VB( config, filling, _vertexTempList, _indexTempList[elemRouter->m_indexInTable] );          
    }    

    void Frise::setFillingSafe( const FriseConfig* _config, const SafeArray<Vec2d>& _fillingOld, SafeArray<Vec2d>& _fillingSafe ) const
    {
        Vec2d posLast(Vec2d::Infinity);
        Vec2d posLastOld(-Vec2d::Infinity);
        u32 fillingCount = _fillingOld.size();    
        u32 fillingSafeCount = 0;

        _fillingSafe.reserve(fillingCount);

        bbool checkParallelEdge = btrue;
        if ( _config->m_fillOffSet == 0.f )
            checkParallelEdge = bfalse; // to not have discontinuity between filling mesh and frieze mesh

        for ( u32 i = 0; i < fillingCount; i++ )
        {
            // skip small edge
            if ( ( _fillingOld[i] -posLast ).sqrnorm() < edgeFill_sqrNormMin )
                continue;

            if ( fillingSafeCount>1 && checkParallelEdge && isParallelEdges( _fillingOld[i], posLast, posLastOld, edgeFill_distMaxToSkipParallelEdge) )
            {
                _fillingSafe[fillingSafeCount -1] = _fillingOld[i];
                posLast = _fillingOld[i];
                continue;
            }

            _fillingSafe.push_back(_fillingOld[i]);
            fillingSafeCount++;
            posLastOld = posLast;
            posLast = _fillingOld[i];              
        }
    }

    void Frise::setFillParams( const FriseConfig* _config, FillingParams& _params ) const
    {
        f32_CosSin(_config->m_fillAngle.ToRadians(), &_params.m_cosAngle, &_params.m_sinAngle);
        _params.m_scaleX = _config->m_fillScale.m_x;
        _params.m_scaleY = _config->m_fillScale.m_y;

        if ( _config->m_useFriezeScale )
        {
            _params.m_scaleX *= f32_Abs(getScale().m_x);
            _params.m_scaleY *= f32_Abs(getScale().m_y);
        }

        if ( _params.m_scaleX != 0.f )
            _params.m_scaleX = 1.f /_params.m_scaleX;
        if ( _params.m_scaleY != 0.f )
            _params.m_scaleY = 1.f /_params.m_scaleY;
    }

    void Frise::setFillUv( FillingParams& _params, Vec2d& _uv, Vec2d& _pos) const
    {
        Vec2d pos = _pos;
        if (!m_isDynamicFrieze)
        {    // scroll UV with position only for non dynamic friezes
            pos.m_x += getPos().m_x;
            pos.m_y += getPos().m_y;
        }

        _uv.m_x = _params.m_cosAngle* pos.m_x - _params.m_sinAngle* pos.m_y;
        _uv.m_y = -_params.m_sinAngle* pos.m_x - _params.m_cosAngle* pos.m_y;
        // translate
        _uv.m_x += m_fillOffSetUv.m_x; 
        _uv.m_y -= m_fillOffSetUv.m_y;
        // scale
        _uv.m_x *= _params.m_scaleX;
        _uv.m_y *= _params.m_scaleY;
    }

    bbool Frise::buildFilling_VB( const FriseConfig* _config, SafeArray<Vec2d>& _filling, MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList& _indexTempList ) const
    {
        bbool filling = bfalse;

            SafeArray<Vec2d> fillingSafe;
            setFillingSafe( _config, _filling, fillingSafe );

            u32 fillingSafeCount = fillingSafe.size();
            if ( fillingSafeCount < 3 )
            return filling;

            // indexes
        u16* outputIndexes = newAlloc(mId_Frieze, u16[fillingSafeCount *4]);
        u32 outputIndexesCount = FillNgon( &fillingSafe[0], fillingSafeCount, outputIndexes) * 3;

            if (outputIndexesCount > 2 )
            {
                filling = btrue;

                // vertexs
                u32 indexVtxFillStart = _vertexTempList.size();
                _vertexTempList.incrSize( fillingSafeCount );

                FillingParams params;
                setFillParams( _config, params);

                for ( u32 i = 0; i < fillingSafeCount; i++ )
                {
                VertexPCT& vtx = _vertexTempList[ indexVtxFillStart +i ];
                    Vec2d& fill = fillingSafe[i];
                    vtx.m_pos.m_x = fill.m_x;
                    vtx.m_pos.m_y = fill.m_y;
                    vtx.m_pos.m_z = 0.f;
                    vtx.setColor(_config->m_textureConfigs[_config->m_fillFriseTextureConfigIndex].m_color.getAsU32());

                    // uvs
                    setFillUv( params, vtx.m_uv, fill );
            }

            _indexTempList.reserve(_indexTempList.capacity() + outputIndexesCount);

            for ( u32 i = 0; i < outputIndexesCount; i++ )
                _indexTempList.push_back(outputIndexes[i] +(u16)indexVtxFillStart);
        }

        delete[] outputIndexes;

        return filling;
    }

    void Frise::buildFilling_Points( const FriseConfig* config, SafeArray<Vec2d>& filling, const ITF_VECTOR<edgeFrieze>& _edgeList, const ITF_VECTOR<edgeRun>& _edgeRunList ) const
    {
        if ( config->m_smoothFilling )
        filling.reserve( m_recomputeData.m_edgeListCount * ( 2 + (u32)config->m_smoothFactorFilling ));
        else
            filling.reserve( m_recomputeData.m_edgeListCount *3 +3 );

        const edgeRun* edgeRunLast = &_edgeRunList[_edgeRunList.size() -1];
        f32 offsetLast = -1.f;

        if ( m_pointsList.isLooping() )
        {
            offsetLast = getFillOffSet( config, edgeRunLast);  
        }
        else
        {   
            // last point
            u32 idEdgeLast = ( edgeRunLast->m_idEdgeStart + edgeRunLast->m_edgeCount +m_recomputeData.m_edgeListCount -1 ) % m_recomputeData.m_edgeListCount;
            const edgeFrieze* edgeLast = &_edgeList[idEdgeLast ];

            if ( isEdgeWithHoleVisual(*edgeLast) )
            {
                filling.push_back( getPosAt(edgeLast->m_idPoint+1));
            }
            else
            {
                filling.push_back( getPosStopOnNormalEdge( edgeLast, getFillOffSet( config, edgeRunLast)));
            }

            
            edgeRunLast = NULL;

            // first point
            const edgeRun* edgeRunCur = &_edgeRunList[0];               
            const edgeFrieze* edgeStart = &_edgeList[edgeRunCur->m_idEdgeStart];

            if ( isEdgeWithHoleVisual(*edgeStart))
            {
                filling.push_back( getPosAt(edgeStart->m_idPoint));
            }
            else
            {
                filling.push_back( getPosStartOnNormalEdge( edgeStart, getFillOffSet( config, edgeRunCur)));
            }       
        }

        const u32 edgeRunListCount = _edgeRunList.size();
        for( u32 idEdgeRun = 0; idEdgeRun < edgeRunListCount; idEdgeRun ++ )
        {
            const edgeRun* edgeRunCur = &_edgeRunList[idEdgeRun];

            // offset position
            f32 offsetCur = getFillOffSet( config, edgeRunCur);           

            if ( config->m_smoothFilling )
            {
                // transition between two edgeRun
                buildOutline_StartEdgeRunCornerRounder( config, filling, _edgeList, edgeRunLast, edgeRunCur, offsetLast, offsetCur, config->m_smoothFactorFilling, bfalse );

                // outline edgeRun
                buildOutline_EdgeRunWithCornerRounder( config, filling, _edgeList, edgeRunCur, offsetCur, config->m_smoothFactorFilling, 0, 0, bfalse );
            }
            else
            {   
                // transition between two edgeRun  
                buildOutline_StartEdgeRunCornerSquare( config, filling, _edgeList, edgeRunLast, edgeRunCur, offsetCur, offsetLast, bfalse );

                // outline edgeRun
                buildOutline_EdgeRunWithCornerSquare( config, filling, _edgeList, edgeRunCur, offsetCur, 0, 0, bfalse );
            }

            edgeRunLast = edgeRunCur;
            offsetLast = offsetCur;
        }
    }

    f32 Frise::getFillOffSet( const FriseConfig* config, const edgeRun* _edgeRun) const
    {
        f32 offsetCur;

        if ( _edgeRun->m_idTex == -1 )
            offsetCur = config->m_fillOffSet;
        else
            offsetCur = config->m_textureConfigs[_edgeRun->m_idTex].m_fillOffset;

        offsetCur = Clamp( offsetCur, 0.f, 1.f );

        return offsetCur;
    }
#ifdef ITF_CTR   //CASA : TOCHECK function dont passe on release, disable optim
#pragma Push   
#pragma O0
#endif
    void Frise::buildVB_FillGradient( ITF_VECTOR<edgeFrieze>& _edgeList, MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList* _indexTempList ) const
    {
        FriseConfig* config = getConfig();
        if ( !config )
            return;

        if( config->m_fillFriseTextureConfigIndex == -1 )
            return;

        // set fill offset to match with gradient
        config->m_fillOffSet = 0.f;
        u32 textureConfigCount = config->m_textureConfigs.size();
        for (u32 i =0; i<textureConfigCount; i++)
        {
            config->m_textureConfigs[i].m_fillOffset = 0.f;
        }
        
        FillingParams params;
        setFillParams( config, params);

        u32 colorExtremity[4];
        u8 alphaNull = 0;
        setVtxColor( config->m_textureConfigs[config->m_fillFriseTextureConfigIndex], colorExtremity[1], colorExtremity[0]);
        colorExtremity[1] = ITFCOLOR_SET_A(colorExtremity[1], alphaNull);
        colorExtremity[3] = colorExtremity[1];
        colorExtremity[2] = colorExtremity[0];

        const MeshElementRouter* elemRouter = getRoutingElement(MeshElementRouter::MeshElementTable_FlagFill, config->m_fillFriseTextureConfigIndex);
        ITF_ASSERT(elemRouter);
        ITF_ASSERT(elemRouter->m_tableID == MeshElementRouter::MeshElementTable_Static);

        for ( i32 i = 0; i < (i32)m_recomputeData.m_edgeListCount-1; i++ )
        {
            edgeFrieze& edgeCur = _edgeList[i];
            edgeFrieze& edgeNext = _edgeList[i+1];

            Vec2d uv[4];
            Vec2d pos[4];

            pos[0] = edgeCur.m_interDown;
            setFillUv( params, uv[0], pos[0] );
            pos[1] = edgeCur.m_interUp;
            setFillUv( params, uv[1], pos[1] );
            pos[2] = edgeNext.m_interDown;
            setFillUv( params, uv[2], pos[2] );
            pos[3] = edgeNext.m_interUp;
            setFillUv( params, uv[3], pos[3] );

            buildVB_Static_Quad( _vertexTempList, _indexTempList[elemRouter->m_indexInTable], &pos[0], &uv[0], &colorExtremity[0], btrue );
        }

        // last quad
        if ( m_recomputeData.m_edgeListCount)
        {
            edgeFrieze& edgeLast = _edgeList[m_recomputeData.m_edgeListCount-1];
            Vec2d uv[4];
            Vec2d pos[4];

            pos[0] = edgeLast.m_interDown;            
            pos[1] = edgeLast.m_interUp;            

            if ( m_pointsList.isLooping() )
            {
                pos[2] = _edgeList[0].m_interDown; 
                pos[3] = _edgeList[0].m_interUp;
            }
            else
            {
                pos[2] = edgeLast.m_points[2];
                pos[3] = edgeLast.m_points[3];
            }

            setFillUv( params, uv[0], pos[0] );
            setFillUv( params, uv[1], pos[1] );
            setFillUv( params, uv[2], pos[2] );            
            setFillUv( params, uv[3], pos[3] );

            buildVB_Static_Quad( _vertexTempList, _indexTempList[elemRouter->m_indexInTable], &pos[0], &uv[0], &colorExtremity[0], btrue );
        }

        //  weld points to match with filling
        edgeFrieze* edgeLast = NULL;

        if ( isLooping() )
            edgeLast = &_edgeList[m_recomputeData.m_edgeListCount -1];

        for ( u32 i=0; i<m_recomputeData.m_edgeListCount; i++)
        {
            edgeFrieze* edgeCur = &_edgeList[i];

            edgeCur->m_points[0] = edgeCur->m_interDown;
            edgeCur->m_points[1] = edgeCur->m_interUp;

            if ( edgeLast )
            {
                edgeLast->m_points[2] = edgeCur->m_interDown;
                edgeLast->m_points[3] = edgeCur->m_interUp;
            }

            edgeLast = edgeCur;
        }
    }
#ifdef ITF_CTR
#pragma Pop
#endif

} // namespace ITF
