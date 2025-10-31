#ifndef _ITF_SOLIDPOLYLINECOMPONENT_H_
#define _ITF_SOLIDPOLYLINECOMPONENT_H_

#ifndef _ITF_POLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/PolylineComponent.h"
#endif //_ITF_POLYLINECOMPONENT_H_

namespace ITF
{
    /*
        This component builds stims for fast-moving platform,
        to ensure they won't go through characters.

        We can define several 'solid' edges to track.
        If they move more than a certain delta, we send a SolidStim.

        The delta is (arbitrarily) determined using the start position of the edge,
        and oriented using its normal.
        The shape is built using prev/current position, and ordered using this direction as well 
        (points must always be clockwise otherwise collisions will go through).
    */
    class SolidPolylineComponent : public PolylineComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(SolidPolylineComponent, PolylineComponent,1361568548);
        void                postLoadProperties();

    public:
        DECLARE_SERIALIZE();

        SolidPolylineComponent();
        virtual ~SolidPolylineComponent();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

    protected:

        const class SolidPolylineComponent_Template* getTemplate() const;

        virtual void        readModel();
        virtual void        processAnimUpdated( class EventAnimUpdated* _animUpdated );

        struct SolidEdgeData
        {
            SolidEdgeData() : m_boneIndex(U32_INVALID) {}

            u32                     m_boneIndex;
            Vec2d                   m_prevPos;
            Vec2d                   m_prevPosEnd;
        };

        typedef ITF_VECTOR <SolidEdgeData> SolidEdgeList;
        SolidEdgeList                   m_solidEdges;

        PhysShapePolygon                m_stimShape;
        PhysShapePolygon::PointsList    m_stimPoints;
    };

    //---------------------------------------------------------------------------------------------------

    class SolidPolylineComponent_Template : public PolylineComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(SolidPolylineComponent_Template,PolylineComponent_Template,3737161728);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(SolidPolylineComponent);

    public:

        struct SolidEdgeData
        {
            DECLARE_SERIALIZE();

            SolidEdgeData() : m_minDelta(0.f) {}

            StringID                m_boneName;
            f32                     m_minDelta;
        };

        typedef ITF_VECTOR <SolidEdgeData> SolidEdgeList;

        SolidPolylineComponent_Template() {}
        ~SolidPolylineComponent_Template() {}

        const SolidEdgeList&            getSolidEdges() const { return m_solidEdges; }

    private:

        SolidEdgeList                   m_solidEdges;
    };

    ITF_INLINE const SolidPolylineComponent_Template* SolidPolylineComponent::getTemplate() const
    {
        return static_cast<const SolidPolylineComponent_Template*>(m_template);
    }
};

#endif //_ITF_SOLIDPOLYLINECOMPONENT_H_