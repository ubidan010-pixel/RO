#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_TRIGGERBOUNCECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_TriggerBounceComponent.h"
#endif //_ITF_RAY_TRIGGERBOUNCECOMPONENT_H_

#ifndef _ITF_RAY_GAMEPLAYSTIMS_H_
#include "rayman/gameplay/Ray_GameplayStims.h"
#endif //_ITF_RAY_GAMEPLAYSTIMS_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_SHAPEDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/ShapeDetectorComponent.h"
#endif //_ITF_SHAPEDETECTORCOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_AIMANAGER_H_
#include "gameplay/managers/AIManager.h"
#endif //_ITF_AIMANAGER_H_

#ifndef _ITF_RAY_AIUTILS_H_
#include "Rayman/gameplay/AI/Utils/Ray_AIUtils.h"
#endif //_ITF_RAY_AIUTILS_H_

#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#include "rayman/gameplay/Ray_GameplayEvents.h"
#endif //_ITF_RAY_GAMEPLAYEVENTS_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_TriggerBounceComponent)

BEGIN_SERIALIZATION_CHILD(Ray_TriggerBounceComponent)
END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(Ray_TriggerBounceComponent)
    //VALIDATE_COMPONENT_PARAM("", m_animComponent, "Ray_TriggerBounceComponent requires an AnimLight/AnimatedComponent");
    VALIDATE_COMPONENT_PARAM("", m_shapeDetector, "Ray_TriggerBounceComponent requires a DetectorComponent");
END_SERIALIZATION()

Ray_TriggerBounceComponent::Ray_TriggerBounceComponent()
: m_phantom(NULL)
, m_animComponent(NULL)
, m_receivedHitAttacker(ITF_INVALID_OBJREF)
, m_receivedHitDir(Vec2d::Zero)
, m_bouncePlayed(bfalse)
, m_disabled(bfalse)
, m_registered(bfalse)
{
}

Ray_TriggerBounceComponent::~Ray_TriggerBounceComponent()
{
    unregisterObjects();
    PHYSWORLD->deallocPhantom(m_phantom);
}

void Ray_TriggerBounceComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_animComponent = m_actor->GetComponent<AnimLightComponent>();  // mandatory, validated
    m_linkComponent = m_actor->GetComponent<LinkComponent>(); // optional
    m_shapeDetector = m_actor->GetComponent<ShapeDetectorComponent>();

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventFruitAttachQuery,1115792865), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventFruitDetach,60508415), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(PunchStim,200533519), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryBlocksHits,133722380), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventDisableCollision,3761196874), this);

    if ( getTemplate()->getSendBounceOnHit() && getTemplate()->getPhantomShape() )
    {
        PhysPhantomInitCommonInfo initInfo;
        SET_OWNER(initInfo, m_actor->getRef());
        initInfo.m_shape = getTemplate()->getPhantomShape();
        initInfo.m_collisionGroup = ECOLLISIONGROUP_CHARACTER;
        initInfo.m_pos = m_actor->get2DPos();
        initInfo.m_angle = m_actor->getAngle();
        initInfo.m_userData = m_actor->getRef();
        m_phantom = PHYSWORLD->allocPhantom(initInfo);
    }
}

void Ray_TriggerBounceComponent::onBecomeActive()
{
    Super::onBecomeActive();

    registerObjects();
}

void Ray_TriggerBounceComponent::onBecomeInactive()
{
    Super::onBecomeInactive();

    unregisterObjects();
}

void Ray_TriggerBounceComponent::onEvent(Event * _event)
{
    Super::onEvent(_event);

    if ( Ray_EventFruitAttachQuery* fruitAttachQuery = _event->DynamicCast<Ray_EventFruitAttachQuery>(ITF_GET_STRINGID_CRC(Ray_EventFruitAttachQuery,1115792865)) )
    {
        processFruitAttachQuery(fruitAttachQuery);
    }
    else if ( Ray_EventFruitDetach* detachEvent = _event->DynamicCast<Ray_EventFruitDetach>(ITF_GET_STRINGID_CRC(Ray_EventFruitDetach,60508415)) )
    {
        processFruitDetach(detachEvent);
    }
    else if ( PunchStim * punch = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)) )
    {
        processReceivePunch(punch);
    }
    else if (EventQueryBlocksHits * queryBlocksHits = _event->DynamicCast<EventQueryBlocksHits>(ITF_GET_STRINGID_CRC(EventQueryBlocksHits,133722380)))
    {
        queryBlocksHits->setCanBlockHits(getTemplate()->getBlocksHits());
    }
    else if ( EventDisableCollision* collision = _event->DynamicCast<EventDisableCollision>(ITF_GET_STRINGID_CRC(EventDisableCollision,3761196874)) )
    {
        setDisabled( collision->getDisable() );
    }
}

