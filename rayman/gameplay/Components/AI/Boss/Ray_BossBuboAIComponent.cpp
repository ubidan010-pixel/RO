#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BOSSBUBOAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Boss/Ray_BossBuboAIComponent.h"
#endif //_ITF_RAY_BOSSBUBOAICOMPONENT_H_

#ifndef _ITF_SHAPEDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/ShapeDetectorComponent.h"
#endif //_ITF_SHAPEDETECTORCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_BossBuboAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_BossBuboAIComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_BossBuboAIComponent)
    //VALIDATE_COMPONENT_PARAM("", m_shapeDetector, "RAY_BossPlantProjectileAIComponent requires a DetectorComponent");
    END_VALIDATE_COMPONENT()

    Ray_BossBuboAIComponent::Ray_BossBuboAIComponent()
        : Super()
        , m_linkComponent(NULL)
        , m_shapeDetector(NULL)
        , m_isHitable(bfalse)
        , m_isDying(bfalse)
        , m_isDisplay(bfalse)
        , m_megaBuboHitPoints(0)
    {
    }

    Ray_BossBuboAIComponent::~Ray_BossBuboAIComponent()
    {
    }

    void Ray_BossBuboAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_linkComponent = m_actor->GetComponent<LinkComponent>();
        m_shapeDetector = m_actor->GetComponent<ShapeDetectorComponent>();          // mandatory

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(PunchStim,200533519), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventGeneric,4063838687), this);
        if(getTemplate()->getIsCrushable())
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608), this);
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventCrushed,3831882623), this);
        }
    }

    void Ray_BossBuboAIComponent::onBecomeActive()
    {
        Super::onBecomeActive();
        if(!m_isHitable)
        {
            m_animComponent->setAnim(getTemplate()->getInvisibleAnim());
        }
    }

    void Ray_BossBuboAIComponent::onEvent(Event* _event)
    {
        Super::onEvent(_event);

        if(PunchStim* hit = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)))
        {
            hitBy(hit->getSender());
        }
        else if(EventInteractionQuery* intQuery = _event->DynamicCast<EventInteractionQuery>(ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608)))
        {
            processInteractionQuery(intQuery);
        }
        else if(EventCrushed* evtCrushed = _event->DynamicCast<EventCrushed>(ITF_GET_STRINGID_CRC(EventCrushed,3831882623)))
        {
            hitBy(evtCrushed->getSender());
        }
        else if(AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)))
        {
            onGameplayEvent(animEvent);
        }
        else if(EventTrigger *triggerEvent = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
        {
            if(!m_isDying)
            {
                m_activator = triggerEvent->getActivator();
                if(getTemplate()->getIsMebaBubo()) triggerMegaBubo(!m_isHitable);
                else triggerBubo(!m_isHitable);
            }
        }
        else if ( EventGeneric* eventGeneric = _event->DynamicCast<EventGeneric>(ITF_GET_STRINGID_CRC(EventGeneric,4063838687)) )
        {
            if(!m_isDying)
            {
                m_activator = eventGeneric->getActivator();
                unsigned long value = (unsigned long)eventGeneric->getId().GetValue();
                if (value == ITF_GET_STRINGID_CRC(Open, 812432275) && !m_isHitable)
                {
                    triggerBubo(btrue);
                }
                else if (value == ITF_GET_STRINGID_CRC(Close, 3883599349) && m_isHitable)
                {
                    triggerBubo(bfalse);
                }
            }
        }
    }

    void Ray_BossBuboAIComponent::triggerMegaBubo(bbool _activate)
    {
        /*
            The megabubo stays once triggered, the following triggers will
            only trigger the "isHitable" property.
        */
        m_isHitable = _activate;
        if(_activate)
        {
            m_megaBuboHitPoints = getTemplate()->getHitPoints();

            if(!m_isDisplay)
            {
                m_isDisplay = btrue;
                m_animComponent->setAnim(getTemplate()->getAppearAnim());
            }
        }
    }

    void Ray_BossBuboAIComponent::triggerBubo(bbool _activate)
    {
        if(_activate)
        {
            m_animComponent->setAnim(getTemplate()->getAppearAnim());
        }
        else
        {
            m_animComponent->setAnim(getTemplate()->getDisappearAnim());
        }
    }

    void Ray_BossBuboAIComponent::onGameplayEvent(AnimGameplayEvent* animEvent)
    {
        unsigned long value = (unsigned long)animEvent->getName().GetValue();
        if(value == ITF_GET_STRINGID_CRC(MRK_AI_BuboOn, 4156823774))
        {
            if(!getTemplate()->getIsMebaBubo())
            {
                m_isHitable = btrue;
            }
        }
        else if(value == ITF_GET_STRINGID_CRC(MRK_AI_TriggerSons, 2491980558))
        {
            if(getTemplate()->getDelayTrigger())
            {
                triggerChildren();
                triggerActivator();
            }

        }
        else if(value == ITF_GET_STRINGID_CRC(MRK_AI_BuboOff, 3835082794))
        {
            m_isHitable = bfalse;
        }
        else if(value == ITF_GET_STRINGID_CRC(MRK_AI_Died, 884050513))
        {
            died();
        }
    }

    void Ray_BossBuboAIComponent::processInteractionQuery(EventInteractionQuery* _evt)
    {
        if ( RAY_GAMEMANAGER->getAllowNpcCrush() )
        {
            if (m_isHitable && _evt->getInteraction() == CharacterInteractionType_None)
            {
                ActorRef ref = _evt->getSender();
                Actor * actor = ref.getActor();
                if (actor)
                {
                    if (actor->getBase().m_y > m_actor->getPos().m_y)
                        _evt->setInteraction(CharacterInteractionType_Crush);

                }
            }
        }
    }

    void Ray_BossBuboAIComponent::hitBy(ActorRef senderRef)
    {
        if(m_isHitable)
        {
            Actor* sender = senderRef.getActor();
            Faction senderFaction = AIUtils::getFaction(sender);

            if(senderFaction == getTemplate()->getAllowedFaction())
            {
                if(getTemplate()->getIsMebaBubo())
                {
                    m_megaBuboHitPoints --;
                    if(m_megaBuboHitPoints > 0)
                    {
                        m_animComponent->setAnim(getTemplate()->getHitAnim());
                        m_animComponent->resetCurTime();
                        return;
                    }
                }

                m_isHitable = bfalse;

                if(!triggerRewards())
                {
					u32 nb = getTemplate()->getNbRewards();
					if(nb)
					{
						Ray_EventSpawnRewardHeart evt;
						evt.setup(m_actor);
						evt.setNumRewards(nb);
						evt.setRewardReceiver(senderRef);

						RAY_GAMEMANAGER->SpawnRewardHeart( &evt );
					}
                }

                if(!getTemplate()->getDelayTrigger())
                {
                    triggerChildren();
                    triggerActivator();
                }

                die();
            }
        }
    }

    void Ray_BossBuboAIComponent::die()
    {
        m_isDying = btrue;
        m_animComponent->setAnim(getTemplate()->getDeathAnim());
    }

    void Ray_BossBuboAIComponent::died()
    {
        m_isDying = bfalse;
    }

    void Ray_BossBuboAIComponent::triggerChildren()
    {
        if (m_linkComponent)
        {
            EventTrigger triggerEvent;
            triggerEvent.setSender(m_actor->getRef());
            triggerEvent.setActivated(btrue);
            m_linkComponent->sendEventToChildren(&triggerEvent);

            EventBossBubonHit bubonEvent;
            bubonEvent.setSender(m_actor->getRef());
            m_linkComponent->sendEventToChildren(&bubonEvent);
        }
    }

    void Ray_BossBuboAIComponent::triggerActivator()
    {
        if (!getTemplate()->getTriggerActivator())
        {
            return;
        }

        if (Actor* activator = m_activator.getActor())
        {
            EventBossBubonHit bubonEvent;
            bubonEvent.setSender(m_actor->getRef());
            activator->onEvent(&bubonEvent);
        }
    }

    bbool Ray_BossBuboAIComponent::triggerRewards()
    {
        bbool foundSome = bfalse;

        if (m_linkComponent)
        {
            const LinkComponent::ChildrenList &children = m_linkComponent->getChildren();
            for(u32 i = 0; i < children.size(); i++)
            {
                Pickable *p= SceneObjectPathUtils::getObjectFromRelativePath(m_actor, children[i].getPath());
                if (p)
                {
                    ActorRef actorRef = p->getRef();
                    Actor* actor = actorRef.getActor();
                    if(actor)
                    {
                        // check if launchable
                        Ray_EventQueryChildLaunch query;
                        actor->onEvent(&query);

                        // *enable* child and launch
                        if (query.canLaunch())
                        {
                            foundSome = btrue;

                            actor->enable();

                            // actually launch
                            Ray_EventChildLaunch launch(m_actor->getPos());
                            launch.setSender(actorRef);
                            actor->onEvent(&launch);
                        }
                    }
                }
            }
        }
        return foundSome;
    }

	//-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_BossBuboAIComponent_Template)
    BEGIN_SERIALIZATION(Ray_BossBuboAIComponent_Template)
        SERIALIZE_MEMBER("invisibleAnim", m_invisibleAnim);
        SERIALIZE_MEMBER("appearAnim", m_appearAnim);
		SERIALIZE_MEMBER("disappearAnim", m_disappearAnim);
		SERIALIZE_MEMBER("hitAnim", m_hitAnim);
		SERIALIZE_MEMBER("deathAnim", m_deathAnim);
		SERIALIZE_MEMBER("allowedFaction", m_allowedFaction);
		SERIALIZE_MEMBER("nbRewards", m_nbRewards);
		SERIALIZE_MEMBER("triggerActivator", m_triggerActivator);
        SERIALIZE_MEMBER("delayTrigger", m_delayTrigger);
		SERIALIZE_MEMBER("isCrushable", m_isCrushable);
		SERIALIZE_MEMBER("isMegaBubo", m_isMegaBubo);
		SERIALIZE_MEMBER("megaBuboHitPoints", m_megaBuboHitPoints);
    END_SERIALIZATION()

    Ray_BossBuboAIComponent_Template::Ray_BossBuboAIComponent_Template()
        : Super()
        , m_allowedFaction(FACTION_UNKNOWN)
        , m_nbRewards(1)
        , m_triggerActivator(bfalse)
        , m_delayTrigger(bfalse)
        , m_isCrushable(btrue)
        , m_isMegaBubo(bfalse)
        , m_megaBuboHitPoints(10)
    {
    }

};