#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BOSSPLANTARENAAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Boss/Ray_BossPlantArenaAIComponent.h"
#endif //_ITF_RAY_BOSSPLANTARENAAICOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_RAY_AIUTILS_H_
#include "Rayman/gameplay/AI/Utils/Ray_AIUtils.h"
#endif //_ITF_RAY_AIUTILS_H_

#ifndef _ITF_WORLD_MANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLD_MANAGER_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#include "rayman/gameplay/Ray_GameplayEvents.h"
#endif //_ITF_RAY_GAMEPLAYEVENTS_H_

#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_BossPlantArenaAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_BossPlantArenaAIComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_BossPlantArenaAIComponent)
    END_VALIDATE_COMPONENT()

    Ray_BossPlantArenaAIComponent::Ray_BossPlantArenaAIComponent()
        : Super()
        , m_phaseNumber(0)
        , m_bubo(ActorRef::InvalidRef)
        , m_moveBubo(bfalse)
        , m_outro(ActorRef::InvalidRef)
		, m_initDone(bfalse)
		, m_buboBoneIndex(0)
#ifdef BOSSP_USERETURNTOMENUFORDEMO
        , TMPDEMO_delayLoadMenu(0.f)
#endif // BOSSP_USERETURNTOMENUFORDEMO
    {
    }

    Ray_BossPlantArenaAIComponent::~Ray_BossPlantArenaAIComponent() {
    }

    void Ray_BossPlantArenaAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);
        getChildren();
		if(m_actor->isStartPaused()) m_actor->disable();
    }

    void Ray_BossPlantArenaAIComponent::onEvent(Event *_event)
    {
        Super::onEvent(_event);

        if(AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)))
        {
            onGameplayEvent(animEvent);
        }
        else if(EventTrigger *triggerEvent = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
        {
            if(triggerEvent->getSender() == m_bubo) goToNextPhase();
            else if(triggerEvent->getActivated()) init();
        }
    }

    void Ray_BossPlantArenaAIComponent::init()
    {
        updateAnimInputs();
        playMusic();
        m_actor->enable();
		m_initDone = btrue;
    }

    void Ray_BossPlantArenaAIComponent::onGameplayEvent(AnimGameplayEvent* animEvent)
    {
        unsigned long value = (unsigned long)animEvent->getName().GetValue();

        if(value == ITF_GET_STRINGID_CRC(MRK_AI_Die, 2937366365))
        {
            m_actor->disable();
        }
        else if(value == ITF_GET_STRINGID_CRC(MRK_AI_ShowWeakPoint, 3186572022)
            || value == ITF_GET_STRINGID_CRC(MRK_AI_HideWeakPoint, 692695698))
        {
            triggerBubo();
        }
        else if(value == ITF_GET_STRINGID_CRC(MRK_AI_StartCine, 488987668))
        {
            startCine();
        }
        else
        {
            shakeCamera(animEvent);
        }
    }

    void Ray_BossPlantArenaAIComponent::Update(f32 _dt)
    {
        Super::Update(_dt);
		if(m_initDone)
		{
			updateAnim();
			updateBubo(_dt);

#ifdef BOSSP_USERETURNTOMENUFORDEMO
			if(TMPDEMO_delayLoadMenu > 0.f)
			{
				if(TMPDEMO_delayLoadMenu > 2.f && TMPDEMO_delayLoadMenu - _dt <= 2.f)
				{
					RAY_GAMEMANAGER->fxFade(Color::zero(), Color::black(), 2.f, btrue);
				}

				TMPDEMO_delayLoadMenu -= _dt;
				if(TMPDEMO_delayLoadMenu <= 0.f)
				{
					RAY_GAMEMANAGER->goToTheMainMenu();
				}
			}
#endif // BOSSP_USERETURNTOMENUFORDEMO
		}
		else
		{
			m_actor->disable();
		}
    }

    void Ray_BossPlantArenaAIComponent::updateAnim()
    {
        if(m_animatedComponent->canGetRootDelta())
        {
            Vec2d delta = m_animatedComponent->getRootDelta();
            delta = delta.Rotate(m_actor->getAngle());
            Vec3d newPosition = delta.to3d() + m_actor->getPos();
            // when an animation is finished, make sure we end on a clean
            // distance, in plain meters
            if(m_animatedComponent->isSubAnimFinished())
            {
                Vec3d distanceDone = newPosition - m_actor->getLocalInitialPos();
                distanceDone.m_x = roundFloat(distanceDone.m_x);
                distanceDone.m_y = roundFloat(distanceDone.m_y);
                newPosition = m_actor->getLocalInitialPos() + distanceDone;
            }
            m_actor->setBoundWorldPos(newPosition);
        }
    }

    void Ray_BossPlantArenaAIComponent::goToNextPhase()
    {
        m_moveBubo = bfalse;
        m_phaseNumber ++;
        updateAnimInputs();
        playMusic();
#ifdef BOSSP_USERETURNTOMENUFORDEMO
            if(m_phaseNumber == 3)
            {
                TMPDEMO_delayLoadMenu = 7.f;
            }
#endif // BOSSP_USERETURNTOMENUFORDEMO
    }

    void Ray_BossPlantArenaAIComponent::playMusic()
    {
        if (m_phaseNumber < getTemplate()->getMusics().size())
        {
            EventPlayMusic& music = const_cast<EventPlayMusic&>(getTemplate()->getMusics()[m_phaseNumber]);
            EVENTMANAGER->broadcastEvent(&music);
        }
    }

    void Ray_BossPlantArenaAIComponent::updateAnimInputs()
    {
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(currentPhase, 2710708517), m_phaseNumber);
    }

    void Ray_BossPlantArenaAIComponent::triggerBubo()
    {
        m_moveBubo ^= btrue;
		if(m_moveBubo) {
			m_buboBoneIndex = m_phaseNumber;
		}
        Actor* bubo = m_bubo.getActor();
        if(bubo)
        {
            EventTrigger triggerEvent;
            triggerEvent.setSender(m_actor->getRef());
            triggerEvent.setActivated(btrue);
            bubo->onEvent(&triggerEvent);
        }
    }

    void Ray_BossPlantArenaAIComponent::updateBubo(f32 _dt)
    {
        Actor* bubo = m_bubo.getActor();

        if(!bubo)// || !m_moveBubo)
        {
            return;
        }

        const StringID boneName = getTemplate()->getBuboBone(m_buboBoneIndex);
        i32 idx = m_animatedComponent->getBoneIndex(boneName);

        if(idx != U32_INVALID)
        {
            AnimBoneDyn* bone = m_animatedComponent->getBone(idx);

            ITF_ASSERT_MSG(bone, "Missing bone or invalid index");

            if(bone)
            {
                f32 z = m_actor->getDepth() + 0.2f;
                bubo->setPos(bone->m_Pos.to3d(z));
                bubo->setAngle(bone->getAngle());
                bubo->setFlipped(m_actor->isFlipped());
                f32 newAngle = bone->getAngle() - MTH_PIBY2;
                bubo->setAngle(newAngle);       
            }
        }
    }

    void Ray_BossPlantArenaAIComponent::getChildren()
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
                    Pickable *p= SceneObjectPathUtils::getObjectFromRelativePath(m_actor, children[i].getPath());
                    if (p)
                    {
                        unsigned long value = (unsigned long)tagValue.GetValue();
                        if(value == ITF_GET_STRINGID_CRC(bubo, 1755405513))
                        {
                            m_bubo = p->getRef();
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

    void Ray_BossPlantArenaAIComponent::shakeCamera(AnimGameplayEvent* _animEvent)
    {
        if (_animEvent->getName() == MRK_Cam_Shake_00)
        {
            CAMERACONTROLLERMANAGER->addShake(ITF_GET_STRINGID_CRC(Quake0, 2087111994));
        }
        else if (_animEvent->getName() == MRK_Cam_Shake_01)
        {
            CAMERACONTROLLERMANAGER->addShake(ITF_GET_STRINGID_CRC(Quake1, 3041659572));
        }
        else if (_animEvent->getName() == MRK_Cam_Shake_02)
        {
            CAMERACONTROLLERMANAGER->addShake(ITF_GET_STRINGID_CRC(Quake2, 3890011725));
        }
        else if (_animEvent->getName() == MRK_Cam_Shake_03)
        {
            CAMERACONTROLLERMANAGER->addShake(ITF_GET_STRINGID_CRC(Quake3, 4080307277));
        }
    }

    void Ray_BossPlantArenaAIComponent::startCine()
    {
        // keep this for mecha boss plant
        LinkComponent *linkComp = m_actor->GetComponent<LinkComponent>();
		if(getTemplate()->getIsMecha())
		{
			if (linkComp)
			{
				EventSequenceControl evt;
				evt.setState(SequencePlayerComponent::State_Playing);
				linkComp->sendEventToChildren(&evt);
			}
		}
		else
		{
            Ray_AIUtils::showBossMedal( m_outro, m_actor->getRef() );
		}
    }

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_BossPlantArenaAIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_BossPlantArenaAIComponent_Template)
        SERIALIZE_MEMBER("isMecha", m_isMecha);
        SERIALIZE_CONTAINER_OBJECT("buboBones", m_buboBones);
        SERIALIZE_CONTAINER_OBJECT("musics", m_musics);
        END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS(Ray_BossPlantArenaAIComponent_Template, BuboBone)
        SERIALIZE_MEMBER("bone", m_bone);
    END_SERIALIZATION()

    Ray_BossPlantArenaAIComponent_Template::Ray_BossPlantArenaAIComponent_Template()
        : Super()
        , m_isMecha(bfalse)
    {
    }
};
