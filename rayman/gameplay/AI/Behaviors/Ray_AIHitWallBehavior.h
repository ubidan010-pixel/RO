#ifndef _ITF_RAY_AIHITWALLBEHAVIOR_H_
#define _ITF_RAY_AIHITWALLBEHAVIOR_H_

#ifndef _ITF_AIRECEIVEHITBEHAVIOR_H_
#include "gameplay/AI/Behaviors/AIReceiveHitBehavior.h"
#endif //_ITF_AIRECEIVEHITBEHAVIOR_H_

namespace ITF
{
    class PhantomComponent;

    class Ray_AIHitWallBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIHitWallBehavior, AIBehavior,3784536657)
        DECLARE_VALIDATE_BEHAVIOR()

    public:

        Ray_AIHitWallBehavior();
        virtual ~Ray_AIHitWallBehavior();

        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                onActivate();
        virtual void                onDeactivate();
        virtual void                onActionFinished();
        virtual void                updateAnimInputs();
        virtual void                update( f32 _dt );
        bbool                       canHitWall(class EventBlockedByPolyline* _event);
        void                        setData(class EventBlockedByPolyline* _event);
    private:

        ITF_INLINE const class Ray_AIHitWallBehavior_Template * getTemplate() const;

        void                        startHitWall( );
        void                        restoreAfterHitWall();

        PhantomComponent*           m_phantomComponent;
        StickToPolylinePhysComponent* m_physComponent;
        AIAction*                       m_hitWall;

        bbool                       m_isInAir;
        u32                         m_wallOrientation;
        f32                         m_angleBeforeHitWall;
        Vec2d                       m_posBeforeHitWall;
        bbool                       m_justEntered;
        bbool                       m_flippedOnHitWall;
        ObjectRef                   m_polyline;
        u32                         m_edgeIndex;
        Vec2d                       m_pos;
    };

    class Ray_AIHitWallBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIHitWallBehavior_Template,TemplateAIBehavior,2941850052);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIHitWallBehavior);

    public:


        Ray_AIHitWallBehavior_Template();
        ~Ray_AIHitWallBehavior_Template();

        ITF_INLINE const AIAction_Template*                getHitWall() const { return m_hitWall; }
        ITF_INLINE f32                                     getHalfWallHeight() const { return m_halfWallHeight; }
        ITF_INLINE f32                                     getMinimumHeightForWallHit() const { return m_minimumHeightForWallHit; }
        ITF_INLINE bbool                                   getFlipOnHitWall() const { return m_flipOnHitWall; }
        ITF_INLINE Angle                                   getMinIncidenceAngleForWallHit() const { return m_minIncidenceAngleForWallHit; }
        ITF_INLINE f32                                     getMinSpeedForWallHit() const { return m_minSpeedForWallHit; }

    private:

        AIAction_Template*                      m_hitWall;
        f32                                     m_halfWallHeight;
        f32                                     m_minimumHeightForWallHit;
        Angle                                   m_minIncidenceAngleForWallHit;
        f32                                     m_minSpeedForWallHit;
        bbool                                   m_flipOnHitWall;
    };

    ITF_INLINE const class Ray_AIHitWallBehavior_Template * Ray_AIHitWallBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIHitWallBehavior_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AIHITWALLBEHAVIOR_H_