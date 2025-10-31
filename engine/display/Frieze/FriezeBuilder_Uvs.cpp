#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

namespace ITF
{
    u32 Frise::getIdEdgeStartWithoutSnap( ITF_VECTOR<edgeFrieze>& _edgeList, edgeRun& _edgeRun ) const
    {
        u32 idEdge = _edgeRun.m_idEdgeStart;

        for ( u32 count = 0; count < _edgeRun.m_edgeCount; count++ )
        {
            idEdge = ( _edgeRun.m_idEdgeStart +count ) %m_recomputeData.m_edgeListCount;

            if ( !_edgeList[idEdge].m_snap )
                return idEdge;
        }

        return idEdge;
    }

    f32 Frise::getUvCornerBy2( const FriseConfig* _config, edgeRun& _edgeRun, edgeFrieze* _edgeCur, edgeFrieze* _edgeLast) const
    {
        f32 uvCornerBy2 = 0.f;

        if ( _config->m_smoothVisual )
        {
            uvCornerBy2 = getCornerRounderUv( _config, abs(_edgeCur->m_cornerAngle), _edgeRun.m_coeff, _edgeCur->m_heightStart, _edgeCur->m_scaleCur );

            if ( !isHeightEqual( *_edgeLast, *_edgeCur) )
            {
                uvCornerBy2 += getCornerRounderUv( _config, abs(_edgeCur->m_cornerAngle), _edgeRun.m_coeff, _edgeLast->m_heightStop, _edgeCur->m_scaleCur );
                uvCornerBy2 *= 0.5f;
            }               
        }
        else
        {
            if ( _edgeCur->m_cornerAngle >0.f )
            {
                uvCornerBy2 = getCornerSquareArcBy2( _edgeCur->m_cornerAngle, _edgeRun.m_coeff, _edgeCur->m_points[1] -_edgeCur->m_interUp, 1.f );

                if ( !isHeightEqual( *_edgeLast, *_edgeCur) )
                {
                    uvCornerBy2 += getCornerSquareArcBy2( _edgeCur->m_cornerAngle, _edgeRun.m_coeff, _edgeLast->m_points[3] -_edgeCur->m_interUp, 1.f );
                    uvCornerBy2 *= 0.5f;
                }
            }
            else
            {
                uvCornerBy2 = getCornerSquareArcBy2( _edgeCur->m_cornerAngle, _edgeRun.m_coeff, _edgeCur->m_points[0] -_edgeCur->m_interDown, 1.f );

                if ( !isHeightEqual( *_edgeLast, *_edgeCur) )
                {
                    uvCornerBy2 += getCornerSquareArcBy2( _edgeCur->m_cornerAngle, _edgeRun.m_coeff, _edgeLast->m_points[2] -_edgeCur->m_interDown, 1.f );
                    uvCornerBy2 *= 0.5f;
                }
            }                
        }

        return uvCornerBy2;
    }

    void Frise::setEdgeNormUv( edgeFrieze* _edge, f32 _ratio, edgeFrieze* _edgeLast, edgeFrieze* _edgeNext ) const
    {
        if ( _edgeLast && isEdgeWithHoleVisual(*_edgeLast) && _edgeNext && isEdgeWithHoleVisual(*_edgeNext) )
        {
            const PolyLineEdge& polyEdge = m_pointsList.getEdgeAt(_edge->m_idPoint);
            _edge->m_normUv = polyEdge.m_length;
        }
        else if ( _edgeLast && isEdgeWithHoleVisual(*_edgeLast) )
        {
            const PolyLineEdge& polyEdge = m_pointsList.getEdgeAt(_edge->m_idPoint);
            Vec2d offset = _edge->m_pos +_edge->m_sight *_edge->m_startCoeff -polyEdge.getPos();

            _edge->m_normUv = _edge->m_norm +offset.norm();
        }
        else if (_edgeNext && isEdgeWithHoleVisual(*_edgeNext) )
        {
            const PolyLineEdge& polyEdge = m_pointsList.getEdgeAt(_edgeNext->m_idPoint);
            Vec2d offset = _edge->m_pos +_edge->m_sight *_edge->m_stopCoeff -polyEdge.getPos();

            _edge->m_normUv = _edge->m_norm +offset.norm();
        }
        else
        {
            _edge->m_normUv = _edge->m_norm;
        }

        _edge->m_normUv /= (( _edge->m_heightStart +_edge->m_heightStop) *_ratio);

        //_edge->m_normUv = _edge->m_norm /(( _edge->m_heightStart +_edge->m_heightStop) *_ratio);
    }

