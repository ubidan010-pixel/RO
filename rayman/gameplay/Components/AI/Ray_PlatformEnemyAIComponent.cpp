#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIPLATFORMENEMYCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_PlatformEnemyAIComponent.h"
#endif //_ITF_RAY_AIPLATFORMENEMYCOMPONENT_H_

#ifndef _ITF_TWEENCOMPONENT_H_
#include "gameplay/Components/Common/TweenComponent.h"
#endif //_ITF_TWEENCOMPONENT_H_

#ifndef _ITF_RAY_AIRECEIVEHITBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ray_AIReceiveHitBehavior.h"
#endif //_ITF_RAY_AIRECEIVEHITBEHAVIOR_H_



namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_PlatformEnemyAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_PlatformEnemyAIComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_PlatformEnemyAIComponent)
    VALIDATE_COMPONENT_PARAM("", m_tweenComponent, "Ray_PlatformEnemyAIComponent requires a TweenComponent");
    END_VALIDATE_COMPONENT()


    Ray_PlatformEnemyAIComponent::Ray_PlatformEnemyAIComponent() 
        : Super()
        , m_tweenComponent(NULL)
        , m_previousPos(Vec2d::Zero)
        , m_speed(Vec2d::Zero)
    {
    }

    Ray_PlatformEnemyAIComponent::~Ray_PlatformEnemyAIComponent()
    {
    }



    void Ray_PlatformEnemyAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_rayCastInitPos = bfalse;
        Super::onActorLoaded(_hotReload);

        m_tweenComponent = m_actor->GetComponent<TweenComponent>();

        if (m_physComponent)
            m_physComponent->setDisabled(btrue);
    }

    void Ray_PlatformEnemyAIComponent::onCheckpointLoaded()
    {
        Super::onCheckpointLoaded();
    }

    void Ray_PlatformEnemyAIComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        m_speed = m_actor->get2DPos() - m_previousPos;
        m_previousPos = m_actor->get2DPos();
    }

    void Ray_PlatformEnemyAIComponent::onBehaviorFinished()
    {
        if ( m_currentBehavior == m_receiveHitBehavior )
        {
            if ( getTemplate()->getRestartTweeningAfterHit() )
            {
                if (m_tweenComponent)   
                {
                    m_tweenComponent->resumeTweening();
                }
            }
        }
        Super::onBehaviorFinished();
    }

    void Ray_PlatformEnemyAIComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

    }

    void Ray_PlatformEnemyAIComponent::updateAnimInput()
    {
        Super::updateAnimInput();

        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(Speed, 4218325830), m_speed.norm());
    }

    void Ray_PlatformEnemyAIComponent::startDeath()
    {
        if (m_tweenComponent)   
            m_tweenComponent->suspendTweening();
        m_physComponent->setDisabled(bfalse);
        Super::startDeath(  );
    }

    void Ray_PlatformEnemyAIComponent::startReceiveHit( HitStim* _hit )
    {
        
        if (m_tweenComponent)
            m_tweenComponent->suspendTweening();

        if ( !getTemplate()->getRestartTweeningAfterHit() )
        {
            m_physComponent->setDisabled(bfalse);
        }
                
        Super::startReceiveHit( _hit );
    }

    void Ray_PlatformEnemyAIComponent::startCrushed()
    {
        if (m_tweenComponent)
            m_tweenComponent->suspendTweening();
        Super::startCrushed(  );
    }

    void Ray_PlatformEnemyAIComponent::startCloseRangeAttack( const bbool _flip )
    {
        if (m_tweenComponent)
            m_tweenComponent->suspendTweening();
        Super::startCloseRangeAttack(_flip);
    }

    void Ray_PlatformEnemyAIComponent::onFinishedCloseRangeAttack()
    {
        if (m_tweenComponent)
            m_tweenComponent->resumeTweening();
        Super::onFinishedCloseRangeAttack();

    }

    //----------------------------------------------------------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_PlatformEnemyAIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_PlatformEnemyAIComponent_Template)
        SERIALIZE_MEMBER( "restartTweeningAfterHit", m_restartTweeningAfterHit );
    END_SERIALIZATION()

    Ray_PlatformEnemyAIComponent_Template::Ray_PlatformEnemyAIComponent_Template()
    : Super()
    , m_restartTweeningAfterHit( bfalse )
    {
    }

    Ray_PlatformEnemyAIComponent_Template::~Ray_PlatformEnemyAIComponent_Template()
    {
    }

} // namespace ITF

