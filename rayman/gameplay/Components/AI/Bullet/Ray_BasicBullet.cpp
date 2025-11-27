#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BASICBULLET_H_
#include "rayman/gameplay/Components/AI/Bullet/Ray_BasicBullet.h"  
#endif //_ITF_RAY_BASICBULLET_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_SHAPECOMPONENT_H_
#include "gameplay/Components/Misc/ShapeComponent.h"
#endif //_ITF_SHAPECOMPONENT_H_

#ifndef _ITF_RAY_GAMEMATERIAL_H_
#include "rayman/gameplay/Ray_GameMaterial.h"
#endif //_ITF_RAY_GAMEMATERIAL_H_

#ifndef _ITF_FXKIT_H_
#include "engine/gameplay/FxKit.h"
#endif //_ITF_FXKIT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_BasicBullet)

    BEGIN_SERIALIZATION(Ray_BasicBullet)
    END_SERIALIZATION()

	//---------------------------------------------------------------------------------------------------------------------------------
    Ray_BasicBullet::Ray_BasicBullet()
        : Super()
		, m_actor( NULL )
		, m_template( NULL )
		, m_aiComponent( NULL )
        , m_fxControllerComponent( NULL )
		, m_isObsolete( bfalse )
        , m_isObsoleteFromHit( bfalse )
        , m_isObsoleteFromExitScreen( bfalse )
        , m_timer(0.f)
        , m_sendStims(btrue)
        , m_currentNumHits(0)
        , m_lastHitRef()
        , m_beforeStimNumHits( 0 )
        , m_hitSuccessfullEvtBounce( bfalse )
        , m_hitSuccessfullEvtImpact( bfalse )
		, m_active( bfalse )
		, m_prevFaction( U32_INVALID )
		, m_moveSpeed( 0.0f )
		, m_autoSeekStarted(bfalse)
		, m_autoSeekTimer(0.0f)
		, m_autoSeekTarget()
        , m_bounceCount(0)
        , m_envTouchedContactsGmat( NULL )
        , m_specificFaction( FACTION_UNKNOWN )
    {
    }

    Ray_BasicBullet::~Ray_BasicBullet()
    {
    }

	//---------------------------------------------------------------------------------------------------------------------------------
	void Ray_BasicBullet::init( Actor * _actor, const class Ray_BasicBullet_Template * _bulletTemplate )
	{
		ITF_ASSERT_CRASH( _actor, "actor is mendatory !" );
		m_actor = _actor; 
		m_template = _bulletTemplate;
		m_aiComponent = m_actor->GetComponent<AIComponent>();
        m_fxControllerComponent = m_actor->GetComponent<FXControllerComponent>();
		m_isObsolete = bfalse;
        m_isObsoleteFromHit = bfalse;
        m_isObsoleteFromExitScreen = bfalse;
		m_active = bfalse;
        m_owner.invalidate();
        m_specificFaction = FACTION_UNKNOWN;
        m_prevFaction = FACTION_UNKNOWN;

		// get default speed
        m_prevMoveDir = Vec2d::Zero;
		m_moveDir = _bulletTemplate->getDefaultSpeed();
		m_moveSpeed = m_moveDir.norm();
		if ( m_moveSpeed > 0.0f ) 
		{
			m_moveDir = m_moveDir / m_moveSpeed;
		}

		//*******************
		// Register events
		//*******************
		// query faction is registered only if the object has no AIcomponent
		if ( m_aiComponent == NULL )
		{
			ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryFaction,3678638658), this);
		}
		ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventHitSuccessful,818883013), this);
		ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSetDirection,187560938), this);
	}

	//---------------------------------------------------------------------------------------------------------------------------------
    void Ray_BasicBullet::onBecomeActive()
    {
		m_active = btrue;
        m_prevFaction = FACTION_UNKNOWN;
		
        // on active, change the actor faction
		if ( m_aiComponent != NULL )
		{
			m_prevFaction = m_aiComponent->getFaction();
			m_aiComponent->setFaction( getFaction() );
		}
        
        m_timer = getTemplate()->getLifetime();
        m_prevPos = m_actor->get2DPos();
        m_sendStims = btrue;
        m_currentNumHits = 0;
        m_lastHitRef.invalidate();
        m_beforeStimNumHits = 0;
        m_hitSuccessfullEvtBounce = bfalse;
        m_hitSuccessfullEvtImpact = bfalse;
        m_isObsolete = bfalse;
        m_isObsoleteFromHit = bfalse;
        m_isObsoleteFromExitScreen = bfalse;
        m_bounceCount = 0;
        m_hitSuccessfullTouchedActor.invalidate();
        m_lastBounceObj.invalidate();
    }

	//---------------------------------------------------------------------------------------------------------------------------------
    void Ray_BasicBullet::onBecomeInactive()
    {
		m_active = bfalse;
		// on inactive, reset the actor faction
		if ( m_aiComponent != NULL )
		{
			m_aiComponent->setFaction( m_prevFaction );
		}
        m_prevFaction = FACTION_UNKNOWN;
    }

    //---------------------------------------------------------------------------------------------------------------------------------
    void Ray_BasicBullet::setFaction( Faction _faction )
    {
        m_specificFaction = _faction;

        // AI faction still changed, update ai faction
        if ( ( m_aiComponent != NULL ) && ( m_prevFaction != FACTION_UNKNOWN ) )
        {
            m_aiComponent->setFaction( getFaction() );
        }        
    }

	//---------------------------------------------------------------------------------------------------------------------------------
    void Ray_BasicBullet::update(f32 _dt)
    {
		if ( !m_active || m_isObsolete )
			return;

        // store prev pos
        m_prevPos = m_actor->get2DPos();

        // autoseek
        if (getTemplate()->getAutoSeek())
        {
            updateAutoSeek(_dt);
        }

		// Update move
        //**************
        // get cam speed to update relative 
        Vec2d camSpeed = ( getTemplate()->isCameraRelative() ? CAMERACONTROLLERMANAGER->getCamSpeed().truncateTo2D() : Vec2d::Zero );
            
		Vec2d nextPos = m_actor->get2DPos() + ( m_moveDir * m_moveSpeed * _dt ) + ( camSpeed * _dt );
		f32 newAngle = m_actor->getAngle();
		// Update Rotation
		if ( getTemplate()->getRotationSpeed() != 0.0f )
		{
			newAngle += canonizeAngle( getTemplate()->getRotationSpeed() * _dt );
		}
        // Compute actor angle keeping actor side
        else if ( !m_prevMoveDir.IsEqual( m_moveDir ) )
        {
            bbool flipped = bfalse;
            AIUtils::getAngleAndFlipped(m_moveDir, newAngle, flipped);
            m_actor->setFlipped(flipped);
        }
        m_prevMoveDir = m_moveDir;

		m_actor->updateComponentPosAngle( nextPos.to3d((m_actor->getPos().m_z) ), newAngle, NULL );

		// Update life time
        if (getTemplate()->getLifetime() > 0)
        {
            m_timer -= _dt;
            if (m_timer < 0)
            {
                playFXInput( FXInput_Death );
                m_isObsolete = btrue;
            }
        }

        if ( getTemplate()->getDestroyOnExitScreen() && !isVisible() )
        {
            // self-destroy on exit screen
            m_isObsolete = btrue;
            m_isObsoleteFromExitScreen = btrue;
            playFXInput( FXInput_Death );
        }

        // check phatoms contacts
        //
        bbool hasHit = checkCharactersContacts();

        // check env contacts before (direct hit request are done) 
        //
        if ( !m_isObsolete && !hasHit )
        {
		    checkEnvContacts();
        }

        // Send stim of no real hit done
        //
      /*  if ( m_sendStims && !m_isObsolete )
        {
            sendStim();
        }*/
	}

	//---------------------------------------------------------------------------------------------------------------------------------
	const PhysShape * Ray_BasicBullet::getShape()
	{
		// Get the given bullet stim shape
		const PhysShape *  stimShape = getTemplate()->getStimShape();
		if (  stimShape == NULL )
		{
			PhysComponent * physComponent = m_actor->GetComponent<PhysComponent>();
			if ( physComponent != NULL )
			{
				m_physComponentShape.setRadius( physComponent->getRadius() );
				stimShape = &m_physComponentShape;
			}
			else
			{
				// get the shape component shape ?
				ShapeComponent * shapeComponent = m_actor->GetComponent<ShapeComponent>();
				if ( shapeComponent != NULL )
				{
					stimShape = shapeComponent->getShape();
				}
			}			
		}
		return stimShape;
	}

    //---------------------------------------------------------------------------------------------------------------------------------
    bbool Ray_BasicBullet::computeEnvTouchedData( const SCollidableContact & _contact )
    {
        PolyLine* poly(NULL);
        const PolyLineEdge* edge(NULL);
        m_envTouchedImpactNormal = _contact.m_normal;
        m_envTouchedImpactPos = _contact.m_contactPoint;
        m_envTouchedActor.invalidate();

        if ( _contact.m_collidableUserData == m_actor->getRef() )
        {
            return bfalse;
        }

        if ( _contact.m_edgeIndex != U32_INVALID )
        {
            AIUtils::getPolyLine(_contact.m_collidableUserData, _contact.m_edgeIndex, poly, edge);

            if ( poly && edge )
            {
                if ( poly->m_isCollisionActive )
                {
                    if ( const GameMaterial_Template * gmat = World::getGameMaterial(edge->getGameMaterial()) )
                    {
                        m_envTouchedContactsGmat = gmat->DynamicCast<Ray_GameMaterial_Template>(ITF_GET_STRINGID_CRC(Ray_GameMaterial_Template,2777394804));
                    }

                    if ( m_envTouchedContactsGmat && ( m_envTouchedContactsGmat->getNoStim() || !m_envTouchedContactsGmat->isSolid() || m_envTouchedContactsGmat->getIsWave() ) )
                    {
                        return bfalse;
                    }
                    m_envTouchedImpactNormal = edge->m_normalizedVector.getPerpendicular();
                    m_envTouchedImpactNormal.normalize();                        
                }

                Actor* ownerActor = poly->getOwnerActor();

                if ( ( ownerActor != NULL ) /*&& AIUtils::isEnemy(m_actor,ownerActor)*/ )
                {
                    m_envTouchedActor = ownerActor->getRef();
                }
                if ( ownerActor == m_actor )
                {
                    return bfalse;
                }
            }
        }

        return btrue;
    }
    //---------------------------------------------------------------------------------------------------------------------------------
    void Ray_BasicBullet::filterContacts()
    {
        PolyLine* poly(NULL);
        const PolyLineEdge* edge(NULL);
        Actor * ownerActor = m_envTouchedContacts.size() > 0 ? AIUtils::getActor(m_owner) : NULL;

        for ( u32 i = 0; i < m_envTouchedContacts.size(); i++ )
        {
            const SCollidableContact& contact = m_envTouchedContacts[i];

            if (    ( contact.m_collidableUserData == m_actor->getRef() ) || 
                    ( ( ownerActor != NULL ) && ( contact.m_collidableUserData == m_owner ) )
               )
            {
                m_envTouchedContacts.eraseKeepOrder(i);
                i--; 
                continue;
            }

            if ( contact.m_edgeIndex != U32_INVALID )
            {
                AIUtils::getPolyLine(contact.m_collidableUserData, contact.m_edgeIndex, poly, edge);

                if ( poly && 
                        (   ( poly->getOwnerActor() == m_actor ) || 
                            ( ( ownerActor != NULL ) && ( poly->getOwnerActor() == ownerActor ) )
                         )
                   )
                {
                    m_envTouchedContacts.eraseKeepOrder(i);
                    i--; 
                    continue;
                }                
            }
        }
    }

    //---------------------------------------------------------------------------------------------------------------------------------
    bbool Ray_BasicBullet::checkCharactersContacts()
    {
        m_envTouchedContacts.clear();
        m_envTouchedContactsGmat = NULL;

		const PhysShape *  curShape = getShape();

		if ( curShape != NULL )
        {
            PHYSWORLD->collidePhantoms( m_prevPos, m_actor->get2DPos(), m_actor->getAngle(), curShape, m_actor->getDepth(), ECOLLISIONFILTER_ALL, m_envTouchedContacts );

            filterContacts();
		
            if ( m_envTouchedContacts.size() > 0 )
            {   
                m_localCharContactProcessed.clear();
                u32 contactCount = m_envTouchedContacts.size();

                for ( u32 contactIdx = 0; contactIdx < contactCount; ++contactIdx  )
                {
                    Actor * actorToHit = NULL;
                    const SCollidableContact& firstContact = m_envTouchedContacts[contactIdx];

                    if ( m_localCharContactProcessed.find( firstContact.m_collidableUserData ) >= 0 )
                    {
                        continue;
                    }
                    if ( !computeEnvTouchedData( firstContact ) )
                    {
                        continue;
                    }
                    m_localCharContactProcessed.push_back( firstContact.m_collidableUserData );

                    // get touched actor if not a polyline
                    //
                    if ( !m_envTouchedActor.isValid() )
                    {
                        m_envTouchedActor = firstContact.m_collidableUserData;
                    }
                    actorToHit = AIUtils::getActor( m_envTouchedActor );
                    
                    if ( actorToHit != NULL )
                    {
                        PunchStim * stim = setupStim();
                        stim->setPhantomRef( firstContact.m_collidable );

                        ITF_ASSERT_MSG(stim, "Can't setup stim");
                        if ( stim != NULL )
                        {
                            stim->addContact( firstContact );
                            /*for ( u32 i = 0; i < m_envTouchedContacts.size(); i++ )
                            {
                                stim->addContact( m_envTouchedContacts[i] );
                            }*/
                            stim->setShape( curShape );

                            actorToHit->onEvent( stim );
                            STIMSMANAGER->recycleStim( stim );
                        }
                        // has been hit ?
                        //
                        if ( m_beforeStimNumHits != m_currentNumHits )
                        {
                            playFXInput( FXInput_Paf );
                            return btrue;
                        }
                        // successive hits on one target ?
                        //
                        if (getTemplate()->getSkipSuccessiveHits() && m_lastHitRef.isValid())
                        {
                            return btrue;
                        }
                        // bounce ?
                        //
                        else if ( getTemplate()->getAllowBounce() && m_hitSuccessfullEvtBounce )
                        {
                            // invert normals
                            for ( u32 i = 0; i < m_envTouchedContacts.size(); i++ )
                            {
                                SCollidableContact& contact = m_envTouchedContacts[i];
                                contact.m_normal = -contact.m_normal;   // because the given normal is inverted
                            }
                            if ( bounce() )
                            {
                                m_lastBounceObj = m_envTouchedActor;
                                playFXInput( FXInput_Bounce );
                                
                                return bfalse;
                            }
                        }
                    }
                }
            }
        }
        m_lastHitRef.invalidate();
        return bfalse;
    }

	//---------------------------------------------------------------------------------------------------------------------------------
	void Ray_BasicBullet::checkEnvContacts()
	{
        // check env ?
		if ( !getTemplate()->getCheckEnvironment() && !getTemplate()->getDestroyOnEnvironment() )
		{
			return;
		}
		m_envTouchedContacts.clear();
        m_envTouchedContactsGmat = NULL;

		const PhysShape *  curShape = getShape();

		if ( curShape != NULL )// getTemplate()->getCollisionRadius() > 0.0 )
		{
			//PhysShapeCircle circle( getTemplate()->getCollisionRadius() );
			PHYSWORLD->checkEncroachment( m_prevPos, m_actor->get2DPos(), m_actor->getAngle(), curShape, ECOLLISIONFILTER_ENVIRONMENT, m_actor->getDepth(), m_envTouchedContacts );

            filterContacts();
		
            if ( m_envTouchedContacts.size() > 0 )
            {   
                Actor * actorToHit = NULL;
                const SCollidableContact& firstContact = m_envTouchedContacts[0];

                if ( !computeEnvTouchedData( firstContact ) )
                {
                    return;
                }
                actorToHit = AIUtils::getActor( m_envTouchedActor );
              
                // Environment HIT
                //-------------------
                /*if ( getTemplate()->getCheckEnvironmentHit() && ( actorToHit != NULL ) )
                {
                    PunchStim * stim = setupStim();
                    ITF_ASSERT_MSG(stim, "Can't setup stim");
                    if (stim)
                    {
                        for ( u32 i = 0; i < m_envTouchedContacts.size(); i++ )
                        {
                            stim->addContact( m_envTouchedContacts[i] );
                        }
                        stim->setShape( curShape );

                        actorToHit->onEvent( stim );
                        STIMSMANAGER->recycleStim( stim );
                    }
                    if ( m_beforeStimNumHits != m_currentNumHits )
                    {
                        playFXInput( FXInput_Paf );
                        return;
                    }
                }*/

                // Environment Destroy
                //----------------------
                if ( getTemplate()->getDestroyOnEnvironment() )
                {
                    m_isObsolete = btrue;
                    playFXInput( FXInput_Impact );
                    return;
                }

                // Environment Bounce
                //---------------------
                if ( getTemplate()->getAllowBounce() && ( m_envTouchedContactsGmat != NULL ) )
                {
                    if ( m_envTouchedContactsGmat->getShooterBounce() && bounce() )
                    {
                        if ( actorToHit != NULL )
                        {
                            m_lastBounceObj = actorToHit->getRef();
                        }
                        // play FX
                        playFXInput( FXInput_Bounce );
                    }
                    else
                    {
                        m_isObsolete = btrue;
                        playFXInput( FXInput_Impact );
                    }
                }                
			}
        }
        m_envTouchedContactsGmat = NULL;
	}

    //---------------------------------------------------------------------------------------------------------------------------------
    void Ray_BasicBullet::setPosOutOfEnvCollisions()
    {
        const PhysShape * curShape = getShape();
        if ( curShape == NULL )
        {
            return;
        }

        // don't manage other shape than circles 
        const PhysShapeCircle * circleShape = curShape->DynamicCast<PhysShapeCircle>(ITF_GET_STRINGID_CRC(PhysShapeCircle,3922519168));
        if ( circleShape == NULL )
            return;

        m_envTouchedContacts.clear();
        m_envTouchedContactsGmat = NULL;

        PHYSWORLD->checkEncroachment( m_actor->get2DPos(), m_actor->get2DPos(), m_actor->getAngle(), curShape, ECOLLISIONFILTER_ENVIRONMENT, m_actor->getDepth(), m_envTouchedContacts );
        
        filterContacts();

        Vec2d edgeNormal;
        PolyLine* poly(NULL);
        const PolyLineEdge* edge(NULL);
        Vec2d vecOut = Vec2d::Zero;
       
        // check all poly normals
        //
        for ( u32 i = 0; i < m_envTouchedContacts.size(); ++i )
        {
            const SCollidableContact& contact = m_envTouchedContacts[i];
            edgeNormal = contact.m_normal;

            if ( contact.m_edgeIndex != U32_INVALID )
            {
                AIUtils::getPolyLine(contact.m_collidableUserData, contact.m_edgeIndex, poly, edge);
                if ( poly && edge )
                {
                    edgeNormal = edge->m_normalizedVector.getPerpendicular();
                    edgeNormal.normalize();
                }
            }

            vecOut += edgeNormal * ( f32_Max( 0.0f, circleShape->getRadius() - ( contact.m_contactPoint - contact.m_testPosition ).norm() ) + 0.02f );
        }
        m_actor->set2DPos( m_actor->get2DPos() + vecOut );
    }

    //---------------------------------------------------------------------------------------------------------------------------------
    bbool Ray_BasicBullet::bounce()
    {
        if ( m_bounceCount >= getTemplate()->getBounceMaxNb() )
        {
            m_isObsolete = btrue;
            return bfalse;
        }

        Vec2d contactPos = Vec2d::Zero;
        Vec2d contactNormal = Vec2d::Zero;
        Vec2d edgeNormal;
        const PhysShape * curShape = getShape();
        const PhysShapeCircle * circleShape = NULL;
        PolyLine* poly(NULL);
        const PolyLineEdge* edge(NULL);
        if ( curShape != NULL )
        {
            circleShape = curShape->DynamicCast<PhysShapeCircle>(ITF_GET_STRINGID_CRC(PhysShapeCircle,3922519168));
        }
        u32 contactCount = m_envTouchedContacts.size();
        u32 finalComputedContactCount = contactCount;
        Vec2d finalUncollPos = m_actor->get2DPos();
        
        Vec2d moveDir = m_actor->get2DPos() - m_prevPos;
        bbool checkMoveDir = bfalse;
        bbool contactPosInit = bfalse;
        if ( !moveDir.IsEqual( Vec2d::Zero, MTH_EPSILON ) )
        {
            checkMoveDir = ( contactCount > 1 ) ? btrue : bfalse;
            moveDir.normalize();
        }

        // check all normals
        //
        for ( u32 i = 0; i < contactCount; ++i )
        {
            const SCollidableContact& contact = m_envTouchedContacts[i];
            edgeNormal = contact.m_normal;

            if ( contact.m_edgeIndex != U32_INVALID )
            {
                AIUtils::getPolyLine(contact.m_collidableUserData, contact.m_edgeIndex, poly, edge);
                if ( poly && edge )
                {
                    edgeNormal = edge->m_normalizedVector.getPerpendicular();
                    edgeNormal.normalize();

                    if ( checkMoveDir && ( edgeNormal.dot( moveDir ) > 0.2f ) )
                    {
                        --finalComputedContactCount;
                        continue;
                    }
                }
            }

            //contactPos += contact.m_testPosition;
            if ( edgeNormal.dot(contactNormal) > -0.95 )
            {
                contactNormal += edgeNormal;
            }
            else
            {
                --finalComputedContactCount;
            }

#ifndef ITF_FINAL
           /* DebugDraw::circle( m_actor->get2DPos(), 0.0f,circleShape->getRadius() + MTH_EPSILON, Color::yellow(), 999.0f );
            DebugDraw::circle( contact.m_testPosition, 0.0f, 0.05f, Color::blue(), 999.0f );
            DebugDraw::circle( contact.m_contactPoint, 0.0f, 0.05f, Color::red(), 999.0f );
            DebugDraw::circle( contact.m_testPosition, 0.0f, circleShape->getRadius() + MTH_EPSILON, Color::blue(), 999.0f );*/
#endif // ITF_FINAL

            // keep out of collisions
            //
            if ( !contactPosInit )//( i == 0 )
            {
#ifndef ITF_FINAL
                /*DebugDraw::circle( m_prevPos, 0.0f,circleShape->getRadius() + MTH_EPSILON, Color::grey(), 999.0f );
                DebugDraw::circle( m_actor->get2DPos(), 0.0f,circleShape->getRadius() + MTH_EPSILON, Color::yellow(), 999.0f );
                DebugDraw::circle( contact.m_testPosition, 0.0f, 0.05f, Color::blue(), 999.0f );
                DebugDraw::circle( contact.m_contactPoint, 0.0f, 0.05f, Color::red(), 999.0f );*/
                //DebugDraw::circle( contact.m_testPosition, 0.0f, circleShape->getRadius() + MTH_EPSILON, Color::blue(), 999.0f );
#endif // ITF_FINAL
                finalUncollPos = contact.m_testPosition;
                contactPos = contact.m_contactPoint;
                contactPosInit = btrue;
            }
#ifndef ITF_FINAL
            /*else DebugDraw::circle( contact.m_testPosition, 0.0f, circleShape->getRadius() + MTH_EPSILON, Color::white(), 999.0f );

            DebugDraw::circle( contact.m_contactPoint, 0.0f, 0.05f, Color::red(), 999.0f );*/
#endif // ITF_FINAL
        }
        // if no computed contact point, invert the move dir
        //
        if ( finalComputedContactCount == 0 )
        {
            ITF_ASSERT( contactCount > 0 );
            if ( contactCount > 0 )
            {
                finalUncollPos = m_envTouchedContacts[0].m_testPosition;
                contactPos = m_envTouchedContacts[0].m_contactPoint;
                contactNormal = moveDir;
            }
            else
            {
                return bfalse;
            }            
        }
        else
        {
            contactNormal = contactNormal / f32( finalComputedContactCount );
            contactNormal.normalize();
        }        

        Vec2d newDir = getMoveDir().normalize();
        f32 vecDot = newDir.dot( contactNormal );

        // bounce on env
        if ( vecDot <= 0.0f )
        {
            f32 angle = newDir.getShortestAngle( contactNormal );
            newDir = contactNormal.Rotate( ( MTH_PIBY2 + ( MTH_PIBY2 - angle ) ) * getSign( contactNormal.getPerpendicular().dot( newDir ) ) );
        }
        else
        {
            //newDir = -newDir;
            newDir = contactNormal;
        }
       
        finalUncollPos -= ( moveDir * 0.02f );

        // check if we dont continue in coll
        //
        if ( ( circleShape != NULL ) && ( ( finalUncollPos - m_actor->get2DPos() ).dot( contactNormal ) < 0.f ) )
        {
            Vec2d vecOut = Vec2d::Zero;
            
            for ( u32 i = 0; i < contactCount; ++i )
            {
                const SCollidableContact& contact = m_envTouchedContacts[i];
                edgeNormal = contact.m_normal;

                if ( contact.m_edgeIndex != U32_INVALID )
                {
                    AIUtils::getPolyLine(contact.m_collidableUserData, contact.m_edgeIndex, poly, edge);
                    if ( poly && edge )
                    {
                        edgeNormal = edge->m_normalizedVector.getPerpendicular();
                        edgeNormal.normalize();
                    }
                }
                vecOut += ( contact.m_contactPoint - contact.m_testPosition ) + ( edgeNormal * ( circleShape->getRadius() + MTH_EPSILON ) );
            }

            finalUncollPos = m_actor->get2DPos() + vecOut + ( newDir * 0.02f );
        }

        m_actor->set2DPos( finalUncollPos );
        m_prevPos = finalUncollPos;

        setMoveDir( newDir );
        m_bounceCount++; 
        return btrue;
    }

    //---------------------------------------------------------------------------------------------------------------------------------
    PunchStim * Ray_BasicBullet::setupStim()
    {
        if (getTemplate()->getNumHits() != 0 && m_currentNumHits == getTemplate()->getNumHits())
        {
            return NULL;
        }

        PunchStim* stim = STIMSMANAGER->requestStim<PunchStim>( getShape() );

        if (!stim)
        {
            ITF_ASSERT_MSG(0, "stim limit reached!");
            return NULL;
        }

        Vec2d stimPos = m_actor->get2DPos();
        if ( m_prevPos.IsEqual( Vec2d::Zero ) )
        {
            m_prevPos = stimPos;
        }
        Vec2d dir = stimPos - m_prevPos;

        dir.normalize();

        stim->setPrevPos(m_prevPos);
        stim->setPos(stimPos);
        stim->setDirection(dir);
        stim->setAngle(m_actor->getAngle());
        stim->setDepth(m_actor->getDepth());
        stim->setFxPos(m_actor->getPos());

        stim->setOriginalSender( m_owner );
        stim->setSender( m_actor->getRef() );
        stim->setLevel( getTemplate()->getHitLevel() );
        stim->setReceivedHitType( getTemplate()->getHitType() );
        // TODO: setCollisionFilter() here?
        stim->setFaction( getFaction() );

        m_hitSuccessfullEvtBounce = bfalse;
        m_hitSuccessfullEvtImpact = bfalse;
        m_hitSuccessfullTouchedActor.invalidate();
        m_beforeStimNumHits = m_currentNumHits;

        return stim;
    }

	//---------------------------------------------------------------------------------------------------------------------------------
    void Ray_BasicBullet::sendStim()
    {
		PunchStim* stim = setupStim();
        if ( stim == NULL )
        {
            return;
        }        
        STIMSMANAGER->sendStim(stim);
    }

	//---------------------------------------------------------------------------------------------------------------------------------
    void Ray_BasicBullet::onEvent( Event* _event )
    {
		if ( EventSetDirection* moveDirEvent = _event->DynamicCast<EventSetDirection>(ITF_GET_STRINGID_CRC(EventSetDirection,187560938)) )
		{
            // Store event sender has owner of the bullet
            m_owner = moveDirEvent->getSender();
			setMoveDir( moveDirEvent->getDirection() );
		}

		if ( !m_active )
		{
			return;
		}

        if ( EventQueryFaction* query = _event->DynamicCast<EventQueryFaction>(ITF_GET_STRINGID_CRC(EventQueryFaction,3678638658)) )
        {
            query->setFaction( getFaction() );
        }
        else if ( EventHitSuccessful* hitSuccessful = _event->DynamicCast<EventHitSuccessful>(ITF_GET_STRINGID_CRC(EventHitSuccessful,818883013)) )
        {
            onHitSuccessful(hitSuccessful);
        }
        /*else if ( EventStimNotify* notifyStim = _event->DynamicCast<EventStimNotify>(ITF_GET_STRINGID_CRC(EventStimNotify,1337973749)) )
        {
            onNotifyStim( notifyStim );
        }*/
    }

	//---------------------------------------------------------------------------------------------------------------------------------
    void Ray_BasicBullet::onHitSuccessful( EventHitSuccessful* _hitSuccessful )
    {
        if (getTemplate()->getSkipSuccessiveHits())
        {
            if (m_lastHitRef == _hitSuccessful->getSender())
            {
                return;
            }
            m_lastHitRef = _hitSuccessful->getSender();
        }

        if ( /*( m_lastBounceObj != _hitSuccessful->getSender() ) && */ _hitSuccessful->getHasBeenDamaged() )
        {
            m_hitSuccessfullEvtImpact = btrue;
            m_hitSuccessfullTouchedActor = _hitSuccessful->getSender();

            if ( getTemplate()->getNumHits() != 0 )
            {
                m_currentNumHits++;
                if (m_currentNumHits == getTemplate()->getNumHits())
			    {
                    m_isObsolete = btrue;
                    m_isObsoleteFromHit = btrue;
			    }
            }
        }        

        m_hitSuccessfullEvtBounce = ( m_lastBounceObj != _hitSuccessful->getSender() );
    }

    //---------------------------------------------------------------------------------------------------------------------------------
    //void Ray_BasicBullet::onNotifyStim( EventStimNotify* _notifyStim )
    //{
    //    const SCollidableContact&   contact = _notifyStim->getContact();

    //    m_envTouchedContacts.clear();
    //    m_envTouchedContactsGmat = NULL;
    //    computeEnvTouchedData( contact );

    //    if ( m_hitSuccessfullEvtImpact )
    //    {
    //        playFXInput( FXInput_Paf );
    //    }        

    //    // "Notify" is sent after target stim event, then hit success must still be arrived
    //    if ( getTemplate()->getAllowBounce() && m_hitSuccessfullEvtBounce )
    //    {
    //        if ( m_beforeStimNumHits == m_currentNumHits )
    //        {
    //            //m_envTouchedContacts.clear();
    //            m_envTouchedContacts.push_back( contact );
    //            m_envTouchedContacts[0].m_normal = -m_envTouchedContacts[0].m_normal; // because the given normal is inverted
    //            
    //            if ( bounce() )
    //            {
    //                m_lastBounceObj = _notifyStim->getSender();
    //                playFXInput( FXInput_Bounce );
    //            }                
    //        }
    //    }
    //}

    //---------------------------------------------------------------------------------------------------------------------------------
    void Ray_BasicBullet::playFXInput( FXInputType _type )
    {
        if ( m_fxControllerComponent != NULL )
        {
            StringID fxName;
            ActorRef bounceObj;

            switch( _type )
            {
                case FXInput_Bounce :
                {
                    bounceObj = m_lastBounceObj;
                    fxName = ITF_GET_STRINGID_CRC(BulletBounce,3403191475);
                    if ( getTemplate()->getFxNameBounce().isValid() )
                    {
                        fxName = getTemplate()->getFxNameBounce();
                    }
                    break;
                }
                case FXInput_Impact :
                {
                    fxName = ITF_GET_STRINGID_CRC(BulletImpact,1829556953);
                    if ( getTemplate()->getFxNameImpact().isValid() )
                    {
                        fxName = getTemplate()->getFxNameImpact();
                    }
                    break;
                }
                case FXInput_Paf :
                {
                    fxName = ITF_GET_STRINGID_CRC(BulletPaf,2135948464);
                    if ( getTemplate()->getFxNamePaf().isValid() )
                    {
                        fxName = getTemplate()->getFxNamePaf();
                    }
                    break;
                }
                case FXInput_Death :
                {
                    fxName = ITF_GET_STRINGID_CRC(BulletDeath,1709438441);
                    if ( getTemplate()->getFxNameDeath().isValid() )
                    {
                        fxName = getTemplate()->getFxNameDeath();
                    }
                    m_envTouchedImpactPos = m_actor->get2DPos();
                    break;
                }                
            }
           
            if ( m_envTouchedContactsGmat != NULL )
            {
                const FxKit * FXKit = m_envTouchedContactsGmat->getFxKit();
                if ( FXKit != NULL )
                {
                    const StringID& finalFxName = FXKit->getFx( fxName );
                    if ( finalFxName.isValid() )
                    {
                        fxName = finalFxName;
                    }                
                }
            }

            if ( fxName.isValid() )
            {
                // kill previous fx before
                if ( getTemplate()->getFxKillAllBefore() )
                {
                    m_fxControllerComponent->stopAllFX();
                }

                u32 fxHandle; 
                if ( m_envTouchedContactsGmat != NULL )
                {
                    fxHandle = m_fxControllerComponent->playMaterialFX( fxName, m_envTouchedContactsGmat );
                }
                else if ( m_hitSuccessfullTouchedActor.isValid() )
                {
                    fxHandle = m_fxControllerComponent->playFeedback( m_actor->getRef(), fxName, m_hitSuccessfullTouchedActor );
                }
                else if ( bounceObj.isValid() )
                {
                    fxHandle = m_fxControllerComponent->playFeedback( m_actor->getRef(), fxName, bounceObj );
                }
                else
                {
                    fxHandle = m_fxControllerComponent->playFX( fxName );
                }
                
                if ( fxHandle != U32_INVALID )
                {
                    m_fxControllerComponent->setFXPosFromHandle( fxHandle, m_envTouchedImpactPos.to3d((m_actor->getPos().m_z) ) );
                    m_fxControllerComponent->setFXAngleFromHandle( fxHandle, m_envTouchedImpactNormal.getAngle() );
                }
            }
        }
    }

    //---------------------------------------------------------------------------------------------------------------------------------
    void Ray_BasicBullet::updateAutoSeek( f32 _dt )
    {
        // NB[LaurentCou]: as the rest of this class this is all very similar to
        // Ray_SeekingAIComponent and derived stuff, in an ideal world this
        // could all get merged
        
        m_autoSeekTimer += _dt;
        
        // start delay
        if (!m_autoSeekStarted)
        {
            if (m_autoSeekTimer >= getTemplate()->getAutoSeekStartDelay())
            {
                m_autoSeekStarted = btrue;
                m_autoSeekTimer = getTemplate()->getAutoSeekRefreshDelay();
            }
            else
            {
                return;
            }
        }
        
        // refresh delay
        if (m_autoSeekTimer >= getTemplate()->getAutoSeekRefreshDelay())
        {
            Actor* closestEnemy = AIUtils::getClosestEnemy(
                m_actor,
                m_actor->isFlipped(),
                getTemplate()->getAutoSeekRange(),
                bfalse);
            if (closestEnemy)
            {
                m_autoSeekTarget = closestEnemy->getRef();
            }
            else
            {
                m_autoSeekTarget = ActorRef::InvalidRef;
            }
            m_autoSeekTimer = 0.0f;
        }

        Actor* target = m_autoSeekTarget.getActor();
        if (!target)
        {
            return;
        }
        
        const Vec2d targetDirection = (target->get2DPos() - m_actor->get2DPos()).normalize();
        const f32 currentAngle = m_actor->getAngle() + (m_actor->isFlipped() ? MTH_PI : 0.0f);
        const f32 targetAngle = targetDirection.getAngle();
        const f32 angleDiff = getShortestAngleDelta(currentAngle, targetAngle);
        const f32 turnAngle = getTemplate()->getAutoSeekMaxTurnAngle().Radians() * _dt;
        f32 angleToTurn = 0.0f;
        const f32 sign = angleDiff > 0.0f? 1.0f:-1.0f;
        if (f32_Abs(angleDiff) > turnAngle)
            angleToTurn = sign * turnAngle;
        else
            angleToTurn = angleDiff;

        f32 finalAngle = currentAngle + angleToTurn;
        NormalizeAngle2PI(finalAngle);
        m_moveDir = Vec2d::Right.Rotate(finalAngle);
    }

    //---------------------------------------------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_BasicBullet_Template)
    BEGIN_SERIALIZATION(Ray_BasicBullet_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("stimShape",	m_stimShape, PHYSWORLD->getShapesFactory());
        SERIALIZE_RAY_FACTION("faction",			m_faction);
        SERIALIZE_MEMBER("lifetime",				m_lifetime);
        SERIALIZE_MEMBER("numHits",					m_numHits);
        SERIALIZE_MEMBER("skipSuccessiveHits",		m_skipSuccessiveHits);
        SERIALIZE_RECEIVEDHITTYPE("hitType",		m_hitType);
        SERIALIZE_MEMBER("hitLevel",		        m_hitLevel);
		SERIALIZE_MEMBER("destroyOnExitScreen",		m_destroyOnExitScreen);
		SERIALIZE_MEMBER("destroyOnEnvironment",	m_destroyOnEnvironment );
        SERIALIZE_MEMBER("checkEnvironmentHit",	    m_checkEnvironmentHit );
		SERIALIZE_MEMBER("checkEnvironment",		m_checkEnvironment );
		SERIALIZE_MEMBER("defaultSpeed",			m_defaultSpeed );
		SERIALIZE_MEMBER("rotationSpeed",			m_rotationSpeed );
        SERIALIZE_MEMBER("allowBounce",			    m_allowBounce );
        SERIALIZE_MEMBER("bounceMaxNb",			    m_bounceMaxNb );
        SERIALIZE_MEMBER("cameraRelative",			m_cameraRelative );
        SERIALIZE_MEMBER("autoSeek",                m_autoSeek);
        SERIALIZE_MEMBER("autoSeekStartDelay",      m_autoSeekStartDelay);
        SERIALIZE_MEMBER("autoSeekRefreshDelay",    m_autoSeekRefreshDelay);
        SERIALIZE_MEMBER("autoSeekRange",           m_autoSeekRange);
        SERIALIZE_MEMBER("autoSeekMaxTurnAngle",    m_autoSeekMaxTurnAngle);

        SERIALIZE_MEMBER("fxNameImpact",            m_fxNameImpact );
        SERIALIZE_MEMBER("fxNameBounce",            m_fxNameBounce );
        SERIALIZE_MEMBER("fxNamePaf",               m_fxNamePaf );
        SERIALIZE_MEMBER("fxNameDeath",             m_fxNameDeath );
        SERIALIZE_MEMBER("fxKillAllBefore",         m_fxKillAllBefore );
    END_SERIALIZATION()

    Ray_BasicBullet_Template::Ray_BasicBullet_Template()
    : m_stimShape(NULL)
    , m_faction(FACTION_UNKNOWN)
    , m_lifetime(-1.f)  // -1 = infinite
    , m_hitType( RECEIVEDHITTYPE_FRONTPUNCH )
    , m_hitLevel( 0 )
    , m_destroyOnExitScreen(btrue)
    , m_numHits(1)
    , m_skipSuccessiveHits(bfalse)
	, m_destroyOnEnvironment( btrue )
	, m_checkEnvironment( bfalse )
    , m_checkEnvironmentHit( bfalse )
	, m_defaultSpeed( Vec2d::Zero )
	, m_rotationSpeed( 0.f )
    , m_autoSeek(bfalse)
    , m_autoSeekStartDelay(1.0f)
    , m_autoSeekRefreshDelay(1.0f)
    , m_autoSeekRange(1.0f)
    , m_autoSeekMaxTurnAngle(btrue, 10.0f)
    , m_allowBounce( bfalse )
    , m_bounceMaxNb( 60 )
    , m_cameraRelative( bfalse )
    , m_fxKillAllBefore( bfalse )
    {
    }

    Ray_BasicBullet_Template::~Ray_BasicBullet_Template()
    {
        SF_DEL(m_stimShape);
    }
};