void Ray_TriggerBounceComponent::processFruitAttachQuery( Ray_EventFruitAttachQuery* _stickEvent )
{
    if ( getTemplate()->getIsFruitStick() && !_stickEvent->getAttached() )
    {
        _stickEvent->setAttached(btrue,bfalse);

        i32 foundIndex = m_attachedActors.find(_stickEvent->getFruit());

        if ( foundIndex == -1 )
        {
            m_attachedActors.push_back(_stickEvent->getFruit());
        }
    }
}

void Ray_TriggerBounceComponent::processFruitDetach( Ray_EventFruitDetach* _detachEvent )
{
    i32 foundIndex = m_attachedActors.find(_detachEvent->getFruit());

    if ( foundIndex != -1 )
    {
        m_attachedActors.eraseNoOrder(foundIndex);
    }
}

void Ray_TriggerBounceComponent::detectBounce()
{
    ActorRefList& actorsInsideThisFrame = m_shapeDetector->getActorsInside();
    u32 numActors = actorsInsideThisFrame.size();

    for ( u32 i = 0; i < numActors; i++ )
    {
        ActorRef actorRef = actorsInsideThisFrame[i];

        if ( m_actorsInside.find(actorRef) == -1 )
        {
            processNewDetectedActor(actorRef);
        }
    }

    m_actorsInside = actorsInsideThisFrame;
}

void Ray_TriggerBounceComponent::processNewDetectedActor( ActorRef _ref )
{
    if ( isMemorized(_ref) && getTemplate()->getHurtLevel() )
    {
        return;
    }

    if ( isAttached(_ref) )
    {
        return;
    }

    Actor* actorActivator = AIUtils::getActor(_ref);

    if ( actorActivator )
    {
        if ( getTemplate()->getFaction() != FACTION_UNKNOWN && !AIUtils::isTarget(getTemplate()->getFaction(),AIUtils::getFaction(actorActivator)) )
        {
            return;
        }

        f32 detectRange = getTemplate()->getDetectRange().ToRadians();

        if ( detectRange )
        {
            Vec2d dirToActor = actorActivator->get2DPos() - m_shapeDetector->getShapePos();
            dirToActor.normalize();
            f32 angle = getVec2Angle(dirToActor);
            f32 minAngle = m_actor->getAngle() + MTH_PIBY2 - (detectRange*0.5f);
            f32 maxAngle = m_actor->getAngle() + MTH_PIBY2 + (detectRange*0.5f);
            f32 clampedAngle = ClampAngle(Angle(bfalse,angle),Angle(bfalse,minAngle),Angle(bfalse,maxAngle)).ToRadians();

            if ( clampedAngle != angle )
            {
                return;
            }
        }

        Vec2d stimDir = getBounceDirection(actorActivator);

        sendBounce( actorActivator,stimDir, bfalse );

        if ( !m_memorized.full() )
        {
            TriggeredMem memData;

            memData.m_ref = _ref;
            memData.m_timer = getTemplate()->getRetriggerDelay();
            memData.m_wasHit = bfalse;

            m_memorized.push_back(memData);
        }
    }
}

