#include "precompiled_gameplay.h"

#ifndef _ITF_ALWAYSACTIVEACTORCOMPONENT_H_
#include "gameplay/Components/Misc/AlwaysActiveActorComponent.h"
#endif //_ITF_ALWAYSACTIVEACTORCOMPONENT_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(AlwaysActiveActorComponent)
    BEGIN_SERIALIZATION_CHILD(AlwaysActiveActorComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(AlwaysActiveActorComponent)
        VALIDATE_COMPONENT_PARAM("", getTemplate()->getStartEvent(), "AlwaysActiveActorComponent requires an start event !");
        VALIDATE_COMPONENT_PARAM("", (  ( getTemplate()->getStopEvent() != NULL ) || 
                                        ( getTemplate()->getSafeTimeOutDuration() >= 0.0f ) ||
                                          getTemplate()->isStoppingOnScreenLeaving()
                                      ), "AlwaysActiveActorComponent requires a stop event !");
    END_SERIALIZATION()

    AlwaysActiveActorComponent::AlwaysActiveActorComponent()
    : Super()
    , m_safeTimeOutTimer( 0.0f )
    , m_started( bfalse )
    , m_enteredScreen( bfalse )
    //, m_needModifiedRequest( bfalse )
    {
    }
	AlwaysActiveActorComponent::~AlwaysActiveActorComponent()
	{
	}

    //----------------------------------------------------------------------------------------------------------------------------
   /* void AlwaysActiveActorComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();
        if ( m_needModifiedRequest )
        {
            m_needModifiedRequest = bfalse;
            m_actor->set2DPos( m_actor->get2DPos() );
        }    
    }*/
	
    //----------------------------------------------------------------------------------------------------------------------------
    void AlwaysActiveActorComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_started = bfalse;
        m_enteredScreen = bfalse;
        m_safeTimeOutTimer = 0.0f;

        if ( getTemplate()->getStartEvent() != NULL )
        {
            m_actor->registerEvent( getTemplate()->getStartEvent()->GetObjectClassCRC(), this );
        }
        if ( getTemplate()->getStopEvent() != NULL )
        {
            m_actor->registerEvent( getTemplate()->getStopEvent()->GetObjectClassCRC(), this );
        }        
    }

    //----------------------------------------------------------------------------------------------------------------------------
    void AlwaysActiveActorComponent::onEvent( Event * _event )
    {
        if ( ( getTemplate()->getStartEvent() != NULL ) && _event->IsClassCRC( getTemplate()->getStartEvent()->GetObjectClassCRC() ) )
        {
            if ( !m_started )
            {
                AddToAlwaysActiveActors();
            }
        }
        
        if ( ( getTemplate()->getStopEvent() != NULL ) && _event->IsClassCRC( getTemplate()->getStartEvent()->GetObjectClassCRC() ) )
        {
            if ( m_started )
            {
                removeFromAlwaysActiveActors();
            }
        }

        // process only activation stop
        //
        if ( EventActivate* eventActivate = _event->DynamicCast<EventActivate>(ITF_GET_STRINGID_CRC(EventActivate,2639596100)) )
        {
            if ( !eventActivate->getActivated() && m_started )
            {
                removeFromAlwaysActiveActors();
            }
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------
    void AlwaysActiveActorComponent::AddToAlwaysActiveActors()
    {
        Scene * scene = m_actor->getScene();
        if ( scene->getAlwaysActiveActors().find(m_actor)< 0 )
        {
            scene->setAlwaysActiveActor(m_actor);
        }
        m_started = btrue;
        m_enteredScreen = bfalse;
        m_safeTimeOutTimer = 0.0f;
        //m_needModifiedRequest = bfalse;
    }
    
    //----------------------------------------------------------------------------------------------------------------------------
    void AlwaysActiveActorComponent::removeFromAlwaysActiveActors( Scene* _pScene /*= NULL*/ )
    {
        Scene * scene = _pScene;
        if ( _pScene == NULL )
        {
            scene = m_actor->getScene();
        }
        //ITF_ASSERT( scene != NULL );
        
        if ( ( scene != NULL ) && scene->getAlwaysActiveActors().find(m_actor)>=0)
        {
            scene->setAlwaysActiveActor(m_actor, bfalse);
            //m_needModifiedRequest = btrue;
        }
        m_started = bfalse;
    }
    //----------------------------------------------------------------------------------------------------------------------------
    void AlwaysActiveActorComponent::onActorRemovedFromScene(Scene* _pScene, bbool _forDeletion)
    {
        Super::onActorRemovedFromScene( _pScene, _forDeletion );
        removeFromAlwaysActiveActors( _pScene );
    }

    //----------------------------------------------------------------------------------------------------------------------------
    void AlwaysActiveActorComponent::onUnloadResources()
    {
         Super::onUnloadResources();
         removeFromAlwaysActiveActors();
    }

    //----------------------------------------------------------------------------------------------------------------------------
    void AlwaysActiveActorComponent::Update( f32 _dt )
    {
        Super::Update( _dt );

        /*if ( m_needModifiedRequest )
        {
            m_needModifiedRequest = bfalse;
            m_actor->set2DPos( m_actor->get2DPos() );
        }*/

        // time out check
        //--------------------
        if ( m_started && ( getTemplate()->getSafeTimeOutDuration() >= 0.0f ) )
        {
            m_safeTimeOutTimer += _dt;
            if ( m_safeTimeOutTimer >= getTemplate()->getSafeTimeOutDuration() )
            {
                removeFromAlwaysActiveActors();
            }
        }

        // "In" screen check
        //--------------------
        if ( m_started && getTemplate()->isStoppingOnScreenLeaving() )
        {
            const AABB screenAABB = CAMERACONTROLLERMANAGER->getAABB();
            if ( screenAABB.checkOverlap( m_actor->getAABB() ) )
            {
                m_enteredScreen = btrue;
            }
            else if ( m_enteredScreen )
            {
                removeFromAlwaysActiveActors();
            }
        }        
    }

    //----------------------------------------------------------------------------------------------------------------------------
    //----------------------------------------------------------------------------------------------------------------------------
    //----------------------------------------------------------------------------------------------------------------------------
    AlwaysActiveActorComponent_Template::AlwaysActiveActorComponent_Template()
        :Super()
        , m_startEvent(NULL)
        , m_stopEvent(NULL)
        , m_safeTimeOutDuration(-1.0f)      //by default no time out
        , m_stopOnScreenLeaving( bfalse )
    {
    }
    //----------------------------------------------------------------------------------------------------------------------------
    AlwaysActiveActorComponent_Template::~AlwaysActiveActorComponent_Template()
    {
        SF_DEL( m_startEvent );
        SF_DEL( m_stopEvent );
    }	

    IMPLEMENT_OBJECT_RTTI(AlwaysActiveActorComponent_Template)
    BEGIN_SERIALIZATION(AlwaysActiveActorComponent_Template)
        SERIALIZE_MEMBER("safeTimeOutDuration",         m_safeTimeOutDuration );
        SERIALIZE_MEMBER("stopOnScreenLeaving",         m_stopOnScreenLeaving );
        SERIALIZE_OBJECT_WITH_FACTORY("startEvent",     m_startEvent, ACTORSMANAGER->getEventFactory() );
        SERIALIZE_OBJECT_WITH_FACTORY("stopEvent",      m_stopEvent, ACTORSMANAGER->getEventFactory() );	
    END_SERIALIZATION()


} // namespace ITF