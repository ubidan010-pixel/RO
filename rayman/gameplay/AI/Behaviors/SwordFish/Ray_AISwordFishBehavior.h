#ifndef _ITF_RAY_AISWORDFISHBEHAVIOR_H_
#define _ITF_RAY_AISWORDFISHBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDBASEBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundBaseBehavior.h"
#endif //_ITF_RAY_AIGROUNDBASEBEHAVIOR_H_

namespace ITF
{
    class Ray_AISwordFishBehavior : public Ray_AIGroundBaseBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AISwordFishBehavior, Ray_AIGroundBaseBehavior,1364897805)

    public:

        Ray_AISwordFishBehavior();
        virtual ~Ray_AISwordFishBehavior();

        virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);


        virtual void                        update( f32 _delta  );


        virtual void                        onActivate();
        virtual void                        onDeactivate();
        virtual void                        onActionFinished();
        virtual void                        onEvent(Event * _event);
        virtual void                        onActorBecomeActive();
#ifdef ITF_SUPPORT_EDITOR
        virtual void                        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

    protected:
        ITF_INLINE const class Ray_AISwordFishBehavior_Template * getTemplate() const;
        void startIdle();
        void updateStim(f32 _delta);
        void  updateAttack( f32 _delta );
        void startAnticipation();
        bbool checkDetection();
        void startAttack();
        void startStuck();


    private:

        AIAction * m_idle;
        AIAction * m_anticipation;
        AIAction * m_attack;
        AIAction * m_stuck;
        PhysShape * m_detectionShape;
        f32 m_timer;
        f32 m_prevGravityMultiplier;
        AIUtils::HitShape   m_shape;
        ITF_MAP<ObjectRef,f32> m_memorizedHits;
        class Ray_SubAnchorComponent* m_subAnchorComponent;
        class PhantomComponent * m_phantomComponent;
        Vec3d m_endPos;
        Vec2d m_prevPos;
    };

    class Ray_AISwordFishBehavior_Template : public Ray_AIGroundBaseBehavior_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AISwordFishBehavior_Template,Ray_AIGroundBaseBehavior_Template,3772646552);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AISwordFishBehavior);

    public:

        Ray_AISwordFishBehavior_Template();
        ~Ray_AISwordFishBehavior_Template();

        ITF_INLINE const AIAction_Template * getIdle() const { return m_idle; }
        ITF_INLINE const AIAction_Template * getAnticipation() const { return m_anticipation; }
        ITF_INLINE const AIAction_Template * getAttack() const { return m_attack; }
        ITF_INLINE const AIAction_Template * getStuck() const { return m_stuck; }
        ITF_INLINE const PhysShape * getDetectionShape() const { return m_detectionShape; }
        ITF_INLINE f32 getAnticipationDuration() const { return m_anticipationDuration; }
        ITF_INLINE f32 getForce() const { return m_force; }
        ITF_INLINE f32 getMaxSpeed() const { return m_maxSpeed; }
        ITF_INLINE f32 getMinSpeed() const { return m_minSpeed; }
        ITF_INLINE f32 getOffsetDist() const { return m_offsetDist; }
    private:

        AIAction_Template * m_idle;
        AIAction_Template * m_anticipation;
        AIAction_Template * m_attack;
        AIAction_Template * m_stuck;
        PhysShape * m_detectionShape;
        f32 m_anticipationDuration;
        f32 m_force;
        f32 m_maxSpeed;
        f32 m_minSpeed;
        f32 m_offsetDist;
    };
    ITF_INLINE const Ray_AISwordFishBehavior_Template * Ray_AISwordFishBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AISwordFishBehavior_Template *>(m_template);
    }

}

#endif //_ITF_RAY_AISWORDFISHBEHAVIOR_H_