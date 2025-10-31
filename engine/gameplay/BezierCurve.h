#ifndef _ITF_BEZIERCURVE_H_
#define _ITF_BEZIERCURVE_H_

#ifndef _ITF_BEZIERPATCH_H_
#include "engine/display/BezierPatch.h"
#endif //_ITF_BEZIERPATCH_H_

#ifndef _ITF_TRANSFORM2D3D_H_
#include "engine/gameplay/Transform.h"
#endif //_ITF_TRANSFORM2D3D_H_

namespace ITF
{
    ITF_INLINE f32 interpolate( const f32 _dist, const f32 _totalLength,
        const f32 _beginLength, const f32 _endLength,
        const f32 _beginValue, const f32 _midValue, const f32 _endValue )
    {
        if (_dist < _beginLength)
        {
            ITF_ASSERT(_beginLength);
            return _beginValue + (_dist / _beginLength) * (_midValue - _beginValue);
        }
        else if (_dist <= _totalLength - _endLength)
        {
            return _midValue;
        }
        else
        {
            ITF_ASSERT(_endLength);
            const f32 factor = 1 - (_totalLength - _dist) / _endLength;
            const f32 width = _midValue + factor * (_endValue - _midValue);
            return width;
        }
    }


    class BezierCurve
    {
        DECLARE_SERIALIZE()

    public:

        BezierCurve() 
            : m_totalLength(0.0f)
        {
        }

        struct Point
        {
            DECLARE_SERIALIZE()
            Point(const Vec3d & _pos, const Vec3d & _tanA, const Vec3d & _tanB) : m_pos(_pos) , m_tanA(_tanA) ,m_tanB(_tanB){}
            Point() : m_pos(Vec3d::Zero), m_tanA(Vec3d::Zero), m_tanB(Vec3d::Zero) {}
            Vec3d m_pos;
            Vec3d m_tanA;
            Vec3d m_tanB;
        };
        typedef SafeArray<Point> PointList;

        struct Edge
        {
            Edge()
                : m_length(0.0f)
                , m_edgeStart(0.0f)
            {}

            ITF_INLINE void sample( const u32 _sampleCount ) { sample(_sampleCount, 1.f / _sampleCount); }
            void sample( const u32 _sampleCount, const f32 _sampleInc );
            f32 getTAtDistance( const f32 _distance ) const;
            f32 getTAtDistance( u32& _sampleIndex, const f32 _distance ) const;
            void getSegment( const f32 _distFrom, const f32 _distTo, Edge& _segmentOut ) const;
            void getSegmentLeft( const f32 _distTo, Edge& _segmentOut ) const;
            void getSegmentRight( const f32 _distFrom, Edge& _segmentOut ) const;

            Bezier4<Vec3d> m_bezier;
            f32 m_length;
            f32 m_edgeStart;
            SafeArray<f32> m_samples;
        };
        typedef ITF_VECTOR<Edge> EdgeList;

        void clear()
        {
            m_points.clear();
            m_edges.clear();
        }

        void resize( const u32 _pointCount )
        {
            // only resize points; edges are resized on buildEdges()
            m_points.resize(_pointCount);
        }


        void addPoint(const Vec3d & _pos, const Vec3d & _tanA, const Vec3d & _tanB);
        void addPoint(const Point & _point) { m_points.push_back(_point); }
        void insertPointBefore(const Point & _point, u32 _index) { m_points.insert(_point,_index); }
        void removePointAt(u32 _index) { m_points.eraseKeepOrder(_index); }
        u32 getNumPoints() const { return m_points.size(); }
        Point& getPointAt(u32 _index) { return m_points[_index]; }
        const Point& getPointAt(u32 _index) const { return m_points[_index]; }
        ITF_INLINE const PointList& getPoints() const { return m_points; }

        void buildEdges( const u32 _samplesPerEdge );
        f32 getTotalLength() const { return m_totalLength; }

        u32 getNumEdges() const { return m_edges.size(); }
        Edge* getEdgeAt(u32 i);
        const Edge* getEdgeAt(u32 i) const;
        ITF_INLINE const EdgeList& getEdges() const { return m_edges; }

