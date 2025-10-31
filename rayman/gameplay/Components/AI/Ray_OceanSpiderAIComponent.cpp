#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_OCEANSPIDERAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_OceanSpiderAIComponent.h"
#endif //_ITF_RAY_OCEANSPIDERAICOMPONENT_H_

#ifndef _ITF_RAY_AIOCEANSPIDERBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/OceanSpider/Ray_AIOceanSpiderBehavior.h"
#endif //_ITF_RAY_AIOCEANSPIDERBEHAVIOR_H_

#ifndef _ITF_AIRECEIVEHITBEHAVIOR_H_
#include "gameplay/AI/Behaviors/AIReceiveHitBehavior.h"
#endif //_ITF_AIRECEIVEHITBEHAVIOR_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_OceanSpiderAIComponent)
    BEGIN_SERIALIZATION_CHILD(Ray_OceanSpiderAIComponent)
    END_SERIALIZATION()

    //*****************************************************************************

    void Ray_OceanSpiderAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        //SPAWNER->declareNeedsSpawnee(m_actor, &m_spawner, getTemplate()->getSpawnPath());

    }

    //*****************************************************************************

    void Ray_OceanSpiderAIComponent::startDeath()
    {
        //if (m_currentBehavior != m_deathBehavior)
        //{
        //    for (u32 i = 0; i  < getTemplate()->getBones().size(); ++i)
        //    {
        //        u32 index = m_animComponent->getBoneIndex(getTemplate()->getBones()[i]);
        //        if (index != U32_INVALID)
        //        {
        //            Vec3d pos;
        //            m_animComponent->getBonePos(index,pos);
        //            pos.m_z = m_actor->getDepth();
        //            m_spawner.getSpawnee(m_actor->getScene(),pos);
        //        }
        //    }
        //}

        Super::startDeath();
    }

    void Ray_OceanSpiderAIComponent::onBehaviorFinished()
    {
        if ( m_currentBehavior == m_receiveHitBehavior )
        {
            if ( m_behavior != NULL )
            {
                setBehavior( m_behavior );
            }
        }
    }

    void Ray_OceanSpiderAIComponent::processActivate( EventActivate * onActivate )
    {
        if (getTemplate()->getListenToActivateEvent())
        {
            setBehavior(m_behavior);
            Ray_AIOceanSpiderBehavior * behavior = m_behavior->DynamicCast<Ray_AIOceanSpiderBehavior>(ITF_GET_STRINGID_CRC(Ray_AIOceanSpiderBehavior,1190991647));
            behavior->activate(onActivate->getActivated());
        }
    }

    void Ray_OceanSpiderAIComponent::processHit( HitStim* _hit )
    {
        // Check of we use the hit "add"
        if ( ( m_animatedComponent != NULL ) && getTemplate()->getaddHitInputName().isValid() )
        {
            if ( !getTemplate()->getAllowMultiHit() || ( m_currentBehavior == m_deathBehavior ) )
            {
                return;
            }

            if ( _hit->getSender() == m_actor->getRef() )
            {
                // don't get hit by our own stims
                return;
            }

            // can't get hit by friendlies, except for domino hits
            if ( !m_receiveHitBehavior->canReceiveHit(_hit))
            {
                return;
            }

            i32 prevHealth = m_health_current;
            receiveHit( _hit );
            
            // notify the sender that he touched us
            Actor* sender = static_cast<Actor*>(_hit->getSender().getObject());
            if (sender)
            {
                if ( m_fxController != NULL )
                {
                    u32 fxHandle = m_fxController->playFeedback( _hit->getSender(), _hit->getActionFromHit(), m_actor->getRef() );
                    m_fxController->setFXPos( fxHandle, _hit->getFxPos() );
                }

                EventHitSuccessful hitSuccessful;
                hitSuccessful.setSender( m_actor->getRef() );
                hitSuccessful.setHasBeenDamaged( prevHealth != m_health_current );
                sender->onEvent(&hitSuccessful);
            }
            m_hitAddAnimTimer = 0.f;
        }
        else
        {
            Super::processHit(_hit);
        }
    }

    void Ray_OceanSpiderAIComponent::Update( f32 _dt )
    {
        Super::Update( _dt );

        if ( getTemplate()->getaddHitInputName().isValid() )
        {
            m_hitAddAnimTimer = f32_Min( m_hitAddAnimTimer + _dt, 99.0f );
        }

    }

    void Ray_OceanSpiderAIComponent::updateAnimInput()
    {
        Super::updateAnimInput();

        if ( ( m_animatedComponent != NULL ) && getTemplate()->getaddHitInputName().isValid() )
        {
            m_animatedComponent->setInput( getTemplate()->getaddHitInputName(), m_hitAddAnimTimer ) ;//m_hitAddAnimTimer );        
        }
    }

    //*****************************************************************************
    // Template
    //*****************************************************************************

    IMPLEMENT_OBJECT_RTTI(Ray_OceanSpiderAIComponent_Template)
        BEGIN_SERIALIZATION_CHILD(Ray_OceanSpiderAIComponent_Template)
        //SERIALIZE_CONTAINER("bones", m_bones);
        //SERIALIZE_MEMBER( "spawnPath", m_spawnPath );
        SERIALIZE_MEMBER( "addHitInputName", m_addHitInputName );
    END_SERIALIZATION()



}