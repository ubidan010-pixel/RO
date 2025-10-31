#ifndef _ITF_RAY_BUBBLEAICOMPONENT_H_
#define _ITF_RAY_BUBBLEAICOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

namespace ITF
{
    class Ray_BubbleAiComponent : public Ray_AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BubbleAiComponent, Ray_AIComponent,2615864492)
        DECLARE_VALIDATE_COMPONENT()
        DECLARE_SERIALIZE();

    public:

        Ray_BubbleAiComponent();
        virtual ~Ray_BubbleAiComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        Update( f32 _dt );
        virtual void        updateAnimInput();

        ITF_INLINE void     setOwner(ActorRef val) { m_owner = val; }
        ITF_INLINE void     setScale(bbool _fast) { m_fastScale = _fast; }
                   void     setLifetime( const f32 _value );
                   void     setFloatForceX( const f32 _value );
        virtual f32         getSoftCollisionRadius() const;

    private:
        ITF_INLINE const class Ray_BubbleAiComponent_Template * getTemplate() const;

        class AnimatedComponent* m_animatedComponent;
        class PhantomComponent* m_phantomComponent;
        class PhysComponent* m_physComponent;

        class Ray_AIBubbleDeathBehavior* m_bubbleBehavior;

        ActorRef m_owner;
        bbool m_fastScale;
        f32 m_scaleInput;
        f32 m_phantomInitialScale;

        void release();
    };

    //-------------------------------------------------------------------------------------
    class Ray_BubbleAiComponent_Template : public Ray_AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BubbleAiComponent_Template, Ray_AIComponent_Template,3255795298);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_BubbleAiComponent);

    public:

        Ray_BubbleAiComponent_Template();
        ~Ray_BubbleAiComponent_Template();
        ITF_INLINE f32 getScaleSpeed() const { return m_scaleSpeed; }
        ITF_INLINE f32 getFastScaleSpeed() const { return m_fastScaleSpeed; }
        ITF_INLINE f32 getMinScale() const { return m_minScale; }
        ITF_INLINE f32 getMaxScale() const { return m_maxScale; }
        //ITF_INLINE StringID getIdleAnim() const { return m_idleAnim; }
        ITF_INLINE StringID getOwnerSnapBone() const { return m_ownerSnapBone; }
        ITF_INLINE StringID getSnapBone() const { return m_snapBone; }
        //ITF_INLINE const Vec2d & getMaxSpeed() const { return m_maxSpeed; }
        //ITF_INLINE const Vec2d & getForce() const { return m_force; }
        //ITF_INLINE f32 getSinkMaxSpeed() const { return m_sinkMaxSpeed; }
        //ITF_INLINE f32 getSinkForce() const { return m_sinkForce; }
        //ITF_INLINE u32 getNumReducedActors() const { return m_numReducedActors; }
        //ITF_INLINE StringID getExplodeAnim() const { return m_explodeAnim; }
        ITF_INLINE const class Ray_AIBubbleDeathBehavior_Template * getBubbleBehavior() const { return m_bubbleBehavior; }

    private:
        class Ray_AIBubbleDeathBehavior_Template * m_bubbleBehavior;
        f32 m_scaleSpeed;
        f32 m_fastScaleSpeed;
        f32 m_minScale;
        f32 m_maxScale;
        //StringID m_idleAnim;
        StringID m_ownerSnapBone;
        StringID m_snapBone;
        //StringID m_explodeAnim;
        //Vec2d m_maxSpeed;
        //Vec2d m_force;
        //f32 m_sinkMaxSpeed;
        //f32 m_sinkForce;
        //u32 m_numReducedActors;
    };

    ITF_INLINE const Ray_BubbleAiComponent_Template* Ray_BubbleAiComponent::getTemplate() const
    {
        return static_cast<const Ray_BubbleAiComponent_Template*>(m_template);
    }

};

#endif //_ITF_RAY_BUBBLEAICOMPONENT_H_