        Vec3d getPosAtT( const f32 _t, const u32 _edgeIndex ) const;
        Vec3d getTanAtT( const f32 _t, const u32 _edgeIndex ) const;
        void getEdgeIndexAtDistance( u32& _edgeIndex, u32& _sampleIndex, const f32 _distance ) const;
        ITF_INLINE u32 getEdgeIndexAtDistance( const f32 _distance ) const { u32 e = 0, t = 0; getEdgeIndexAtDistance(e, t, _distance); return e; }
        ITF_INLINE f32 getTAtDistance( const f32 _distance, const u32 _edgeIndex ) const { return m_edges[_edgeIndex].getTAtDistance(_distance); }
        ITF_INLINE Vec3d getPosAtDistance( const f32 _distance ) const;
        ITF_INLINE Vec3d getPosAtDistance( u32& _edgeIndex, u32& _sampleIndex, const f32 _distance ) const;
        ITF_INLINE void getPosTanAtDistance( const f32 _distance, Vec3d& _pos, Vec3d& _tan ) const;
        ITF_INLINE void getPosTanAtDistance( u32& _edgeIndex, u32& _sampleIndex, const f32 _distance, Vec3d& _pos, Vec3d& _tan ) const;

        void getCut(BezierCurve& _out, u32& _endEdgeIndexOut, f32 _length) const;

    private:

        PointList m_points;
        EdgeList m_edges;
        f32 m_totalLength;
    };


    typedef ITF_VECTOR<BezierPatch> BezierPatchList;

    class BezierCurveRenderer
    {
    public:
        BezierCurveRenderer() 
            : m_beginLength(2.f)
            , m_endLength(2.f)
            , m_beginWidth(1.f)
            , m_midWidth(1.f)
            , m_endWidth(1.f)
            , m_beginAlpha(0.f)
            , m_midAlpha(1.f)
            , m_endAlpha(0.f)
            , m_startUV(0.f)
            , m_tileLength(1.0f)
            , m_uvMode(UV_MODE_OPTIMUM)
            , m_color(Color::white())
            , m_fogColor(0.f, Color::white().m_r, Color::white().m_g, Color::white().m_b)
            , m_depth(0.0f)
            , m_tessellationRatio(0.f)
            , m_UVy(0.0f,1.0f)
        {
        }
        ~BezierCurveRenderer();

        void onLoaded(const class BezierCurveRenderer_Template * _template, ResourceGroup * _group);

        void draw( BezierPatchList& _patches, const BezierCurve& _curve, const Transform3d* _transform = NULL, bbool _flipUv = bfalse );
        ITF_INLINE void draw( const BezierCurve& _curve, const Transform3d* _transform = NULL, bbool _flipUv = bfalse )
        {
            m_patches.resize(_curve.getNumEdges());
            draw(m_patches, _curve, _transform, _flipUv);
        }

        ITF_INLINE f32 getBeginLength() const { return m_beginLength; }
        ITF_INLINE void setBeginLength( const f32 _value ) { m_beginLength = _value; }
        ITF_INLINE f32 getEndLength() const { return m_endLength; }
        ITF_INLINE void setEndLength( const f32 _value ) { m_endLength = _value; }

        ITF_INLINE f32 getBeginWidth() const { return m_beginWidth; }
        ITF_INLINE void setBeginWidth( const f32 _value ) { m_beginWidth = _value; }
        ITF_INLINE f32 getMidWidth() const { return m_midWidth; }
        ITF_INLINE void setMidWidth( const f32 _value ) { m_midWidth = _value; }
        ITF_INLINE f32 getEndWidth() const { return m_endWidth; }
        ITF_INLINE void setEndWidth( const f32 _value ) { m_endWidth = _value; }

        ITF_INLINE f32 getBeginAlpha() const { return m_beginAlpha; }
        ITF_INLINE void setBeginAlpha( const f32 _value ) { m_beginAlpha = _value; }
        ITF_INLINE f32 getMidAlpha() const { return m_midAlpha; }
        ITF_INLINE void setMidAlpha( const f32 _value ) { m_midAlpha = _value; }
        ITF_INLINE f32 getEndAlpha() const { return m_endAlpha; }
        ITF_INLINE void setEndAlpha( const f32 _value ) { m_endAlpha = _value; }

        ITF_INLINE f32 getStartUV() const { return m_startUV; }
        ITF_INLINE void setStartUV( const f32 _value ) { m_startUV = _value; }
        ITF_INLINE f32 getTileLength() const { return m_tileLength; }
        ITF_INLINE void setTileLength( const f32 _value ) { m_tileLength = _value; }
        ITF_INLINE BEZIER_PATCH_UVMODE getUvMode() const { return m_uvMode; }
        ITF_INLINE void setUvMode( const BEZIER_PATCH_UVMODE _value ) { m_uvMode = _value; }

