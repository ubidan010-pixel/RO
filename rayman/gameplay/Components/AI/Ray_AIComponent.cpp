#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_RAY_GAMEMATERIAL_H_
#include "rayman/gameplay/Ray_GameMaterial.h"
#endif //_ITF_RAY_GAMEMATERIAL_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

namespace ITF
{

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(Ray_AIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_AIComponent_Template)
        SERIALIZE_MEMBER("reactivateOnCheckpoint",m_reactivateOnCheckpoint);
        SERIALIZE_MEMBER("customCheckpointHandling",m_customCheckpointHandling);
        SERIALIZE_MEMBER("softCollisionRadius",m_softCollisionRadius);
        SERIALIZE_OBJECT_WITH_FACTORY("reward", m_receiveHitReward, ACTORSMANAGER->getEventFactory());
        SERIALIZE_MEMBER("invincibleToDangerousMaterial",m_invincibleToDangerousMaterial);
        SERIALIZE_MEMBER("alsoCheckEncroachedDangerousMaterials", m_alsoCheckEncroachedDangerousMaterials);
    END_SERIALIZATION()


    Ray_AIComponent_Template::Ray_AIComponent_Template()
	: m_customCheckpointHandling(bfalse)
    , m_reactivateOnCheckpoint(btrue)
    , m_softCollisionRadius(0.0f)
    , m_receiveHitReward(NULL)
    , m_invincibleToDangerousMaterial(bfalse)
    , m_alsoCheckEncroachedDangerousMaterials(bfalse)
    {
    }

    Ray_AIComponent_Template::~Ray_AIComponent_Template()
    {
        SF_DEL(m_receiveHitReward);
    }

    void Ray_AIComponent_Template::onTemplateLoaded()
    {
    }

	///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(Ray_AIComponent)
    BEGIN_SERIALIZATION_CHILD(Ray_AIComponent)
    END_SERIALIZATION()

    Ray_AIComponent::Ray_AIComponent() 
        : Super()
        , m_physComponent(NULL)
        , m_lastHitType(RECEIVEDHITTYPE_UNKNOWN)
		, m_hasBeenVaccumed( bfalse )
        , m_softCollisionRadiusMulitiplier(1.0f)
        , m_receiveHitReward(NULL)
        , m_rewardGiven(bfalse)
    {
    }

    Ray_AIComponent::~Ray_AIComponent()
    {
#ifdef ITF_SUPPORT_EDITOR
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(Ray_EventQueryLumCount,3794901482), this);
#endif

