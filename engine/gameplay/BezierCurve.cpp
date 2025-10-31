#include "precompiled_engine.h"

#ifndef _ITF_BEZIERCURVE_H_
#include "engine/gameplay/BezierCurve.h"
#endif // _ITF_BEZIERCURVE_H_


namespace ITF
{

    //////////////////////////////////////////////////////////////////////////
    // BEZIER CURVE
    //////////////////////////////////////////////////////////////////////////
    BEGIN_SERIALIZATION(BezierCurve)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_CONTAINER_OBJECT("points", m_points);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS(BezierCurve, Point)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("pos", m_pos);
            SERIALIZE_MEMBER("tanA", m_tanA);
            SERIALIZE_MEMBER("tanB", m_tanB);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    void BezierCurve::buildEdges( const u32 _samplesPerEdge )
    {
        if (m_points.size() == 0)
        {
            m_edges.clear();
            return;
        }

        m_edges.resize(m_points.size()-1);

        for (u32 i = 0; i < m_points.size()-1;++i)
        {
            BezierCurve::Edge &edge = m_edges[i];
            edge.m_length = 0.0f;
            edge.m_edgeStart = 0.0f;
            edge.m_bezier.m_p0 = m_points[i].m_pos;
            edge.m_bezier.m_p1 = m_points[i].m_tanB;
            edge.m_bezier.m_p2 = m_points[i+1].m_tanA;
            edge.m_bezier.m_p3 = m_points[i+1].m_pos;
            edge.m_samples.clear();
        }

        m_totalLength = 0.0f;
        if (_samplesPerEdge > 0)
        {
            const f32 sampleInc = f32_Inv((f32)_samplesPerEdge);

            for (u32 i = 0; i < m_edges.size(); ++i)
            {
                BezierCurve::Edge & edge = m_edges[i];
                edge.m_edgeStart = m_totalLength;
                edge.sample(_samplesPerEdge, sampleInc);
                m_totalLength += edge.m_length;
            }
        }

    }

    void BezierCurve::Edge::sample( const u32 _sampleCount, const f32 _sampleInc )
    {
        m_samples.resize(_sampleCount);

        f32 length = 0.0f;
        f32 t = _sampleInc;
        Vec3d prev = m_bezier.m_p0;

        for (u32 j = 0; j < _sampleCount; ++j)
        {
            const Vec3d pos = m_bezier.getPos(t);
            length += (pos - prev).norm();
            m_samples[j] = m_edgeStart + length;

            prev = pos;
            t += _sampleInc;
        }

        m_length = length;
    }

    void BezierCurve::getEdgeIndexAtDistance( u32& _edgeIndex, u32& _sampleIndex, const f32 _distance ) const
    {
        ITF_ASSERT_MSG(_distance <= m_totalLength + MTH_EPSILON, "_distance too big!");

        for (; _edgeIndex < m_edges.size(); ++_edgeIndex)
        {
            const BezierCurve::Edge & edge = m_edges[_edgeIndex];
            if ( (edge.m_edgeStart + edge.m_length) >= _distance )
            {
                return;
            }

            _sampleIndex = 0;
        }

        _edgeIndex = U32_INVALID;
        _sampleIndex = 0;
    }

    f32 BezierCurve::Edge::getTAtDistance( const f32 _distance ) const
    {
        ITF_ASSERT_MSG(_distance <= (m_edgeStart + m_length), "_distance too big!");

        const u32 sampleCount = m_samples.size();
        for (u32 i = 0; i < sampleCount; ++i)
        {
            f32 sampleDist = m_samples[i];
            if (_distance <= sampleDist)
            {
                const f32 prevDist = (i > 0) ? m_samples[i-1] : m_edgeStart;
                const f32 sampleT = f32_Inv((f32)sampleCount);
                return sampleT * ( (f32)i + (_distance - prevDist) / (sampleDist - prevDist) );
            }
        }

        return -1.0f;
    }

    f32 BezierCurve::Edge::getTAtDistance( u32& _i, const f32 _distance ) const
    {
        ITF_ASSERT_MSG(_distance <= (m_edgeStart + m_length), "_distance too big!");

        const u32 sampleCount = m_samples.size();
        for ( ; _i < sampleCount; ++_i)
        {
            f32 sampleDist = m_samples[_i];
            if (_distance <= sampleDist)
            {
                const f32 prevDist = (_i > 0) ? m_samples[_i-1] : m_edgeStart;
                const f32 sampleT = f32_Inv((f32)sampleCount);
                return sampleT * ( (f32)_i + (_distance - prevDist) / (sampleDist - prevDist) );
            }
        }

        _i = U32_INVALID;
        return -1.0f;
    }

    void BezierCurve::Edge::getSegment( const f32 _distFrom, const f32 _distTo, Edge& _segmentOut ) const
    {
        const f32 tFrom = getTAtDistance(_distFrom);
        const f32 tTo = getTAtDistance(_distTo);
        m_bezier.getSegment(tFrom, tTo, _segmentOut.m_bezier);

        _segmentOut.m_edgeStart = _distFrom;
        _segmentOut.sample(m_samples.size());
    }