        ITF_INLINE const Color& getColor() const { return m_color; }
        ITF_INLINE void setColor( const Color& _value ) { m_color = _value; }
        ITF_INLINE const Color& getFogColor() const { return m_fogColor; }
        ITF_INLINE void setFogColor( const Color& _value ) { m_fogColor = _value; }

        ITF_INLINE const ResourceID& getTextureID() const { return m_textureID; }

        ITF_INLINE f32 getDepth() const { return m_depth; }
        ITF_INLINE void setDepth( const f32 _value ) { m_depth = _value; }

        // set tessellation ratio to 0 to max out (use the whole patch)
        ITF_INLINE f32 getTessellationRatio() const { return m_tessellationRatio; }
        ITF_INLINE void setTessellationRatio( const f32 _value ) { m_tessellationRatio = _value; }

        ITF_INLINE const Vec2d& getUVy() const { return m_UVy; }
        ITF_INLINE void setUVy( const Vec2d& _val ) { m_UVy = _val; }

        f32 interpolateAlpha( const f32 _dist, const f32 _totalLength ) const;
        f32 interpolateWidth( const f32 _dist, const f32 _totalLength ) const;

    private:
        f32 m_beginLength;  // shared by width & alpha
        f32 m_endLength;    // shared by width & alpha
        f32 m_beginWidth;
        f32 m_midWidth;
        f32 m_endWidth;
        f32 m_beginAlpha;
        f32 m_midAlpha;
        f32 m_endAlpha;
        f32 m_startUV;
        f32 m_tileLength;
        BEZIER_PATCH_UVMODE m_uvMode;
        Color m_color;
        Color m_fogColor;
        ResourceID m_textureID;
        f32 m_depth;
        f32 m_tessellationRatio;
        BezierPatchList m_patches;
        Vec2d m_UVy;
    };

    class BezierCurveRenderer_Template
    {
        DECLARE_SERIALIZE()
    public:
        BezierCurveRenderer_Template() 
            : m_beginLength(2.f)
            , m_endLength(2.f)
            , m_beginWidth(1.f)
            , m_midWidth(1.f)
            , m_endWidth(1.f)
            , m_beginAlpha(0.f)
            , m_midAlpha(1.f)
            , m_endAlpha(0.f)
            , m_uvMode(UV_MODE_OPTIMUM)
            , m_startUV(0.f)
            , m_tileLength(1.f)
            , m_color(Color::white())
            , m_fogColor(0.f, Color::white().m_r, Color::white().m_g, Color::white().m_b)
            , m_tessellationLength(0.1f)
        {
        }

        ITF_INLINE f32 getBeginLength() const { return m_beginLength; }
        ITF_INLINE f32 getEndLength() const { return m_endLength; }
        ITF_INLINE f32 getBeginWidth() const { return m_beginWidth; }
        ITF_INLINE f32 getMidWidth() const { return m_midWidth; }
        ITF_INLINE f32 getEndWidth() const { return m_endWidth; }
        ITF_INLINE f32 getBeginAlpha() const { return m_beginAlpha; }
        ITF_INLINE f32 getMidAlpha() const { return m_midAlpha; }
        ITF_INLINE f32 getEndAlpha() const { return m_endAlpha; }
        ITF_INLINE f32 getStartUV() const { return m_startUV; }
        ITF_INLINE f32 getTileLength() const { return m_tileLength; }
        ITF_INLINE BEZIER_PATCH_UVMODE getMode() const { return m_uvMode; }
        ITF_INLINE const Color& getColor() const { return m_color; }
        ITF_INLINE const Color& getFogColor() const { return m_fogColor; }
        ITF_INLINE const Path& getTextureFile() const { return m_textureFile; }
        ITF_INLINE f32 getTessellationLength() const { return m_tessellationLength; }

    private:
        f32 m_beginLength;  // shared by width & alpha
        f32 m_endLength;    // shared by width & alpha
        f32 m_beginWidth;
        f32 m_midWidth;
        f32 m_endWidth;
        f32 m_beginAlpha;
        f32 m_midAlpha;
        f32 m_endAlpha;
        f32 m_startUV;
        f32 m_tileLength;
        BEZIER_PATCH_UVMODE m_uvMode;
        Color m_color;
        Color m_fogColor;
        Path m_textureFile;
        f32 m_tessellationLength;
    };


