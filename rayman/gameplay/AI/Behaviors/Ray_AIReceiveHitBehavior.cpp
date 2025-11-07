#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIRECEIVEHITBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ray_AIReceiveHitBehavior.h"
#endif //_ITF_RAY_AIRECEIVEHITBEHAVIOR_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_AIRECEIVEHITACTION_H_
#include "gameplay/AI/Actions/AIReceiveHitAction.h"
#endif //_ITF_AIRECEIVEHITACTION_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_RAY_AVOIDANCECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_AvoidanceComponent.h"
#endif //_ITF_RAY_AVOIDANCECOMPONENT_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_POLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/PolylineComponent.h"
#endif //_ITF_POLYLINECOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIReceiveHitBehavior_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AIReceiveHitBehavior_Template)
        SERIALIZE_CONTAINER_OBJECT("receiveHits", m_receiveHits);
        SERIALIZE_MEMBER("canReceiveRehits", m_canReceiveRehits);
        SERIALIZE_MEMBER("hurtDuration", m_hurtDuration);
        SERIALIZE_MEMBER("maxNumberOfSuccessiveHits", m_maxNumberOfSuccessiveHits);

        SERIALIZE_OBJECT_WITH_FACTORY("dominoHitShape", m_dominoHitShape, PHYSWORLD->getShapesFactory());
        SERIALIZE_MEMBER("canRehit", m_canRehit);
        SERIALIZE_MEMBER("dominoHitSpeedMultiplier", m_dominoHitSpeedMultiplier);
        SERIALIZE_MEMBER("minHitLevelForDomino", m_minHitLevelForDomino);
        SERIALIZE_MEMBER("disableStickOnWallsOnHit", m_disableStickOnWallsOnHit);
        SERIALIZE_MEMBER("receiveDamageFromDomino", m_receiveDamageFromDomino);
        SERIALIZE_MEMBER("canBubblize", m_canBubblize);
        SERIALIZE_OBJECT_WITH_FACTORY("ceilingAction", m_ceilingAction, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("wallAction", m_wallAction, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_MEMBER("deathMarkerName", m_deathMarkerName );
    END_SERIALIZATION()

    Ray_AIReceiveHitBehavior_Template::Ray_AIReceiveHitBehavior_Template()
    : m_canReceiveRehits(btrue)
    , m_hurtDuration(0.5f)
    , m_maxNumberOfSuccessiveHits(5)
	, m_dominoHitShape(NULL)
	, m_dominoHitSpeedMultiplier(0.2f)
	, m_canRehit(bfalse)
    , m_minHitLevelForDomino(1)
    , m_disableStickOnWallsOnHit(bfalse)
    , m_canBubblize(btrue)
    , m_receiveDamageFromDomino(btrue)
    , m_ceilingAction(NULL)
    , m_wallAction(NULL)
    {
    }

    Ray_AIReceiveHitBehavior_Template::~Ray_AIReceiveHitBehavior_Template()
    {
		SF_DEL(m_dominoHitShape);
        SF_DEL(m_ceilingAction);
        SF_DEL(m_wallAction);
    }

    BEGIN_SERIALIZATION_SUBCLASS(Ray_AIReceiveHitBehavior_Template, ReceiveHitData)
        SERIALIZE_CONTAINER("types", m_types);
        SERIALIZE_MEMBER("level", m_level);
        SERIALIZE_MEMBER("useIfAlive", m_useIfAlive);
        SERIALIZE_MEMBER("useIfDead", m_useIfDead);
        SERIALIZE_MEMBER("useIfInAir",m_useIfInAir);
        SERIALIZE_MEMBER("useIfOnGround",m_useIfOnGround);
        SERIALIZE_MEMBER("isInterruptible", m_isInterruptible);
        SERIALIZE_OBJECT_WITH_FACTORY("action", m_action, ACTORSMANAGER->getAIActionsFactory());

    END_SERIALIZATION()

    Ray_AIReceiveHitBehavior_Template::ReceiveHitData::~ReceiveHitData()
    {
        SF_DEL(m_action);
    }

    IMPLEMENT_OBJECT_RTTI(Ray_AIReceiveHitBehavior)

    BEGIN_VALIDATE_BEHAVIOR(Ray_AIReceiveHitBehavior)
        VALIDATE_BEHAVIOR_PARAM("", m_phantomComponent || m_polylineComponent, "Ray_AIReceiveHitBehavior requires a PhantomComponent");
    END_VALIDATE_BEHAVIOR()

    Ray_AIReceiveHitBehavior::Ray_AIReceiveHitBehavior()
        : Super()
        , m_phantomComponent(NULL)
        , m_avoidanceComponent(NULL)
        , m_physComponent(NULL)
        , m_polylineComponent(NULL)
        , m_consecutiveHitCount(U32_INVALID)
        , m_isInAir(bfalse)
        , m_hurtTimer(0.0f)
        , m_justEntered(btrue)
		, m_dominoHitShape(NULL)
		, m_reHit(bfalse)
        , m_lastHitFrame( U32_INVALID )

    {
    }

    Ray_AIReceiveHitBehavior::~Ray_AIReceiveHitBehavior()
    {
        // registered actions have already been destroyed by AIBehavior
        // set them to null here to avoid deleting heap data
        for (u32 i = 0; i < m_receiveHits.size(); i++)
        {
            m_receiveHits[i].m_action = NULL;
        }
		SF_DEL(m_dominoHitShape);

    }

    Ray_AIReceiveHitBehavior::ReceiveHitData::~ReceiveHitData()
    {
    }

    void Ray_AIReceiveHitBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_receiveHits.resize(getTemplate()->getReceiveHits().size());

        for (u32 i = 0; i < m_receiveHits.size(); i++)
        {
            m_receiveHits[i].m_action = createAiActionAndCast<AIReceiveHitAction>(getTemplate()->getReceiveHits()[i].m_action);
            ITF_ASSERT_CRASH(m_receiveHits[i].m_action != NULL, "Ray_AIReceiveHitBehavior has no action or a mistyped action!");
        }
        m_ceilingAction = static_cast<AIReceiveHitAction*>(createAiAction(getTemplate()->getCeilingAction()));
        m_wallAction = static_cast<AIReceiveHitAction*>(createAiAction(getTemplate()->getWallAction()));
        // setup re-hit
        m_reHit = bfalse;
        //
        Super::onActorLoaded(_hotReload);

        m_phantomComponent = m_actor->GetComponent<PhantomComponent>(); // mandatory, validated
        m_avoidanceComponent = m_actor->GetComponent<Ray_AvoidanceComponent>(); // not mandatory
        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>(); // not mandatory
        m_polylineComponent = m_actor->GetComponent<PolylineComponent>(); // not mandatory if phatomComponent

		ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventHitSuccessful,818883013),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryIsReceivingHit,1252094534),m_aiComponent);

		if (getTemplate()->getDominoHitShape())
		{
			if ( !m_dominoHitShape ||
				 m_dominoHitShape->GetObjectClassCRC() != getTemplate()->getDominoHitShape()->GetObjectClassCRC() )
			{
				SF_DEL(m_dominoHitShape);
				m_dominoHitShape = PHYSWORLD->getShapesFactory()->CreateObject<PhysShape>(getTemplate()->getDominoHitShape()->GetObjectClassCRC());
			}

			// make sure the existing scaled shape is not marked as flipped before copying the points
			if ( PhysShapePolygon* polygon = m_dominoHitShape->DynamicCast<PhysShapePolygon>(ITF_GET_STRINGID_CRC(PhysShapePolygon,3261252112)) )
			{
				polygon->setFlipped(bfalse);
			}

			PhysShape::CopyShapeScaled(getTemplate()->getDominoHitShape(), m_dominoHitShape, m_actor->getScale());
		}

    }

	PhysShape * Ray_AIReceiveHitBehavior::getDominoHitShape() const
	{
		if (m_dominoHitShape )
			return m_dominoHitShape;
		else
            return m_phantomComponent?m_phantomComponent->getShape():NULL;
	}


    void Ray_AIReceiveHitBehavior::onActivate()
    {
        m_consecutiveHitCount = 1;

        if (m_avoidanceComponent)
        {
            // if we're in front of Rayman and getting hit repeatedly (eg mitraille),
            // we don't want other AIs to come closer.
            m_avoidanceComponent->setEnabled(btrue);
        }

        if (m_phantomComponent) 
    		m_phantomComponent->setShape(ITF_GET_STRINGID_CRC(dominoPhantom,2108039163));

        if (getTemplate()->getDisableStickOnWallsOnHit())
            m_physComponent->setStickOnWalls(bfalse);

        m_justEntered = btrue;
    }

    void Ray_AIReceiveHitBehavior::onDeactivate()
    {
        Super::onDeactivate();

        m_consecutiveHitCount = U32_INVALID;

        if (m_avoidanceComponent)
        {
            m_avoidanceComponent->setEnabled(bfalse);
        }

    }

    bbool Ray_AIReceiveHitBehavior::canReceiveHit( HitStim* _hit )
    {
        PunchStim* hit = _hit->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519));
        ITF_ASSERT_MSG(hit, "received hit is not a punch!");
        if (!hit)
        {
            return bfalse;
        }

        if (m_consecutiveHitCount != U32_INVALID && m_consecutiveHitCount >= getTemplate()->getMaxNumberOfSuccessiveHits())
            return bfalse;

        if ( hit->getSender() == m_actor->getRef() )
        {
            // don't get hit by our own stims
            return bfalse;
        }


        bbool isDominoHit = _hit->IsClassCRC(ITF_GET_STRINGID_CRC(RehitStim,1008690800));

        if (isDominoHit && !getTemplate()->getCanReceiveRehits())
            return bfalse;

        // can't get hit by friendlies, except for domino hits
        if ( !isDominoHit && !AIUtils::isEnemyHit(_hit, m_aiComponent->getFaction()) )
        {
            return bfalse;
        }


        if ( m_aiComponent->getCurrentBehavior() == this )
        {
            // can't receive a domino hit while already getting hit
            if ( isDominoHit )
            {
                return bfalse;
            }


            // find out if the hit we're currently receiving can be interrupted
            for (u32 i = 0; i < m_receiveHits.size(); i++)
            {
                ReceiveHitData& data = m_receiveHits[i];

                if ( isCurrentAction(data.m_action) )
                {
                    if (!getTemplate()->getReceiveHits()[i].m_isInterruptible)
                        return bfalse;
                }
            }
        }


        return btrue;
    }

    // setData has to be called AFTER setBehavior
    // (it is assumed that the behavior is active as we'll set the action here)
    void Ray_AIReceiveHitBehavior::setData( HitStim* _hit )
    {
        //restoreAfterHitWall();


        ITF_ASSERT_MSG(m_aiComponent->getCurrentBehavior() == this, "Ray_AIReceiveHitBehavior has to be activated BEFORE you setData!");
        
        PunchStim* hit = _hit->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519));
        ITF_ASSERT_MSG(hit, "received hit is not a punch!");
        if (!hit)
        {
            m_aiComponent->onBehaviorFinished();
            return;
        }
        if (m_consecutiveHitCount != U32_INVALID)
        {
            m_consecutiveHitCount++;
        }

        m_identifier = hit->getIdentifier();

        ActorRef previousAttacker = m_aiComponent->getLastAttacker();

        // apply the damage right now, so we know if it was a deadly hit
        bbool receiveDamage = AIUtils::isDamageHit(_hit, m_aiComponent->getFaction()) || (getTemplate()->getReceiveDamageFromDomino() && _hit->DynamicCast<RehitStim>(ITF_GET_STRINGID_CRC(RehitStim,1008690800)));
        m_aiComponent->receiveHit(hit, receiveDamage, 0);
        bbool alive = (m_aiComponent->getHealth() > 0);


        if (!alive && getTemplate()->getCanBubblize())
        {
            ActorRef perpetrator = hit->getSender();
            if (RehitStim * rehit = hit->DynamicCast<RehitStim>(ITF_GET_STRINGID_CRC(RehitStim,1008690800)))
            {
                perpetrator = rehit->getRewardReceiver();
            }
            RAY_GAMEMANAGER->bubblizerAchievement(m_actor->getRef(),perpetrator,hit->getIdentifier());
        }


        // select the proper action
        AIReceiveHitAction* receiveHitAction = NULL;
        m_isInAir = m_physComponent ? (m_physComponent->getStickedPolyline() == NULL) : bfalse;
       
        AIUtils::EdgeType edgeType = AIUtils::EdgeType_Invalid;
        if (!m_isInAir && m_physComponent)
        {
            const PolyLineEdge * poly = m_physComponent->getStickedEdge();
            if (poly)
                edgeType = AIUtils::getEdgeType(*poly);
        }

        if (edgeType == AIUtils::EdgeType_Ceiling && m_ceilingAction)
        {
            receiveHitAction = m_ceilingAction;
        }
        else if (edgeType == AIUtils::EdgeType_Wall && m_wallAction)
        {
            receiveHitAction = m_wallAction;
        }
        else
        {
            for (u32 i = 0; i < m_receiveHits.size(); i++)
            {
                const Ray_AIReceiveHitBehavior_Template::ReceiveHitData& dataTemplate = getTemplate()->getReceiveHits()[i];

                if ( ( dataTemplate.m_types.size() == 0 || dataTemplate.m_types.find(hit->getReceivedHitType()) != -1 ) &&
                    ( dataTemplate.m_level == U32_INVALID || dataTemplate.m_level == hit->getLevel() ) &&
                    ( alive && dataTemplate.m_useIfAlive || !alive && dataTemplate.m_useIfDead ) &&
                    ( m_isInAir == dataTemplate.m_useIfInAir || !m_isInAir && dataTemplate.m_useIfOnGround))
                {
                    receiveHitAction = m_receiveHits[i].m_action;
                    break;
                }
            }
        }

        u32 lastHitFrame = m_lastHitFrame;
        m_lastHitFrame = CURRENTFRAME;

        if (m_fxController)
        {
            // NB[LaurentCou]: avoid restarting the same SFX several times in a
            // few frames (RO-11003) -> if the hurt countdown is started, ignore
            // SL : it's good to test if it's the same sender too....
            if ( ( m_lastHitFrame > lastHitFrame + 1 ) || ( previousAttacker != hit->getSender() ) )
            {
                // JP : until all necessary data are correctly defined with the new structure, then use the old one instead
                const StringID feedbackAction = hit->getActionFromHit();
                u32 fxHandle = m_fxController->playFeedback(hit->getSender(), feedbackAction, m_actor->getRef());

                //RO-12880	[3C | Fight] Hit FX : Probl�me de positionnement
                //hit fx pos given by rayman is based on a bone this bone may not always be properly positionned,
                // in the case where contacts are given use the average position
                const u32 numContacts = hit->getContacts().size();
                if (numContacts)
                {
                    Vec2d hitPos(0.0f,0.0f);
                    f32 count = 0.0f;
                    for (u32 i = 0; i < hit->getContacts().size();++i)
                    {
                    const SCollidableContact & contact = hit->getContacts()[i];
                        hitPos += contact.m_contactPoint;
                        count += 1.0f;
                    }
                    hitPos /= count;
                    m_fxController->setFXPos(fxHandle, hitPos.to3d(m_actor->getDepth()));
                }
                else
                    m_fxController->setFXPos(fxHandle, hit->getFxPos());
            }
		}


        m_hurtTimer = getTemplate()->getHurtDuration();

        if (receiveHitAction)
        {
            receiveHitAction->setData(hit);
            setAction(receiveHitAction, btrue);
            m_animComponent->resetCurTime();    // TODO: should this go in setAction?
        }
        else 
        {
            Ray_AIComponent * ray_aiComponent = m_aiComponent->DynamicCast<Ray_AIComponent>(ITF_GET_STRINGID_CRC(Ray_AIComponent,2143961386));
            if ( ( ray_aiComponent == NULL ) || !ray_aiComponent->hasBeenVacuumed() )
            {
                ITF_ASSERT_MSG(!alive, "no receiveHit action found for this hit!");
            }
            
            m_aiComponent->onBehaviorFinished();
        }

		//
		if (_hit->getLevel() >= getTemplate()->getMinHitLevelForDomino() && getTemplate()->getCanRehit())
		{
			m_reHit = btrue;
			updateRehit();
		}
		else
			m_reHit = bfalse;

    }

	//
	void Ray_AIReceiveHitBehavior::updateRehit()
	{
		PhysShape* shape = m_dominoHitShape;
		if (!shape )
		{
            if (m_phantomComponent)
    			shape = m_phantomComponent->getShape();
		}
		else
		{
			// flip shape if necessary
			if ( PhysShapePolygon* polygon = m_dominoHitShape->DynamicCast<PhysShapePolygon>(ITF_GET_STRINGID_CRC(PhysShapePolygon,3261252112)) )
			{
				polygon->setFlipped(m_actor->isFlipped());
			}
		}
		if (shape)
		{

            PhysContactsContainer contacts;
            PHYSWORLD->collidePhantoms(m_actor->get2DPos(), m_actor->get2DPos(), m_actor->getAngle(),
                shape, m_actor->getDepth(), ECOLLISIONFILTER_ALL,
                contacts);
            
            ITF_MAP<ActorRef,ITF_VECTOR<SCollidableContact*> > actorCollisions;

            const u32 contactCount = contacts.size();

            for ( u32 i = 0; i < contactCount; i++ )
            {
                BaseObject* obj = contacts[i].m_collidableUserData.getObject();
                if (!obj)
                {
                    continue;
                }

                // die on polylines
                if ( PolyLine* polyLine = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747)) )
                {
                    if (polyLine->getOwnerActor() == m_actor)
                    {
                        continue;
                    }
                    else if (polyLine->getOwnerActor())
                        actorCollisions[polyLine->getOwnerActor()->getRef()].push_back(&contacts[i]);
                }
                else
                {
                    ActorRef otherActorRef = contacts[i].m_collidableUserData;

                    if (otherActorRef == m_actor->getRef())
                    {
                        continue;
                    }

                    Actor* otherActor = otherActorRef.getActor();
                    if (!otherActor)
                    {
                        continue;
                    }

                    actorCollisions[otherActorRef].push_back(&contacts[i]);
                }
            }
            RehitStim stim;
            stim.setReceivedHitType(RECEIVEDHITTYPE_FRONTPUNCH);
            stim.setIsRadial(bfalse);
            stim.setLevel(m_aiComponent->getLastHitLevel());
            stim.setSender(m_actor->getRef());
            stim.setFaction(m_aiComponent->getFaction());
            stim.setIdentifier(m_identifier);
			Vec2d speed = m_physComponent->getSpeed();
			speed.normalize();
			if (speed == Vec2d::Zero)
			{
				speed = m_aiComponent->getLastHitDir();
			}
			stim.setDirection(speed);
            Ray_AIComponent * ray_aiComponent = m_aiComponent->DynamicCast<Ray_AIComponent>(ITF_GET_STRINGID_CRC(Ray_AIComponent,2143961386));
            if (ray_aiComponent)
            {
                stim.setRewardReceiver(ray_aiComponent->getRewardReceiver());
            }

            const f32 z = m_actor->getDepth();
            stim.setAngle(m_actor->getAngle());

            for (ITF_MAP<ActorRef,ITF_VECTOR<SCollidableContact*> >::iterator it = actorCollisions.begin(); it != actorCollisions.end(); ++it)
            {

                ITF_VECTOR<SCollidableContact*> & collidables = it->second;
                SCollidableContact* firstContact = collidables[0];

                stim.setFxPos(firstContact->m_contactPoint.to3d(z));
                stim.setPos(firstContact->m_contactPoint);
                for (ITF_VECTOR<SCollidableContact*>::iterator it2 = collidables.begin(); it2 != collidables.end(); ++it2)
                {
                    if (stim.getContacts().size() < stim.getContacts().capacity())
                        stim.addContact(*(*it2));
                }

                Actor * actor = it->first.getActor();
                if (actor)
                    actor->onEvent(&stim);
                stim.resetContacts();
            }
        }

	}

    void Ray_AIReceiveHitBehavior::onActionFinished()
    {
        Super::onActionFinished();
        m_aiComponent->onBehaviorFinished();
    }

    void Ray_AIReceiveHitBehavior::updateAnimInputs()
    {
        Super::updateAnimInputs();

        if (m_animatedComponent)
        {
            m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(ReceivedHitCount,2225937995), m_consecutiveHitCount);
            m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(IsInAir,1210226103), m_isInAir);
            m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(HurtMode,2985216329), m_hurtTimer != 0.0f);
        }
    }

	void Ray_AIReceiveHitBehavior::processHitSuccessful( EventHitSuccessful* _onHitSuccessful )
	{
		ObjectRef other = _onHitSuccessful->getSender();

		if ( other != m_actor->getRef() )
		{
            Actor * actor = ActorRef(other).getActor();
            if ( m_physComponent != NULL && actor && AIUtils::getFaction(actor) != RAY_FACTION_BREAKABLE )
            {
			    m_physComponent->setSpeed(m_physComponent->getSpeed() * getTemplate()->getDominoHitSpeedMultiplier());
            }
			m_reHit = bfalse;
		}
	}

    void Ray_AIReceiveHitBehavior::onEvent(Event * _event)
    {
        Super::onEvent(_event);

        if (AnimGameplayEvent * onMarker = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)))
        {
            if ( ( getTemplate()->getDeathMarkerName().isValid() && ( onMarker->getName() == getTemplate()->getDeathMarkerName() ) ) || ( onMarker->getName() == MRK_Death ) )
            {
                if (m_aiComponent->getHealth() <= 0)                
                    m_aiComponent->onBehaviorFinished();
            }
        }
		else if (EventHitSuccessful *onHitSuccessful = _event->DynamicCast<EventHitSuccessful>(ITF_GET_STRINGID_CRC(EventHitSuccessful,818883013)))
		{
			processHitSuccessful(onHitSuccessful);
		}
        else if (Ray_EventQueryIsReceivingHit* eventReceivingHit = _event->DynamicCast<Ray_EventQueryIsReceivingHit>(ITF_GET_STRINGID_CRC(Ray_EventQueryIsReceivingHit,1252094534)))
        {
            eventReceivingHit->setValue(btrue);
        }
    }

    void Ray_AIReceiveHitBehavior::update( f32 _dt )
    {
        Super::update(_dt);
        m_hurtTimer = Max(0.0f,m_hurtTimer-_dt);
        if (m_justEntered)
        {
            m_justEntered = bfalse;
        }
        else
        {
            m_isInAir = m_physComponent ? (m_physComponent->getStickedPolyline() == NULL) : bfalse;
        }

		if (m_dominoHitShape)
		{
			if ( PhysShapePolygon* polygon = m_dominoHitShape->DynamicCast<PhysShapePolygon>(ITF_GET_STRINGID_CRC(PhysShapePolygon,3261252112)) )
			{
				polygon->setFlipped(m_actor->isFlipped());
			}
		}
		if (m_reHit)
		{
			updateRehit();
		}

    }

#ifdef ITF_SUPPORT_EDITOR
    void Ray_AIReceiveHitBehavior::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        if (m_reHit)
        {
            PhysShape* shape = m_dominoHitShape;
            if (!shape)
            {
                if (m_phantomComponent)
                    shape = m_phantomComponent->getShape();
            }
            else
            {
                // flip shape if necessary
                if ( PhysShapePolygon* polygon = m_dominoHitShape->DynamicCast<PhysShapePolygon>(ITF_GET_STRINGID_CRC(PhysShapePolygon,3261252112)) )
                {
                    polygon->setFlipped(m_actor->isFlipped());
                }
            }
            if (shape)
                DebugDraw::shape(m_actor->getPos(),m_actor->getAngle(),shape,Color::blue(),0.0f,"DOMINO");
        }
    }
#endif // ITF_SUPPORT_EDITOR
}