    void Frise::setEdgeRunNormUv( FriseConfig* config, ITF_VECTOR<edgeFrieze>& _edgeList, edgeRun& _edgeRun ) const
    {
#ifdef DEVELOPER_JAY_FRIEZE
        ITF_ASSERT( _edgeRun.m_edgeCount>0 );
#endif

        edgeFrieze* edgeCur = &_edgeList[_edgeRun.m_idEdgeStart];
        edgeFrieze* edgeLast = NULL;
        f32 edgeLastCornerSign = 0.f;
        f32 edgeLastUvToAddWanted = 0.f;
        f32 ratioByScalePoint = 0.5f /m_recomputeData.m_heightScale;

        edgeFrieze* edgeNext = NULL;
        if ( isLooping() || _edgeRun.m_idEdgeStart+1 <m_recomputeData.m_edgeListCount)
        {
            edgeNext = &_edgeList[getIdEdgeNext(_edgeRun.m_idEdgeStart)];
        }

        // set edge first
        if ( isLooping() || _edgeRun.m_idEdgeStart >0)
            edgeLast = &_edgeList[getIdEdgeLast(_edgeRun.m_idEdgeStart)];

        setEdgeNormUv( edgeCur, ratioByScalePoint, edgeLast, edgeNext);
        edgeLast = edgeCur;

        for ( u32 count = 1; count < _edgeRun.m_edgeCount; count ++ )
        {            
            // set edge cur
            u32 idEdgeCur = ( _edgeRun.m_idEdgeStart + count ) % m_recomputeData.m_edgeListCount;
            edgeCur = &_edgeList[idEdgeCur];

            // get edge next
            edgeNext = NULL;
            if ( isLooping() || idEdgeCur+1 <m_recomputeData.m_edgeListCount)
                edgeNext = &_edgeList[getIdEdgeNext(idEdgeCur)];

            setEdgeNormUv( edgeCur, ratioByScalePoint, edgeLast, edgeNext);

            f32 uvCornerBy2 = getUvCornerBy2( config, _edgeRun, edgeCur, edgeLast);

//#ifdef DEVELOPER_JAY_FRIEZE
            //      debugDrawFloat( transformPos(edgeCur->m_pos +Vec2d(0.f,0.1f)), uvCornerBy2, Color::green() );
//#endif
            f32 edgeCurCornerSign = getSign(edgeCur->m_cornerAngle);

            if ( edgeLast->m_snap )
            {
                if ( count == 1 )
                    edgeLastUvToAddWanted += uvCornerBy2;
            }
            else
            {
                // last edge in edgeRun
                if ( edgeCur->m_snap && count== _edgeRun.m_edgeCount -1)
                    edgeLast->m_normUv += uvCornerBy2;

                if ( edgeCurCornerSign == edgeLastCornerSign )
                    edgeLast->m_normUv += uvCornerBy2 +edgeLastUvToAddWanted;
                else
                    edgeLast->m_normUv += Max( edgeLastUvToAddWanted, uvCornerBy2);

                edgeLastUvToAddWanted = 0.f;
            }

            edgeLastUvToAddWanted += uvCornerBy2;
            edgeLast = edgeCur;
            edgeLastCornerSign = edgeCurCornerSign;
        }

        if ( !edgeLast->m_snap )
            edgeLast->m_normUv += edgeLastUvToAddWanted;

//#ifdef DEVELOPER_JAY_FRIEZE
        //         f32 totalUv = 0.f;
        //         for ( u32 count = 0; count < _edgeRun.m_edgeCount; count ++ )
        //             {
        //             u32 idEdgeCur = ( _edgeRun.m_idEdgeStart + count ) % m_edgeListCount;
        //             edgeFrieze& edge = _edgeList[idEdgeCur];
        //             totalUv += edge.m_normUv;
        // 
        //             debugDrawFloat( transformPos(edge.m_pos), edge.m_norm, Color::black() );
        //             debugDrawFloat( transformPos(edge.m_pos -Vec2d(0.f,0.1f)), edge.m_normUv, Color::red() );
        //         }
        //         edgeFrieze& edgeStop = _edgeList[_edgeRun.m_edgeCount-1];
        //         debugDrawFloat( transformPos( edgeStop.m_pos +edgeStop.m_sight), totalUv, Color::black() );
//#endif
    }

