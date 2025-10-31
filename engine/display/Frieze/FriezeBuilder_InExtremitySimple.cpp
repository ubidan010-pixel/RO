#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

namespace ITF
{
    void Frise::buildFrieze_InExtremitySimple( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, MeshStatic_VertexList& _vertexTempList_Static, MeshStatic_IndexList* _indexTempList_Static, MeshAnim_VertexList& _vertexTempList_Anim, MeshAnim_IndexList* _indexTempList_Anim ) const
    {
        FriseConfig* config = getConfig();
        if ( !config )
            return;

        copyEdgeFromPolyline(_edgeList);        

        m_recomputeData.m_texRatio = 1.0;
        f32 extremityRatio = 0.5f;

        subtractOffsetExtremity( _edgeList, getExtremityLong(extremityRatio) );

        buildEdgeList( _edgeList );

        if ( m_recomputeData.m_edgeListCount == 0 )
            return;

#ifdef DEVELOPER_JAY_FRIEZE
        debugDrawEdgeList(_edgeList);        
#endif

        if ( !buildEdgeRunList_InExtremity( _edgeList, _edgeRunList ) )
            return;

        setEdgeRunCoeffUv( config, _edgeList, _edgeRunList[0] );

        buildVB_InExtremity( _edgeList, _edgeRunList, _vertexTempList_Static, _indexTempList_Static, _vertexTempList_Anim, _indexTempList_Anim, extremityRatio );
    }

} // namespace ITF
