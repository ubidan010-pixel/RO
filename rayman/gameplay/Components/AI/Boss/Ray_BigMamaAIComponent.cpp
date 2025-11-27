#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BIGMAMAAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Boss/Ray_BigMamaAIComponent.h"
#endif //_ITF_RAY_BIGMAMAAICOMPONENT_H_

#ifndef _ITF_RAY_REWARDMANAGER_H_
#include "rayman/gameplay/Ray_RewardManager.h"
#endif //_ITF_RAY_REWARDMANAGER_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_RAY_AIUTILS_H_
#include "Rayman/gameplay/AI/Utils/Ray_AIUtils.h"
#endif //_ITF_RAY_AIUTILS_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_BigMamaAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_BigMamaAIComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_BigMamaAIComponent)
    END_VALIDATE_COMPONENT()

    Ray_BigMamaAIComponent::Ray_BigMamaAIComponent()
        : Super()
        , m_currentPhase(0)
        , m_eyesHitPoints(10)
    {
    }

    Ray_BigMamaAIComponent::~Ray_BigMamaAIComponent()
    {
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(Ray_EventBouncePropagate,889182957), this);
    }

    void Ray_BigMamaAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(Ray_EventBouncePropagate,889182957),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(PunchStim,200533519), this);
        getChildren();
        m_eyesHitPoints = getTemplate()->getEyesHitPoints();
        SPAWNER->declareNeedsSpawnee(m_actor, &m_eyeSpawner, getTemplate()->getEyeSpawn());
        m_fxController = m_actor->GetComponent<FXControllerComponent>();
    }

    void Ray_BigMamaAIComponent::onEvent(Event* _event)
    {
        Super::onEvent(_event);

        if(Ray_EventBouncePropagate *bounceEvent = _event->DynamicCast<Ray_EventBouncePropagate>(ITF_GET_STRINGID_CRC(Ray_EventBouncePropagate,889182957)))
        {
            goToNextPhase();
        }
        else if(PunchStim* _evt = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)))
        {
            processPunch(_evt);
        }
        else if(EventTrigger* _evt = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
        {
            if(_evt->getSender() == m_bubo || _evt->getSender() == m_secondBubo || _evt->getSender() == m_megaBubo)
            {
                if(_evt->getSender() == m_megaBubo)
                {
                    ejectPlayers(m_finalEjectPos);
                    REWARD_MANAGER->Action_New(Ray_RewardManager::Action_KillBigMama, GAMEMANAGER->getMainIndexPlayer());
                }
                else ejectPlayers(m_ejectOnHeadPos);
                goToNextPhase();
            }
            else if(_evt->getActivated())
            {
                init();
            }
        }
        else if(AnimGameplayEvent* _animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)))
        {
            onGameplayEvent(_animEvent);
        }
    }

    void Ray_BigMamaAIComponent::onGameplayEvent(AnimGameplayEvent* _animEvent)
    {
        unsigned long value = (unsigned long)_animEvent->getName().GetValue();
        if(value == ITF_GET_STRINGID_CRC(MRK_AI_EjectPlayers, 1167403535))
        {
            ejectPlayers(m_ejectPos);
        }
        else if(value == ITF_GET_STRINGID_CRC(MRK_AI_TriggerBubo, 795340486))
        {
            triggerBubo(bfalse);
        }
        else if(value == ITF_GET_STRINGID_CRC(MRK_AI_TriggerMegaBubo, 1647583131))
        {
            triggerBubo(btrue);
        }
        else if(value == ITF_GET_STRINGID_CRC(MRK_AI_StartCine, 488987668))
        {
            startCine();
        }
        else if(value == ITF_GET_STRINGID_CRC(MRK_AI_PreviousPhase, 4060380575))
        {
            goToPreviousPhase();
            ejectPlayers(m_ejectPos);
        }
    }

    void Ray_BigMamaAIComponent::processPunch(PunchStim* _evt)
    {
        if(m_eyesHitPoints > 0)
        {
            ActorRef senderRef = _evt->getSender();
            Actor* sender = senderRef.getActor();
            Faction senderFaction = AIUtils::getFaction(sender);

            if(senderFaction == getTemplate()->getAllowedFaction())
            {
                m_eyesHitPoints --;

                if (m_fxController)
                {
                    Actor* sender = AIUtils::getActor(_evt->getSender());
                    if(sender)
                    {
                        Vec3d fxPos = sender->getPos();
                        const StringID feedbackAction = _evt->getActionFromHit();
                        u32 fxHandle = m_fxController->playFeedback(_evt->getSender(), feedbackAction, m_actor->getRef());
                        m_fxController->setFXPosFromHandle(fxHandle, fxPos);//_evt->getFxPos());
                    }
                }

				updateAnimInputs();

                CAMERACONTROLLERMANAGER->addShake(ITF_GET_STRINGID_CRC(Quake1, 3041659572));
                spawnEyes();
                if(!m_eyesHitPoints)
                {
                    goToNextPhase();
                }
            }
        }
    }

    void Ray_BigMamaAIComponent::init()
    {
        m_actor->enable();
        updateAnimInputs();
    }

	void Ray_BigMamaAIComponent::getChildren()
    {
        LinkComponent *linkComp = m_actor->GetComponent<LinkComponent>();
        if (linkComp)
        {
            StringID tagValue;

            const LinkComponent::ChildrenList &children = linkComp->getChildren();
            for(u32 i = 0; i < children.size(); i++)
            {
                if(children[i].getTagValue(ITF_GET_STRINGID_CRC(target, 4232938102), tagValue))
                {
                    if(Pickable *p= SceneObjectPathUtils::getObjectFromRelativePath(m_actor, children[i].getPath()))
                    {
                        unsigned long value = (unsigned long)tagValue.GetValue();
                        if(value == ITF_GET_STRINGID_CRC(eject, 3379621094))
                        {
                            m_ejectPos = p->getRef();
                        }
                        else if(value == ITF_GET_STRINGID_CRC(finalEject, 1930307414))
                        {
                            m_finalEjectPos = p->getRef();
                        }
                        else if(value == ITF_GET_STRINGID_CRC(ejectOnHead, 3458655323))
                        {
                            m_ejectOnHeadPos = p->getRef();
                        }
                        else if(value == ITF_GET_STRINGID_CRC(bubo, 1755405513))
                        {
                            m_bubo = p->getRef();
                        }
                        else if(value == ITF_GET_STRINGID_CRC(secondBubo, 2843365732))
                        {
                            m_secondBubo = p->getRef();
                        }
                        else if(value == ITF_GET_STRINGID_CRC(megaBubo, 278717207))
                        {
                            m_megaBubo = p->getRef();
                        }
                        else if(value == ITF_GET_STRINGID_CRC(cineOutro, 2016714390))
                        {
                            m_outro = p->getRef();
                        }
                    }
                }
            }
        }
    }

    void Ray_BigMamaAIComponent::triggerBubo(bbool _isMega)
    {
        Actor* bubo;
        if(_isMega) bubo = m_megaBubo.getActor();
        else if(m_currentPhase < 3) bubo = m_bubo.getActor();
        else bubo = m_secondBubo.getActor();

        if(bubo)
        {
            EventTrigger triggerEvent;
            triggerEvent.setSender(m_actor->getRef());
            triggerEvent.setActivated(btrue);
            bubo->onEvent(&triggerEvent);
        }
    }

    void Ray_BigMamaAIComponent::goToNextPhase()
    {
        m_currentPhase ++;
        updateAnimInputs();
        playMusic();
    }

    void Ray_BigMamaAIComponent::goToPreviousPhase()
    {
        m_currentPhase --;
        updateAnimInputs();
    }

    void Ray_BigMamaAIComponent::updateAnimInputs()
    {
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(currentPhase, 2710708517), m_currentPhase);

		u32 animIndex = m_eyesHitPoints ?
			m_nbEyeAnims - ((m_nbEyeAnims * m_eyesHitPoints) / getTemplate()->getEyesHitPoints())
			: m_nbEyeAnims + 1;
		m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(nbEyes, 436315771), animIndex);
    }

    void Ray_BigMamaAIComponent::ejectPlayers(ActorRef pos)
    {
        Ray_EventBounceToLayer bounceEvent(
            8.f,                // height
            10.f,               // height 2
            5.f,                // speed
            pos,                // actor ref
            Vec2d::Zero,        // target offset
            0);                 // hurt level
        bounceEvent.setSender(m_actor->getRef());   

        AIUtils::sendEventToLivePlayers(&bounceEvent);
    }

    const AnimBoneDyn* Ray_BigMamaAIComponent::getBone(const StringID& _bone)
    {
        i32 idx = m_animatedComponent->getBoneIndex(_bone);

        if(idx != U32_INVALID)
        {
            AnimBoneDyn* bone = m_animatedComponent->getBone(idx);

            ITF_ASSERT_MSG(bone, "Missing bone or invalid index");

            return bone;
        }
        return NULL;
    }

    void Ray_BigMamaAIComponent::spawnEyes()
    {
        const AnimBoneDyn* bone = getBone(getTemplate()->getEyeSpawnBone());
        if(!bone)
        {
            return;
        }
        Vec2d position = bone->m_Pos;
        f32 z = m_actor->getPos().m_z + 0.05f;
        Vec3d pos = position.to3d(z);

        u32 n = getTemplate()->getEyesPerHit();
        for(u32 i = 0; i < n; i ++)
        {
            m_eyeSpawner.getSpawnee(m_actor->getScene(),pos);
        }
    }

    void Ray_BigMamaAIComponent::playMusic()
    {
        // no music for intro so currentPhase - 1
        u32 index = m_currentPhase - 1;
        if (index < getTemplate()->getMusics().size())
        {
            EventPlayMusic& music = const_cast<EventPlayMusic&>(getTemplate()->getMusics()[index]);
            EVENTMANAGER->broadcastEvent(&music);
        }
    }

    void Ray_BigMamaAIComponent::startCine()
    {
        //Ray_AIUtils::showBossMedal(m_outro, m_actor->getRef());

        if ( Actor* outro = m_outro.getActor() )
        {
            EventTrigger trigger;
            trigger.setSender(m_actor->getRef());
            trigger.setActivator(m_actor->getRef());
            trigger.setActivated(btrue);
            outro->onEvent(&trigger);
        }
    }

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_BigMamaAIComponent_Template);
    BEGIN_SERIALIZATION_CHILD(Ray_BigMamaAIComponent_Template);
        SERIALIZE_MEMBER("eyesHitPoints", m_eyesHitPoints);
        SERIALIZE_MEMBER("eyesPerHit", m_eyesPerHit);
        SERIALIZE_MEMBER("eyeSpawn", m_eyeSpawn);
        SERIALIZE_MEMBER("eyeSpawnBone", m_eyeSpawnBone);
        SERIALIZE_MEMBER("allowedFaction", m_allowedFaction);

        SERIALIZE_MEMBER("ejectHeight1", m_ejectHeight1);
        SERIALIZE_MEMBER("ejectHeight2", m_ejectHeight2);
        SERIALIZE_MEMBER("ejectSpeed", m_ejectSpeed);

        SERIALIZE_CONTAINER_OBJECT("musics", m_musics);
    END_SERIALIZATION();

    Ray_BigMamaAIComponent_Template::Ray_BigMamaAIComponent_Template()
        : Super()
        , m_eyesHitPoints(10)
        , m_eyesPerHit(5)
        , m_ejectHeight1(5.f)
        , m_ejectHeight2(8.f)
        , m_ejectSpeed(5.f)
        , m_allowedFaction(FACTION_UNKNOWN)
    {
    }
};
