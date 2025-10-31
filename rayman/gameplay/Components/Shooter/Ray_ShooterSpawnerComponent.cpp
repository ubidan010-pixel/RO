#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SHOOTERSPAWNERCOMPONENT_H_
#include "rayman/gameplay/Components/Shooter/Ray_ShooterSpawnerComponent.h"
#endif //_ITF_RAY_SHOOTERSPAWNERCOMPONENT_H_

#ifndef _ITF_TIMEDSPAWNERAICOMPONENT_H_
#include "gameplay/Components/AI/TimedSpawnerAIComponent.h"
#endif //_ITF_TIMEDSPAWNERAICOMPONENT_H_

#ifndef _ITF_TWEENCOMPONENT_H_
#include "gameplay/Components/Common/TweenComponent.h"
#endif //_ITF_TWEENCOMPONENT_H_

#ifndef _ITF_LINKMANAGER_H_
#include "gameplay/Managers/LinkManager.h"
#endif //_ITF_LINKMANAGER_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_DYNAMICFOGCOMPONENT_H_
#include "rayman/gameplay/components/Misc/Ray_DynamicFogComponent.h"
#endif //_ITF_RAY_DYNAMICFOGCOMPONENT_H_

#ifdef SHOOTER_PROFILING
    #include "rayman/gameplay/Managers/Ray_ShooterGameModeController.h"
#endif //SHOOTER_PROFILING

namespace ITF
{
    #define s_RewardTag ITF_GET_STRINGID_CRC(reward,958092744)
    #define s_BindTag ITF_GET_STRINGID_CRC(bind,1448673207)
    #define s_DelayTag ITF_GET_STRINGID_CRC(delay,161465807)
    #define s_FogTag ITF_GET_STRINGID_CRC(fog,1895684582)

    IMPLEMENT_OBJECT_RTTI(Ray_ShooterSpawnerComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_ShooterSpawnerComponent)

        SERIALIZE_ENUM_BEGIN( "tweenId", m_selectedTweenId );
        if ( ( getTemplate() != NULL ) && ( serializer->getProperties()&ESerializerProperty_Tool ) )
        {
            const ITF_VECTOR<StringID> & list = getTemplate()->getTweenInstructionSetList();
            for ( u32 i = 0; i < list.size(); i++ )
            {
                serializer->SerializeEnumVar( list[i], list[i].getDebugString() );
            }
            serializer->SerializeEnumVar( U32_INVALID, "invalid" );
        }
        SERIALIZE_ENUM_END();

        SERIALIZE_ENUM_BEGIN( "spawnActorId", m_selectedActorIdInBank );
        if ( ( getTemplate() != NULL ) && ( serializer->getProperties()&ESerializerProperty_Tool ) )
        {
            const ITF_VECTOR<ActorSpawnBank_Template::ActorSpawnData> & list = getTemplate()->getBank()->getBankList();
            for ( u32 i = 0; i < list.size(); i++ )
            {
                serializer->SerializeEnumVar( list[i].m_id, list[i].m_id.getDebugString() );
            }
            serializer->SerializeEnumVar( U32_INVALID, "invalid" );
        }
        SERIALIZE_ENUM_END();

        BEGIN_CONDITION_BLOCK(ESerializeGroup_Data)
            SERIALIZE_MEMBER("beforeCamRelativeInitialPos",m_beforeCamRelativeInitialPos);
        END_CONDITION_BLOCK()

    END_SERIALIZATION()

    Ray_ShooterSpawnerComponent::Ray_ShooterSpawnerComponent()
    : m_selectedActorIdInBank( U32_INVALID )
    , m_selectedTweenId( U32_INVALID )
    , m_modifierTweenId( StringID::InvalidId )
    , m_checkStopUpdate( bfalse )
    , m_allSpawneeDead( bfalse )
    , m_updateWaitDeadSpawneeDisable( bfalse )
    , m_linkedSpawnersDisabled( bfalse )
    , m_checkDisableNextFrame( bfalse )
    , m_checkRewards(bfalse)
    , m_spawnData( NULL )
    , m_vacuumedReward( NULL )
    , m_spawnData_SpawneeOnSpawnEvent( NULL )
    , m_beforeCamRelativeInitialPos( F32_INFINITY, F32_INFINITY, F32_INFINITY )
    , m_lastDeadSpawneeHasBeenVacuumed( bfalse )
    , m_restartSpawnIfAllPrevSpawneeAreDead( bfalse )
    , m_startEvent( NULL )
    , m_fogEvent( NULL )
    {
    }
    
    //----------------------------------------------------------------------------------------------------------------------------
    Ray_ShooterSpawnerComponent::~Ray_ShooterSpawnerComponent()
    {
        SF_DEL( m_spawnData_SpawneeOnSpawnEvent );
        SF_DEL( m_vacuumedReward );
        SF_DEL( m_startEvent );
        SF_DEL( m_fogEvent );

        // Camera relative 
        //
        if ( getTemplate() && getTemplate()->isCameraRelative() )
        {
            CAMERACONTROLLERMANAGER->unbindObject( m_actor );
        }

        clearBinds();

#ifdef ITF_SUPPORT_EDITOR
        EVENTMANAGER->unregisterEvent( ITF_GET_STRINGID_CRC(Ray_EventQueryLumCount,3794901482), this );
#endif //ITF_SUPPORT_EDITOR

    }

    //----------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterSpawnerComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();

