#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_ANGLERFISHAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AnglerFishAIComponent.h"
#endif //_ITF_RAY_ANGLERFISHAICOMPONENT_H_

#ifndef _ITF_RAY_AIOCEANSPIDERBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/OceanSpider/Ray_AIOceanSpiderBehavior.h"
#endif //_ITF_RAY_AIOCEANSPIDERBEHAVIOR_H_

#ifndef _ITF_AIRECEIVEHITBEHAVIOR_H_
#include "gameplay/AI/Behaviors/AIReceiveHitBehavior.h"
#endif //_ITF_AIRECEIVEHITBEHAVIOR_H_

#ifndef _ITF_RAY_ABYSSLIGHTCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_AbyssLightComponent.h"
#endif //_ITF_RAY_ABYSSLIGHTCOMPONENT_H_

#ifndef _ITF_TWEENCOMPONENT_H_
#include "gameplay/Components/Common/TweenComponent.h"
#endif //_ITF_TWEENCOMPONENT_H_

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_AnglerFishAIComponent)
    BEGIN_SERIALIZATION_CHILD(Ray_AnglerFishAIComponent)
    END_SERIALIZATION()

    //*****************************************************************************

    void Ray_AnglerFishAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        if ( !getTemplate()->getLightSpawnPath().isEmpty() )
        {
            SPAWNER->declareNeedsSpawnee( m_actor, &m_lightSpawner, getTemplate()->getLightSpawnPath() );
        }
    }

    //*****************************************************************************
    void Ray_AnglerFishAIComponent::onBecomeActive()
    {
        Vec3d pos = m_actor->getPos();

        if ( m_animComponent != NULL )
        {
            m_boneIndex = m_animComponent->getBoneIndex( getTemplate()->getSpawnBone() );
            if ( m_boneIndex != U32_INVALID)
            {
                m_animComponent->getBonePos( m_boneIndex, pos, btrue );
                pos.m_z = m_actor->getDepth();                
            }
        }

        if ( m_lightSpawner.isInitialized() && ( m_lightActor.getObject() == NULL ) )
        {
            Actor * actor = m_lightSpawner.getSpawnee( m_actor->getScene(), pos );
            if ( actor != NULL )
            {
                m_lightActor = actor->getRef();
                m_actor->getBinding()->bindChild( m_lightActor );
            }            
        }
    }
     //*****************************************************************************
    void Ray_AnglerFishAIComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();

        /*m_actor->getBinding()->unbindChild( m_lightActor );
        m_actor->getBinding()->forceRemovePending();*/
    }

    //*****************************************************************************

    void Ray_AnglerFishAIComponent::startDeath()
    {
        if  ( m_currentBehavior != m_deathBehavior )
        {
            Actor * light = AIUtils::getActor( m_lightActor );
            if ( light != NULL )
            {
                // unbind 
                m_actor->getBinding()->unbindChild( m_lightActor );
                m_lightActor.invalidate();

                // stop the light with a delay
                Ray_AbyssLightComponent * ligtComp = light->GetComponent<Ray_AbyssLightComponent>();
                if ( ligtComp != NULL )
                {
                    ligtComp->stopWithDelay( getTemplate()->getDeathLightStopDelay(), btrue );
                }
            }

            TweenComponent * tweenComp = m_actor->GetComponent<TweenComponent>();
            if ( tweenComp != NULL )
            {
                tweenComp->suspendTweening();
            }

            EventDisableCollision disableColl(btrue);
            m_actor->onEvent( &disableColl );
        }
        Super::startDeath();
    }

    //*****************************************************************************
    void Ray_AnglerFishAIComponent::Update( f32 _dt )
    {
        Super::Update( _dt );

        if  ( m_currentBehavior != m_deathBehavior )
        {
            Actor * light = AIUtils::getActor( m_lightActor );
            if ( light != NULL )
            {
                Vec3d pos = m_actor->getPos();

                if ( ( m_animComponent != NULL ) && ( m_boneIndex != U32_INVALID ) )
                {
                    m_animComponent->getBonePos( m_boneIndex, pos, btrue );
                    pos.m_z = 0.0f;
                }
                light->setPos( pos );
            }
        }
    }

    void Ray_AnglerFishAIComponent::onBehaviorFinished()
    {
        if ( m_currentBehavior == m_receiveHitBehavior )
        {
            if ( m_behavior != NULL )
            {
                setBehavior( m_behavior );
            }
        }
    }

    /*void Ray_AnglerFishAIComponent::processActivate( EventActivate * onActivate )
    {
        if (getTemplate()->getListenToActivateEvent())
        {
            setBehavior(m_behavior);
            Ray_AIOceanSpiderBehavior * behavior = m_behavior->DynamicCast<Ray_AIOceanSpiderBehavior>(ITF_GET_STRINGID_CRC(Ray_AIOceanSpiderBehavior,1190991647));
            behavior->activate(onActivate->getActivated());
        }
    }*/

    //*****************************************************************************
    // Template
    //*****************************************************************************

    IMPLEMENT_OBJECT_RTTI(Ray_AnglerFishAIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_AnglerFishAIComponent_Template)
        SERIALIZE_MEMBER( "spawnBone",              m_spawnBone );
        SERIALIZE_MEMBER( "lightSpawnPath",         m_lightSpawnPath );
        SERIALIZE_MEMBER( "deathLightStopDelay",    m_deathLightStopDelay );
    END_SERIALIZATION()



}