    u32 Frise::getCornerStepNb( f32 _angle, f32 _step, f32 _scale ) const
    {
#ifdef DEVELOPER_JAY_FRIEZE
        ITF_ASSERT( _angle >= 0.f );
#endif
        return Max( 1u, u32( floor( 0.5f + (_angle *_step *_scale) ) ) );
    }

    f32 Frise::getStepCornerCoeff( f32 _uvCoeff, f32 _angle ) const
    {
#ifdef DEVELOPER_JAY_FRIEZE
        ITF_ASSERT( _angle >= 0.f );
#endif
        //     return _uvCoeff * ( 1.f - ( 0.3f * _angle / MTH_PI ) );
        return _uvCoeff;
    }

    f32 Frise::getCornerRounderUv( const FriseConfig* config, f32 _angle, f32 _uvCoeff, f32 _height, f32 _scale ) const
    {
        u32 stepNb = getCornerStepNb( _angle, config->m_smoothFactorVisual, _scale );
        f32 stepCornerAngle = _angle / (f32) stepNb;   
        f32 uvXStep = getCornerEdgeNorm( stepCornerAngle, _height ) * getStepCornerCoeff( _uvCoeff, _angle );   

        return uvXStep * (f32)stepNb * 0.5f;
    }

    void Frise::setCornerRounderDatas( FriseConfig* config, const edgeFrieze* _edge, f32 _uvCoeff, u32& _stepNb, f32& _stepCornerAngle, f32& _uvXStep, f32& _arcBy2, f32 _uvXsign ) const
    {
        f32 angle = f32_Abs(_edge->m_cornerAngle);
        _stepNb = getCornerStepNb( angle, config->m_smoothFactorVisual, _edge->m_scaleCur );
        _stepCornerAngle = angle / (f32) _stepNb;   
        _uvXStep = getCornerEdgeNorm( _stepCornerAngle, _edge->m_heightStart ) * getStepCornerCoeff( _uvCoeff, angle ) *_uvXsign;   
        _arcBy2 = _uvXStep * (f32)_stepNb * 0.5f;
    }

    f32 Frise::getCornerSquareArcBy2( f32 _cornerAngle, f32 _uvCoeff, Vec2d _vector, f32 _uvXsign ) const
    {        
        return ( _vector.norm() * getStepCornerCoeff( _uvCoeff, f32_Abs(_cornerAngle) ) *_uvXsign );   
    }

    f32 Frise::getUvX_Start() const
    {  
        if ( m_recomputeData.m_uvXoffSet == 0.f || m_recomputeData.m_texRatio == 0.f )
            return 0.f;  

        m_recomputeData.m_uvXoffSet = f32_Modulo( m_recomputeData.m_uvXoffSet, 1.f /m_recomputeData.m_texRatio );
        m_recomputeData.m_uvXoffSet = floorf(m_recomputeData.m_uvXoffSet);

        return ( m_recomputeData.m_uvXoffSet *m_recomputeData.m_texRatio );
    }  

    void Frise::setEdgeRunCoeffUv( FriseConfig* config, ITF_VECTOR<edgeFrieze>& _edgeList, edgeRun& _edgeRun ) const
    {
        setEdgeRunNormUv( config, _edgeList, _edgeRun );

        if ( m_recomputeData.m_texRatio == 0.f )
        {
            _edgeRun.m_coeff = 0.f;
            return;
        }

        f32 longEdgeRun = 0.f;
        for ( u32 edgeCount = 0; edgeCount < _edgeRun.m_edgeCount; edgeCount++ )
        {
            u32 idEdge = ( _edgeRun.m_idEdgeStart + edgeCount ) % m_recomputeData.m_edgeListCount;
            longEdgeRun += _edgeList[idEdge].m_normUv;
        }

        //  if all edges are snaped, uv continuity is impossible
        if ( longEdgeRun == 0.f )
        {
            _edgeRun.m_coeff = 0.f;//texScaleX;
            return;
        }

        f32 texScaleX = 1.f / m_recomputeData.m_widthScale;
        longEdgeRun *= texScaleX;

        //  long wanted
        f32 longInt = floorf(longEdgeRun);
        f32 longWanted  = Max( m_recomputeData.m_texRatio, longInt +floorf( 0.5f + ( longEdgeRun -longInt ) /m_recomputeData.m_texRatio ) * m_recomputeData.m_texRatio );

        _edgeRun.m_coeff = texScaleX *longWanted /longEdgeRun;
    }