        clearBinds();
    }

    //----------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterSpawnerComponent::clearBinds()
    {
        // remove still binded actor
        for ( u32 i = 0; i < m_actorsBindedToSpawnee.size(); ++i )
        {
            const ActorBindedToSpawnee & st = m_actorsBindedToSpawnee[i];

            if ( Actor * actorToBind = AIUtils::getActor( st.m_actorToBind ) )
            {
                actorToBind->setParentBind( NULL );
            }
        }

        // spawnee binds remove 
        //
        for (u32 i = 0; i < m_spawnedObjDataList.size(); i++)
        {
            SpawnedObjData & spawneeData = m_spawnedObjDataList[i];
            SafeArray<ActorRef> & bindedActorList = spawneeData.m_bindedObj;
            Actor* spawnee = spawneeData.m_spawnedObj.getActor();

            if ( spawnee != NULL )
            {
                if ( ( getTemplate() != NULL ) && needSpawneeBind() )
                {
                    //if ( spawnee->getParent() == m_actor->getRef() )
                    {
                        spawnee->setParentBind( NULL );
                        spawnee->disable();
                    }
                }
            }
                                
            if ( bindedActorList.size() > 0 )
            {
                for ( u32 u = 0; u < bindedActorList.size(); ++u )
                {
                    if ( Actor* bindedActor = bindedActorList[u].getActor() )
                    {
                        if ( ( spawnee == NULL ) || ( bindedActor->getParent() == spawnee->getRef() ) )
                        {
                            bindedActor->setParentBind( NULL );
                        }
                    }
                }
                bindedActorList.clear();
            }
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------
    const Path & Ray_ShooterSpawnerComponent::getSpawnActorPath() const
    {
        if ( getTemplate()->getBank()->isValid() )
        {
            return getTemplate()->getBank()->getActorPathFromId( m_selectedActorIdInBank );
        }
        return Super::getSpawnActorPath();
    }

    //---------------------------------------------------------------------------------------------
    void Ray_ShooterSpawnerComponent::declareSpawnee()
    {
        if ( !( getTemplate()->getBank()->isValid() && ( m_selectedActorIdInBank == U32_INVALID ) ) )
        {
            Super::declareSpawnee();
        }
    }
    //---------------------------------------------------------------------------------------------
    void Ray_ShooterSpawnerComponent::onSpawneeCreated( Actor * _spawner, Actor * _spawnee )
    {
        Ray_ShooterSpawnerComponent * comp = _spawner->GetComponent<Ray_ShooterSpawnerComponent>();
        if ( comp == NULL )
        {
            return;
        }
        comp->onSpawneeCreated( _spawnee );
    }

    //----------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterSpawnerComponent::onSpawneeCreated( Actor * _spawnee )
    {
        // Associate the tween instruction set 
        if ( _spawnee != NULL )
        {
            // add spawnee data to the list 
            //
            SpawnedObjData addData;
            addData.m_spawnedObj = _spawnee->getRef();
            m_spawnedObjDataList.push_back( addData );

            SpawnedObjData & spawneeData = m_spawnedObjDataList.back();

            // activate it
            _spawnee->enable();
            
            if ( getSelectedTweenID() != U32_INVALID )
            {
                TweenComponent * tweenComp = _spawnee->GetComponent<TweenComponent>();
                if ( tweenComp != NULL )
                {
                    tweenComp->playSet( getSelectedTweenID() );
                }
            }

            // Relative cam
            if ( isCameraRelative() )
            {
                CAMERACONTROLLERMANAGER->bindObject( _spawnee, btrue );
            }
            // need bind
            if ( needSpawneeBind() )
            {
                _spawnee->createParentBind( btrue, m_actor, ActorBind::Root, 0, bfalse );
            }

            // set specific spawnee data
            //
            if ( m_spawnData != NULL )
            {
                // use auto flip ?
                if ( m_spawnData->m_flip && ( f32_Abs( m_actor->getAngle() ) > MTH_PIBY2 ) )
                {
                    _spawnee->setFlipped( btrue );
                }
                
                if ( ( m_spawnData->m_onSpawnEvent != NULL ) && ( m_spawnData_SpawneeOnSpawnEvent != NULL ) )
                {
                    // has set direction event to zero, use spwaner direction
                    //
                    if ( EventSetDirection* dirEvent = m_spawnData->m_onSpawnEvent->DynamicCast<EventSetDirection>(ITF_GET_STRINGID_CRC(EventSetDirection,187560938)) )
                    {
                        if ( dirEvent->getDirection().IsEqual( Vec2d::Zero ) )
                        {
                            (static_cast<EventSetDirection*>( m_spawnData_SpawneeOnSpawnEvent ) )->setDirection( Vec2d::Right.Rotate( m_actor->getAngle() ) );
                        }
                    }

                    _spawnee->onEvent( m_spawnData_SpawneeOnSpawnEvent );
                }
                // has timed spawn modifier set it to spawnee
                //
                if ( m_timedSpawnerModifierActor.isValid() )
                {
                    if ( Actor * modifierActor = AIUtils::getActor( m_timedSpawnerModifierActor ) )
                    {
                        TimedSpawnerModifierComponent * modifier = modifierActor->GetComponent<TimedSpawnerModifierComponent>();
                        ITF_ASSERT( modifier != NULL );

                        if ( modifier != NULL )
                        {
                            EventTimedSpawnerDataSet evtData;
                            evtData.setSender( m_actor->getRef() );
                            TimedSpawnerData::DataToEvent( modifier->getTimedSpawnerData(), evtData );
                            _spawnee->onEvent( &evtData );
                        }
                    }
                }
                // bind actor to spawnee ?
                //
                for ( u32 i = 0; i < m_actorsBindedToSpawnee.size(); ++i )
                {
                    const ActorBindedToSpawnee & st = m_actorsBindedToSpawnee[i];

                    // spawned actor number is equal to the 'bind actor to spawnee' number => bind the actor
                    if ( ( st.m_spawneeNum - 1 ) == ( m_spawnedObjDataList.size() - 1 ) )
                    {
                        if ( Actor * actorToBind = AIUtils::getActor( st.m_actorToBind ) )
                        {
                            actorToBind->createParentBind( btrue, _spawnee, ActorBind::Root, 0, bfalse );

                            // store the binded actor
                            spawneeData.m_bindedObj.push_back( st.m_actorToBind );
                        }

                        // remove the actor from the list
                        m_actorsBindedToSpawnee.eraseNoOrder( i );
                        i--;
                    }
                }
            }

            // update Fog ?
            //
            if ( m_fogEvent != NULL )
            {
                _spawnee->onEvent( m_fogEvent );
            }
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterSpawnerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        // init position
        applyCamRelativeInitialPos();

        if ( m_timedSpawner.isSpawneeValid() )
        {
            bbool initializeReward = btrue;

            // get spawnee spwan data 
            //
            m_spawnData = getTemplate()->getBank()->getActorSpawnDataFromId( m_selectedActorIdInBank );
            if ( m_spawnData != NULL )
            {
                m_timedSpawner.recycleSpawnee( m_spawnData->m_recycle );
                m_timedSpawner.useSpawnerScale( m_spawnData->m_useScale );

                SF_DEL( m_spawnData_SpawneeOnSpawnEvent );
                m_spawnData_SpawneeOnSpawnEvent = NULL;
                if ( m_spawnData->m_onSpawnEvent != NULL )
                {
                    m_spawnData_SpawneeOnSpawnEvent = static_cast<Event*>(m_spawnData->m_onSpawnEvent->CreateNewObject() );
                    BinaryClone(const_cast<Event*>(m_spawnData->m_onSpawnEvent), m_spawnData_SpawneeOnSpawnEvent );
                }

                // get spawnee user data (bitfield)
                // 1 = don't use reward
                // 2 = don't start spawn if not all previous actors are not dead
                if ( m_spawnData->m_userData & 1 )
                {
                    initializeReward = bfalse;
                }
                if ( m_spawnData->m_userData & 2 )
                {
                    m_restartSpawnIfAllPrevSpawneeAreDead = btrue;
                }
            }

            if ( initializeReward )
            {
                // declare reward generator
                //
                if ( !getTemplate()->getRewardSpawnPath().isEmpty() )
                {
                    SPAWNER->declareNeedsSpawnee( m_actor, &m_rewardGenerator, getTemplate()->getRewardSpawnPath() );
                }
                if ( !getTemplate()->getReward5xSpawnPath().isEmpty() )
                {
                    SPAWNER->declareNeedsSpawnee( m_actor, &m_reward5xGenerator, getTemplate()->getReward5xSpawnPath() );
                }

                // init vacuumed reward
                //
                if ( getTemplate()->getVacuumedReward() != NULL )
                {
                    m_vacuumedReward = getTemplate()->getVacuumedReward()->CreateNewObject()->DynamicCast<Ray_EventSpawnReward>(ITF_GET_STRINGID_CRC(Ray_EventSpawnReward,145553727));;
                    BinaryClone(const_cast<Ray_EventSpawnReward*>(getTemplate()->getVacuumedReward()),m_vacuumedReward);
                }
            }
        }
        m_timedSpawner.registerOnSpawneeCreatedCB( (TimedSpawner::OnSpawneeCreatedCB)onSpawneeCreated );

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(EventQueryIsDead,2482198443), this );
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(Ray_EventShooterSpawnerDataSet,538828432), this );

#ifdef ITF_SUPPORT_EDITOR
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(Ray_EventQueryLumCount,3794901482),this);
#endif //ITF_SUPPORT_EDITOR
#ifdef SHOOTER_PROFILING
        SP_ADD( "Spawner", m_actor->getRef() )
#endif //SHOOTER_PROFILING

    }

    //----------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterSpawnerComponent::onEvent( Event * _event)
    {
        Super::onEvent( _event );

        if ( EventQueryIsDead* deadQuery = _event->DynamicCast<EventQueryIsDead>(ITF_GET_STRINGID_CRC(EventQueryIsDead,2482198443)) ) // is finished ??
        {
            deadQuery->setIsDead( isSpawnFinished() );
        }
        else if ( Ray_EventQueryLumCount *queryLumCount = _event->DynamicCast<Ray_EventQueryLumCount>(ITF_GET_STRINGID_CRC(Ray_EventQueryLumCount,3794901482)))
        {
            if ( m_actor->getWorld() != SPAWNER->getSpawnerWorld() )
            {
                const Ray_EventSpawnRewardLum * rewardLums = IRTTIObject::SafeDynamicCast<const Ray_EventSpawnRewardLum>( getTemplate()->getVacuumedReward(), ITF_GET_STRINGID_CRC(Ray_EventSpawnRewardLum,812488722) );
                // is really lums reward ?
                if ( ( m_vacuumedReward != NULL ) && ( rewardLums != NULL ) )
                {
                    const TimedSpawnerData*	spawnData = m_timedSpawner.getTimedSpawnerData();
                    if ( ( spawnData != NULL ) && ( spawnData->m_burstCount > 0 ) )
                    {
                        queryLumCount->addNormalLums( spawnData->m_burstCount * spawnData->m_burstElementsCount );
                    }
                }
            }
        }
        else if ( Ray_EventShooterSpawnerDataSet * _modifier = _event->DynamicCast<Ray_EventShooterSpawnerDataSet>(ITF_GET_STRINGID_CRC(Ray_EventShooterSpawnerDataSet,538828432)))
        {
            processModifierEvent( _modifier );
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterSpawnerComponent::processStartEvent( Event * _event )
    {
        if ( m_restartSpawnIfAllPrevSpawneeAreDead )
        {
            if ( ( m_spawnedObjDataList.size() > 0 ) && !m_allSpawneeDead )
            {
                return;
            }
        }

        Super::processStartEvent( _event );
        
        //m_spawnedObjList.clear();
        //m_spawnedObjDeadList.clear();
        m_spawnedObjDataList.clear();
        m_rewardLinkedSpawnerList.clear();
        m_rewardLinkedSpawnerDeadList.clear();
        m_rewardObjList.clear();
        m_checkRewards = bfalse;

        m_allSpawneeDead = bfalse;
        m_updateWaitDeadSpawneeDisable = needSpawneeBind() || m_restartSpawnIfAllPrevSpawneeAreDead;
        m_linkedSpawnersDisabled = bfalse;
        m_checkDisableNextFrame = bfalse;
        m_lastDeadSpawnee.invalidate();
        m_lastDeadSpawneeHasBeenVacuumed = bfalse;
        m_lastDeadSpawneeHasBeenVacuumedOwner.invalidate();
        m_timedSpawnerModifierActor.invalidate();
        m_actorsBindedToSpawnee.clear();

        LinkComponent * linkComponent = m_actor->GetComponent<LinkComponent>();
        if ( linkComponent != NULL )
        {
            // check children
            //-----------------
            //ObjectRefList * children  = LINKMANAGER->getChildren( m_actor->getRef() );
            const LinkComponent::ChildrenList& children = linkComponent->getChildren();

            if ( children.size() > 0 )
            {
                for ( u32 i = 0; i < children.size() ; ++i )
                {
                    if ( BaseObject * obj = linkComponent->getChildObject( children[i].getPath() ) )
                    {
                        Actor * actor = AIUtils::getActor( obj->getRef() );
                        if ( actor != NULL )
                        {
                            const ChildEntry& childEntry = children[i];
                            i32 tagVal;

                            // Bind child to spawned actors
                            //
                            if ( childEntry.getTagValue( s_BindTag, tagVal ) )
                            {
                                ITF_WARNING( m_actor, tagVal > 0, "Invalid link 'bind' tag value, must be > 0" );

                                ActorBindedToSpawnee st;
                                st.m_actorToBind = actor->getRef();
                                st.m_spawneeNum = tagVal;
                                m_actorsBindedToSpawnee.push_back( st );
                                
                                // will be bind to spawnee, bind it now to the spawner to allow multi spawner binding
                                //
                                if ( !actor->getParent().isValid() )
                                {
                                    // unknown crash BLIND... I don't understand why the child is still in child list..
                                    // maybe a copy/paste of actors in an other map ??
                                    const ObjectRefList&  childList = m_actor->getChildrenBindHandler().getChildren();
                                    if ( childList.find( obj->getRef() ) < 0 )
                                    {
                                        actor->createParentBind( btrue, m_actor, ActorBind::Root, 0, bfalse );
                                    }
                                    else
                                    {
                                        ITF_ASSERT( 0 );// try to catch this case.
                                    }
                                }
                            }                            
                            // is linked to a "shooter spawner component" => add it in reward spawner check list
                            //
                            else if ( Ray_ShooterSpawnerComponent * spawerComp = actor->GetComponent<Ray_ShooterSpawnerComponent>() )
                            {
                                m_rewardLinkedSpawnerList.push_back( actor->getRef() );
                                m_rewardLinkedSpawnerDeadList.push_back( bfalse );
                            }
                            // Is linked to a "timed spawner AI component" => give him spawn data and broadcast him the event
                            //
                            else if ( TimedSpawnerAIComponent * aiComp = actor->GetComponent<TimedSpawnerAIComponent>() )
                            {
                                aiComp->setSpawnerData( m_timedSpawnerData );
                                f32 fTagVal;
                                if ( childEntry.getTagValue( s_DelayTag, fTagVal ) )
                                {
                                    aiComp->setSpawnDelay( fTagVal );
                                }
                                actor->onEvent(_event);
                            }
                            // Is linked to a reward actor => add it in the reward list
                            //
                            else if ( childEntry.getTagValue( s_RewardTag, tagVal ) )
                            {
                                m_rewardObjList.push_back( actor->getRef() );
                                // disable actor
                                actor->disable();
                                m_checkRewards = btrue;
                            }
                            else if ( Ray_DynamicFogComponent * fogComp = actor->GetComponent<Ray_DynamicFogComponent>() )
                            {
                                if ( m_fogEvent == NULL )
                                {
                                    m_fogEvent = newAlloc( mId_GameplayMisc, Ray_EventDynamicFogParamSet() );
                                }
                                if ( m_fogEvent != NULL )
                                {
                                    m_fogEvent->setFogColor( fogComp->getFogColor() );
                                    m_fogEvent->setFogMaxDepth( fogComp->getFogMaxDepth() );
                                }
                            }
                            // linked to a timed spawn modifier, it's "timed spawn data" for spawned actors
                            //
                            else if ( TimedSpawnerModifierComponent * modifier = actor->GetComponent<TimedSpawnerModifierComponent>() )
                            {
                                m_timedSpawnerModifierActor = actor->getRef();
                            }                            
                        }
                    }
                }
            }
        }
      
        // Camera relative 
        //
        if ( getTemplate()->isCameraRelative() )
        {
            CAMERACONTROLLERMANAGER->bindObject( m_actor, btrue );
        }

        m_checkRewards = m_checkRewards || m_rewardGenerator.isInitialized();
        m_checkStopUpdate = m_checkRewards || 
                            needSpawneeBind() || 
                            m_rewardGenerator.isInitialized() || 
                            ( m_actorsBindedToSpawnee.size() > 0 ) || 
                            m_restartSpawnIfAllPrevSpawneeAreDead;
    }

    //----------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterSpawnerComponent::processModifierEvent( Ray_EventShooterSpawnerDataSet * _modifier )
    {
        if ( _modifier == NULL )
        {
            return;
        }

        m_modifierTweenId = _modifier->getTweenID();
        // reset all
        //
        clearBinds();

        m_timedSpawner.reset();

        // clone the start event for modifier start
        //
        if ( ( m_startEvent == NULL ) && ( getTemplate()->getStartEvent() != NULL ) )
        {
            m_startEvent = getTemplate()->getStartEvent()->CreateNewObject()->DynamicCast<Event>(ITF_GET_STRINGID_CRC(Event,772421353));;
            BinaryClone(const_cast<Event*>(getTemplate()->getStartEvent()),m_startEvent);
        }

        if ( m_startEvent != NULL )
        {
            processStartEvent( m_startEvent );
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterSpawnerComponent::disable()
    {
        if ( !m_checkStopUpdate )
        {
            Super::disable();

            // Camera relative 
            //
            if ( getTemplate()->isCameraRelative() )
            {
                CAMERACONTROLLERMANAGER->unbindObject( m_actor );
            }

             clearBinds();
        }
    }
   	//----------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterSpawnerComponent::Update( f32 _deltaTime )
    {
        if ( m_checkStopUpdate )
        {
            hasAllSpawneeDead();
            checkActorBindedToSpawnee();
            bbool linkedSpawnerAllDead = checkLinkedSpawners();
            
            if ( m_allSpawneeDead && linkedSpawnerAllDead )
            {
                if ( m_checkRewards )
                {
                    activateRewards();
                    m_checkRewards = bfalse;
                }
                
                if ( !m_updateWaitDeadSpawneeDisable && ( m_rewardGeneratedList.size() == 0 ) )
                {
                    m_checkStopUpdate = bfalse;
                    disable();
                }
            }

            // check spwanee disable
            //
            bbool checkAllDisabled = ( m_allSpawneeDead && m_updateWaitDeadSpawneeDisable );
            bbool disableSpawner = m_linkedSpawnersDisabled && !checkAllDisabled;
            bbool prevNextFrame = m_checkDisableNextFrame && !checkAllDisabled;

            if ( !disableSpawner && isSpawnFinished() )
            {
                m_checkDisableNextFrame = bfalse;
                bbool allDisable = btrue;

                for (u32 i = 0; i < m_spawnedObjDataList.size(); i++)
                {
                    SpawnedObjData & objData = m_spawnedObjDataList[i];
                    // Not dead
                    if ( !objData.m_isDead || checkAllDisabled )
                    {
                        Actor* spawn = objData.m_spawnedObj.getActor();
                        if ( ( spawn == NULL ) || ( ( spawn != NULL ) && !spawn->isActive() /*!spawn->isVisible( CAMERA )*/ ) )
                        {
                            if ( ( spawn != NULL ) && objData.m_justSpawned )
                            {
                                allDisable = bfalse;
                                continue;
                            }
                            else if ( !checkAllDisabled && !m_updateWaitDeadSpawneeDisable )
                            {
                                m_checkDisableNextFrame = btrue;
                            }
                        }
                        else
                        {
                            objData.m_justSpawned = bfalse;
                            allDisable = bfalse;
                        }
                    }
                }
                
                if ( checkAllDisabled && allDisable )
                {
                    disableSpawner = btrue;
                }
                if ( m_checkDisableNextFrame && prevNextFrame )
                {
                    disableSpawner = btrue;
                }
                if ( allDisable && !m_allSpawneeDead && m_updateWaitDeadSpawneeDisable )
                {
                    disableSpawner = btrue;
                }
            }

            // check generated reward destruction
            bbool rewardsDestroyed = checkRewardsDestruction();
            if ( !m_checkRewards && isSpawnFinished() )
            {
                disableSpawner = rewardsDestroyed && ( disableSpawner || ( !checkAllDisabled && m_allSpawneeDead ) );
            }
            
            if ( disableSpawner )
            {
                m_checkStopUpdate = bfalse;
                disable();
            }
        }

        if ( !isSpawnFinished() )
        {
            Super::Update(_deltaTime);
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------
    bbool Ray_ShooterSpawnerComponent::hasAllSpawneeDead()
    {
        if ( !m_allSpawneeDead )
        {
            bbool allDead = isSpawnFinished() ? btrue : bfalse;

            for (u32 i = 0; i < m_spawnedObjDataList.size(); i++)
            {
                SpawnedObjData & objData = m_spawnedObjDataList[i];
                // Not dead
                if ( !objData.m_isDead )
                {
                    Actor* spawn = objData.m_spawnedObj.getActor();
                    if ( spawn != NULL )
                    {
                        bbool vacuumed = bfalse;

                        EventQueryIsDead query;
                        spawn->onEvent(&query);
                        if ( !query.getIsDead() )
                        {
                            // check vacuumed
                            Ray_EventQueryHasBeenVacuumed queryVacuumed;
                            spawn->onEvent(&queryVacuumed);
                            vacuumed = queryVacuumed.getHasBeenVacuumed();
                            if ( !vacuumed )
                            {
                                allDead = bfalse;
                                continue;
                            }
                            else
                            {
                                m_lastDeadSpawneeHasBeenVacuumedOwner = queryVacuumed.getVacuumedOwner();
                            }
                        }
                        objData.m_isDead = btrue;
                        m_lastDeadSpawnee = objData.m_spawnedObj;                        
                        m_lastDeadSpawneeHasBeenVacuumed = vacuumed;
                    }
                    else
                    {
                        allDead = bfalse;
                        continue;
                    }
                }
            }
            m_allSpawneeDead = allDead;
        }
        return m_allSpawneeDead;
    }

    //----------------------------------------------------------------------------------------------------------------------------
    bbool Ray_ShooterSpawnerComponent::checkLinkedSpawners()
    {
        bbool res = btrue;

        for ( u32 i = 0; i < m_rewardLinkedSpawnerList.size() ; ++i )
        {
            if ( m_rewardLinkedSpawnerDeadList[i] )
                continue;

            Actor * actor = m_rewardLinkedSpawnerList[i].getActor();
            if ( actor != NULL )
            {
                Ray_ShooterSpawnerComponent * spawerComp = actor->GetComponent<Ray_ShooterSpawnerComponent>();
                if ( spawerComp != NULL )
                {
                    bbool alldead = spawerComp->hasAllSpawneeDead();
                    res = res && alldead;
                    if ( !alldead )
                    {
                        if ( !actor->isActive() && spawerComp->isSpawnFinished() )
                        {
                            m_linkedSpawnersDisabled = btrue;
                        }
                        continue;
                    }
                    m_rewardLinkedSpawnerDeadList[i] = btrue;
                    m_lastDeadSpawnee = spawerComp->getLastDeadSpawnee( m_lastDeadSpawneeHasBeenVacuumed, m_lastDeadSpawneeHasBeenVacuumedOwner );
                }
            }
        }
        return res;
    }

    //----------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterSpawnerComponent::activateRewards()
    {
        Actor * lastDeadActor = m_lastDeadSpawnee.getActor();

        for ( u32 i = 0; i < m_rewardObjList.size() ; ++i )
        {
            Actor * actor = m_rewardObjList[i].getActor();
            if ( actor != NULL )
            {
                EventTrigger trig;
                trig.setActivated( btrue );
                actor->onEvent( &trig );

                // pos rewards on last dead actor
                if ( lastDeadActor != NULL )
                {
                    actor->set2DPos( lastDeadActor->get2DPos() );
                }                
                actor->enable();
            }
        }

        // auto reward
        //--------------
        if ( m_spawnedObjDataList.size() > 0 )
        {
            // Last has been vacuumed => auto pickup
            if ( m_lastDeadSpawneeHasBeenVacuumed && ( m_vacuumedReward != NULL ) )
            {
                Actor * owner = AIUtils::getActor(m_lastDeadSpawneeHasBeenVacuumedOwner);
                // send reward
                if ( ( owner != NULL ) && ( m_vacuumedReward != NULL ) )
                {
                    m_vacuumedReward->clear();
                    if ( lastDeadActor != NULL )
                    {
                        m_vacuumedReward->setup( lastDeadActor );
                    }
                    else
                    {
                        m_vacuumedReward->setup( owner );
                        m_vacuumedReward->setScene( m_actor->getScene() );
                    }
                    m_vacuumedReward->setRewardReceiver( m_lastDeadSpawneeHasBeenVacuumedOwner );
                    m_vacuumedReward->setEjectionDir( -Vec2d::Right );
                    m_vacuumedReward->setNumRewards( m_spawnedObjDataList.size() );
                    RAY_GAMEMANAGER->onEvent(m_vacuumedReward);
                }
            }
            // last is dead
            //
            else if ( m_rewardGenerator.isInitialized() )
            {
                Vec2d initSpawnPos = m_actor->get2DPos();
                Vec2d spawnPos;

                if ( lastDeadActor != NULL )
                {
                    initSpawnPos = lastDeadActor->get2DPos();
                }
                u32 totalRewardNb = m_spawnedObjDataList.size();
                u32 simpleRewardNb = totalRewardNb;
                u32 x5RewardNb = 0;
                
                if ( m_reward5xGenerator.isInitialized() )
                {
                    x5RewardNb = ( totalRewardNb / 5 );
                    simpleRewardNb = simpleRewardNb - ( x5RewardNb * 5 );

                    for ( u32 i = 0; i < x5RewardNb; ++i )
                    {
                        spawnPos = initSpawnPos;
                        if ( i > 0 )
                        {
                            Vec2d offsetVec = Vec2d::Right.Rotate( Seeder::getSharedSeeder().GetFloat( -MTH_PI, MTH_PI ) );
                            offsetVec *= Seeder::getSharedSeeder().GetFloat( 0.2f, 0.5f );
                            spawnPos += offsetVec;
                        }
                        Actor * reward = m_reward5xGenerator.getSpawnee( CURRENTWORLD->getRootScene(), spawnPos.to3d( m_actor->getPos().m_z ), 0.f );
                        if ( reward != NULL )
                        {
                            m_rewardGeneratedList.push_back( RewardGenerated( reward->getRef() ) );
                        }
                    }
                }

                for ( u32 i = 0; i < simpleRewardNb; ++i )
                {
                    spawnPos = initSpawnPos;
                    if ( ( i > 0 ) || ( x5RewardNb > 0 ) )
                    {
                        Vec2d offsetVec = Vec2d::Right.Rotate( Seeder::getSharedSeeder().GetFloat( -MTH_PI, MTH_PI ) );
                        offsetVec *= Seeder::getSharedSeeder().GetFloat( ( x5RewardNb > 0 ) ? 0.4f : 0.2f, ( x5RewardNb > 0 ) ? 0.7f : 0.5f );
                        spawnPos += offsetVec;
                    }
                    Actor * reward = m_rewardGenerator.getSpawnee( CURRENTWORLD->getRootScene(), spawnPos.to3d( m_actor->getPos().m_z ), 0.f );
                    if ( reward != NULL )
                    {
                        m_rewardGeneratedList.push_back( RewardGenerated( reward->getRef() ) );
                    }
                }                

                //// create the reward
                //Actor * rewardActor = m_rewardGenerator.getSpawnee( CURRENTWORLD->getRootScene(), spawnPos.to3d(( m_actor->getPos().m_z ) ), 0.f );
                //if ( rewardActor != NULL )
                //{
                //    Ray_EventSetBubblePrizeRewardNumber rewardNumChangeEvt( m_spawnedObjDataList.size() );
                //    rewardActor->onEvent( &rewardNumChangeEvt );
                //}
            }
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------
    // Check if all generated rewards are not more visible and can be destroyed
    //
    bbool Ray_ShooterSpawnerComponent::checkRewardsDestruction()
    {
        for ( u32 i = 0; i < m_rewardGeneratedList.size(); ++i )
        {
            bbool remove = btrue;
            RewardGenerated & rewardData = m_rewardGeneratedList[i];
            Actor * reward = AIUtils::getActor( rewardData.m_reward );
            if ( reward != NULL )
            {
                bbool isvisible = reward->isVisible( CAMERA );
                
                rewardData.m_hasBeenVisible = rewardData.m_hasBeenVisible || isvisible;
                
                remove = ( !isvisible && rewardData.m_hasBeenVisible ) || !reward->isEnabled();
            }

            if ( remove )
            {
                if ( reward != NULL )
                {
                    if ( reward->isSerializable() )
                    {
                        reward->disable();
                    }
                    else
                    {
                        reward->requestDestruction();
                    }
                }
                m_rewardGeneratedList.eraseNoOrder( i );
                --i;
            }
        }

        return ( m_rewardGeneratedList.size() == 0 ) ? btrue : bfalse;
    }

    //----------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterSpawnerComponent::checkActorBindedToSpawnee()
    {
        // check each spawnee death 
        //--------------------------
        for (u32 i = 0; i < m_spawnedObjDataList.size(); i++)
        {
            SpawnedObjData & spawneeData = m_spawnedObjDataList[i];
            SafeArray<ActorRef> & bindedActorList = spawneeData.m_bindedObj;

            Actor* spawnee = spawneeData.m_spawnedObj.getActor();
        
            // clear spawnee bind if needed
            if ( ( spawneeData.m_isDead || ( spawnee == NULL ) ) && ( bindedActorList.size() > 0 ) )
            {
                for ( u32 u = 0; u < bindedActorList.size(); ++u )
                {
                    if ( Actor* bindedActor = bindedActorList[u].getActor() )
                    {
                        if ( ( spawnee == NULL ) || ( bindedActor->getParent() == spawnee->getRef() ) )
                        {
                            bindedActor->setParentBind( NULL );
                        }
                    } 
                }    
                bindedActorList.clear();
            }
        }
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterSpawnerComponent::onCheckpointLoaded()
    {
        Super::onCheckpointLoaded();        
        applyCamRelativeInitialPos();
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterSpawnerComponent::applyCamRelativeInitialPos()
    {
        if ( getTemplate()->isCameraRelative() && !m_beforeCamRelativeInitialPos.IsEqual( Vec3d( F32_INFINITY, F32_INFINITY, F32_INFINITY ) ) )
        {
            m_actor->setLocalInitialPos( m_beforeCamRelativeInitialPos );
            m_actor->setLocalPos( m_beforeCamRelativeInitialPos );
        }    
    }

#ifdef ITF_SUPPORT_EDITOR
    //----------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterSpawnerComponent::drawEdit( class ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit( _drawInterface, _flags );

        StringID actorID = m_selectedActorIdInBank;
        StringID tweenID = m_selectedTweenId;
        StringID ModifierTweenID = m_modifierTweenId;


        // Display spawned actor
        String s_text = "Actor : ";
        if ( actorID.isValid() )
        {
            s_text += actorID.getDebugString();
        }
        else
        {
            s_text += "Invalid";
        }

        // Display tween instruction
        s_text += "\nTween : ";
        if ( tweenID.isValid() )
        {
            s_text += tweenID.getDebugString();
        }
        else
        {
            s_text += "Invalid";
        }
        
        if  ( ModifierTweenID.isValid() )
        {
            s_text += "\nModifier Tween : ";
            s_text += ModifierTweenID.getDebugString();
            tweenID = ModifierTweenID;
        }

        Vec3d res2d;
        Vec3d displayPos = m_actor->getPos();
        displayPos.m_y += 0.5f;
        GFX_ADAPTER->compute3DTo2D( displayPos, res2d );
        GFX_ADAPTER->drawDBGText( s_text, res2d.m_x, res2d.m_y );


        if (tweenID.isValid())
        {
            if ( Actor* referenceSpawn = SPAWNER->getReferenceItem(m_timedSpawner.getGenerator()).getActor() )
            {
                if ( TweenComponent* tween = referenceSpawn->GetComponent<TweenComponent>() )
                {
                    const Transform3d initTransform(m_actor->getBoundWorldInitialPos(), m_actor->getBoundWorldInitialAngle());
                    tween->drawSet(initTransform, tweenID);
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterSpawnerComponent::onEditorMove(bbool _modifyInitialPos)
    {
        Super::onEditorMove(_modifyInitialPos);
        if ( getTemplate()->isCameraRelative() )
        {
            m_beforeCamRelativeInitialPos = m_actor->getWorldInitialPos();
        }
    }
#endif // ITF_SUPPORT_EDITOR

    //-------------------------------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_ShooterSpawnerModifierComponent)
    BEGIN_SERIALIZATION_CHILD(Ray_ShooterSpawnerModifierComponent)
        SERIALIZE_ENUM_BEGIN( "tweenId", m_selectedTweenId );
            if ( ( getTemplate() != NULL ) && ( serializer->getProperties()&ESerializerProperty_Tool ) )
            {
                const ITF_VECTOR<StringID> & list = getTemplate()->getTweenInstructionSetList();
                for ( u32 i = 0; i < list.size(); i++ )
                {
                    serializer->SerializeEnumVar( list[i], list[i].getDebugString() );
                }
                serializer->SerializeEnumVar( U32_INVALID, "invalid" );
            }
        SERIALIZE_ENUM_END();
    END_SERIALIZATION()

    //---------------------------------------------------------------------------------------------
    void Ray_ShooterSpawnerModifierComponent::sendEventToChildren()
    {
        LinkComponent * linkComp = m_actor->GetComponent<LinkComponent>();
        ITF_ASSERT( linkComp != NULL );
        if ( linkComp != NULL )
        {
            Ray_EventShooterSpawnerDataSet evtData;
            evtData.setSender( m_actor->getRef() );
            Ray_ShooterSpawnerModifierComponent::DataToEvent( getTimedSpawnerData(), m_selectedTweenId, evtData );
            linkComp->sendEventToChildren( &evtData );
        }
    }

#ifdef ITF_SUPPORT_EDITOR
    //----------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterSpawnerModifierComponent::drawEdit( class ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit( _drawInterface, _flags );

        StringID tweenID = m_selectedTweenId;
        String s_text = "Tween : ";

        // Display tween instruction
        if ( tweenID.isValid() )
        {
            s_text += tweenID.getDebugString();
        }
        else
        {
            s_text += "Invalid";
        }
        
        Vec3d res2d;
        Vec3d displayPos = m_actor->getPos();
        displayPos.m_y += 0.5f;
        GFX_ADAPTER->compute3DTo2D( displayPos, res2d );
        GFX_ADAPTER->drawDBGText( s_text, Color::green(), res2d.m_x, res2d.m_y );
    }
#endif // ITF_SUPPORT_EDITOR

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_ShooterSpawnerComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_ShooterSpawnerComponent_Template)
        SERIALIZE_OBJECT(    "bank",                         m_bank );
        SERIALIZE_CONTAINER( "tweenInstructionSetList",      m_tweenInstructionSetList );
        SERIALIZE_MEMBER(    "cameraRelative",               m_cameraRelative );
        SERIALIZE_MEMBER(    "bindSpawnee",                  m_bindSpawnee );
        SERIALIZE_MEMBER(    "rewardSpawnPath",              m_rewardSpawnPath );
        SERIALIZE_MEMBER(    "reward5xSpawnPath",            m_reward5xSpawnPath );
        SERIALIZE_OBJECT_WITH_FACTORY("vacuumedReward",      m_vacuumedReward, ACTORSMANAGER->getEventFactory() );
    END_SERIALIZATION()

    Ray_ShooterSpawnerComponent_Template::Ray_ShooterSpawnerComponent_Template()
    : Super()
    , m_cameraRelative( bfalse )
    , m_bindSpawnee( bfalse )
    , m_vacuumedReward( NULL )
    {
    }

    Ray_ShooterSpawnerComponent_Template::~Ray_ShooterSpawnerComponent_Template()
    {
        SF_DEL( m_vacuumedReward );
    }


    //-------------------------------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_ShooterSpawnerModifierComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_ShooterSpawnerModifierComponent_Template)
        SERIALIZE_CONTAINER( "tweenInstructionSetList",      m_tweenInstructionSetList );
    END_SERIALIZATION()
}