    ITF_INLINE BezierCurve::Edge* BezierCurve::getEdgeAt( u32 i )
    {
        ITF_ASSERT(i < m_edges.size());
        if (i < m_edges.size())
            return &m_edges[i]; 
        else
            return NULL;
    }

    ITF_INLINE const BezierCurve::Edge* BezierCurve::getEdgeAt( u32 i ) const
    {
        ITF_ASSERT(i < m_edges.size());
        if (i < m_edges.size())
            return &m_edges[i];
        else
            return NULL;
    }

    ITF_INLINE void BezierCurve::addPoint( const Vec3d & _pos, const Vec3d & _tanA, const Vec3d & _tanB )
    {
        m_points.push_back(Point(_pos,_tanA,_tanB));
    }

    ITF_INLINE Vec3d BezierCurve::getPosAtT( const f32 _t, const u32 _edgeIndex ) const 
    {
        ITF_ASSERT(m_edges.size());
        ITF_ASSERT(_edgeIndex < m_edges.size());
        return m_edges[_edgeIndex].m_bezier.getPos(_t);
    }

    ITF_INLINE Vec3d BezierCurve::getTanAtT( const f32 _t, const u32 _edgeIndex ) const
    {
        ITF_ASSERT(m_edges.size());
        ITF_ASSERT(_edgeIndex < m_edges.size());
        return m_edges[_edgeIndex].m_bezier.getTangent(_t);
    }

    ITF_INLINE Vec3d BezierCurve::getPosAtDistance( const f32 _distance ) const
    {
        const u32 edgeIndex = getEdgeIndexAtDistance(_distance);
        const f32 t = getTAtDistance(_distance, edgeIndex);
        return getPosAtT(t, edgeIndex);
    }

    ITF_INLINE void BezierCurve::getPosTanAtDistance( const f32 _distance, Vec3d& _pos, Vec3d& _tan ) const
    {
        const u32 edgeIndex = getEdgeIndexAtDistance(_distance);
        const f32 t = getTAtDistance(_distance, edgeIndex);
        _pos = getPosAtT(t, edgeIndex);
        _tan = getTanAtT(t, edgeIndex);
    }

    ITF_INLINE Vec3d BezierCurve::getPosAtDistance( u32& _edgeIndex, u32& _sampleIndex, const f32 _distance ) const
    {
        getEdgeIndexAtDistance(_edgeIndex, _sampleIndex, _distance);
        const Edge& edge = m_edges[_edgeIndex];
        const f32 t = edge.getTAtDistance(_sampleIndex, _distance);
        return edge.m_bezier.getPos(t);
    }

    ITF_INLINE void BezierCurve::getPosTanAtDistance( u32& _edgeIndex, u32& _sampleIndex, const f32 _distance, Vec3d& _pos, Vec3d& _tan ) const
    {
        getEdgeIndexAtDistance(_edgeIndex, _sampleIndex, _distance);
        const Edge& edge = m_edges[_edgeIndex];
        const f32 t = edge.getTAtDistance(_sampleIndex, _distance);
        _pos = edge.m_bezier.getPos(t);
        _tan = edge.m_bezier.getTangent(t);
    }

    //////////////////////////////////////////////////////////////////////////
    // BEZIER CURVE RENDERER
    //////////////////////////////////////////////////////////////////////////
    ITF_INLINE f32 BezierCurveRenderer::interpolateAlpha( const f32 _dist, const f32 _totalLength ) const
    {
        const f32 alpha = interpolate(_dist, _totalLength,
            m_beginLength, m_endLength,
            m_beginAlpha, m_midAlpha, m_endAlpha);
        ITF_ASSERT(alpha >= 0.f);
        ITF_ASSERT(alpha <= 1.f);
        return Clamp(alpha, 0.f, 1.f);
    }

    ITF_INLINE f32 BezierCurveRenderer::interpolateWidth( const f32 _dist, const f32 _totalLength ) const
    {
        return interpolate(_dist, _totalLength,
            m_beginLength, m_endLength,
            m_beginWidth, m_midWidth, m_endWidth);
    }
}
#endif // _ITF_BEZIERCURVE_H_


