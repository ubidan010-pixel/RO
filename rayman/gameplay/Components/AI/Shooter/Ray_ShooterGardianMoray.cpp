#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SHOOTERGARDIANMORAY_H_
#include "rayman/gameplay/Components/AI/Shooter/Ray_ShooterGardianMoray.h"
#endif //_ITF_RAY_SHOOTERGARDIANMORAY_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_RAY_REWARDMANAGER_H_
#include "rayman/gameplay/Ray_RewardManager.h"
#endif //_ITF_RAY_REWARDMANAGER_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_ShooterGardianMoray)
    IMPLEMENT_OBJECT_RTTI(Ray_ShooterGardianMorayBodyPart)

    BEGIN_SERIALIZATION_CHILD(Ray_ShooterGardianMoray)
    END_SERIALIZATION() 

    //--------------------------------------------------------------------------------------------------------------------------------
    //                                                      HEAD PART
    //--------------------------------------------------------------------------------------------------------------------------------
    Ray_ShooterGardianMoray::Ray_ShooterGardianMoray()
    : Super()
    , m_waterLevel( 0.0f )
    , m_waterLevelPolyLine( NULL )
    , m_spawnFxTimer( 0.0f )
    , m_headInWater( bfalse )
    , m_tailInWater( bfalse )
    , m_dead( bfalse )
    , m_tailVisible( bfalse )
    , m_deathRewardTimer( 0.0f )
    , m_deathNextSpawnTime( 0.0f )
    , m_deathLastSpawnDelay( 0.0f )
    {
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    Ray_ShooterGardianMoray::~Ray_ShooterGardianMoray()
    {
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventPlayerActivationChanged,1593682208), this );
    }   

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterGardianMoray::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded( _hotReload );

        ACTOR_REGISTER_EVENT_COMPONENT( m_actor, ITF_GET_STRINGID_CRC(Ray_EventShooterBossDataSet,3071700792), this );

        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventPlayerActivationChanged,1593682208),this);

        if ( !getTemplate()->getDeathRewardSpawnPath().isEmpty() )
        {
            SPAWNER->declareNeedsSpawnee( m_actor, &m_deathRewardGenerator, getTemplate()->getDeathRewardSpawnPath() );
        }
        m_deathRewardTimer = 0.0f;
        m_deathNextSpawnTime = 0.0f;
        m_deathLastSpawnDelay = 0.0f;
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterGardianMoray::onEvent( Event* _event )
    {
        Super::onEvent( _event );

        if ( Ray_EventShooterBossDataSet* bossDataSet = _event->DynamicCast<Ray_EventShooterBossDataSet>( ITF_GET_STRINGID_CRC(Ray_EventShooterBossDataSet,3071700792 ) ) )
        {
            if ( bossDataSet->getDataInt() == 1 )
            {
                for ( u32 i = 0 ; i < m_bodyParts.size(); ++i )
                {
                    ITF_ASSERT( m_bodyParts[i] != NULL );
                    if ( m_bodyParts[i] != NULL )
                    {
                        Ray_ShooterGardianMorayBodyPart * guardianPart = static_cast< Ray_ShooterGardianMorayBodyPart * >( m_bodyParts[i] );
                        guardianPart->setHealthFactor( bossDataSet->getDataFloat() );
                    }                    
                }
            }            
        }
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterGardianMoray::onBecomeActive()
    {
        Super::onBecomeActive();

        m_dead = bfalse;
        m_tailVisible = bfalse;
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    Ray_ShooterGardianMorayBodyPart * Ray_ShooterGardianMoray::getBodyPart( u32 _idx )
    { 
        if ( _idx < m_bodyParts.size() ) 
        {
            return static_cast<Ray_ShooterGardianMorayBodyPart*>( m_bodyParts[_idx] ); 
        }
        return NULL; 
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterGardianMoray::Update( f32 _dt )
    {
        Super::Update( _dt );

        i32 bodyPartNb = m_bodyParts.size();

       

        // check tail visibility to start the reward timer
        //-------------------------------------------------
        if ( !m_tailVisible && ( bodyPartNb > 0 ) && ( m_bodyParts[bodyPartNb-1] != NULL ) )
        {
            Ray_ShooterGardianMorayBodyPart * morayPart = static_cast<Ray_ShooterGardianMorayBodyPart *>( m_bodyParts[bodyPartNb-1] );
            if ( ( morayPart->getActor() != NULL ) && morayPart->getActor()->isVisible( CAMERA ) ) 
            {
                m_tailVisible = btrue;
                REWARD_MANAGER->Action_TimerReset(Ray_RewardManager::Timer_BlueBaron, GAMEMANAGER->getMainIndexPlayer());
                REWARD_MANAGER->Action_TimerStart(Ray_RewardManager::Timer_BlueBaron, GAMEMANAGER->getMainIndexPlayer());
            }
        }

        // dead ?
        //--------
        if ( m_isDying && !m_dead )
        {
            REWARD_MANAGER->Action_New( Ray_RewardManager::Action_ShooterKillBoss, GAMEMANAGER->getMainIndexPlayer());
            REWARD_MANAGER->Action_TimerStop(Ray_RewardManager::Timer_BlueBaron, GAMEMANAGER->getMainIndexPlayer());
            m_dead = btrue;
        }

        if ( m_dead )
        {
            updateDeathReward( _dt );
        }

        // Water level check; concider that the moray stay in water region !
        //----------------------------------------------------------------------
        if ( m_waterLevelPolyLine == NULL )
        {
            PolyLine* curPolyLine = AIUtils::getPolylineRegion( m_actor->getDepth(),s_RegionName_Swim, m_actor->get2DPos() );
            if ( curPolyLine != NULL )
            {
                m_waterLevel = curPolyLine->getAABB().getMax().m_y;
                m_waterLevelPolyLine = curPolyLine;
            }
            else
            {
                for ( i32 i = 0; i < bodyPartNb; ++i )
                {
                    BodyPart* bodyPart = static_cast<BodyPart*>(m_bodyParts[i]);
                    Actor* bodyPartActor = bodyPart->getActor();
                    if ( bodyPartActor )
                    {
                        curPolyLine = AIUtils::getPolylineRegion( m_actor->getDepth(),s_RegionName_Swim, bodyPartActor->get2DPos() );
                        if ( curPolyLine != NULL )
                        {
                            m_waterLevel = curPolyLine->getAABB().getMax().m_y;
                            m_waterLevelPolyLine = curPolyLine;
                            break;
                        }
                    }
                }                
            }
        }
        // check if still valid
        else
        {
            PolyLine* curPolyLine = AIUtils::getPolylineRegion( m_actor->getDepth(),s_RegionName_Swim, Vec2d( m_actor->get2DPos().m_x, m_waterLevel - 0.5f ) );
            if ( ( curPolyLine != NULL ) && ( curPolyLine != m_waterLevelPolyLine ) )
            {
                m_waterLevelPolyLine = ( curPolyLine != NULL ) ? curPolyLine : NULL;
                if ( m_waterLevelPolyLine != NULL )
                {
                    m_waterLevel = curPolyLine->getAABB().getMax().m_y;                
                }
            }        
        }

        // check parts in water
        //---------------------
        if ( ( m_waterLevelPolyLine != NULL ) && ( m_waterLevelPolyLine->m_ownerFrise != NULL ) )
        {   
            Vec2d startPos = m_actor->get2DPos();
            Vec2d segment;
            Vec2d waterPos1( 0.0f, m_waterLevel );
            Vec2d waterPos2( 0.0f, m_waterLevel );
            Vec2d intersection;
            Vec2d midPos;
            const f32 testWidth = getTemplate()->getWaterTestWidth() * m_actor->getScale().m_x;
            const f32 bubonWidth = getTemplate()->getWaterTestBubonWidth() * m_actor->getScale().m_x;
            f32 perturbation = m_currentSpeed * getTemplate()->getWaterPerturbationForce();

            i32 randPartIdxFX = -2;
            m_spawnFxTimer -= _dt;
            
            if ( ( m_lastPartInWater.size() > 0 ) && ( m_spawnFxTimer <= 0.0f ) )
            {
                m_spawnFxTimer = getTemplate()->getWaterSpawnFxDelay();

                randPartIdxFX = m_lastPartInWater[ Seeder::getSharedSeeder().GetU32( 0, m_lastPartInWater.size() - 1 ) ];
            }
            m_lastPartInWater.clear();
            bbool bPartInWater = bfalse;
            u32 fxSentId = 0;
            Vec2d fxPos;
            Vec2d fxSegment, firstSegment, secondSegment, headFxPos, tailFXPos;
            bbool playHeadFx = bfalse; 
            bbool playTailFx = bfalse;
            
            for ( i32 i = -1; i < bodyPartNb; ++i )
            {
                Ray_ShooterGardianMorayBodyPart * morayPart = NULL;
                fxSentId = 0;
                bPartInWater = bfalse;

                if ( i < 0 )
                {
                    segment = -( Vec2d::Right.Rotate( m_actor->getAngle() + MTH_PIBY2 ) * getTemplate()->getWaterTestHeadLength() * m_actor->getScale().m_x );
                    startPos = m_actor->get2DPos() - segment;
                }
                else
                {
                    morayPart = static_cast<Ray_ShooterGardianMorayBodyPart *>( m_bodyParts[i] );

                    if ( morayPart->getActor() == NULL )
                        continue;
                    
                    if ( i + 1 < bodyPartNb )
                    {
                        BodyPart* bodyPart = static_cast<BodyPart*>(m_bodyParts[i + 1]);
                        Actor* bodyPartActor = bodyPart->getActor();
                        if ( bodyPartActor == NULL )
                        {
                            continue;
                        }
                        segment = bodyPartActor->get2DPos() - startPos;
                    }
                    else
                    {
                        BodyPart* bodyPart = static_cast<BodyPart*>(m_bodyParts[bodyPartNb-1]);
                        Actor* bodyPartActor = bodyPart->getActor();
                        segment = -( Vec2d::Right.Rotate( bodyPartActor->getAngle() + MTH_PIBY2  ) * getTemplate()->getWaterTestTailLength() * m_actor->getScale().m_x );
                    }
                }
                
                waterPos1.m_x = startPos.m_x;
                waterPos2.m_x = (startPos + segment).m_x;
                waterPos1.m_x += ( waterPos1.m_x < waterPos2.m_x ) ? -1.0f : 1.0f;
                waterPos2.m_x += ( waterPos2.m_x <= waterPos1.m_x ) ? -1.0f : 1.0f;

                firstSegment = segment;

                if ( Segment_Segment( startPos, startPos + segment, waterPos1, waterPos2, intersection ) )
                {
                    EventSwimingPlouf plouf( intersection, perturbation, segment.norm(), bfalse );
                    m_waterLevelPolyLine->m_ownerFrise->onEvent( &plouf );

                    bPartInWater = btrue;

                    if ( ( randPartIdxFX == i ) && ( fxSentId == 0 ) )
                    {                        
                        fxPos = intersection;
                        fxSentId = 1;
                    }

                    // head/tail in water ?
                    if ( i < 0 ) 
                    {
                        playHeadFx  = btrue;
                        headFxPos = intersection;
                    }
                    else if ( i == bodyPartNb - 1  ) 
                    {
                        playTailFx = btrue;
                        tailFXPos = intersection;
                    }
                    
                    //DebugDraw::circle(intersection, 0.0f, 0.1f, Color::green() );
                }
                
                /*DebugDraw::line2D( startPos, startPos + segment , 0.0f, Color::black() );
                DebugDraw::line2D( waterPos1, waterPos2, 0.0f, Color::yellow() );*/

                midPos = startPos + ( segment * 0.5f );
                Vec2d testDir = segment.normalize().getPerpendicular();
                
                f32 widthRight = ( testWidth * 0.5f );
                f32 widthLeft = widthRight;

                // check left / right bubon
                if ( morayPart != NULL )
                {
                    if ( i == ( bodyPartNb - 1 ) )
                    {
                        widthRight = ( getTemplate()->getWaterTestTailWidth() * m_actor->getScale().m_x * 0.5f );
                        widthLeft = widthRight;                    
                    }
                    else
                    {
                        widthRight += ( morayPart->isBubonAlive( bfalse ) ? bubonWidth : 0.0f );
                        widthLeft += ( morayPart->isBubonAlive( btrue ) ? bubonWidth : 0.0f );
                    }
                }  
                else 
                {
                    widthRight = ( getTemplate()->getWaterTestHeadWidth() * m_actor->getScale().m_x * 0.5f );
                    widthLeft = widthRight;                
                }
                
                segment = ( -testDir * widthLeft ) - ( testDir * widthRight );
                midPos += ( testDir * widthRight );
                
                waterPos1.m_x = midPos.m_x;
                waterPos2.m_x = (midPos + segment).m_x;
                waterPos1.m_x += ( waterPos1.m_x < waterPos2.m_x ) ? -1.0f : 1.0f;
                waterPos2.m_x += ( waterPos2.m_x <= waterPos1.m_x ) ? -1.0f : 1.0f;

                secondSegment = segment;

                if ( Segment_Segment( midPos, midPos + segment, waterPos1, waterPos2, intersection ) )
                {
                    EventSwimingPlouf plouf( intersection, perturbation, segment.norm(), bfalse );
                    m_waterLevelPolyLine->m_ownerFrise->onEvent( &plouf );

                    bPartInWater = btrue;
                    
                    // send Fx
                    if ( ( randPartIdxFX == i ) && ( fxSentId == 0 ) )
                    {                        
                        fxPos = intersection;
                        fxSentId = 2;
                    }

                    // head/tail in water ?
                    if ( i < 0 ) 
                    {
                        playHeadFx  = btrue;
                        headFxPos = intersection;
                    }
                    else if ( i == bodyPartNb - 1  ) 
                    {
                        playTailFx = btrue;
                        tailFXPos = intersection;
                    }

                    //DebugDraw::circle(intersection, 0.0f, 0.1f, Color::green() );
                }

                //DebugDraw::line2D( midPos, midPos + segment , 0.0f, Color::black() );
                
                if( i < 0 )
                {
                    startPos = m_actor->get2DPos();
                }
                else if ( i + 1 < bodyPartNb )
                {
                    BodyPart* bodyPart = static_cast<BodyPart*>(m_bodyParts[i + 1]);
                    startPos = bodyPart->getActor()->get2DPos();
                }
                else 
                {
                    BodyPart* bodyPart = static_cast<BodyPart*>(m_bodyParts[bodyPartNb - 1]);
                    startPos = bodyPart->getActor()->get2DPos();
                }

                // get the good segment
                if ( ( randPartIdxFX == i ) &&  ( fxSentId != 0 ) )
                {    
                    fxSegment = ( fxSentId == 1 ) ? secondSegment : firstSegment;
                    
                    if ( m_fxController != NULL )
                    {
                        f32 dist = f32_Abs( Vec2d::Right.dot( fxSegment ) * 0.5f );
                        Vec2d finalPos = fxPos + ( Vec2d::Right * Seeder::getSharedSeeder().GetFloat( -dist, dist ) );

                        u32 fxHandle = m_fxController->playFX( ITF_GET_STRINGID_CRC(Surf,4252188527) );
                        m_fxController->setFXPosFromHandle( fxHandle, finalPos.to3d( m_actor->getDepth() ) );
                    }
                }            

                // safe part in water
                if ( bPartInWater )
                {
                    m_lastPartInWater.push_back(i);
                }
            }

            
            if ( m_fxController != NULL )
            {
                if ( ( playHeadFx && !m_headInWater ) || ( !playHeadFx && m_headInWater ) )
                {
                    const StringID s_waterHead        = ITF_GET_STRINGID_CRC(WaterHead,3360312869); 
                    u32 fxHandle = m_fxController->playFX( s_waterHead );
                    m_fxController->setFXPosFromHandle( fxHandle, headFxPos.to3d( m_actor->getDepth() ) );

                    m_headInWater = playHeadFx;
                }

                if ( ( playTailFx && !m_tailInWater ) || ( !playTailFx && m_tailInWater ) )
                {
                    u32 fxHandle = m_fxController->playFX( ITF_GET_STRINGID_CRC(WaterTail,752947623) );
                    m_fxController->setFXPosFromHandle( fxHandle, tailFXPos.to3d( m_actor->getDepth() ) );

                    m_tailInWater = playTailFx;
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterGardianMoray::updateDeathReward( f32 _dt )
    {
        if ( m_dead && m_deathRewardGenerator.isInitialized() )
        {
            if ( getTemplate()->getDeathRewardSpawnDuration() == 0.0f )
            {
                if ( m_deathRewardTimer == 0.0f )
                {
                    m_deathRewardTimer = 1.0f;
                    for ( u32 i = 0; i < getTemplate()->getDeathRewardNumber(); ++i )
                    {
                        spawnDeathReward();
                    }
                }
            }
            else if ( m_deathRewardTimer < getTemplate()->getDeathRewardSpawnDuration() )
            {
                m_deathRewardTimer += _dt;

                if ( m_deathRewardTimer >= m_deathNextSpawnTime )
                {
                    spawnDeathReward();

                    if ( getTemplate()->getDeathRewardNumber() > 0 )
                    {
                        f32 delay = getTemplate()->getDeathRewardSpawnDuration() / f32( getTemplate()->getDeathRewardNumber() );
                        f32 newDelay = Seeder::getSharedSeeder().GetFloat( delay * 0.2f, delay );
                        m_deathNextSpawnTime += ( delay - m_deathLastSpawnDelay ) + newDelay;
                        m_deathLastSpawnDelay = newDelay;
                    }
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterGardianMoray::spawnDeathReward()
    {
        const AABB& camAABB = CAMERACONTROLLERMANAGER->getAABB();
        Vec2d spawnPos = camAABB.getCenter() + ( Vec2d::Right * camAABB.getWidth() * 0.3333f );

        Vec2d offsetVec = Vec2d::Right.Rotate( Seeder::getSharedSeeder().GetFloat( getTemplate()->getDeathRewardSpawnAngle().m_x, getTemplate()->getDeathRewardSpawnAngle().m_y ) );
        offsetVec *= Seeder::getSharedSeeder().GetFloat( getTemplate()->getDeathRewardSpawnDist().m_x, getTemplate()->getDeathRewardSpawnDist().m_y );
        Actor * reward = m_deathRewardGenerator.getSpawnee( CURRENTWORLD->getRootScene(), spawnPos.to3d(m_actor->getDepth() ) + offsetVec.to3d(m_actor->getDepth() ) );

        if ( reward != NULL )
        {
            Ray_EventChildLaunch launch( m_actor->getPos() );
            launch.setSender(m_actor->getRef());
            reward->onEvent(&launch);
        }    
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------
    #define s_AnimInput_BubonLeft ITF_GET_STRINGID_CRC(BubonLeft,3734885601)
    #define s_AnimInput_BubonRight ITF_GET_STRINGID_CRC(BubonRight,3529360153)
    #define s_AnimInput_TailState ITF_GET_STRINGID_CRC(TailState,3912768146)
    #define s_AnimMrk_LeftAnimFinished ITF_GET_STRINGID_CRC(MRK_LeftAnimFinished,1906975189)
    #define s_AnimMrk_RightAnimFinished ITF_GET_STRINGID_CRC(MRK_RightAnimFinished,3779025080)
    #define s_AnimMrk_TailAnimFinished ITF_GET_STRINGID_CRC(MRK_TailAnimFinished,1651109387)
    #define s_AnimMrk_SwapToTail ITF_GET_STRINGID_CRC(MRK_SwapToTail,1729677991)
    #define s_AnimMrk_StartTailTransfo ITF_GET_STRINGID_CRC(MRK_StartTailTransfo,4090151917)

    const f32 k_tailInterpolationDuration = 0.3f;

    //--------------------------------------------------------------------------------------------------------------------------------
    //                                                      BODY PART
    //--------------------------------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------
    Ray_ShooterGardianMorayBodyPart::Ray_ShooterGardianMorayBodyPart()
    : Super()
    , m_defaultMaxHealthMulti( -1 )
    , m_bubonReward( NULL )
    , m_destroyRewardGiven( bfalse )
    , m_healthFactor( 1.0f )
    , m_prevHealthFactor( 1.0f )
    , m_tailInterpolationTimer( 0.0f )
    {
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    Ray_ShooterGardianMorayBodyPart::~Ray_ShooterGardianMorayBodyPart()
    {
        destroyPhantom( m_leftBubon.m_phantom );
        destroyPhantom( m_rightBubon.m_phantom );
     
        SF_DEL( m_bubonReward );
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterGardianMorayBodyPart::onInit()
    {
        Super::onInit();

        createPhantom( m_leftBubon.m_phantom, m_leftBubon.m_shape );
        createPhantom( m_rightBubon.m_phantom, m_rightBubon.m_shape );

        m_leftBubon.m_health = getTemplate()->getBubonHealth();
        m_leftBubon.m_boneName = getTemplate()->getLeftBubonBoneName();
        m_leftBubon.m_animInput = s_AnimInput_BubonLeft;

        m_rightBubon.m_health = getTemplate()->getBubonHealth();
        m_rightBubon.m_boneName = getTemplate()->getRightBubonBoneName();
        m_rightBubon.m_animInput = s_AnimInput_BubonRight;

        if ( !getTemplate()->getBubonRewardSpawnPath().isEmpty() )
        {
            SPAWNER->declareNeedsSpawnee( m_parent, &m_rewardGenerator, getTemplate()->getBubonRewardSpawnPath() );
        }
        if ( !getTemplate()->getTailPath().isEmpty() )
        {
            SPAWNER->declareNeedsSpawnee( m_parent, &m_tailGenerator, getTemplate()->getTailPath() );
        }
        if ( ( m_bubonReward == NULL ) && ( getTemplate()->getBubonReward() != NULL ) )
        {
            m_bubonReward = getTemplate()->getBubonReward()->CreateNewObject()->DynamicCast<Ray_EventSpawnReward>(ITF_GET_STRINGID_CRC(Ray_EventSpawnReward,145553727));
            if ( m_bubonReward != NULL )
            {
                BinaryClone(const_cast<Ray_EventSpawnReward*>(getTemplate()->getBubonReward()),m_bubonReward);
            }
        }

        m_defaultMaxHealthMulti = getTemplate()->getHealth();

        updateMultiPlayerHealth();
        
        m_destroyRewardGiven = bfalse;
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterGardianMorayBodyPart::updateMultiPlayerHealth()
    {
        u32 numPlayer = GAMEMANAGER->getNumActivePlayers();

        i32 defaultMaxHealthMulti = getTemplate()->getHealth() + i32( f32( getTemplate()->getHealth() ) * getTemplate()->getMultiPlayerLifePointFactor() * f32_Max( 0.f, f32( numPlayer ) - 1.0f ) );
        m_curHealth = i32( f32( defaultMaxHealthMulti ) * floorf( ( m_curHealth / m_prevHealthFactor ) + 0.5f ) / f32( m_defaultMaxHealthMulti ) );

        m_curHealth = i32( floorf( (m_curHealth * m_healthFactor) + 0.5f ) );

        m_defaultMaxHealthMulti = defaultMaxHealthMulti;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    Actor* Ray_ShooterGardianMorayBodyPart::createBodyPart()
    {
        if ( isTail() )
        {
            return spawnTail();
        }
        return Super::createBodyPart();
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterGardianMorayBodyPart::clear()
    {
        Super::clear();

        Actor * tail = AIUtils::getActor( m_tailActor );
        if ( tail != NULL )
        {
            tail->requestDestruction();
        }
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    Actor * Ray_ShooterGardianMorayBodyPart::spawnBodyPart()
    {
        Actor * actor = Super::spawnBodyPart();
        if ( ( actor != NULL ) && ( m_tailActor != getActorRef() ) )
        {
            ACTOR_REGISTER_EVENT_COMPONENT(actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301), m_parentComponent);
        }
        return actor;
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    Actor * Ray_ShooterGardianMorayBodyPart::spawnTail()
    {
        ITF_ASSERT( !m_tailActor.isValid() );       // recreate the tail ???

        if ( m_tailGenerator.isInitialized() )
        {
            Actor * tail = m_tailGenerator.getSpawnee( m_parent->getScene(), m_parent->getPos() );
            if ( tail != NULL )
            {
                m_tailActor = tail->getRef();            
                tail->setAngle(m_parent->getAngle());
                tail->setScale(m_parent->getScale());
                ACTOR_REGISTER_EVENT_COMPONENT(tail,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301), m_parentComponent);
            }
            return tail;
        }
        return NULL;
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterGardianMorayBodyPart::swapToTail()
    {
        if ( getActorRef() == m_tailActor )
            return;

        Actor * curBodyActor = getActorRef().getActor();
        ITF_ASSERT( curBodyActor != NULL );
        
        if ( curBodyActor != NULL ) 
        {
            curBodyActor->requestDestruction();
            setActorRef(ActorRef::InvalidRef);
            setActor(NULL);
        }

        if ( m_tailActor.isValid() )        // tail must have been spawned before
        {
            setActorRef(m_tailActor);
            validate();
            // reset bones
            m_leftBubon.m_boneIdx = U32_INVALID;
            m_rightBubon.m_boneIdx = U32_INVALID;
            changeBubonState( m_leftBubon, m_leftBubon.m_curState );
            changeBubonState( m_rightBubon, m_rightBubon.m_curState );

            if ( getActor() )
            {
                EventShow showActor( 1.0f, 0.001f );
                getActor()->onEvent(&showActor);
            }
        }
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    // Keep the last part alive to switch the head directly in bubble state
    //
    bbool Ray_ShooterGardianMorayBodyPart::removeAndKeepAliveOnDeath()
    {
        return ( ( getBodyPartCount() <= 1 ) ? btrue : bfalse );
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    //
    bbool Ray_ShooterGardianMorayBodyPart::updateKeepAlive( f32 _dt )
    {
        if ( removeAndKeepAliveOnDeath() && m_curPlayingAnim.isValid() )
        {
            return bfalse;
        }
        return btrue;
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterGardianMorayBodyPart::onEvent( Event * _event )
    {
        Super::onEvent( _event );

        if ( Ray_EventQueryLumCount *queryLumCount = _event->DynamicCast<Ray_EventQueryLumCount>(ITF_GET_STRINGID_CRC(Ray_EventQueryLumCount,3794901482)) )
        {
            // 2 bubons + part destruction
            queryLumCount->addNormalLums( ( getTemplate()->getRewardNumber() * 2 ) + getTemplate()->getDestroyRewardNumber() );
        }
        else if ( EventPlayerActivationChanged * playerActivationHasChanged = _event->DynamicCast<EventPlayerActivationChanged>(ITF_GET_STRINGID_CRC(EventPlayerActivationChanged,1593682208)) )
        {
            updateMultiPlayerHealth();
        }
       
        if (AnimGameplayEvent * animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
        {
            // Check transfo start
            //
            if ( ( animEvent->getName() == s_AnimMrk_StartTailTransfo ) && ( getIndex() == getBodyPartCount() - 2 ) )
            {
                startTailTransformation();
            }
            
            // Skip other part anim events
            if ( _event->getSender() != getActorRef() )
                return;

            if ( animEvent->getName() == s_AnimMrk_LeftAnimFinished )
            {
                onBubonStateFinished( m_leftBubon );
            }
            else if ( animEvent->getName() == s_AnimMrk_RightAnimFinished )
            {
                onBubonStateFinished( m_rightBubon );
            }
            else if ( animEvent->getName() == s_AnimMrk_SwapToTail )
            {
                Actor * tail = AIUtils::getActor(m_tailActor);
                ITF_ASSERT( tail != NULL );
                if ( tail )        // tail must have been spawned before
                {
                    EventShow showActor( 1.0f, 0.05f );
                    tail->onEvent( &showActor );
                }
            }            
        }
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterGardianMorayBodyPart::onAnimFinished()
    {
        if ( m_curPlayingAnim == getTemplate()->getTransfoTotailAnim() )
        {
            swapToTail();
        }
        Super::onAnimFinished();
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterGardianMorayBodyPart::update( f32 _dt )
    {
        // create the tail if not yet
        //
        if ( isTail() && ( getActorRef() != m_tailActor ) && !m_tailActor.isValid() )
        {
            startTailTransformation();
        }

        Super::update( _dt );

        ITF_ASSERT( getAnimComponent() != NULL );
        if ( getAnimComponent() == NULL )
            return;

        // update bubon phatom
        //
        updateBubon( m_leftBubon );
        updateBubon( m_rightBubon );

        m_tailInterpolationTimer = f32_Max( 0.0f, m_tailInterpolationTimer - _dt );
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterGardianMorayBodyPart::startTailTransformation()
    {
        if ( ( getActorRef() != m_tailActor ) && !m_tailActor.isValid() )
        {
            Actor * tail = spawnTail();
            if ( tail != NULL )
            {
                if ( getTemplate()->getTransfoTotailAnim().isValid() )
                {
                    EventShow hideActor( 0.0f, 0.001f );
                    tail->onEvent(&hideActor);

                    // start the transformation
                    playAnim( getTemplate()->getTransfoTotailAnim() );
                }
                else
                {
                    swapToTail();
                }
            }

            // start the tail interpolation 
            m_tailInterpolationTimer = k_tailInterpolationDuration;
        }
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterGardianMorayBodyPart::onTrajectoryUpdate()
    {
        // update tail object pos etc.
        if ( m_tailActor.isValid() && ( getActorRef() != m_tailActor ) )
        {
            Actor * tail = AIUtils::getActor(m_tailActor);
            ITF_ASSERT( tail != NULL );
            if ( tail )
            {
                tail->set2DPos( getActor()->get2DPos() );
                tail->setAngle( getActor()->getAngle() );
                tail->setDepth( getActor()->getDepth() );
                tail->setScale( getActor()->getScale() );
            }
        }
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterGardianMorayBodyPart::positionOnTrajectory( const Vec3d& _pos, const f32 _angle )
    {
        u32 bodyPartCount = getBodyPartCount();
        f32 finalAngle = _angle;

        if ( ( isTail() || m_tailActor.isValid() ) && ( bodyPartCount > 1 ) )
        {
            Ray_ShooterGardianMorayBodyPart * prevBodypart = static_cast<Ray_ShooterGardianMoray*>( m_parentComponent )->getBodyPart( getIndex() - 1 );
            if ( prevBodypart != NULL )
            {
                Angle prevBodyAngle, partAngle;
                prevBodyAngle.SetRadians( prevBodypart->getAngle() );
                partAngle.SetRadians( _angle );
                
                finalAngle = Angle::Lerp( 0.4f + ( 0.6f * ( m_tailInterpolationTimer / k_tailInterpolationDuration ) ), prevBodyAngle, partAngle ).ToRadians();
            }
        }
        
        Super::positionOnTrajectory( _pos, finalAngle );
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterGardianMorayBodyPart::updateBubon( Bubon & _bubon )
    {
        if ( getAnimComponent() == NULL )
            return;

        if ( _bubon.m_phantom != NULL )
        {
            if  ( ( _bubon.m_boneIdx == U32_INVALID) && _bubon.m_boneName.isValid() ) 
            {
                _bubon.m_boneIdx = getAnimComponent()->getBoneIndex( _bubon.m_boneName );
            }
            if ( _bubon.m_boneIdx != U32_INVALID )
            {
                Vec2d bonepos;
                getAnimComponent()->getBonePos( _bubon.m_boneIdx, bonepos );
                _bubon.m_phantom->setPos( bonepos );
                _bubon.m_phantom->updateAABB();
            }

            //DebugDraw::shape( _bubon.m_phantom->getPos(), 0.0f, 0.0f, _bubon.m_phantom->getShape(), Color::white() );

           sendBubonStim( _bubon );
        }

        if ( !_bubon.m_checkWantedStateNextFrame && _bubon.m_wantedState != _bubon.m_curState )
        {
            changeBubonState( _bubon, _bubon.m_wantedState );
        }
        _bubon.m_checkWantedStateNextFrame = bfalse;
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    bbool Ray_ShooterGardianMorayBodyPart::receiveBubonHit( Bubon & _bubon, HitStim * _hitStim )
    {
        if ( _hitStim->getLevel() >= getTemplate()->getDamageLevels().size() )
        {
            ITF_ASSERT( _hitStim->getLevel() < getTemplate()->getDamageLevels().size() );
            return bfalse;
        }

        if ( _bubon.m_health == 0 )
            return bfalse;

        _bubon.m_health = Max( 0, _bubon.m_health - i32(  getTemplate()->getDamageLevels()[_hitStim->getLevel()] ) );
        
        // destroy it
        //
        if ( _bubon.m_health == 0 )
        {
            if ( _bubon.m_phantom != NULL )
            {
                if ( m_rewardGenerator.isInitialized() )
                {
                    Vec3d pos = _bubon.m_phantom->getPos().to3d(( m_parent->getPos().m_z ) );

                    for ( u32 i = 0; i < getTemplate()->getRewardNumber(); ++i )
                    {
                        f32 randY = Seeder::getSharedSeeder().GetFloat( 0.25f, 1.0f );
                        f32 randX = Seeder::getSharedSeeder().GetFloat( -1.0f, 1.0f );
                        Actor * rewardActor = m_rewardGenerator.getSpawnee( getActor()->getScene(), pos + Vec3d( 0.0f + randX, 1.0f + randY + ( i * 0.25f ), 0.0f), 0.f );
                        if ( rewardActor != NULL )
                        {
                            Ray_EventChildLaunch launchBubble( pos );
                            launchBubble.setSender(m_parent->getRef());
                            rewardActor->onEvent(&launchBubble);
                        }
                    }                
                }
                else 
                {
                    sendLumsReward( getTemplate()->getRewardNumber(), _hitStim, &_bubon );
                }
            }

            changeBubonState( _bubon, Death );
            destroyPhantom( _bubon.m_phantom );
        }
        else 
        {
            changeBubonState( _bubon, Hit );
        }
        return btrue;
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterGardianMorayBodyPart::sendLumsReward( u32 _number, HitStim * _hitStim, Bubon * _bubon )
    {
        if ( m_bubonReward != NULL )
        {
            m_bubonReward->clear();
            m_bubonReward->setup( getActor() );
            if ( _bubon != NULL )
            {
                m_bubonReward->setPos( _bubon->m_phantom->getPos().to3d( getActor()->getDepth() ) );
            }
            if ( _hitStim != NULL )
            {
                m_bubonReward->setRewardReceiver( _hitStim->getOriginalSender().isValid() ? _hitStim->getOriginalSender() : _hitStim->getSender() );
            }
            m_bubonReward->setNumRewards( _number );
            //m_bubonReward->setEjectionDir( -Vec2d::Right );
            GAMEMANAGER->onEvent(m_bubonReward);
        }
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    bbool Ray_ShooterGardianMorayBodyPart::receiveHit( HitStim * _hitStim, bbool _leftSide )
    {
        if ( getActorRef() != m_tailActor )
        {
            return bfalse;
        }
        
        bbool wasPlayingHit = ( m_curPlayingAnim == getTemplate()->getLeftHitAnim() ) || ( m_curPlayingAnim == getTemplate()->getRightHitAnim() ) ? btrue : bfalse;
        bbool res = Super::receiveHit( _hitStim, _leftSide );
        // restart hit animation, update bubons state to avoid bad bubon anim
        if ( res && wasPlayingHit )
        {
            if ( m_leftBubon.m_curState == Death )
            {
                changeBubonState( m_leftBubon, Dead );
            }
            if ( m_rightBubon.m_curState == Death )
            {
                changeBubonState( m_rightBubon, Dead );
            }
        }

        if ( ( m_curPlayingAnim == getTemplate()->getDeathAnim() ) && !m_destroyRewardGiven )
        {
            m_destroyRewardGiven = btrue;

            sendLumsReward( getTemplate()->getDestroyRewardNumber(), _hitStim, NULL );

            // is the last part ? yes consider part death now
            if ( removeAndKeepAliveOnDeath() )
            {
                m_isDead = btrue;
            }
        }

        return res;
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    bbool Ray_ShooterGardianMorayBodyPart::processHitStim( HitStim * _hitStim )
    {
        bbool hashit = bfalse;

        if ( _hitStim == NULL )
            return hashit;

        if ( ( m_leftBubon.m_phantom != NULL ) && ( _hitStim->getPhantomRef() == m_leftBubon.m_phantom->getRef() ) )
        {
            hashit = receiveBubonHit( m_leftBubon, _hitStim );
        }
        else if ( ( m_rightBubon.m_phantom != NULL ) && ( _hitStim->getPhantomRef() == m_rightBubon.m_phantom->getRef() ) )
        {
            hashit = receiveBubonHit( m_rightBubon, _hitStim );
        }

        return hashit;
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterGardianMorayBodyPart::createPhantom( PhysPhantom * & _phatom, PhysShapeCircle & _shape )
    {
        ITF_ASSERT_MSG( !_phatom, "Phantom already created");

        _shape.setRadius( getTemplate()->getBubonPhantomSize() );

        PhysPhantomInitCommonInfo initInfo;
        SET_OWNER(initInfo, m_parent->getRef());
        initInfo.m_shape = &_shape;
        initInfo.m_collisionGroup = ECOLLISIONGROUP_CHARACTER;
        initInfo.m_pos = m_parent->get2DPos();
        initInfo.m_angle = m_parent->getAngle();
        initInfo.m_userData = m_parent->getRef();
        _phatom = PHYSWORLD->allocPhantom(initInfo);

        PHYSWORLD->insertPhantom( _phatom, m_parent->getDepth() );
    }
    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterGardianMorayBodyPart::destroyPhantom( PhysPhantom * & _phatom )
    {
        if ( _phatom != NULL )
        {
            PHYSWORLD->removePhantom(_phatom);
            PHYSWORLD->deallocPhantom(_phatom);
            _phatom = NULL;
        }
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterGardianMorayBodyPart::changeBubonState( Bubon & _bubon, BubonState _state )
    {
        if ( getAnimComponent() != NULL )
        {
            BubonState newState = _state;
            _bubon.m_wantedState = newState;
            
            switch( _state )
            {
                case Hit:
                {
                    if ( _bubon.m_curState ==  Hit )
                    {
                        _bubon.m_wantedState = Hit;
                        _bubon.m_checkWantedStateNextFrame = btrue;
                        newState = Stand;
                    }
                    break;
                }
            }

            _bubon.m_curState = newState;
            u32 inputVal = u32( newState );

            if ( _bubon.m_animInput.isValid() )
            {
                getAnimatedComponent()->setInput( _bubon.m_animInput, inputVal );
            }
        }
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterGardianMorayBodyPart::onBubonStateFinished( Bubon & _bubon )
    {
        if ( _bubon.m_curState != Death )
        {
            BubonState nextState = Stand;
            changeBubonState( _bubon, nextState );
        }
        else
        {
            changeBubonState( _bubon, Dead );
        }
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterGardianMorayBodyPart::sendBubonStim( Bubon & _bubon )
    {
        if ( ( _bubon.m_phantom == NULL ) ||( _bubon.m_health == 0 ) )
            return;

        Vec2d stimPos = _bubon.m_phantom->getPos();
        
        PunchStim* stim = STIMSMANAGER->requestStim<PunchStim>( &(_bubon.m_shape) );

        if (!stim)
        {
            ITF_ASSERT_MSG(0, "stim limit reached!");
            return;
        }

        /*if ( m_prevPos.IsEqual( Vec2d::Zero ) )
        {
            m_prevPos = stimPos;
        }*/
        Vec2d dir = stimPos - getActor()->get2DPos();
        dir.normalize();

        stim->setPrevPos(stimPos);
        stim->setPos(stimPos);
        stim->setDirection(dir);
        stim->setAngle(getActor()->getAngle());
        stim->setDepth(getActor()->getDepth());
        stim->setFxPos( stimPos.to3d( getActor()->getDepth() ) );

        stim->setOriginalSender( m_parent->getRef() );
        stim->setSender( m_parent->getRef() );
        stim->setLevel( getTemplate()->getHitLevel() );
        stim->setReceivedHitType( getTemplate()->getHitType() );
        stim->setFaction( getTemplate()->getFaction() );

        STIMSMANAGER->sendStim(stim);
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    //                                                      TAIL PART
    //--------------------------------------------------------------------------------------------------------------------------------
    //IMPLEMENT_OBJECT_RTTI(Ray_ShooterGardianMorayTail)

    //Ray_ShooterGardianMorayTail::Ray_ShooterGardianMorayTail()
    //    :Super()
    //    , m_phantom( NULL )
    //    , m_phantomInserted( bfalse )
    //    , m_checkWantedStateNextFrame( bfalse )
    //    , m_phantomBoneIdx( U32_INVALID )
    //{
    //}

    ////--------------------------------------------------------------------------------------------------------------------------------
    //Ray_ShooterGardianMorayTail::~Ray_ShooterGardianMorayTail()
    //{
    //    destroyPhantom( m_phantom );
    //}

    ////--------------------------------------------------------------------------------------------------------------------------------
    //void Ray_ShooterGardianMorayTail::onInit()
    //{
    //    Super::onInit();

    //    createPhantom( m_phantom, getTemplate()->getPhantomShape() );

    //    enablePhatom( m_phantom, m_phantomInserted );

    //    m_curState = m_wantedState = Closed;
    //    m_checkWantedStateNextFrame = bfalse;
    //}

    ////--------------------------------------------------------------------------------------------------------------------------------
    //Actor * Ray_ShooterGardianMorayTail::spawnBodyPart()
    //{
    //    Actor * actor = Super::spawnBodyPart();
    //    if ( actor != NULL )
    //    {
    //        actor->registerEvent( AnimGameplayEvent::GetClassNameStatic(), m_parentComponent );
    //    }
    //    return actor;
    //}

    ////--------------------------------------------------------------------------------------------------------------------------------
    //void Ray_ShooterGardianMorayTail::onEvent( Event * _event )
    //{
    //    Super::onEvent( _event );

    //    if (AnimGameplayEvent * animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
    //    {
    //        if ( animEvent->getName() == s_AnimMrk_TailAnimFinished )
    //        {
    //            onStateFinished();
    //        }
    //    }
    //}
    ////--------------------------------------------------------------------------------------------------------------------------------
    //void Ray_ShooterGardianMorayTail::createPhantom( PhysPhantom * & _phatom, const PhysShape * _shape )
    //{
    //    ITF_ASSERT_MSG( !_phatom, "Phantom already created");
    //    ITF_ASSERT_MSG( _shape != NULL, "invalid shape ! ");

    //    PhysPhantomInitCommonInfo initInfo;
    //    SET_OWNER(initInfo, m_parent->getRef());
    //    initInfo.m_shape = _shape;
    //    initInfo.m_collisionGroup = ECOLLISIONGROUP_CHARACTER;
    //    initInfo.m_pos = m_parent->get2DPos();
    //    initInfo.m_angle = m_parent->getAngle();
    //    initInfo.m_userData = m_parent->getRef();
    //    _phatom = newAlloc(mId_Phys, PhysPhantom(initInfo));

    //}
    ////--------------------------------------------------------------------------------------------------------------------------------
    //void Ray_ShooterGardianMorayTail::destroyPhantom( PhysPhantom * & _phatom )
    //{
    //    if ( _phatom != NULL )
    //    {
    //        PHYSWORLD->removePhantom(_phatom);
    //        SF_DEL(_phatom);
    //        _phatom = NULL;
    //    }
    //}
    ////--------------------------------------------------------------------------------------------------------------------------------
    //void Ray_ShooterGardianMorayTail::disablePhatom( PhysPhantom * & _phatom, bbool & _insertFlag )
    //{
    //    if ( ( _phatom != NULL ) && _insertFlag )
    //    {
    //        PHYSWORLD->removePhantom( _phatom );
    //        _insertFlag = bfalse;
    //    }    
    //}

    ////--------------------------------------------------------------------------------------------------------------------------------
    //void Ray_ShooterGardianMorayTail::enablePhatom( PhysPhantom * & _phatom, bbool & _insertFlag )
    //{
    //    if ( ( _phatom != NULL ) && !_insertFlag )
    //    {
    //        PHYSWORLD->insertPhantom( _phatom, m_parent->getDepth() );
    //        _insertFlag = btrue;
    //    }
    //}

    ////--------------------------------------------------------------------------------------------------------------------------------
    //void Ray_ShooterGardianMorayTail::changeState( TailState _state )
    //{
    //    if ( m_animatedComponent != NULL )
    //    {
    //        TailState newState = _state;
    //        m_wantedState = newState;
    //        
    //        m_curState = newState;
    //        u32 inputVal = u32( newState );

    //        m_animatedComponent->setInput( s_AnimInput_TailState, inputVal );
    //    }
    //}

    ////--------------------------------------------------------------------------------------------------------------------------------
    //void Ray_ShooterGardianMorayTail::onStateFinished()
    //{
    //    //if ( _bubon.m_curState != Death )
    //    {
    //        TailState nextState = Opened;
    //        changeState( nextState );
    //    }
    //}

    ////--------------------------------------------------------------------------------------------------------------------------------
    //bbool Ray_ShooterGardianMorayTail::processHitStim( HitStim * _hitStim )
    //{
    //    bbool hashit = bfalse;

    //    if ( _hitStim == NULL )
    //        return hashit;

    //    if ( ( m_phantom != NULL ) && m_phantomInserted && ( _hitStim->getPhantomRef() == m_phantom->getRef() ) )
    //    {
    //        changeState( Hit );
    //        hashit = btrue;
    //        
    //    }
    //    return hashit;
    //}

    ////--------------------------------------------------------------------------------------------------------------------------------
    //void Ray_ShooterGardianMorayTail::update( f32 _dt )
    //{
    //    Super::update( _dt );

    //    if ( ( m_animatedComponent == NULL ) || ( m_phantom == NULL ) ) 
    //        return;

    //    if ( m_phantomInserted )
    //    {
    //        if  ( ( m_phantomBoneIdx == U32_INVALID ) && getTemplate()->getPhantomBoneName().isValid() ) 
    //        {
    //            m_phantomBoneIdx = m_animatedComponent->getBoneIndex( getTemplate()->getPhantomBoneName() );
    //        }
    //        if ( m_phantomBoneIdx != U32_INVALID )
    //        {
    //            Vec2d bonepos;
    //            m_animComponent->getBonePos( m_phantomBoneIdx, bonepos );
    //            m_phantom->setPos( bonepos );
    //            m_phantom->updateAABB();
    //        }

    //        DebugDraw::shape( m_phantom->getPos(), 0.0f, 0.0f, m_phantom->getShape(), Color::white() );
    //    }

    //    /*if ( !_bubon.m_checkWantedStateNextFrame && _bubon.m_wantedState != _bubon.m_curState )
    //    {
    //        changeBubonState( _bubon, _bubon.m_wantedState );
    //    }
    //    _bubon.m_checkWantedStateNextFrame = bfalse;*/
    //}


    //--------------------------------------------------------------------------------------------------------------------------------
    //                                                      TEMPLATES
    //--------------------------------------------------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_ShooterGardianMoray_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_ShooterGardianMoray_Template)
        SERIALIZE_MEMBER( "waterPerturbationForce",     m_waterPerturbationForce );
        SERIALIZE_MEMBER( "waterTestWidth",             m_waterTestWidth );
        SERIALIZE_MEMBER( "waterTestBubonWidth",        m_waterTestBubonWidth );
        SERIALIZE_MEMBER( "waterTestTailLength",        m_waterTestTailLength );
        SERIALIZE_MEMBER( "waterTestTailWidth",         m_waterTestTailWidth );
        SERIALIZE_MEMBER( "waterTestHeadLength",        m_waterTestHeadLength );
        SERIALIZE_MEMBER( "waterTestHeadWidth",         m_waterTestHeadWidth );
        SERIALIZE_MEMBER( "waterSpawnFxDelay",          m_waterSpawnFxDelay );
        
        SERIALIZE_MEMBER( "deathRewardSpawnPath",       m_deathRewardSpawnPath );
        SERIALIZE_MEMBER( "deathRewardNumber",          m_deathRewardNumber );
        SERIALIZE_MEMBER( "deathRewardSpawnDuration",   m_deathRewardSpawnDuration );
        SERIALIZE_MEMBER( "deathRewardSpawnDist",       m_deathRewardSpawnDist );
        SERIALIZE_MEMBER( "deathRewardSpawnAngle",      m_deathRewardSpawnAngle );
    END_SERIALIZATION()    

    Ray_ShooterGardianMoray_Template::Ray_ShooterGardianMoray_Template() 
    :Super()
    , m_waterPerturbationForce( 10.0f )
    , m_waterTestWidth( 5.0f )
    , m_waterTestBubonWidth( 2.0f )
    , m_waterTestTailLength( 2.0f )
    , m_waterTestTailWidth( 4.0f )
    , m_waterTestHeadLength( 2.0f )
    , m_waterTestHeadWidth( 4.0f )
    , m_waterSpawnFxDelay ( 0.1f )
    , m_deathRewardNumber( 1 )
    , m_deathRewardSpawnDuration( 2.0f )
    , m_deathRewardSpawnDist( Vec2d( 0.0f, 1.0f ) )
    , m_deathRewardSpawnAngle( Vec2d( 0.0f, MTH_PI ) )
    {

    }

//--------------------------------------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_ShooterGardianMorayBodyPart_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_ShooterGardianMorayBodyPart_Template)
        SERIALIZE_MEMBER( "leftBubonBoneName",      m_leftBubonBoneName );
        SERIALIZE_MEMBER( "rightBubonBoneName",     m_rightBubonBoneName );
        SERIALIZE_MEMBER( "bubonPhantomSize",       m_bubonPhantomSize );
        SERIALIZE_MEMBER( "bubonRewardSpawnPath",   m_bubonRewardSpawnPath );
        SERIALIZE_MEMBER( "bubonHealth",            m_bubonHealth );
        SERIALIZE_MEMBER( "rewardNumber",           m_rewardNumber );
        SERIALIZE_MEMBER( "destroyRewardNumber",    m_destroyRewardNumber );
        SERIALIZE_OBJECT_WITH_FACTORY("bubonReward",m_bubonReward, ACTORSMANAGER->getEventFactory() );  // non spawned rewards
        SERIALIZE_RAY_FACTION("faction",			m_faction);
        SERIALIZE_RECEIVEDHITTYPE("hitType",		m_hitType);
        SERIALIZE_MEMBER("hitLevel",		        m_hitLevel);
        SERIALIZE_MEMBER("tailPath",		        m_tailPath);
        SERIALIZE_MEMBER("transfoTotailAnim",           m_transfoTotailAnim );
        SERIALIZE_MEMBER("multiPlayerLifePointFactor",  m_multiPlayerLifePointFactor );
    END_SERIALIZATION()

    Ray_ShooterGardianMorayBodyPart_Template::Ray_ShooterGardianMorayBodyPart_Template()
    : Super()
    , m_bubonPhantomSize( 0.25f )
    , m_bubonHealth( -1 )
    , m_faction( FACTION_UNKNOWN )
    , m_hitType( RECEIVEDHITTYPE_FRONTPUNCH )
    , m_hitLevel( 0 )
    , m_rewardNumber( 0 )
    , m_destroyRewardNumber( 0 )
    , m_multiPlayerLifePointFactor( 1.0f )
    , m_bubonReward( NULL )
    {
    }

    Ray_ShooterGardianMorayBodyPart_Template::~Ray_ShooterGardianMorayBodyPart_Template()
    {
        SF_DEL( m_bubonReward );
    }

    

    //--------------------------------------------------------------------------------------------------------------------------------
    //IMPLEMENT_OBJECT_RTTI(Ray_ShooterGardianMorayTail_Template)
    //
    //BEGIN_SERIALIZATION_CHILD(Ray_ShooterGardianMorayTail_Template)
    //    SERIALIZE_OBJECT_WITH_FACTORY("phantomShape", m_phantomShape, PHYSWORLD->getShapesFactory());
    //    SERIALIZE_MEMBER( "phantomBoneName",  m_phantomBoneName );
    //    /*SERIALIZE_MEMBER( "openAnimation",  m_openAnimation );
    //    SERIALIZE_MEMBER( "closeAnimation", m_closeAnimation );
    //    SERIALIZE_MEMBER( "hitAnimation",   m_hitAnimation );*/
    //END_SERIALIZATION()


    //Ray_ShooterGardianMorayTail_Template::Ray_ShooterGardianMorayTail_Template()
    //: Super()
    //, m_phantomShape( NULL )
    //{
    //}

    //Ray_ShooterGardianMorayTail_Template::~Ray_ShooterGardianMorayTail_Template()
    //{
    //    SF_DEL( m_phantomShape );
    //}
}




