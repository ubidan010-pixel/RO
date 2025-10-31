#ifndef _ITF_RAY_AIWATERBASEBEHAVIOR_H_
#define _ITF_RAY_AIWATERBASEBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{
    class Ray_AIWaterBaseBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIWaterBaseBehavior, AIBehavior,463503610);

    public:

        Ray_AIWaterBaseBehavior();
        virtual ~Ray_AIWaterBaseBehavior();

        virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                        onActivate();
        virtual void                        onDeactivate();
        virtual void                        update( f32 _dt );
        virtual void                        updateSoundInputs();

        void                                setSwimingPolyline( ObjectRef _ref );
        ObjectRef                           getSwimingPolyline() const { return m_swimingPolyline; }

        u32                                 getSurfaceEdge() const { return m_surfaceEdge; }
        f32                                 getSurfaceEdgeT() const { return m_surfaceEdgeT; }
        u32                                 getClosestEdge() const { return m_closestEdge; }
        f32                                 getClosestEdgeT() const { return m_closestEdgeT; }
        bbool                               getIsOnSurface() const { return m_onSurface; }
        bbool                               getIsInside() const { return m_inside; }
        f32                                 getWaterResistance() const { return m_waterResistance; }
        f32                                 getDistanceFromWater() const { return m_distanceFromWater; }

    private:

        ITF_INLINE const class Ray_AIWaterBaseBehavior_Template * getTemplate() const;

        void                                setSwimingPolylineParameters( PolyLine* _poly );

        StickToPolylinePhysComponent*       m_physComponent;
        ObjectRef                           m_swimingPolyline;
        u32                                 m_surfaceEdge;
        f32                                 m_surfaceEdgeT;
        u32                                 m_closestEdge;
        f32                                 m_closestEdgeT;
        f32                                 m_waterResistance;
        f32                                 m_distanceFromWater;
        f32                                 m_distanceFromSurface;
        bbool                               m_onSurface;
        bbool                               m_inside;
        bbool                               m_active;
    };

    class Ray_AIWaterBaseBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIWaterBaseBehavior_Template,TemplateAIBehavior,3065357194);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIWaterBaseBehavior);

    public:

        Ray_AIWaterBaseBehavior_Template();
        ~Ray_AIWaterBaseBehavior_Template(){}

        f32                                 getGravityMultiplier() const { return m_gravityMultiplier; }

    private:

        f32                                 m_gravityMultiplier;
    };

    ITF_INLINE const class Ray_AIWaterBaseBehavior_Template * Ray_AIWaterBaseBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIWaterBaseBehavior_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AIWATERBASEBEHAVIOR_H_

