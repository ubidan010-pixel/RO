#ifndef _ITF_RAY_AIPLATFORMENEMYCOMPONENT_H_
#define _ITF_RAY_AIPLATFORMENEMYCOMPONENT_H_

#ifndef _ITF_RAY_AIGROUNDENEMYCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_GroundEnemyAIComponent.h"
#endif //_ITF_RAY_AIGROUNDENEMYCOMPONENT_H_

namespace ITF
{
    class AIBehavior;
    class Ray_AIGroundRoamBehavior;
    class Ray_AIReceiveHitBehavior;

    class Ray_PlatformEnemyAIComponent : public Ray_GroundEnemyAIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_PlatformEnemyAIComponent, Ray_GroundEnemyAIComponent,1233007951)
        DECLARE_VALIDATE_COMPONENT()
        DECLARE_SERIALIZE();

    public:

        Ray_PlatformEnemyAIComponent();
        virtual ~Ray_PlatformEnemyAIComponent();

        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                onCheckpointLoaded();
        virtual void                Update( f32 _dt );
        virtual void                onBehaviorFinished();
        virtual void                onEvent( Event* _event);

        virtual void                updateAnimInput();
    protected:
        virtual void                startDeath();
        virtual void                startReceiveHit( HitStim* _hit );
        virtual void                startCrushed();
        virtual void                startCloseRangeAttack(const bbool _flip);
        virtual void                onFinishedCloseRangeAttack();
    private:
        ITF_INLINE const class Ray_PlatformEnemyAIComponent_Template * getTemplate() const;
        class TweenComponent *      m_tweenComponent;
        Vec2d                       m_previousPos;
        Vec2d                       m_speed;
    };


    //---------------------------------------------------------------------------------------------------------------------------------------

    class Ray_PlatformEnemyAIComponent_Template : public Ray_GroundEnemyAIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_PlatformEnemyAIComponent_Template, Ray_GroundEnemyAIComponent_Template,2213022351);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_PlatformEnemyAIComponent);

    public:

        Ray_PlatformEnemyAIComponent_Template();
        ~Ray_PlatformEnemyAIComponent_Template();

        ITF_INLINE bbool getRestartTweeningAfterHit() const { return m_restartTweeningAfterHit; }

    private:

        bbool m_restartTweeningAfterHit;
    };
    ITF_INLINE const Ray_PlatformEnemyAIComponent_Template * Ray_PlatformEnemyAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_PlatformEnemyAIComponent_Template *>(m_template);
    }

}

#endif //_ITF_RAY_AIPLATFORMENEMYCOMPONENT_H_
