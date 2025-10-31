#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BOSSBIRDAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Boss/Ray_BossBirdAIComponent.h"
#endif //_ITF_RAY_BOSSBIRDAICOMPONENT_H_

#ifndef _ITF_RAY_AIUTILS_H_
#include "Rayman/gameplay/AI/Utils/Ray_AIUtils.h"
#endif //_ITF_RAY_AIUTILS_H_

#ifndef _ITF_RAY_GEYSERPLATFORMAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_GeyserPlatformAIComponent.h"
#endif //_ITF_RAY_GEYSERPLATFORMAICOMPONENT_H_

#ifndef _ITF_RAY_BOSSBIRDPAWNAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Boss/Ray_BossBirdPawnAIComponent.h"
#endif //_ITF_RAY_BOSSBIRDPAWNAICOMPONENT_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_BossBirdAIComponent)

        BEGIN_SERIALIZATION_CHILD(Ray_BossBirdAIComponent)
        END_SERIALIZATION()

        BEGIN_VALIDATE_COMPONENT(Ray_BossBirdAIComponent)
        END_VALIDATE_COMPONENT()

    Ray_BossBirdAIComponent::Ray_BossBirdAIComponent()
        : Super()
        , m_currentPhase(0)
        , m_tail(ActorRef::InvalidRef)
        , m_bubo(ActorRef::InvalidRef)
		, m_skullCoin(ActorRef::InvalidRef)
		, m_skullCoin2(ActorRef::InvalidRef)
        , m_currentSequence(ActorRef::InvalidRef)
        , m_sucking(ActorRef::InvalidRef)
        , m_tailIsReady(btrue)
        , m_nbEggsToSpawn(0)
        , m_eggDelay(0.f)
        , m_highBirds(bfalse)
        , m_lumsToSpawn(50)
		, m_nbPlayersHanging(0)
		, m_tailDelay(0.f)
		, m_tailUsed(bfalse)
		, m_initDone(bfalse)
    {
    }

    Ray_BossBirdAIComponent::~Ray_BossBirdAIComponent()
    {
    }

    void Ray_BossBirdAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);
        SPAWNER->declareNeedsSpawnee(m_actor, &m_eggGenerator, getTemplate()->getEgg());
        SPAWNER->declareNeedsSpawnee(m_actor, &m_birdGenerator, getTemplate()->getBird());
        getChildren();
		if(m_actor->isStartPaused()) m_actor->disable();
    }

    void Ray_BossBirdAIComponent::onEvent(Event* _event)
    {
        Super::onEvent(_event);

        if(AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)))
        {
            onGameplayEvent(animEvent);
        }
        else if(EventTrigger *triggerEvent = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
        {
            if(triggerEvent->getSender() == m_bubo)
            {
				triggerSkullCoin();
                stopEggs();
                goToNextPhase();
                //playMusic();
            }
            else if(triggerEvent->getSender() == m_tail) triggerTail(triggerEvent);
            else if(triggerEvent->getActivated()) init();
        }
    }

    void Ray_BossBirdAIComponent::init()
    {
        m_actor->enable();
        if(getTemplate()->getPlayIntroMusic()) playMusic();
		m_lumsToSpawn = getTemplate()->getMaxLums();
		m_initDone = btrue;
    }

    void Ray_BossBirdAIComponent::onGameplayEvent(AnimGameplayEvent* _animEvent)
    {
        unsigned long value = (unsigned long)_animEvent->getName().GetValue();

        if(value == ITF_GET_STRINGID_CRC(MRK_AI_Die, 2937366365))
        {
            m_actor->disable();
        }
        else if(value == ITF_GET_STRINGID_CRC(MRK_AI_Flip, 1084193828))
        {
            m_actor->setFlipped(!m_actor->isFlipped());
        }
        else if(value == ITF_GET_STRINGID_CRC(MRK_AI_ResetFlip, 935910858))
        {
            m_actor->setFlipped(bfalse);
        }
        else if(value == ITF_GET_STRINGID_CRC(MRK_AI_IntroSequence, 2151821804))
        {
            if(getTemplate()->getPlayIntro()) startSequence();
        }
        else if(value == ITF_GET_STRINGID_CRC(MRK_AI_StartOutro, 3886861901))
        {
            if(getTemplate()->getPlayOutro() && !getTemplate()->getIsMecha()) Ray_AIUtils::showBossMedal(m_currentSequence, m_actor->getRef());
            else startSequence();
        }
        else if(value == ITF_GET_STRINGID_CRC(MRK_AI_TriggerSequence, 216940924))
        {
            startSequence();
        }
        else if(value == ITF_GET_STRINGID_CRC(MRK_AI_TriggerSucking, 1704650015))
        {
            triggerGeyser();
        }
        else if(value == ITF_GET_STRINGID_CRC(MRK_AI_TriggerBubo, 795340486))
        {
            triggerBubo();
        }
        else if(value == ITF_GET_STRINGID_CRC(MRK_AI_DropEggs, 3324998758))
        {
            startEggs();
        }
        else if(value == ITF_GET_STRINGID_CRC(MRK_AI_SpawnBirds, 1445189389))
        {
            spawnBirds();
        }
        else if(value == ITF_GET_STRINGID_CRC(MRK_AI_TriggerTail, 1469519454))
        {
            m_tailIsReady ^= btrue;
            enableTail();
        }
        else if(value == ITF_GET_STRINGID_CRC(MRK_AI_BirdHigh, 767817042))
        {
            m_highBirds = btrue;
        }
        else if(value == ITF_GET_STRINGID_CRC(MRK_AI_BirdLow, 3519290944))
        {
            m_highBirds = bfalse;
        }
        else
        {
            shakeCamera(_animEvent);
        }
    }

    void Ray_BossBirdAIComponent::Update( f32 _dt )
    {
        Super::Update(_dt);
		if(m_initDone)
		{
			updateAnim();
			checkTailDelay(_dt);
			dropEggs(_dt);
		}
		else
		{
			m_actor->disable();
		}
    }

    void Ray_BossBirdAIComponent::updateAnim()
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

    void Ray_BossBirdAIComponent::getChildren()
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
                        unsigned long value = tagValue.GetValue();
                        if(value == ITF_GET_STRINGID_CRC(bubo, 1755405513))
                        {
                            m_bubo = p->getRef();
                        }
                        else if(value == ITF_GET_STRINGID_CRC(sequence, 1724815235))
                        {
                            m_currentSequence = p->getRef();
                        }
                        else if(value == ITF_GET_STRINGID_CRC(sucking, 1664156104))
                        {
                            m_sucking = p->getRef();
                        }
                        else if(value == ITF_GET_STRINGID_CRC(tail, 2612276000))
                        {
                            m_tail = p->getRef();
                        }
						else if(value == ITF_GET_STRINGID_CRC(skullCoin, 820627580))
						{
							m_skullCoin = p->getRef();
						}
						else if(value == ITF_GET_STRINGID_CRC(skullCoin2, 2287265862))
						{
							m_skullCoin2 = p->getRef();
						}
                    }
                }
            }
        }
    }

    const AnimBoneDyn* Ray_BossBirdAIComponent::getBone(const StringID& _bone)
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

	void Ray_BossBirdAIComponent::checkTailDelay(f32 _dt)
	{
		if(!m_currentPhase && m_nbPlayersHanging)
		{
			m_tailDelay -= _dt;
			if(m_tailDelay <= 0.f)
			{
				goToNextPhase();
				//playMusic();
			}
		}
	}

    void Ray_BossBirdAIComponent::triggerTail(EventTrigger* _evt)
    {
        if(m_tailIsReady)
        {
			if(_evt->getActivated())
			{
				// intro: hang on the tail for a while to wake up the boss
				if(!m_currentPhase)
				{
					if(!m_nbPlayersHanging)
					{
						m_tailDelay = getTemplate()->getTailDelay();
					}
					m_nbPlayersHanging ++;
				}
				// other phase: reset it
				else if(!m_tailUsed)
				{
					m_tailUsed = btrue;
					m_animatedComponent->resetTree();
				}
			}
			else if(!m_currentPhase)
			{
				m_nbPlayersHanging --;
			}
        }
    }

    void Ray_BossBirdAIComponent::enableTail()
    {
        Actor* tail = m_tail.getActor();
        if(tail)
        {
            if(m_tailIsReady)
            {
                tail->enable();
            }
            else
            {
				m_tailUsed = bfalse;
                tail->disable();
            }
        }
    }

    void Ray_BossBirdAIComponent::triggerBubo()
    {
        Actor* bubo = m_bubo.getActor();
        if(bubo)
        {
            EventTrigger triggerEvent;
            triggerEvent.setSender(m_actor->getRef());
            triggerEvent.setActivated(btrue);
            bubo->onEvent(&triggerEvent);
        }
    }

	void Ray_BossBirdAIComponent::triggerGeyser()
    {
        Actor* actor = m_sucking.getActor();
        if(actor)
        {
            EventTrigger evt;
            evt.setSender(m_actor->getRef());
            evt.setActivated(btrue);
            actor->onEvent(&evt);
        }
    }

    void Ray_BossBirdAIComponent::goToNextPhase()
    {
        m_currentPhase++;
        updateAnimInputs();
		playMusic();
    }

    void Ray_BossBirdAIComponent::updateAnimInputs()
    {
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(currentPhase, 2710708517), m_currentPhase);
    }

    void Ray_BossBirdAIComponent::shakeCamera(AnimGameplayEvent* _animEvent)
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

    f32 Ray_BossBirdAIComponent::blend(f32 _from, f32 _to, f32 _factor)
    {
        return _from + (_to - _from) * AIUtils::limit(_factor, 0.f, 1.f);
    }

    void Ray_BossBirdAIComponent::spawnBirds()
    {
        u32 i;
        u32 n = getTemplate()->getNbBirds();
        Vec3d position = m_actor->getPos();
        f32 x = position.m_x;
        f32 xSign = (m_actor->isFlipped() ? 1.f : -1.f);
        f32 xOffsetMin = getTemplate()->getBirdXOffset();
        f32 xOffsetMax = xOffsetMin + getTemplate()->getBirdXSpace();
        position.m_y += m_highBirds ? getTemplate()->getBirdYStart() : getTemplate()->getBirdYLow();
        f32 space = getTemplate()->getBirdYSpace();

        position.m_y -= 0.5f * space * (f32)n;

        for(i = 0; i < n; i ++ )
        {
            position.m_x = x + xSign
                * blend(xOffsetMin, xOffsetMax,
                    abs(blend(-1.f, 1.f,
                        (n > 1 ? (f32)i / (f32)(n - 1) : 0.f))));
            Actor* actor = m_birdGenerator.getSpawnee(m_actor->getScene(), position);
            if(actor)
            {
                actor->setFlipped(m_actor->isFlipped());
                setupSpawnedObject(actor);
            }
            position.m_y += space;
        }
    }

	void Ray_BossBirdAIComponent::triggerSkullCoin()
	{
		if(m_skullCoin.isValid())
		{
			Actor* actor = m_skullCoin.getActor();
			if(!actor) return;

			Ray_EventQueryChildLaunch query;
			actor->onEvent(&query);
			if (query.canLaunch())
			{
				actor->enable();
				Ray_EventChildLaunch launch(m_actor->getPos());
				actor->onEvent(&launch);
			}
	
			m_skullCoin = m_skullCoin2;
			m_skullCoin2 = ActorRef::InvalidRef;
		}
	}

    void Ray_BossBirdAIComponent::startEggs()
    {
        m_nbEggsToSpawn = getTemplate()->getNbEggs();
        m_eggDelay = getTemplate()->getFirstEggDelay();
    }

    void Ray_BossBirdAIComponent::stopEggs()
    {
        m_nbEggsToSpawn = 0;
    }

    void Ray_BossBirdAIComponent::dropEggs(f32 _dt)
    {
        if(!m_nbEggsToSpawn)
        {
            return;
        }
        m_eggDelay -= _dt;
        if(m_eggDelay > 0.f)
        {
            return;
        }
        m_nbEggsToSpawn --;
        m_eggDelay += getTemplate()->getEggDelay();

        const AnimBoneDyn* bone = getBone(getTemplate()->getEggBone());
        if(!bone)
        {
            return;
        }
        Vec2d position = bone->m_Pos;
        f32 z = m_actor->getPos().m_z - 0.05f;
        Actor* actor = m_eggGenerator.getSpawnee(m_actor->getScene(),position.to3d(z));
        if(actor)
        {
            actor->setFlipped(m_actor->isFlipped());
            setupSpawnedObject(actor);
        }
    }

    void Ray_BossBirdAIComponent::setupSpawnedObject(Actor* _actor)
    {
        Ray_BossBirdPawnAIComponent* ai = _actor->GetComponent<Ray_BossBirdPawnAIComponent>();
        if(ai)
        {
            ai->setup(m_actor->getRef());
        }
    }

	u32 Ray_BossBirdAIComponent::getNbLumsToSpawn(u32 _nbWanted)
	{
		if(_nbWanted > m_lumsToSpawn) _nbWanted = m_lumsToSpawn;
		m_lumsToSpawn -= _nbWanted;
		return _nbWanted;
	}

    void Ray_BossBirdAIComponent::moveToNextSequence()
    {
        Actor* seq = m_currentSequence.getActor();
        if(seq)
        {
            LinkComponent *linkComp = seq->GetComponent<LinkComponent>();
            if (linkComp)
            {
                const LinkComponent::ChildrenList &children = linkComp->getChildren();
                if(children.size())
                {
                    Pickable *p= SceneObjectPathUtils::getObjectFromRelativePath(m_actor, children[0].getPath());
                    if (p)
                    {
                        m_currentSequence = p->getRef();
                        return;
                    }
                }
            }
        }
    }

    void Ray_BossBirdAIComponent::startSequence()
    {
        Actor* seq = m_currentSequence.getActor();
        if(seq)
        {
            EventSequenceControl evt;
            evt.setState(SequencePlayerComponent::State_Playing);
            seq->onEvent(&evt);
            moveToNextSequence();
        }
    }

    void Ray_BossBirdAIComponent::playMusic()
    {
        u32 index = m_currentPhase;
        if(!getTemplate()->getPlayIntroMusic()) index --;
        if (index < getTemplate()->getMusics().size())
        {
            EventPlayMusic& music = const_cast<EventPlayMusic&>(getTemplate()->getMusics()[index]);
            EVENTMANAGER->broadcastEvent(&music);
        }
    }

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_BossBirdAIComponent_Template);
    BEGIN_SERIALIZATION_CHILD(Ray_BossBirdAIComponent_Template);
		SERIALIZE_MEMBER("isMecha", m_isMecha);
		SERIALIZE_MEMBER("playIntroMusic", m_playIntroMusic);
		SERIALIZE_MEMBER("playIntro", m_playIntro);
		SERIALIZE_MEMBER("tailDelay", m_tailDelay);
		SERIALIZE_MEMBER("egg", m_egg);
		SERIALIZE_MEMBER("eggBone", m_eggBone);
		SERIALIZE_MEMBER("nbEggs", m_nbEggs);
		SERIALIZE_MEMBER("firstEggDelay", m_firstEggDelay);
		SERIALIZE_MEMBER("eggDelay", m_eggDelay);
		SERIALIZE_MEMBER("bird", m_bird);
		SERIALIZE_MEMBER("nbBirds", m_nbBirds);
		SERIALIZE_MEMBER("birdYStart", m_birdYStart);
		SERIALIZE_MEMBER("birdYLow", m_birdYLow);
		SERIALIZE_MEMBER("birdXOffset", m_birdXOffset);
		SERIALIZE_MEMBER("birdXSpace", m_birdXSpace);
		SERIALIZE_MEMBER("birdYSpace", m_birdYSpace);
		SERIALIZE_MEMBER("maxLums", m_maxLums);
		SERIALIZE_CONTAINER_OBJECT("musics", m_musics);
    END_SERIALIZATION();

    Ray_BossBirdAIComponent_Template::Ray_BossBirdAIComponent_Template()
        : Super()
        , m_isMecha(bfalse)
        , m_playIntroMusic(btrue)
        , m_playIntro(btrue)
        , m_playOutro(btrue)
		, m_tailDelay(1.f)
        , m_nbEggs(5)
        , m_firstEggDelay(3.f)
        , m_eggDelay(0.5f)
        , m_nbBirds(3)
        , m_birdYStart(5.f)
        , m_birdYLow(-5.f)
        , m_birdXOffset(12.f)
        , m_birdXSpace(5.f)
        , m_birdYSpace(-2.f)
        , m_maxLums(50)
    {
    }

};