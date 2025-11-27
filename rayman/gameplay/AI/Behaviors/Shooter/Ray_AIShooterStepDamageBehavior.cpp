#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AISHOOTERSTEPDAMAGEBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Shooter/Ray_AIShooterStepDamageBehavior.h"
#endif //_ITF_RAY_AISHOOTERSTEPDAMAGEBEHAVIOR_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIShooterStepDamageBehavior)
		BEGIN_VALIDATE_BEHAVIOR(Ray_AIShooterStepDamageBehavior)
    END_VALIDATE_BEHAVIOR()


    Ray_AIShooterStepDamageBehavior::Ray_AIShooterStepDamageBehavior()
        : Super()
        ,m_curStepDamage(NULL)
    {
    }

    Ray_AIShooterStepDamageBehavior::~Ray_AIShooterStepDamageBehavior()
    {
    }

	//---------------------------------------------------------------------------------------------------------------
	void Ray_AIShooterStepDamageBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
	{
		// after action add
		Super::onActorLoaded(_hotReload);
        
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(HitStim,2121727796), m_aiComponent );
        //EventHitSuccessful
	}

	//---------------------------------------------------------------------------------------------------------------
	void Ray_AIShooterStepDamageBehavior::onActivate()
	{
        m_curStepDamage = getTemplate()->getStepDamageFromHealth( m_aiComponent->getHealth() );
        if ( m_curStepDamage != NULL )
        {
            playAnimation( m_curStepDamage->m_defaultAnim );
        }
	}

	//---------------------------------------------------------------------------------------------------------------
	void Ray_AIShooterStepDamageBehavior::onEvent( Event * _event )
	{
		Super::onEvent( _event );

		// Is received only at the behavior start
		//------------------------------------------
		if ( HitStim * hit = _event->DynamicCast<HitStim>(ITF_GET_STRINGID_CRC(HitStim,2121727796)) )
		{
            bbool receiveDamage = AIUtils::isDamageHit(hit, m_aiComponent->getFaction()) || hit->DynamicCast<RehitStim>(ITF_GET_STRINGID_CRC(RehitStim,1008690800));
            m_aiComponent->receiveHit(hit, receiveDamage, 0);

            m_curStepDamage = getTemplate()->getStepDamageFromHealth( m_aiComponent->getHealth() );
            if ( m_curStepDamage != NULL )
            {
                playAnimation( m_curStepDamage->m_hitAnim );

                if ( m_fxController != NULL )
                {
                    const StringID feedbackAction = hit->getActionFromHit();
                    u32 fxHandle = m_fxController->playFeedback(hit->getSender(), feedbackAction, m_actor->getRef());
                    m_fxController->setFXPosFromHandle(fxHandle, hit->getFxPos());
                }
            }
            
            if ( Actor * sender = AIUtils::getActor( hit->getSender() ) )
            {
                EventHitSuccessful success;
                success.setSender( m_actor->getRef() );
                sender->onEvent( &success );
            }
		}
    /*    else if ( EventHitSuccessful* hitSuccessful = _event->DynamicCast<EventHitSuccessful>(ITF_GET_STRINGID_CRC(EventHitSuccessful,818883013)) )
        {
            onHitSuccessful(hitSuccessful);
        }*/
	}

    //---------------------------------------------------------------------------------------------------------------
    /*void Ray_AIShooterStepDamageBehavior::onHitSuccessful( EventHitSuccessful * _hitSuccessfulEvt )
    {
        if ( getTemplate()->getHitNumber() > 0 )
        {
            m_hitCount++; 
            if ( m_hitCount >= getTemplate()->getHitNumber() )
            {
                ejectActionFinished( bfalse );                
            }
        }
    }*/

    //---------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterStepDamageBehavior::playAnimation( const StringID & _anim )
    {
        if ( m_animComponent )
        {
            m_animComponent->resetCurTime();
            m_animComponent->setAnim( _anim );
        }
        else if ( m_animatedComponent )
        {
            m_animatedComponent->resetCurTime();
            m_animatedComponent->setAnim( _anim );
        }
        m_curAnim = _anim;
    }

    //---------------------------------------------------------------------------------------------------------------
    bbool Ray_AIShooterStepDamageBehavior::isAnimFinished() const
    {
        if ( m_animatedComponent )
        {
            return m_animatedComponent->isMainNodeFinished();
        }
        else if ( m_animComponent )
        {
            return m_animComponent->isSubAnimFinished();
        }
        
        return bfalse;
    }   

	//---------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterStepDamageBehavior::update( f32 _delta  )
    {
        Super::update(_delta);

        if ( m_curStepDamage != NULL )
        {
            if ( ( m_curAnim != m_curStepDamage->m_defaultAnim ) && isAnimFinished() )
            {
                playAnimation( m_curStepDamage->m_defaultAnim );
            }        
        }
    }
	//---------------------------------------------------------------------------------------------------------------
	IMPLEMENT_OBJECT_RTTI(Ray_AIShooterStepDamageBehavior_Template)
	BEGIN_SERIALIZATION_CHILD(Ray_AIShooterStepDamageBehavior_Template)
		SERIALIZE_CONTAINER_OBJECT("stepDamageList",	m_stepDamageList );
	END_SERIALIZATION()

    BEGIN_SERIALIZATION( StepDamage )
        SERIALIZE_MEMBER( "startHealth" ,   m_startHealth );
        SERIALIZE_MEMBER( "hitAnim" ,       m_hitAnim );
        SERIALIZE_MEMBER( "defaultAnim" ,   m_defaultAnim );
    END_SERIALIZATION()

	Ray_AIShooterStepDamageBehavior_Template::Ray_AIShooterStepDamageBehavior_Template()
	: Super()
	{
	}

	Ray_AIShooterStepDamageBehavior_Template::~Ray_AIShooterStepDamageBehavior_Template()
	{

	}

    const StepDamage *	Ray_AIShooterStepDamageBehavior_Template::getStepDamageFromHealth( i32 _health )	const
    {
        i32 foundIdx = -1;

        for ( u32 i = 0; i < m_stepDamageList.size(); ++i )
        {
            if ( m_stepDamageList[i].m_startHealth >= _health )
            {
                foundIdx = (i32)i;
            }
        }
        if ( foundIdx>= 0 )
        {
            return &m_stepDamageList[foundIdx];
        }
        return NULL;
    }

}