void Ray_TriggerBounceComponent::Update( f32 _deltaTime )
{
    Super::Update(_deltaTime);
    
    if (m_disabled)
    {
        return;
    }

    detectBounce();
    bbool isPlayed = m_bouncePlayed;
    if (m_animComponent)
    {
        isPlayed = isPlayed && !m_animComponent->isAnimLooped() && m_animComponent->isSubAnimFinished();
    }

    if ( isPlayed  )
    {
		if(getTemplate()->getDisableAtBounce())
		{
			m_actor->disable(); 
		}
		else
		{
            if (m_animComponent)
            {
			    m_animComponent->setAnim(getTemplate()->getIdleAnim());
            }
			m_bouncePlayed = bfalse;
		}
    }

    if ( m_receivedHitAttacker != ITF_INVALID_OBJREF )
    {
        bbool found = bfalse;
        u32 numMemorized = m_punchHurtMemorized.size();

        for ( u32 i = 0; i < numMemorized; i++ )
        {
            if ( m_punchHurtMemorized[i].m_ref == m_receivedHitAttacker )
            {
                found = btrue;
                break;
            }
        }

        if ( !found )
        {
            Actor* attacker = AIUtils::getActor(m_receivedHitAttacker);

            if ( attacker )
            {
                sendBounce( attacker,-m_receivedHitDir, btrue );

                if ( !m_punchHurtMemorized.full() )
                {
                    TriggeredMem newMem;
                    newMem.m_ref = m_receivedHitAttacker;
                    newMem.m_timer = getTemplate()->getRetriggerDelay();
                    newMem.m_wasHit = btrue;
                    m_punchHurtMemorized.push_back(newMem);
                }
            }
        }

        m_receivedHitAttacker = ITF_INVALID_OBJREF;
    }

    updateMemorized(_deltaTime);

    if ( m_phantom )
    {
        m_phantom->setPos3D(m_actor->getPos());
        m_phantom->setAngle(m_actor->getAngle());
        m_phantom->updateAABB();
    }
}

void Ray_TriggerBounceComponent::updateMemorized( f32 _dt )
{
    u32 memIndex = 0;
    u32 numMemorized = m_memorized.size();
    
    while ( memIndex < numMemorized )
    {
        TriggeredMem& memData = m_memorized[memIndex];
        bbool decr = bfalse;

        memData.m_timer = Max(memData.m_timer-_dt,0.f);

        if ( memData.m_timer == 0.f )
        {
            bbool stillInside = m_shapeDetector->isActorInside(memData.m_ref);

            if ( stillInside && !isAttached(memData.m_ref) && getTemplate()->getIsSendMoreThanOnce() )
            {
                Actor* receiver = memData.m_ref.getActor();

                if ( receiver )
                {
                    Vec2d stimDir = getBounceDirection(receiver);

                    sendBounce( receiver, stimDir, memData.m_wasHit );
                    memData.m_timer = getTemplate()->getRetriggerDelay();
                }
            }
            
            if ( !stillInside && memData.m_timer == 0.f )
            {
                m_memorized.eraseNoOrder(memIndex);
                numMemorized--;
                decr = btrue;
            }
        }
        
        if ( !decr )
        {
            memIndex++;
        }
    }

    memIndex = 0;
    numMemorized = m_punchHurtMemorized.size();

    while ( memIndex < numMemorized )
    {
        TriggeredMem& memData = m_punchHurtMemorized[memIndex];

        memData.m_timer -= _dt;

        if ( memData.m_timer <= 0.f )
        {
            m_punchHurtMemorized.eraseNoOrder(memIndex);
            numMemorized--;
        }
        else
        {
            memIndex++;
        }
    }
}

bbool Ray_TriggerBounceComponent::isMemorized( ActorRef _ref ) const
{
    u32 numMemorized = m_memorized.size();

    for ( u32 i = 0; i < numMemorized; i++ )
    {
        if ( m_memorized[i].m_ref == _ref )
        {
            return btrue;
        }
    }

    return bfalse;
}

Vec2d Ray_TriggerBounceComponent::getBounceDirection( Actor* _actor ) const
{
    Vec2d stimDir;

    if ( !getTemplate()->getIsRadial() )
    {
        stimDir = getTemplate()->getStimDirection();

        if ( !getTemplate()->getUseDirAsWorldAngle() )
        {
            stimDir = stimDir.Rotate(m_actor->getAngle());
        }
    } 
    else if (!getTemplate()->getIsRadialConstrained() )
    {
        stimDir = _actor->get2DPos() - m_actor->get2DPos();
        stimDir.normalize();
    }
    else
    {
        stimDir = Ray_AIUtils::getRadialConstrainedDir(m_actor->get2DPos(), _actor->get2DPos());
    }

    return stimDir;
}

