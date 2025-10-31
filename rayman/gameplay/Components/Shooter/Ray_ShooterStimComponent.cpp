#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_RAY_SHOOTERSTIMCOMPONENT_H_
#include "rayman/gameplay/Components/Shooter/Ray_ShooterStimComponent.h"
#endif //_ITF_RAY_SHOOTERSTIMCOMPONENT_H_

#ifdef SHOOTER_PROFILING
#include "rayman/gameplay/Managers/Ray_ShooterGameModeController.h"
#include "rayman/gameplay/Ray_GameManager.h"
#endif //SHOOTER_PROFILING

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_ShooterStimComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_ShooterStimComponent)
    END_SERIALIZATION()

	//--------------------------------------------------------------------------------------------------------------------------------
    Ray_ShooterStimComponent::Ray_ShooterStimComponent()
        : Super()
        , m_fxTimerBeforeDestroy( 0.0f )
        , m_bulletObsolete( bfalse )
        , m_fxcontroller( NULL )
        , m_stopped( bfalse )
    {
    }

    Ray_ShooterStimComponent::~Ray_ShooterStimComponent()
    {
    }
	//--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterStimComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
		Super::onActorLoaded(_hotReload);
        m_bulletAbility.init( m_actor, getTemplate()->getBasicBulletTemplate() );
        m_fxcontroller = m_actor->GetComponent<FXControllerComponent>();

        // Remove visible optim if auto removed out of screen
        if ( m_bulletAbility.getDestroyOnExitScreen() )
        {
            m_actor->setUpdateType( Pickable::UpdateType_OffscreenAllowed );
        }

#ifdef SHOOTER_PROFILING
        SP_ADD( "ShooterStim", m_actor->getRef() )
#endif //SHOOTER_PROFILING
    }

	//--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterStimComponent::onBecomeActive()
    {
        Super::onBecomeActive();
		m_bulletAbility.onBecomeActive();
        m_bulletObsolete = bfalse;
        m_fxTimerBeforeDestroy = 0.0f;
        m_stopped = bfalse;

        GraphicComponent * graphComp = m_actor->GetComponent<GraphicComponent>();
        if ( graphComp != NULL )
        {
            graphComp->setAlpha( 1.f );
        }        
    }

	//--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterStimComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();
        m_bulletAbility.onBecomeInactive();

        // destroy/disable the object if it has been disabled by the engine
        if ( !m_stopped )
        {
            stop();
        }
    }
	
	//--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterStimComponent::Update(f32 _dt)
    {
        Super::Update(_dt);
		
		// Update bullet
		m_bulletAbility.update(_dt);
		if ( m_bulletAbility.isObsolete() )
		{
            if ( !m_bulletObsolete )
            {
                m_fxTimerBeforeDestroy = 0.f;
                //if ( m_bulletAbility.hasHit() )
                {
                    m_fxTimerBeforeDestroy = getTemplate()->getFxDelayBeforeDestroy();
                    GraphicComponent * graphComp = m_actor->GetComponent<GraphicComponent>();
                    if ( graphComp != NULL )
                    {
                        graphComp->setAlpha( 0.f );
                    }
                   /* EventShow hideactor( 0.0f, 0.0f );
                    m_actor->onEvent(&hideactor);*/
                }                
            }
            m_bulletObsolete = btrue;
		}

        if ( m_bulletObsolete )
        {
            m_fxTimerBeforeDestroy -= _dt;
            if ( ( m_fxTimerBeforeDestroy <= 0.f ) || ( ( m_fxcontroller != NULL ) && m_fxcontroller->IsAllFXStopped() ) )
            {
                stop();
            }
        }
    }	
   
	//--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterStimComponent::stop()
    {
        // fade out
       /* EventShow fade(0, 0.3f);
        m_actor->onEvent(&fade);*/

        m_stopped = btrue;
		if ( m_actor->isSerializable() )
		{
			m_actor->disable();
		}
		else
		{
			m_actor->requestDestruction();
		}
    }

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_ShooterStimComponent_Template)
    BEGIN_SERIALIZATION(Ray_ShooterStimComponent_Template)
        SERIALIZE_OBJECT("basicBullet", m_basicBullet);
        SERIALIZE_MEMBER("fxDelayBeforeDestroy", m_fxDelayBeforeDestroy);
    END_SERIALIZATION()

    Ray_ShooterStimComponent_Template::Ray_ShooterStimComponent_Template()
    : m_fxDelayBeforeDestroy( 0.0f )
    {
    }

    Ray_ShooterStimComponent_Template::~Ray_ShooterStimComponent_Template()
    {
    }
};
