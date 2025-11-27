#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BOSSBIRDPAWNAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Boss/Ray_BossBirdPawnAIComponent.h"
#endif //_ITF_RAY_BOSSBIRDPAWNAICOMPONENT_H_

#ifndef _ITF_RAY_BOSSBIRDAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Boss/Ray_BossBirdAIComponent.h"
#endif //_ITF_RAY_BOSSBIRDAICOMPONENT_H_

#ifndef _ITF_SHAPEDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/ShapeDetectorComponent.h"
#endif //_ITF_SHAPEDETECTORCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_TWEENCOMPONENT_H_
#include "gameplay/Components/Common/TweenComponent.h"
#endif //_ITF_TWEENCOMPONENT_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_BossBirdPawnAIComponent)

        BEGIN_SERIALIZATION_CHILD(Ray_BossBirdPawnAIComponent)
        END_SERIALIZATION()

        BEGIN_VALIDATE_COMPONENT(Ray_BossBirdPawnAIComponent)
        //VALIDATE_COMPONENT_PARAM("", m_shapeDetector, "RAY_BossPlantProjectileAIComponent requires a DetectorComponent");
        END_VALIDATE_COMPONENT()

    Ray_BossBirdPawnAIComponent::Ray_BossBirdPawnAIComponent()
        : Super()
        , m_isHitable(btrue)
        , m_timer(0.f)
        , m_speed(Vec2d::Zero)
        , m_yGround(0.f)
        , m_rotation(0.f)
        , m_gravity(-10.f)
    {
    }

    Ray_BossBirdPawnAIComponent::~Ray_BossBirdPawnAIComponent()
    {
    }

    void Ray_BossBirdPawnAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        m_fxController = m_actor->GetComponent<FXControllerComponent>();
        m_shapeDetector = m_actor->GetComponent<ShapeDetectorComponent>();          // mandatory
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(PunchStim,200533519), this);
        m_speed = getTemplate()->getInitSpeed();
		f32 speedRand = getTemplate()->getSpeedRand();
		if(speedRand != 0.f)
		{
			m_speed.m_x *= Seeder::getSharedSeeder().GetFloat(1.f - speedRand, 1.f + speedRand);
		}
        f32 scale = getTemplate()->getScale();
        m_actor->setScale(Vec2d(scale, scale));
        m_yGround = getTemplate()->getYGround();
        m_gravity = getTemplate()->getGravity();
        f32 rotation = getTemplate()->getRotation();
        m_rotation = Seeder::getSharedSeeder().GetFloat(-rotation, rotation);
        Scene* scene=m_actor->getScene();
        if(scene->getAlwaysActiveActors().find(m_actor)<0) scene->setAlwaysActiveActor(m_actor);
    }

    void Ray_BossBirdPawnAIComponent::onEvent(Event* _event)
    {
        Super::onEvent(_event);

        if(PunchStim* hit = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)))
        {
            receiveHit(hit);
        }
        else if(AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)))
        {
            onGameplayEvent(animEvent);
        }
    }

    void Ray_BossBirdPawnAIComponent::onGameplayEvent(AnimGameplayEvent* animEvent)
    {
        if(animEvent->getName().GetValue() == ITF_GET_STRINGID_CRC(MRK_AI_Die, 2937366365))
        {
            dieNow();
        }
    }

    void Ray_BossBirdPawnAIComponent::receiveHit(PunchStim* _hit)
    {
        if(m_isHitable)
        {
            m_isHitable = bfalse;
            die();
            spawnReward(_hit->getSender());
            Vec2d hitDirection = _hit->getDirection();
            m_gravity = -30.f;
            m_rotation = Seeder::getSharedSeeder().GetFloat(-20.f, 20.f);
            m_speed = _hit->getDirection().normalize() * getTemplate()->getHitSpeed();
            m_timer = getTemplate()->getLifeDuration() - 1.5f;

            if (m_fxController)
            {
                Actor* sender = AIUtils::getActor(_hit->getSender());
                if(sender)
                {
                    Vec3d fxPos = sender->getPos();
                    const StringID feedbackAction = _hit->getActionFromHit();
                    u32 fxHandle = m_fxController->playFeedback(_hit->getSender(), feedbackAction, m_actor->getRef());
                    m_fxController->setFXPosFromHandle(fxHandle, _hit->getFxPos());
                }
            }

        }
    }

    void Ray_BossBirdPawnAIComponent::stopTween()
    {
        TweenComponent* tween = m_actor->GetComponent<TweenComponent>();
        if(tween)
        {
            tween->pause();
        }
    }

    void Ray_BossBirdPawnAIComponent::Update( f32 _dt )
    {
        Super::Update(_dt);
        detectActorsInside();
        move(_dt);
    }

    void Ray_BossBirdPawnAIComponent::move(f32 _dt)
    {
        m_timer += _dt;

        if(m_timer > getTemplate()->getLifeDuration())
        {
            dieNow();
            return;
        }

        Vec3d position = m_actor->getPos();
        m_speed.m_x = AIUtils::michelSmooth(m_speed.m_x, 0.f, _dt * getTemplate()->getXBreakFactor());
        m_speed.m_y += m_gravity * _dt;
        position += m_speed.to3d() * _dt;
        if(position.m_y <= m_yGround && m_speed.m_y < 0.f && abs(position.m_x) < getTemplate()->getXMax())
        {
            position.m_y = m_yGround;
            m_actor->setPos(position);
            m_speed.m_y *= -0.3f;
            if(m_isHitable)
            {
                die();
            }
        }
        else
        {
            m_actor->setPos(position);
        }

        m_actor->setAngle(m_actor->getAngle() + _dt * m_rotation);
    }

    void Ray_BossBirdPawnAIComponent::setup(const ActorRef &_boss)
    {
        m_boss = _boss;
        if(m_actor->isFlipped())
        {
            m_speed.m_x *= -1.f;
        }
    }

    bbool Ray_BossBirdPawnAIComponent::isEnemy(Faction _mine, Faction _his)
    {
        if(m_isHitable)
        {
            return AIUtils::isTarget(_mine, _his);
        }
        return _mine == _his;
    }

    void Ray_BossBirdPawnAIComponent::detectActorsInside()
    {
        if(!m_shapeDetector)// || !m_isHitable)
        {
            return;
        }
        ActorRefList& actorsInsideThisFrame = m_shapeDetector->getActorsInside();
        u32 numActors = actorsInsideThisFrame.size();

        bbool didCollision = bfalse;

        Faction faction = getTemplate()->getFaction();

        for ( u32 i = 0; i < numActors; i++ )
        {
            Actor* actorActivator = AIUtils::getActor(actorsInsideThisFrame[i]);
            if(actorActivator && isEnemy(faction, AIUtils::getFaction(actorActivator)))
            {
                Vec2d bounceDirection = (actorActivator->getPos() - m_actor->getPos()).truncateTo2D();
                bounceDirection = bounceDirection.normalize();
                sendHitMessage(actorActivator, bounceDirection);
                didCollision = btrue;
            }
        }
        if(didCollision)
        {
            die();
        }
    }

    void Ray_BossBirdPawnAIComponent::sendHitMessage(Actor* _receiver, const Vec2d& _dir)
    {
        BounceStim  bounceStim;
        PunchStim   punchStim;
        EventStim*  sendStim;

        punchStim.setReceivedHitType(RECEIVEDHITTYPE_HURTBOUNCE);
        punchStim.setIsRadial(true);
        punchStim.setDirection(_dir);
        punchStim.setFxPos(_receiver->getPos());
        punchStim.setLevel(0);
        punchStim.setFaction(getTemplate()->getFaction());
        sendStim = &punchStim;

        sendStim->setSender(m_actor->getRef());
        sendStim->setAngle(m_actor->getAngle());
        sendStim->setPos(m_shapeDetector->getShapePos());

        _receiver->onEvent(sendStim);
    }

    void Ray_BossBirdPawnAIComponent::die()
    {
        m_isHitable = bfalse;
        m_animComponent->setAnim(getTemplate()->getDeathAnim());
    }

    void Ray_BossBirdPawnAIComponent::dieNow()
    {
        if (m_actor->isSerializable())
        {
            m_actor->disable();
        }
        else
        {
            m_actor->requestDestruction();
        }
    }

    void Ray_BossBirdPawnAIComponent::spawnReward(ActorRef _actor)
    {
		Actor* boss = m_boss.getActor();
		if(!boss) return;

		Ray_BossBirdAIComponent* ai = boss->GetComponent<Ray_BossBirdAIComponent>();
		if(!ai) return;

		u32 nb = ai->getNbLumsToSpawn(getTemplate()->getNbRewards());
		if(!nb) return;

		Ray_EventSpawnRewardLum evt;
		evt.setup(m_actor);
		evt.setNumRewards(nb);
		evt.setRewardReceiver(_actor);

		RAY_GAMEMANAGER->SpawnRewardLum( &evt );
	}

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_BossBirdPawnAIComponent_Template)
    BEGIN_SERIALIZATION(Ray_BossBirdPawnAIComponent_Template)
        SERIALIZE_MEMBER("faction", m_faction);
        SERIALIZE_MEMBER("gravity", m_gravity);
        SERIALIZE_MEMBER("scale", m_scale);
        SERIALIZE_MEMBER("xMax", m_xMax);
        SERIALIZE_MEMBER("lifeDuration", m_lifeDuration);
        SERIALIZE_MEMBER("yGround", m_yGround);
        SERIALIZE_MEMBER("initSpeed", m_initSpeed);
		SERIALIZE_MEMBER("speedRand", m_speedRand);
        SERIALIZE_MEMBER("rotation", m_rotation);
        SERIALIZE_MEMBER("xBreakFactor", m_xBreakFactor);
        SERIALIZE_MEMBER("deathAnim", m_deathAnim);
        SERIALIZE_MEMBER("hitSpeed", m_hitSpeed);
		SERIALIZE_MEMBER("nbRewards", m_nbRewards);
    END_SERIALIZATION()

    Ray_BossBirdPawnAIComponent_Template::Ray_BossBirdPawnAIComponent_Template()
    : Super()
    , m_faction(0)
    , m_gravity(-10.f)
    , m_scale(1.f)
    , m_xMax(10.f)
    , m_lifeDuration(10.f)
    , m_initSpeed(Vec2d::Zero)
    , m_xBreakFactor(0.f)
    , m_yGround(0.f)
    , m_hitSpeed(20.f)
	, m_speedRand(0.05f)
    , m_rotation(0.f)
	, m_nbRewards(5)
    {
    }
};