void Ray_TriggerBounceComponent::sendBounce( Actor* _receiver, const Vec2d& _dir, bbool _fromHit )
{
    Pickable* bounceToObj = findBounceToObj(_receiver);
    bbool bounceRet = bfalse;

    if ( bounceToObj )
    {
        bounceRet = sendBounceToObj(_receiver,bounceToObj);
    }
    else
    {
        bounceRet = sendBounceRegular(_receiver,_dir);
    }

    if ( bounceRet )
    {
        if ( m_animComponent && getTemplate()->getBounceAnim().isValid() && ( !_fromHit || getTemplate()->getPlayBounceAnimOnHit() ) )
        {
            m_animComponent->setAnim(getTemplate()->getBounceAnim());
            m_animComponent->resetCurTime();
            m_bouncePlayed = btrue;
        }

        // check if this was a crush attack
        Ray_EventQueryHitType hitTypeQuery;
        _receiver->onEvent(&hitTypeQuery);
        bbool isCrushAttack = (hitTypeQuery.getHitType() == PUNCHTYPE_CRUSH);

        // inform other components that a bounce was triggered (e.g. dark birds,
        // bubble prize bumper)
        Ray_EventTriggerBounce bounceEvent;
        bounceEvent.setCrushAttack(isCrushAttack);
        m_actor->onEvent(&bounceEvent);
    }
}

Pickable* Ray_TriggerBounceComponent::findBounceToObj( Actor* _receiver ) const
{
    Pickable* bounceToObj = NULL;

    if ( m_linkComponent && getTemplate()->getUseBounceToPickable() )
    {
        const LinkComponent::ChildrenList& children = m_linkComponent->getChildren();

        if ( !children.empty() )
        {
            Vec2d dirToActor = _receiver->get2DPos() - m_actor->get2DPos();
            f32 minDist = 0.f;
            
            dirToActor.normalize();

            Vec2d separation = dirToActor.getPerpendicular();
            bbool useAngle = getTemplate()->getUseAngleToDiscardBounceToPickable();

            for ( LinkComponent::ChildrenList::const_iterator it = children.begin(); it != children.end(); ++it )
            {
                const ObjectPath& path = it->getPath();
                Pickable* obj;
                
                if ( path.getIsAbsolute() )
                {
                    obj = SceneObjectPathUtils::getObjectFromAbsolutePath(path);
                }
                else
                {
                    obj = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, path);
                }

                if ( obj )
                {
                    // not bouncing to bubble prizes
                    if (Actor* actor = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)))
                    {
                        if (Ray_AIUtils::isBubblePrize(actor))
                        {
                            continue;
                        }
                    }

                    Vec2d vecToObj = obj->get2DPos() - m_actor->get2DPos();

                    if ( useAngle )
                    {
                        Vec2d dirToObj = vecToObj;
                        
                        dirToObj.normalize();

                        f32 crossP = separation.cross(dirToObj);

                        if ( crossP > 0.f )
                        {
                            continue;
                        }
                    }

                    f32 dist = vecToObj.sqrnorm();

                    if ( !bounceToObj || dist < minDist )
                    {
                        bounceToObj = obj;
                        minDist = dist;
                    }
                }
            }
        }
    }

    return bounceToObj;
}

bbool Ray_TriggerBounceComponent::sendBounceRegular( Actor* _receiver, const Vec2d& _dir )
{
    BounceStim bounceStim;
    PunchStim punchStim;
    EventStim* sendStim;

    if ( !getTemplate()->getHurtLevel() )
    {
        bounceStim.setBounceType(getTemplate()->getBounceType());
        bounceStim.setBounceDir(_dir);
        bounceStim.setIsRadial(getTemplate()->getIsRadial() && !getTemplate()->getIsRadialConstrained());
        bounceStim.setMultiplier(getTemplate()->getBounceMultiplier());

        sendStim = &bounceStim;
    }
    else
    {
        punchStim.setReceivedHitType(RECEIVEDHITTYPE_HURTBOUNCE);
        punchStim.setIsRadial(getTemplate()->getIsRadial() && !getTemplate()->getIsRadialConstrained());
        punchStim.setDirection(_dir);
        punchStim.setFxPos(_receiver->getPos());
        punchStim.setLevel(getTemplate()->getHurtLevel()-1);
        punchStim.setFaction(getTemplate()->getFaction());
        sendStim = &punchStim;
    }

    sendStim->setSender(m_actor->getRef());
    sendStim->setAngle(m_actor->getAngle());
    sendStim->setPos(m_shapeDetector->getShapePos());

    _receiver->onEvent(sendStim);

    return ( ( sendStim == &bounceStim ) && !bounceStim.getSkipped() ) || ( sendStim == &punchStim );
}