        SF_DEL(m_receiveHitReward);
    }

    void Ray_AIComponent::onCheckpointLoaded()
    {
        Super::onCheckpointLoaded();

        if (!getTemplate()->getCustomCheckpointHandling()) // if health is used for special checkpoint handling, e.g. utensil traps
        {
            if (getTemplate()->getReactivateOnCheckpoint() )
            {
                setHealth(getTemplate()->getHealth_default());
                m_rewardGiven = bfalse;

                if (!m_actor->isStartPaused())
                {
                    m_actor->enable();
                }
            }
            else
            {
                if (getHealth()<=0)
                {
                    m_actor->disable();
                }
            }
        }
    }

    void Ray_AIComponent::receiveHit( HitStim* _hit, bbool _damageDependsOnLevel /*= btrue*/, u32 _forcedDamage /*= 0*/ )
    {
        if ( !m_hasBeenVaccumed )
        {
            if (RehitStim * rehit = _hit->DynamicCast<RehitStim>(ITF_GET_STRINGID_CRC(RehitStim,1008690800)))
            {
                m_rewardReceiver = rehit->getRewardReceiver();
            }
            else
            {
                if(_hit->getOriginalSender().isValid())
                {
                    m_rewardReceiver = _hit->getOriginalSender();
                }
                else
                {
                    m_rewardReceiver = _hit->getSender();
                }
            }

            Super::receiveHit(_hit, _damageDependsOnLevel, _forcedDamage);


            if ( PunchStim* punch = _hit->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)) )
            {
                m_lastHitType = punch->getReceivedHitType();
            }
            else
            {
                m_lastHitType = RECEIVEDHITTYPE_UNKNOWN;
            }
        }
    }

    void Ray_AIComponent::receiveHit( ActorRef _attacker, u32 _level, const Vec2d& _dir, const Vec3d& _fxPos, bbool _damageDependsOnLevel /*= btrue*/, u32 _forcedDamage /*= 0*/ )
    {
        Super::receiveHit(_attacker, _level, _dir, _fxPos, _damageDependsOnLevel, _forcedDamage);
        if ( m_receiveHitReward != NULL && getRewardReceiver().isValid() && !m_rewardGiven )
        {
            m_rewardGiven = btrue;
            m_receiveHitReward->clear();
            m_receiveHitReward->setup(m_actor);
            //m_receiveHitReward->setNumRewards(1);
            m_receiveHitReward->setRewardReceiver(getRewardReceiver());
            m_receiveHitReward->setEjectionDir(_dir);
            GAMEMANAGER->onEvent(m_receiveHitReward);
        }

        m_lastHitType = RECEIVEDHITTYPE_UNKNOWN;
    }

    void Ray_AIComponent::receiveCrush( EventCrushed* _crushed )
    {
        Actor * actor = ActorRef(_crushed->getSender()).getActor();


        bbool damage = btrue;
        if (actor && AIUtils::isEnemy(getFaction(),AIUtils::getFaction(actor),&damage))
        {
            if (damage)
            {
                bbool isCrusherSmall = _crushed->getCharacterSize()==CHARACTERSIZE_SMALL;
                bbool isAISmall = bfalse;
                bbool damage = btrue;
                if (isCrusherSmall && !isAISmall && getHealth()!=0)
                {
                    damage = bfalse;
                    m_rewardReceiver.invalidate();
                }
                else
                {
                    m_rewardReceiver = _crushed->getSender();
                }
                receiveHit( _crushed->getSender(), 0, _crushed->getDirection(), _crushed->getFxPos(),damage );
            }
        }
    }

    void Ray_AIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        if (getTemplate()->getReactivateOnCheckpoint() && !m_actor->isStartPaused())
            m_actor->enable();

        Super::onActorLoaded(_hotReload);

        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();    // not mandatory

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryAIData,2473202000),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventDetach,889341216),this);
#ifdef ITF_SUPPORT_EDITOR
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(Ray_EventQueryLumCount,3794901482),this);
#endif

        if ( getTemplate()->getReceiveHitReward() )
        {
            m_receiveHitReward = getTemplate()->getReceiveHitReward()->CreateNewObject()->DynamicCast<Ray_EventSpawnReward>(ITF_GET_STRINGID_CRC(Ray_EventSpawnReward,145553727));;
            BinaryClone(const_cast<Ray_EventSpawnReward*>(getTemplate()->getReceiveHitReward()),m_receiveHitReward);
        }

        m_rewardGiven = bfalse;

    }

    void Ray_AIComponent::updateMaterials()
    {
        // send a stim if we're sticking to a dangerous material
        PolyLine *alreadyTestedDangerousPoly=NULL;
        if (m_physComponent)
        {
            PolyLine* poly;
            const PolyLineEdge* edge;
            ObjectRef polyLineRef = m_physComponent->getStickedPolylineRef();
            AIUtils::getPolyLine(polyLineRef, m_physComponent->getStickedEdgeIndex(), poly, edge);

            if (poly && edge)
            {
                const Ray_GameMaterial_Template* mat = (const Ray_GameMaterial_Template*)(World::getGameMaterial(edge->getGameMaterial()));
                if (mat)
                {
                    if (processMaterial(poly, edge, mat))
                    {
                        alreadyTestedDangerousPoly = poly;
                    }
                }
            }
        }

        if (!alreadyTestedDangerousPoly
            && getTemplate()->alsoCheckEncroachedDangerousMaterials())
        {
            if (m_phantomComponent)
            {
                PhysShape *shape1 = m_phantomComponent->getShape();
                if (shape1)
                {
                    PhysContactsContainer contacts;
                    Vec2d shapePos = m_phantomComponent->getShapePos();
                    bbool found;
                    found = PHYSWORLD->checkEncroachment( shapePos, shapePos, m_actor->getAngle(),
                        shape1,
                        ECOLLISIONFILTER_ALL,
                        m_actor->getDepth(),
                        contacts );
                    for (u32 i=0; i<contacts.size(); i++)
                    {
                        const SCollidableContact &contact = contacts[i];
                        if ( contact.m_edgeIndex == U32_INVALID )
                        {
                            // If it's not a polyline we pass
                            continue;
                        }
                        PolyLine* polyLine;
                        const PolyLineEdge* edge;

                        AIUtils::getPolyLine(contact.m_collidableUserData,contact.m_edgeIndex,
                            polyLine,edge);

                        if ( !polyLine || !edge )
                        {
                            continue;
                        }
                        if (polyLine == alreadyTestedDangerousPoly)
                        {
                            continue;
                        }

                        const Ray_GameMaterial_Template* mat = (const Ray_GameMaterial_Template*)(World::getGameMaterial(edge->getGameMaterial()));
                        if (mat)
                        {
                            if (mat->getDangerousLevel()>0)
                            {
                                if (processMaterial(polyLine, edge, mat))
                                {
                                    break;
                                }
                            }
                        }

                    }
                }

            }
        }
    }

    void Ray_AIComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        updateMaterials();
    }

    bbool Ray_AIComponent::processMaterial( PolyLine* _poly, const PolyLineEdge* _edge, const Ray_GameMaterial_Template* _mat )
    {
        bbool hurts = bfalse;
        if ( _mat->getDangerousLevel() > 0)
        {
            if (!getTemplate()->getInvincibleToDangerousMaterial())
            {
                hurts = btrue;
                if (_mat->getIsBounce())
                {
                    PunchStim stim;

                    stim.setSender(m_actor->getRef());
                    stim.setLevel(_mat->getDangerousLevel());
                    stim.setReceivedHitType(RECEIVEDHITTYPE_HURTBOUNCE);
                    stim.setDirection(_edge->m_normalizedVector.getPerpendicular());
                    stim.setFaction(RAY_FACTION_MEGAENEMY);

                    if (_poly->getOwnerActor())
                    {
                        stim.setSender(_poly->getOwnerActor()->getRef());
                    }

                    m_actor->onEvent(&stim);
                }
                else
                {
                    PunchStim stim;
                    stim.setSender(m_actor->getRef());
                    stim.setLevel(_mat->getDangerousLevel());
                    stim.setReceivedHitType(RECEIVEDHITTYPE_EJECTY);
                    stim.setFaction(getFaction());

                    stim.setDirection(_edge->m_normalizedVector.getPerpendicular());

                    if (_poly->getOwnerActor())
                    {
                        stim.setSender(_poly->getOwnerActor()->getRef());
                    }

                    m_actor->onEvent(&stim);
                }
            }
        }
        else 
        {
            if (_mat->getIsBounce())
            {
                BounceStim stim;
                stim.setSender(m_actor->getRef());
                stim.setBounceDir(_edge->m_normalizedVector.getPerpendicular());
                stim.setBounceType(BOUNCETYPE_BUMPER);
                stim.setIsRadial(bfalse);
                if (_poly->getOwnerActor())
                {
                    stim.setSender(_poly->getOwnerActor()->getRef());
                }
                m_actor->onEvent(&stim);
            }
        }
        return hurts;
    }

    void Ray_AIComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event  );
        if (Ray_EventQueryAIData * _query = _event->DynamicCast<Ray_EventQueryAIData>(ITF_GET_STRINGID_CRC(Ray_EventQueryAIData,2473202000)))
        {
            _query->setSoftCollisionRadius(getSoftCollisionRadius());
        }
        else if ( Ray_EventQueryLumCount *queryLumCount = _event->DynamicCast<Ray_EventQueryLumCount>(ITF_GET_STRINGID_CRC(Ray_EventQueryLumCount,3794901482)))
        {
#ifdef ITF_SUPPORT_EDITOR
            processEventQueryLumCount(queryLumCount);
#endif
        }
        else if (EventDetach* _detachEvent = _event->DynamicCast<EventDetach>(ITF_GET_STRINGID_CRC(EventDetach,889341216)))
        {
            if ( m_physComponent )
            {
                m_physComponent->unstick();
            }
        }
    }