    void BezierCurve::Edge::getSegmentLeft( const f32 _distTo, Edge& _segmentOut ) const
    {
        const f32 t = getTAtDistance(_distTo);
        Bezier4<Vec3d> tmp;
        m_bezier.split(t, _segmentOut.m_bezier, tmp);

        _segmentOut.m_edgeStart = m_edgeStart;
        _segmentOut.sample(m_samples.size());
    }

    void BezierCurve::Edge::getSegmentRight( const f32 _distFrom, Edge& _segmentOut ) const
    {
        const f32 t = getTAtDistance(_distFrom);
        Bezier4<Vec3d> tmp;
        m_bezier.split(t, tmp, _segmentOut.m_bezier);

        _segmentOut.m_edgeStart = _distFrom;
        _segmentOut.sample(m_samples.size());
    }

    void BezierCurve::getCut( BezierCurve& _out, u32& _endEdgeIndexOut, const f32 _length ) const
    {
        _out.clear();

        _endEdgeIndexOut = getEdgeIndexAtDistance(_length);
        const f32 t = getTAtDistance(_length, _endEdgeIndexOut);

        for (u32 i=0; i<=_endEdgeIndexOut; ++i)
        {
            _out.addPoint(getPointAt(i));
        }

        if (t >= 1.f)
        {
            _out.addPoint(getPointAt(_endEdgeIndexOut + 1));
        }
        else
        {
            // last point (tangB doesn't matter, but need to backtrack prev point's tangB)
            const Edge* endEdge = getEdgeAt(_endEdgeIndexOut);
            Bezier4<Vec3d> left, right;
            endEdge->m_bezier.split(t, left, right);
            _out.addPoint(left.m_p3, left.m_p2, Vec3d::Zero);
            _out.getPointAt(_endEdgeIndexOut).m_tanB = left.m_p1;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // BEZIER CURVE RENDERER
    //////////////////////////////////////////////////////////////////////////

    void BezierCurveRenderer::draw( BezierPatchList& _patches, const BezierCurve & _curve, const Transform3d* _transform /*= NULL*/, bbool _flipUV /*= bfalse*/ )
    {
        Texture* texture = static_cast<Texture*>(m_textureID.getResource());
        ITF_ASSERT(texture);
        if (!texture)
        {
            return;
        }

        const u32 patchCount = _curve.getNumEdges();
        ITF_ASSERT_CRASH(patchCount <= _patches.size(), "you need to resize your patches yourself! I don't do it because it can cause z-fighting when drawing only a portion of a curve.");
        ITF_ASSERT(patchCount);
        if (patchCount == 0)
        {
            return;
        }

        const f32 totalLength = _curve.getTotalLength();
        ITF_ASSERT_MSG(totalLength, "you need to sample your curve!");
        if (totalLength < MTH_EPSILON)
        {
            return;
        }


        BezierPatchParams patchParams;

        // params shared by all the patches
        patchParams.m_z = m_depth;
        patchParams.m_texture = texture;
        patchParams.m_uvMode = m_uvMode;
        patchParams.m_startColor = patchParams.m_endColor = m_color;
        patchParams.m_fogColor = m_fogColor;
        patchParams.m_startUV.m_y = m_UVy.m_x;
        patchParams.m_endUV.m_y = m_UVy.m_y;
        if (_flipUV)
        {
            const f32 temp = patchParams.m_startUV.m_y;
            patchParams.m_startUV.m_y = patchParams.m_endUV.m_y;
            patchParams.m_endUV.m_y = temp;
        }


        f32 t = 0.0f;
        f32 curTile = m_startUV;
        f32 curWidth = m_beginLength ? m_beginWidth : m_midWidth;
        f32 curAlpha = m_beginLength ? m_beginAlpha : m_midAlpha;

        for (u32 i = 0; i < patchCount; ++i) 
        {
            const BezierCurve::Edge* edge = _curve.getEdgeAt(i);

            f32 countTileSeg = edge->m_length / m_tileLength;
            patchParams.m_startUV.m_x = curTile;
            patchParams.m_endUV.m_x = curTile + countTileSeg;
            curTile += countTileSeg;

            const f32 t0 = t;
            const f32 t1 = t + edge->m_length;
            t = t1;

            if (edge->m_length < MTH_EPSILON)
            {
                continue;
            }


            if (_transform)
            {
                patchParams.m_p0 = _transform->transformPos(edge->m_bezier.m_p0).truncateTo2D();
                patchParams.m_p1 = _transform->transformPos(edge->m_bezier.m_p1).truncateTo2D();
                patchParams.m_p2 = _transform->transformPos(edge->m_bezier.m_p2).truncateTo2D();
                patchParams.m_p3 = _transform->transformPos(edge->m_bezier.m_p3).truncateTo2D();
            }
            else
            {
                patchParams.m_p0 = edge->m_bezier.m_p0.truncateTo2D();
                patchParams.m_p1 = edge->m_bezier.m_p1.truncateTo2D();
                patchParams.m_p2 = edge->m_bezier.m_p2.truncateTo2D();
                patchParams.m_p3 = edge->m_bezier.m_p3.truncateTo2D();
            }


            // width
            patchParams.m_widthLimitA = 0.f;
            patchParams.m_widthLimitB = 1.f;
            patchParams.m_widthValueA = curWidth;
            patchParams.m_widthValueB = interpolateWidth(t1, totalLength);
            patchParams.m_startWidth = patchParams.m_widthValueA;
            patchParams.m_endWidth   = patchParams.m_widthValueB;
            curWidth = patchParams.m_widthValueB;


            // alpha
            patchParams.m_alphaLimitA = 0.f;
            patchParams.m_alphaLimitB = 1.f;
            patchParams.m_alphaValueA = curAlpha;
            patchParams.m_alphaValueB = interpolateAlpha(t1, totalLength);
            patchParams.m_startColor.m_a = patchParams.m_alphaValueA;
            patchParams.m_endColor.m_a   = patchParams.m_alphaValueB;
            curAlpha = patchParams.m_alphaValueB;


            if (t0 < m_beginLength && m_beginLength < t1)
            {
                // OVERLAPS BEGIN SECTION
                const f32 t = _curve.getTAtDistance(m_beginLength, i);  // caution: the shader works with t, not d

                patchParams.m_widthLimitA = t;
                patchParams.m_startWidth = interpolateWidth(m_beginLength, totalLength);

                patchParams.m_alphaLimitA = t;
                patchParams.m_startColor.m_a = interpolateAlpha(m_beginLength, totalLength);
            }
            // no 'else'; small (eg single-patch) curves can overlap both the begin and end sections
            if (t0 < totalLength - m_endLength && totalLength - m_endLength < t1)
            {
                // OVERLAPS END SECTION
                const f32 t = _curve.getTAtDistance(totalLength - m_endLength, i);  // caution: the shader works with t, not d

                patchParams.m_widthLimitB = t;
                patchParams.m_endWidth = interpolateWidth(totalLength - m_endLength, totalLength);

                patchParams.m_alphaLimitB = t;
                patchParams.m_endColor.m_a = interpolateAlpha(totalLength - m_endLength, totalLength);
            }


            if ( m_tessellationRatio > 0 )
            {
                patchParams.m_vdivToDraw = Max((u32)1, (u32)(edge->m_length * m_tessellationRatio));
            }


            _patches[i].drawPatch(patchParams);
        }
    }

    void BezierCurveRenderer::onLoaded( const BezierCurveRenderer_Template * _template, ResourceGroup * _group )
    {
        if (!_template->getTextureFile().isEmpty())
        {
            m_textureID = _group->addResource(Resource::ResourceType_Texture, _template->getTextureFile());
        }

        m_beginAlpha = _template->getBeginAlpha();
        m_midAlpha = _template->getMidAlpha();
        m_endAlpha = _template->getEndAlpha();
        m_beginWidth = _template->getBeginWidth();
        m_midWidth = _template->getMidWidth();
        m_endWidth = _template->getEndWidth();
        m_beginLength = _template->getBeginLength();
        m_endLength = _template->getEndLength();
        m_startUV = _template->getStartUV();
        m_tileLength = _template->getTileLength();
        m_color = _template->getColor();
        m_fogColor = _template->getFogColor();
        m_uvMode = _template->getMode();
        if (_template->getTessellationLength() > 0)
        {
            m_tessellationRatio = f32_Inv(_template->getTessellationLength());
        }
    }

    BezierCurveRenderer::~BezierCurveRenderer()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    // BEZIER RENDERER TEMPLATE
    //////////////////////////////////////////////////////////////////////////
    BEGIN_SERIALIZATION(BezierCurveRenderer_Template)
        SERIALIZE_MEMBER("beginLength",m_beginLength);
        SERIALIZE_MEMBER("endLength",m_endLength);
        SERIALIZE_MEMBER("beginWidth",m_beginWidth);
        SERIALIZE_MEMBER("midWidth",m_midWidth);
        SERIALIZE_MEMBER("endWidth",m_endWidth);
        SERIALIZE_MEMBER("beginAlpha",m_beginAlpha);
        SERIALIZE_MEMBER("midAlpha",m_midAlpha);
        SERIALIZE_MEMBER("endAlpha",m_endAlpha);
        SERIALIZE_MEMBER("startUV",m_startUV);
        SERIALIZE_MEMBER("tileLength",m_tileLength);
        SERIALIZE_MEMBER("color",m_color);
        SERIALIZE_MEMBER("fogColor",m_fogColor);
        SERIALIZE_MEMBER("texture",m_textureFile);
        SERIALIZE_UVMODE("uvMode",m_uvMode);
        SERIALIZE_MEMBER("tessellationLength",m_tessellationLength);
    END_SERIALIZATION()

}

