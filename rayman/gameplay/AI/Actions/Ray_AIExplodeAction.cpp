#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIEXPLODEACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIExplodeAction.h"
#endif //_ITF_RAY_AIEXPLODEACTION_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_



namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIExplodeAction)
    IMPLEMENT_OBJECT_RTTI(Ray_AIExplodeAction_Template)
    
    //-----------------------------------------------------------------------------------------------------------------------
    Ray_AIExplodeAction::Ray_AIExplodeAction()
    : Super()
    , m_curExplodeRadius( 0.f )
    , m_curExplodeTime( 0.f )
    , m_spawnFragments( bfalse )
    , m_waitTimerBeforeDestruction( 0.0f )
    , m_reward( NULL )
    {
    }

    Ray_AIExplodeAction::~Ray_AIExplodeAction()
    {
        SF_DEL( m_reward );

#ifdef ITF_SUPPORT_EDITOR
        EVENTMANAGER->unregisterEvent( ITF_GET_STRINGID_CRC(Ray_EventQueryLumCount,3794901482), m_aiComponent );
#endif //ITF_SUPPORT_EDITOR
    }

    //-----------------------------------------------------------------------------------------------------------------------
    void Ray_AIExplodeAction::onEvent( Event* _event)
    {
        Super::onEvent(_event);

        if ( Ray_EventQueryLumCount *queryLumCount = _event->DynamicCast<Ray_EventQueryLumCount>(ITF_GET_STRINGID_CRC(Ray_EventQueryLumCount,3794901482)))
        {
            if (m_actor->getWorld() != SPAWNER->getSpawnerWorld())
            {
                const Ray_EventSpawnRewardLum *reward = IRTTIObject::SafeDynamicCast<const Ray_EventSpawnRewardLum>(getTemplate()->getReward(),ITF_GET_STRINGID_CRC(Ray_EventSpawnRewardLum,812488722));
                if ( reward != NULL )
                {
                    u32 maxNumOfLum = 999;
                    u32 curNum;
                    // find the maximum number of lums
                    for (u32 i = 0; i < getTemplate()->getNumRewards().size(); ++i )
                    {
                        curNum = getTemplate()->getNumRewards()[i];
                        if ( curNum > maxNumOfLum )
                        {
                            maxNumOfLum = curNum;
                        }
                    }
                    queryLumCount->addNormalLums( maxNumOfLum );
                }
            }
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------
    void Ray_AIExplodeAction::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded( _hotReload );

        // create fragments generator ?
        //
        m_spawnFragments = bfalse;
        if ( getTemplate()->getSpawnFragmentsPath().getStringID().isValid() && ( getTemplate()->getSpawnFragmentsNb() > 0 ) )
        {
            m_spawnFragments = btrue;
            SPAWNER->declareNeedsSpawnee(m_actor, &m_fragmentsGenerator, getTemplate()->getSpawnFragmentsPath() );
        }

        // max > min ?
        ITF_ASSERT( getTemplate()->getMaxRadius() >= getTemplate()->getMinRadius() );

#ifdef ITF_SUPPORT_EDITOR
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(Ray_EventQueryLumCount,3794901482),m_aiComponent);
#endif

        initReward();
    }

    //-----------------------------------------------------------------------------------------------------------------------
    void Ray_AIExplodeAction::initReward()
    {
        if ( getTemplate()->getReward() )
        {
            m_reward = getTemplate()->getReward()->CreateNewObject()->DynamicCast<Ray_EventSpawnReward>(ITF_GET_STRINGID_CRC(Ray_EventSpawnReward,145553727));
            BinaryClone(const_cast<Ray_EventSpawnReward*>(getTemplate()->getReward()),m_reward);
        }
    }
    //-----------------------------------------------------------------------------------------------------------------------
    void Ray_AIExplodeAction::spawnReward()
    {
        if ( m_reward != NULL && m_aiComponent->getLastAttacker().isValid() && m_aiComponent->getLastHitLevel() < getTemplate()->getNumRewards().size())
        {
            m_reward->clear();
            m_reward->setup(m_actor);
            ITF_ASSERT(m_aiComponent->getLastHitLevel() < getTemplate()->getNumRewards().size());
            m_reward->setNumRewards(getTemplate()->getNumRewards()[m_aiComponent->getLastHitLevel()]);
            m_reward->setRewardReceiver(m_aiComponent->getLastAttacker());
            if ( Ray_AIComponent * rayai = m_aiComponent->DynamicCast<Ray_AIComponent>(ITF_GET_STRINGID_CRC(Ray_AIComponent,2143961386)) )
            {
                m_reward->setRewardReceiver( rayai->getRewardReceiver() );
            }
            
            m_reward->setEjectionDir(m_aiComponent->getLastHitDir());
            GAMEMANAGER->onEvent(m_reward);
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------
    void Ray_AIExplodeAction::onActivate()
    {
        Super::onActivate();

        m_aiComponent->setHealth( 0 );
        m_curExplodeRadius = getTemplate()->getMinRadius();
        m_curExplodeTime = 0.0f;
        m_waitTimerBeforeDestruction = 0.0f;

        if ( m_fxController != NULL )
        {
            StringID fxName;
			if ( getTemplate()->getFxName().isValid() ) 
			{
				fxName = getTemplate()->getFxName();
			}
			else
				fxName = ITF_GET_STRINGID_CRC(Explode,522485580);
            u32 fxHandle = m_fxController->playFX( fxName );
            m_fxController->setFXPos( fxHandle, m_actor->getPos() );
        }

        // spawn fragments ?
        //---------------------
        if ( m_spawnFragments )
        {
            ITF_ASSERT( m_fragmentsGenerator.isInitialized() );
            ITF_ASSERT( getTemplate()->getSpawnFragmentsNb() > 0 );
            
            f32 deltaAngle = 0.0f;
            if ( getTemplate()->getSpawnFragmentsNb() - 1 > 0 )
            {
                deltaAngle = getTemplate()->getSpawnFragmentsDeltaAngle().ToRadians() / f32( getTemplate()->getSpawnFragmentsNb() - 1 );
            }
            Vec2d initVec = Vec2d::Right.Rotate( getTemplate()->getSpawnFragmentsStartAngle().ToRadians() );// - ( getTemplate()->getSpawnFragmentsDeltaAngle().ToRadians() * 0.5f ) - ( deltaAngle * 0.5f ) );
            
            for ( u32 i = 0; i < getTemplate()->getSpawnFragmentsNb(); ++i )
            {
                Vec2d destPos = m_actor->get2DPos() + ( initVec * getTemplate()->getMaxRadius() );
                
                Actor * spawnee = m_fragmentsGenerator.getSpawnee( CURRENTWORLD->getRootScene(), m_actor->getPos(), m_actor->getAngle() );
                if ( spawnee != NULL )
                {
                    // Bezier set
                    Vec3d dir = ( initVec * getTemplate()->getMaxRadius() ).to3d(m_actor->getPos().m_z );
                    EventSetupBezier bezier(    m_actor->getPos(), 
                                                m_actor->getPos() + ( dir * 0.333f ),
                                                m_actor->getPos() + ( dir * 0.666f ),
                                                destPos.to3d(m_actor->getPos().m_z )
                                            );
                    spawnee->onEvent( &bezier );

                    // Direction set
                    EventSetDirection directionEvt( initVec );
                    spawnee->onEvent( &directionEvt );
                }
                
                initVec = initVec.Rotate(deltaAngle);
            }
        }

        spawnReward();
    }

    //-----------------------------------------------------------------------------------------------------------------------
    void Ray_AIExplodeAction::onDeactivate()
    {
        Super::onDeactivate();
    }

    //-----------------------------------------------------------------------------------------------------------------------
    void Ray_AIExplodeAction::update( f32 _dt )
    {
        Super::update(_dt);

        bbool finished = btrue;
        // Explode radius
        //----------------
        if ( ( getTemplate()->getDuration() > 0.0f ) && ( m_curExplodeTime < getTemplate()->getDuration() ) )
        {
            finished = bfalse;
            m_curExplodeTime += _dt;
            if ( m_curExplodeTime >= getTemplate()->getDuration() )
            {
                m_curExplodeTime = getTemplate()->getDuration();
                finished = btrue;
            }
            f32 factor = 0.0f;
            if ( getTemplate()->getDuration() > 0.0f )
            {
                factor = m_curExplodeTime / getTemplate()->getDuration();
            }
            m_curExplodeRadius = getTemplate()->getMinRadius() + ( ( getTemplate()->getMaxRadius() - getTemplate()->getMinRadius() ) * factor );

            sendStim();
        }

        if ( getAnimAction().isValid() )
        {
            finished = finished && isAnimFinished();
        }
        else
        {
            GraphicComponent * graphComp = m_actor->GetComponent<GraphicComponent>();
            if ( graphComp != NULL )
            {
                graphComp->setAlpha( 0.f );
            }
        }

        // destruction timer ( for FXs...)
        //-----------------------------------
        if ( m_waitTimerBeforeDestruction > 0.0f )
        {
            m_waitTimerBeforeDestruction -= _dt;
            if ( m_waitTimerBeforeDestruction <= 0.0f )
            {
                // Destroy request
                if (m_actor->isSerializable())
                {
                    m_actor->disable();
                }
                else
                {
                    m_actor->requestDestruction();
                }
            }
        }

        if ( finished && ( m_waitTimerBeforeDestruction == 0.0f ) )
        {
            // need destruction ?
            if ( getTemplate()->getDestroyAtEnd() )
            {
                m_waitTimerBeforeDestruction = 1.0f;
                /*EventShow hideActor( 0.0f, 0.0f );
                m_actor->onEvent( &hideActor );*/                
            }
            else
            {
                m_aiBehavior->onActionFinished();
            }
        }
    }   

    //---------------------------------------------------------------------------------------------------------------------------------
    void Ray_AIExplodeAction::sendStim()
    {  
        m_stimShapeCircle.setRadius( m_curExplodeRadius );

        PunchStim* stim = STIMSMANAGER->requestStim<PunchStim>( &m_stimShapeCircle );

        if (!stim)
        {
            ITF_ASSERT_MSG(0, "stim limit reached!");
            return;
        }

        Vec2d stimPos = m_actor->get2DPos();
        stim->setPrevPos(stimPos);
        stim->setPos(stimPos);
        stim->setDirection(Vec2d::Right);
        stim->setAngle(m_actor->getAngle());
        stim->setDepth(m_actor->getDepth());
        stim->setFxPos(m_actor->getPos());
        stim->setSender(m_actor->getRef());
        stim->setLevel( getTemplate()->getHitLevel() );
        stim->setReceivedHitType( getTemplate()->getHitType() );
        stim->setFaction(m_aiComponent->getFaction());
        

        STIMSMANAGER->sendStim(stim);
#ifndef ITF_FINAL
        //STIMSMANAGER->toggleDrawStims(); 
#endif // ITF_FINAL

        // waiting FX ?
#ifdef ITF_SUPPORT_DEBUGFEATURE
        if ( getTemplate()->getDbgDrawExplodeRadius() )
        {
            DebugDraw::shape( stim->getPos(),0.f,stim->getAngle(),stim->getShape(),Color::white(), 0.2f );
        }        
#endif // ITF_SUPPORT_DEBUGFEATURE
    }

    //-----------------------------------------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------------------------------------
    BEGIN_SERIALIZATION_CHILD(Ray_AIExplodeAction_Template)
        SERIALIZE_MEMBER            ( "minRadius",              m_minRadius );
        SERIALIZE_MEMBER            ( "maxRadius",              m_maxRadius );
        SERIALIZE_MEMBER            ( "duration",               m_duration );
        SERIALIZE_MEMBER            ( "checkEnv",               m_checkEnv ); // !! Not done yet : need to get actors arround before and check env between cur pos and actor pos .... !!!!! 
        SERIALIZE_RECEIVEDHITTYPE   ( "hitType",	            m_hitType );
        SERIALIZE_MEMBER            ( "hitLevel",               m_hitLevel );
        SERIALIZE_MEMBER            ( "destroyAtEnd",           m_destroyAtEnd );
        SERIALIZE_MEMBER            ( "fxName",                 m_fxName );
        SERIALIZE_MEMBER            ( "spawnFragmentsPath",     m_spawnFragmentsPath );
        SERIALIZE_MEMBER            ( "spawnFragmentsNb",       m_spawnFragmentsNb );
        SERIALIZE_MEMBER            ( "spawnFragmentsDeltaAngle",m_spawnFragmentsDeltaAngle );  // is the angle divided by the fragment nb
        SERIALIZE_MEMBER            ( "spawnFragmentsStartAngle",m_spawnFragmentsStartAngle );  // is the start spawn angle
        BEGIN_CONDITION_BLOCK(ESerialize_DataRaw)
#ifdef ITF_SUPPORT_DEBUGFEATURE
            SERIALIZE_MEMBER        ( "dbgDrawExplodeRadius",   m_dbgDrawExplodeRadius );
#endif // ITF_SUPPORT_DEBUGFEATURE
        END_CONDITION_BLOCK()
        SERIALIZE_OBJECT_WITH_FACTORY("reward",                 m_reward, ACTORSMANAGER->getEventFactory());
        SERIALIZE_CONTAINER         ("numRewards",              m_numRewards);
    END_SERIALIZATION()

    Ray_AIExplodeAction_Template::Ray_AIExplodeAction_Template()
    : Super()
    , m_minRadius( 0.f )
    , m_maxRadius( 1.f )
    , m_duration( 0.f )
    , m_checkEnv( bfalse )
    , m_hitType( RECEIVEDHITTYPE_FRONTPUNCH )
    , m_hitLevel( 0 )
    , m_destroyAtEnd( btrue )
    , m_spawnFragmentsNb( 0 )
#ifdef ITF_SUPPORT_DEBUGFEATURE
    , m_dbgDrawExplodeRadius( bfalse )
#endif // ITF_SUPPORT_DEBUGFEATURE
    , m_reward(NULL)
    {
        m_numRewards.push_back(0);
        m_numRewards.push_back(1);
        m_numRewards.push_back(2);
        m_numRewards.push_back(4);

        m_spawnFragmentsDeltaAngle.SetRadians( MTH_2PI );
        m_spawnFragmentsStartAngle.SetRadians( 0 );
    }  

    Ray_AIExplodeAction_Template::~Ray_AIExplodeAction_Template()
    {
        SF_DEL( m_reward );
    }
}

  