#ifdef ITF_SUPPORT_EDITOR
    void Ray_AIComponent::processEventQueryLumCount(Ray_EventQueryLumCount *_queryLumCount)
    {
        if ((m_actor->getWorld() == SPAWNER->getSpawnerWorld()) || m_actor->isSpawned())
        {
            return;
        }
        const Ray_EventSpawnRewardLum *reward = IRTTIObject::SafeDynamicCast<Ray_EventSpawnRewardLum>(getTemplate()->getReceiveHitReward(),ITF_GET_STRINGID_CRC(Ray_EventSpawnRewardLum,812488722));
        if (reward)
            _queryLumCount->addNormalLums(reward->getNumRewards());

        //Also sends to other behaviors which are not current behavior
        const SafeArray<AIBehavior*> &behaviors = getBehaviors();
        for (u32 i=0; i<behaviors.size(); i++)
        {
            AIBehavior *behavior = behaviors[i];
            if (behavior && behavior!=m_currentBehavior)
            {
                behavior->onEvent(_queryLumCount);
            }
        }
    }
#endif

    void Ray_AIComponent::playLastHitFx()
    {
        if (m_fxController)
        {
            u32 fxHandle = m_fxController->playFeedback(getLastAttacker(),ITF_GET_STRINGID_CRC(hit_crush,3169038330),m_actor->getRef());
            m_fxController->setFXPosFromHandle(fxHandle, getLastHitFxPos());
        }
    }

    ITF::f32 Ray_AIComponent::getSoftCollisionRadius() const
    {
        return m_actor->getScale().m_y * getTemplate()->getSoftCollisionRadius() * m_softCollisionRadiusMulitiplier;
    }

#ifdef ITF_SUPPORT_EDITOR
    void Ray_AIComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit( _drawInterface,_flags );
        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        //DebugDraw::circle(m_actor->getPos(),getSoftCollisionRadius(),Color::pink(),0.0f,"SoftCollision");
    }
#endif // ITF_SUPPORT_EDITOR
} // namespace ITF