bbool Ray_TriggerBounceComponent::sendBounceToObj( Actor* _receiver, Pickable* _obj )
{
    Ray_EventBounceToLayer bounceEvent(getTemplate()->getBounceToActorHeight(),
                                       getTemplate()->getBounceToActorHeight2(),
                                       getTemplate()->getBounceToActorSpeed(),
                                       _obj->getRef(),
                                       getTemplate()->getBounceToActorTargetOffset(),
                                       getTemplate()->getHurtLevel());

    bounceEvent.setSender(m_actor->getRef());
    _receiver->onEvent(&bounceEvent);

    return !bounceEvent.getSkipped();
}

void Ray_TriggerBounceComponent::processReceivePunch( PunchStim* _punch )
{
    if ( !getTemplate()->getSendBounceOnHit() )
    {
        return;
    }

    if ( _punch->getLevel() > getTemplate()->getIgnoreHitLevel())
    {
        return;
    }

    bbool found = bfalse;
    u32 numMemorized = m_punchHurtMemorized.size();

    for ( u32 i = 0; i < numMemorized; i++ )
    {
        if ( m_punchHurtMemorized[i].m_ref == _punch->getSender() )
        {
            found = btrue;
            break;
        }
    }

    if ( !found )
    {
        Actor* actor = AIUtils::getActor(_punch->getSender());

        if ( actor )
        {
            m_receivedHitAttacker = _punch->getSender();
            m_receivedHitDir = _punch->getDirection();

            EventHitSuccessful hitSuccess;
            hitSuccess.setSender( m_actor->getRef() );
            hitSuccess.setHasBeenDamaged( bfalse );
            actor->onEvent( &hitSuccess );
        }
    }
}

void Ray_TriggerBounceComponent::setDisabled( bbool _val )
{
    if ( m_disabled != _val )
    {
        m_disabled = _val;

        if (m_disabled)
        {
            unregisterObjects();
            m_actorsInside.clear();
            m_memorized.clear();
            m_punchHurtMemorized.clear();
        }
        else
        {
            registerObjects();
        }
    }
}

void Ray_TriggerBounceComponent::onDepthChanged( f32 _oldZ, f32 _newZ )
{
    Super::onDepthChanged( _oldZ,_newZ );
    
    if (getTemplate()->getRegisterToAIManager() && m_registered)
    {
        AI_MANAGER->changeDepth(m_actor->getRef(),_oldZ,_newZ);
    }
}

void Ray_TriggerBounceComponent::registerObjects()
{
    if (!m_registered)
    {
        m_registered = btrue;
        
        if ( m_phantom )
        {
            PHYSWORLD->insertPhantom(m_phantom, m_actor->getDepth());
        }
        
        if (getTemplate()->getRegisterToAIManager())
        {
            AIData * data = AI_MANAGER->addActor(m_actor->getRef(),m_actor->getDepth());
            if (data)
                data->m_faction = getTemplate()->getFaction();
        }
    }
}

void Ray_TriggerBounceComponent::unregisterObjects()
{
    if (m_registered)
    {
        m_registered = bfalse;
        
        if ( m_phantom )
        {
            PHYSWORLD->removePhantom(m_phantom);
        }
        
        if (getTemplate()->getRegisterToAIManager())
        {
            AI_MANAGER->removeActor(m_actor->getRef(),m_actor->getDepth());
        }
    }
}