    bbool Frise::setEdgeRunListCoeffUv( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList ) const
    {
        FriseConfig* config = getConfig();
        if ( !config )
            return bfalse;

        bbool isTextureConfigValid = bfalse;

        const u32 edgeRunListCount = _edgeRunList.size();
        for( u32 idEdgeRun = 0; idEdgeRun < edgeRunListCount; idEdgeRun++ )
        {
            edgeRun& edgeRunCur = _edgeRunList[idEdgeRun];

            if ( edgeRunCur.m_idTex == -1 )
                continue;

            isTextureConfigValid = btrue;

            setEdgeRunCoeffUv( config, _edgeList, edgeRunCur );
        }

        return isTextureConfigValid;
    }

    void Frise::orderEdgeRunListByTextureId( ITF_VECTOR<edgeRun>& _edgeRunList ) const
    {
        u32 edgeRunListCount = _edgeRunList.size();

        if ( edgeRunListCount < 2 )
            return;

        u32 edgeRunCurId = 0;

        //  search first edgeRun
        if ( m_pointsList.isLooping() )
        {
            u32 searchId = edgeRunListCount -1;

            while ( _edgeRunList[searchId].m_idTex == _edgeRunList[edgeRunCurId].m_idTex && searchId > 0 )
            {
                edgeRunCurId = searchId;
                searchId--;
            }
        }

        // temp vector
        ITF_VECTOR<edgeRun> edgeRunListTemp;
        edgeRunListTemp.reserve(edgeRunListCount);
        edgeRunListTemp.push_back( _edgeRunList[edgeRunCurId] );
        u32 edgeRunCurIdTemp = 0;

        for ( u32 count = 1; count < edgeRunListCount; count++ )
        {
            edgeRunCurId = ( edgeRunCurId +1 ) % edgeRunListCount;

            if ( edgeRunListTemp[edgeRunCurIdTemp].m_idTex == _edgeRunList[edgeRunCurId].m_idTex )
            {
                edgeRunListTemp[edgeRunCurIdTemp].m_edgeCount += _edgeRunList[edgeRunCurId].m_edgeCount;
                continue;
            }

            // add temp edgeRun
            edgeRunListTemp.push_back( _edgeRunList[edgeRunCurId] );
            edgeRunCurIdTemp++;
        }

        // clear old
        _edgeRunList.clear();

        //  copy temp vector
        u32 edgeRunListTempCount = edgeRunListTemp.size();
        for ( u32 i = 0; i < edgeRunListTempCount; i++ )
        {
            _edgeRunList.push_back(edgeRunListTemp[i]);
        }
    }

    bbool Frise::flipEdge( f32 _offset ) const
    {
        return _offset < 0;
    }

    void Frise::updateUvs( const edgeRun& _edgeRun, const edgeFrieze* _edge, Vec2d& _uvUp, Vec2d& _uvDown, f32 _uvXsign, f32 _uvUpXOffSet, f32 _uvDownXOffSet ) const
    {
        f32 uvXQuad = _edge->m_normUv * _edgeRun.m_coeff *_uvXsign;
        _uvUp.m_x += uvXQuad + _uvUpXOffSet;
        _uvDown.m_x += uvXQuad + _uvDownXOffSet;
    }

    void Frise::updateEdgeCornerUvs( CornerRounder& _corner, f32& _vtxUvX, f32 _uvLeft, f32 _uvRight ) const
    {
        if ( !_corner.m_edgeLast->m_snap )
        {
            // uv edge last
            if ( _corner.m_edgeCur->m_snap && *_corner.m_edgeCountCur == _corner.m_edgeRun->m_edgeCount -1)               
                _corner.m_uvCorner->m_x -= _uvLeft +_uvRight;
            else
                _corner.m_uvCorner->m_x -= _uvLeft;                

            // udpate uv edge last
            _vtxUvX = _corner.m_uvCorner->m_x;

            // uv next
            *_corner.m_uvXoffsetCorner -= _uvRight;
        }
        else
        {
            // uvs next
            *_corner.m_uvXoffsetCorner -= _uvLeft +_uvRight;
        }
    }

    f32 Frise::getCoeffScaleStepFanToBlend( const edgeFrieze* _edgeCur, const edgeFrieze* _edgeLast) const
    {
        return Max( _edgeCur->m_scaleCur, f32_Abs( _edgeCur->m_heightStart -_edgeLast->m_heightStop) *m_recomputeData.m_heightScaleInv );
    } 

} // namespace ITF