#ifdef ITF_SUPPORT_EDITOR
void Ray_TriggerBounceComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
{
    Super::drawEdit(_drawInterface, _flags);

    if (_flags != ActorComponent::DrawEditFlag_All)
        return;

    if ( m_shapeDetector && m_shapeDetector->getCurrentShape() )
    {
        PhysShapeCircle* circle = m_shapeDetector->getCurrentShape()->DynamicCast<PhysShapeCircle>(ITF_GET_STRINGID_CRC(PhysShapeCircle,3922519168));

        if ( circle )
        {
            f32 detectRange = getTemplate()->getDetectRange().ToRadians();
            f32 minAngle = m_actor->getAngle() + MTH_PIBY2 - (detectRange*0.5f);
            f32 maxAngle = m_actor->getAngle() + MTH_PIBY2 + (detectRange*0.5f);

            Vec2d myPos = m_shapeDetector->getShapePos();
            Vec2d minDir = Vec2d::Right.Rotate(minAngle)*circle->getRadius();
            Vec2d maxDir = Vec2d::Right.Rotate(maxAngle)*circle->getRadius();
            DebugDraw::line3D(myPos.to3d(m_actor->getPos().m_z),(myPos+minDir).to3d(m_actor->getPos().m_z));
            DebugDraw::line3D(myPos.to3d(m_actor->getPos().m_z),(myPos+maxDir).to3d(m_actor->getPos().m_z));
        }
    }
}
#endif // ITF_SUPPORT_EDITOR

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(Ray_TriggerBounceComponent_Template)
BEGIN_SERIALIZATION(Ray_TriggerBounceComponent_Template)

    SERIALIZE_OBJECT_WITH_FACTORY("phantom",m_phantomShape,PHYSWORLD->getShapesFactory());
    SERIALIZE_MEMBER("hurtLevel",m_hurtLevel);
    SERIALIZE_RAY_BOUNCETYPE("bounceType",m_bounceType);
    SERIALIZE_MEMBER("faction",m_faction);
    SERIALIZE_MEMBER("retriggerDelay",m_retriggerDelay);
    SERIALIZE_MEMBER("idleAnim",m_idleAnim);
    SERIALIZE_MEMBER("bounceAnim",m_bounceAnim);
    SERIALIZE_MEMBER("direction",m_stimDirection);
    SERIALIZE_MEMBER("useDirAsWorldAngle",m_useDirAsWorldAngle);
    SERIALIZE_MEMBER("stickFruit",m_fruitSticks);
    SERIALIZE_MEMBER("sendBounceOnHit",m_sendBounceOnHit);
    SERIALIZE_MEMBER("radial",m_isRadial);
    SERIALIZE_MEMBER("radialConstrained",m_isRadialConstrained);
    SERIALIZE_MEMBER("height", m_bounceToActorheight);
    SERIALIZE_MEMBER("height2", m_bounceToActorheight2);
    SERIALIZE_MEMBER("speed", m_bounceToActorSpeed);
    SERIALIZE_MEMBER("multiplier", m_bounceMultiplier);
    SERIALIZE_MEMBER("offset", m_bounceToActorTargetOffset);
    SERIALIZE_MEMBER("detectRange", m_detectRange);
    SERIALIZE_MEMBER("sendMoreThanOnce", m_sendMoreThanOnce);
    SERIALIZE_MEMBER("useBounceToPickable", m_useBounceToPickable);
    SERIALIZE_MEMBER("useAngleToDiscardBounceToPickable", m_useAngleToDistardBounceToPickable);
    SERIALIZE_MEMBER("ignoreHitLevel", m_ignoreHitLevel);
    SERIALIZE_MEMBER("blocksHits", m_blocksHits);
	SERIALIZE_MEMBER("disableAtBounce", m_disableAtBounce);
    SERIALIZE_MEMBER("registerToAIManager", m_registerToAIManager);
    SERIALIZE_MEMBER("playBounceAnimOnHit", m_playBounceAnimOnHit);

END_SERIALIZATION()

Ray_TriggerBounceComponent_Template::Ray_TriggerBounceComponent_Template()
: m_phantomShape(NULL)
, m_bounceType(BOUNCETYPE_BUMPER)
, m_hurtLevel(0)
, m_faction(FACTION_UNKNOWN)
, m_retriggerDelay(1.f)
, m_bounceToActorheight(3.f)
, m_bounceToActorheight2(5.f)
, m_bounceToActorSpeed(7.f)
, m_bounceMultiplier(1.f)
, m_stimDirection(Vec2d::Up)
, m_bounceToActorTargetOffset(Vec2d::Zero)
, m_useDirAsWorldAngle(bfalse)
, m_fruitSticks(bfalse)
, m_sendBounceOnHit(bfalse)
, m_isRadial(bfalse)
, m_isRadialConstrained(bfalse)
, m_sendMoreThanOnce(bfalse)
, m_useBounceToPickable(btrue)
, m_useAngleToDistardBounceToPickable(bfalse)
, m_ignoreHitLevel(1)
, m_blocksHits(bfalse)
, m_disableAtBounce(bfalse)
, m_registerToAIManager(btrue)
, m_playBounceAnimOnHit(btrue)
{
}

Ray_TriggerBounceComponent_Template::~Ray_TriggerBounceComponent_Template()
{
    SF_DEL(m_phantomShape);
}

} // namespace ITF