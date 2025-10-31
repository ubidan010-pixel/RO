#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BOSSMORAYBODYPART_H_
#include "rayman/gameplay/Components/AI/Ray_BossMorayBodyPart.h"
#endif //_ITF_RAY_BOSSMORAYBODYPART_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_RAY_BOSSMORAYAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BossMorayAIComponent.h"
#endif //_ITF_RAY_BOSSMORAYAICOMPONENT_H_

#ifndef _ITF_DEFORMONTRAJECTORYCOMPONENT_H_
#include "gameplay/Components/Common/DeformOnTrajectoryComponent.h"
#endif //_ITF_DEFORMONTRAJECTORYCOMPONENT_H_

#ifndef _ITF_RAY_SEEKINGJELLYFISHAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_SeekingJellyfishAIComponent.h"
#endif //_ITF_RAY_SEEKINGJELLYFISHAICOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_BossMorayBodyPart)

    Ray_BossMorayBodyPart::Ray_BossMorayBodyPart()
        : Super()
        , m_aiComponent(NULL)
        , m_state(State_Idle)
        , m_pendingState(State_Invalid)
        , m_missileSpawnRequested(bfalse)
        , m_missileSpawnRegionIndex(U32_INVALID)
    {
    }

    Ray_BossMorayBodyPart::~Ray_BossMorayBodyPart()
    {
    }

    void Ray_BossMorayBodyPart::onInit()
    {
        Super::onInit();

        m_aiComponent = m_parentComponent->DynamicCast<Ray_BossMorayAIComponent>(ITF_GET_STRINGID_CRC(Ray_BossMorayAIComponent,2045799600));
        ITF_WARNING(m_parent, m_aiComponent, "Ray_BossMorayBodyPart requires a Ray_BossMorayAIComponent");

        ACTOR_REGISTER_EVENT_COMPONENT(m_parent, ITF_GET_STRINGID_CRC(Ray_EventBossMorayFireMissile,1956304621), m_parentComponent );
        ACTOR_REGISTER_EVENT_COMPONENT(m_parent, ITF_GET_STRINGID_CRC(EventGeneric,4063838687), m_parentComponent );
        ACTOR_REGISTER_EVENT_COMPONENT(m_parent, ITF_GET_STRINGID_CRC(EventBossBubonHit,1018185172), m_parentComponent );
        ACTOR_REGISTER_EVENT_COMPONENT(m_parent, ITF_GET_STRINGID_CRC(Ray_EventBossMorayNodeReached,491301045), m_parentComponent );

        if (!getTemplate()->getBuboPath().isEmpty())
        {
            Spawner::getInstance()->declareNeedsSpawnee(m_parent->getResourceGroup(), &m_buboSpawner, getTemplate()->getBuboPath());
        }

        if (!getTemplate()->getMissilePath().isEmpty())
        {
            Spawner::getInstance()->declareNeedsSpawnee(m_parent->getResourceGroup(), &m_missileSpawner, getTemplate()->getMissilePath());
        }
    }

    Actor* Ray_BossMorayBodyPart::spawnBodyPart()
    {
        Actor* actor = Super::spawnBodyPart();

        if (actor)
        {
            ACTOR_REGISTER_EVENT_COMPONENT(actor, ITF_GET_STRINGID_CRC(EventPolylineBlockingContact,1992742979), m_parentComponent );
        }

        return actor;
    }

    void Ray_BossMorayBodyPart::clear()
    {
        Super::clear();

        if ( Actor* actor = getActorRef().getActor() )
        {
            actor->clearRegisteredEvents();    // HACK for RO-17333
        }

        if ( Actor* bubo = m_bubo.getActor() )
        {
            bubo->requestDestruction();
        }

        for (u32 i = 0; i < m_missiles.size(); i++)
        {
            if (Actor* missile = m_missiles[i].getActor())
            {
                missile->requestDestruction();
            }
        }
    }

    void Ray_BossMorayBodyPart::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( Ray_EventBossMorayFireMissile* eventFire = _event->DynamicCast<Ray_EventBossMorayFireMissile>(ITF_GET_STRINGID_CRC(Ray_EventBossMorayFireMissile,1956304621)) )
        {
            processFireMissile(eventFire);
        }
        else if ( _event->IsClassCRC(ITF_GET_STRINGID_CRC(EventBossBubonHit,1018185172)) )
        {
            processBuboHit(_event);
        }
        else if ( EventGeneric* eventGeneric = _event->DynamicCast<EventGeneric>(ITF_GET_STRINGID_CRC(EventGeneric,4063838687)) )
        {
            if (eventGeneric->getId() == "ShowBubo")
            {
                if (m_state != State_CloseRangeAttack)
                {
                    changeStateNextFrame(State_BuboSpawn);
                }

                toggleBubo();   // turn on
            }
            else if (eventGeneric->getId() == "HideBubo")
            {
                toggleBubo();   // turn off
            }
            else if (eventGeneric->getId() == "Scream")
            {
                if (m_state != State_CloseRangeAttack)
                {
                    changeStateNextFrame(State_Scream);
                }
            }
        }
        else if ( EventPolylineBlockingContact* block = _event->DynamicCast<EventPolylineBlockingContact>(ITF_GET_STRINGID_CRC(EventPolylineBlockingContact,1992742979)) )
        {
            processSpike(block);
        }
        else if ( Ray_EventBossMorayNodeReached* nodeReached = _event->DynamicCast<Ray_EventBossMorayNodeReached>(ITF_GET_STRINGID_CRC(Ray_EventBossMorayNodeReached,491301045)) )
        {
            processNodeReached(nodeReached);
        }
    }

    void Ray_BossMorayBodyPart::update( f32 _dt )
    {
        // HACK: we don't need to call the super that checks for isAnimFinished etc...
        //Super::update(_dt);

        updateState();

        updateBubo();

        updateMissileSpawner();
    }

    void Ray_BossMorayBodyPart::updateState()
    {
        bbool justChangedState = bfalse;
        if (m_pendingState != State_Invalid)
        {
            changeState(m_pendingState);
            m_pendingState = State_Invalid;
            justChangedState = btrue;
        }

        switch (m_state)
        {
        case State_Idle:
            {
                checkCloseRangeAttack();
            }
            break;

        case State_Scream:
        case State_BuboSpawn:
        case State_BuboHit:
        case State_FireMissile:
        case State_CloseRangeAttack:
        case State_Spike:
            {
                if (!justChangedState && isAnimFinished())
                {
                    if (m_aiComponent->getIsDead())
                    {
                        changeState(State_Death);
                    }
                    else
                    {
                        changeState(State_Idle);
                    }
                }
            }
            break;

        case State_Dash_In:
            {
                if (!justChangedState && isAnimFinished())
                {
                    changeState(State_Dash_Loop);
                }
            }
            break;

        case State_Dash_Loop:
            {
                // nothing to do ATM...
            }
            break;

        case State_Dash_Out:
            {
                if (!justChangedState && isAnimFinished())
                {
                    changeState(State_Idle);
                }
            }
            break;

        case State_Death:
            {
                // nothing to do ATM...
            }
            break;
        }
    }

    void Ray_BossMorayBodyPart::changeState( const State _newState )
    {
        //ITF_ASSERT(m_state != _newState);
        m_state = _newState;

        switch (m_state)
        {
        case State_Idle:
            {
                playAnim(ITF_GET_STRINGID_CRC(Idle,1633200266));
            }
            break;

        case State_Scream:
            {
                playAnim(ITF_GET_STRINGID_CRC(Scream,2840079744));
            }
            break;

        case State_BuboSpawn:
            {
                playAnim(ITF_GET_STRINGID_CRC(BuboSpawn,2701842961));
            }
            break;

        case State_BuboHit:
            {
                playAnim(ITF_GET_STRINGID_CRC(BuboHit,2444450640));
            }
            break;

        case State_FireMissile:
            {
                playAnim(ITF_GET_STRINGID_CRC(FireMissile,2237533638));
            }
            break;

        case State_CloseRangeAttack:
            {
                playAnim(ITF_GET_STRINGID_CRC(CloseRangeAttack,3230415730));
            }
            break;

        case State_Spike:
            {
                playAnim(ITF_GET_STRINGID_CRC(Spike,2883205975));
            }
            break;

        case State_Dash_In:
            {
                playAnim(ITF_GET_STRINGID_CRC(Dash_In,3518691003));
                toggleBubo();   // turn on
            }
            break;

        case State_Dash_Loop:
            {
                playAnim(ITF_GET_STRINGID_CRC(Dash_Loop,899574037));
            }
            break;

        case State_Dash_Out:
            {
                playAnim(ITF_GET_STRINGID_CRC(Dash_Out,2936711012));
                toggleBubo();   // turn off
            }
            break;

        case State_Death:
            {
                playAnim(ITF_GET_STRINGID_CRC(Death,2142372323));
            }
            break;
        }
    }





    StringID Ray_BossMorayBodyPart::getCurrentBubo() const
    {
        if (!m_buboSpawner.isInitialized())
        {
            return StringID::Invalid;
        }

        const StringID& bubo = m_aiComponent->getCurrentBubo();
        if (!bubo.isValid())
        {
            return StringID::Invalid;
        }


        const StringIDList& bubos = getTemplate()->getBuboAttachBones();
        if (bubos.find(bubo) != -1)
        {
            return bubo;
        }

        return StringID::Invalid;
    }

    void Ray_BossMorayBodyPart::spawnBubo( const Vec3d& attachPos, const f32 attachAngle )
    {
        if ( Actor* bubo = m_buboSpawner.getSpawnee(getActor()->getScene(), attachPos, attachAngle) )
        {
            m_bubo = bubo->getRef();
        }
    }

    void Ray_BossMorayBodyPart::toggleBubo()
    {
        if (!getCurrentBubo().isValid())
        {
            return;
        }


        // triggering the bubo toggles it
        if ( Actor* bubo = m_bubo.getActor() )
        {
            EventTrigger trigger;
            trigger.setSender(getActorRef());
            trigger.setActivator(m_parent->getRef());
            trigger.setActivated(btrue);
            bubo->onEvent(&trigger);
        }
    }

    void Ray_BossMorayBodyPart::updateBubo()
    {
        StringID buboId = getCurrentBubo();
        if (!buboId.isValid())
        {
            return;
        }


        Vec3d attachPos;
        f32 attachAngle;
        getBoneCoord(buboId, attachPos, attachAngle);
        attachPos.m_z += 0.002f;    // bubos go in front of the body part
        attachAngle -= MTH_PIBY2;


        if ( Actor* bubo = m_bubo.getActor() )
        {
            bubo->setPos(attachPos);
            bubo->setAngle(attachAngle);
        }
        else
        {
            spawnBubo(attachPos, attachAngle);
        }
    }

    void Ray_BossMorayBodyPart::processBuboHit( Event* _event )
    {
        if (m_state != State_Death)
        {
            changeStateNextFrame(State_BuboHit);
        }

        dispatchMissiles(_event);
    }

    void Ray_BossMorayBodyPart::startDeath()
    {
        ITF_ASSERT(getActor());
        if (!getActor())
        {
            return;
        }

        AIUtils::disableCollision(getActor());

        if (DeformOnTrajectoryComponent* deformComponent = getActor()->GetComponent<DeformOnTrajectoryComponent>())
        {
            deformComponent->pause();
        }

        //changeStateNextFrame(State_Death);    // no need to go to death just yet: play the normal BuboHit, then go to death
    }


    void Ray_BossMorayBodyPart::processFireMissile( const Ray_EventBossMorayFireMissile* _eventFire )
    {
        if (!m_missileSpawner.isInitialized())
        {
            return;
        }

        m_missileSpawnRequested = btrue;
        m_missileSpawnRegionIndex = _eventFire->getBodyPartIndex();
        changeStateNextFrame(State_FireMissile);
    }

    void Ray_BossMorayBodyPart::updateMissileSpawner()
    {
        if (!m_missileSpawner.isInitialized())
        {
            return;
        }

        if (m_missileSpawnRequested)
        {
            m_missileSpawnRequested = bfalse;

            Vec3d spawnPos;
            f32 spawnAngle;
            getBoneCoord(getTemplate()->getMissileBone(), spawnPos, spawnAngle);
            //DebugDraw::line3D(spawnPos, spawnPos + Vec2d::Right.Rotate(spawnAngle).to3d() * 5, Color::white(), 10, 3);
            spawnPos.m_z += m_aiComponent->getTemplate()->getBodyPartsZSpacing() * (getBodyPartCount() * 2); // missiles go under ALL the body parts
            spawnAngle -= MTH_PIBY2;

            if ( Actor* missile = m_missileSpawner.getSpawnee(getActor()->getScene(), spawnPos, spawnAngle  ) )
            {
                m_missiles.push_back(missile->getRef());

                missile->enable();

                EventSetDirection eventDirection;
                eventDirection.setSender(m_parent->getRef());
                eventDirection.setDirection(Vec2d::Right.Rotate(spawnAngle));
                missile->onEvent(&eventDirection);


                if (Ray_SeekingJellyfishAIComponent* jelly = missile->GetComponent<Ray_SeekingJellyfishAIComponent>())
                {
                    if (PolyLine* region = m_aiComponent->getMissileRegion(m_missileSpawnRegionIndex)) 
                    {
                        jelly->setRegion(region->getRef());
                    }
                    else
                    {
                        jelly->setIgnoreRegions(btrue);
                    }
                }
            }
        }
    }

    void Ray_BossMorayBodyPart::dispatchMissiles( Event* _event )
    {
        for (u32 i = 0; i < m_missiles.size(); i++)
        {
            if ( Actor* missile = m_missiles[i].getActor() )
            {
                missile->onEvent(_event);
            }
        }

        // don't clear, we want to destroy all spawns on requestDestructions
        //m_missiles.clear();
    }



    void Ray_BossMorayBodyPart::processSpike( EventPolylineBlockingContact* _contact )
    {
        if (!isBody())
        {
            return;
        }

        PolyLine* poly = (PolyLine*)_contact->getPolyline().getObject();
        if ( !poly || poly->getOwnerActor() != getActor() )
        {
            return;
        }

        if (m_state != State_Spike)
        {
            changeStateNextFrame(State_Spike);
        }
    }



    void Ray_BossMorayBodyPart::checkCloseRangeAttack()
    {
        if ( !isHead() )
        {
            return;
        }

        if ( Actor* target = AIUtils::getClosestLivePlayer(getActor()) )
        {
            const f32 targetSqrDist = (target->get2DPos() - getActor()->get2DPos()).sqrnorm();
            if ( targetSqrDist < sqr(getTemplate()->getCloseRangeAttackDistance()) )
            {
                changeState(State_CloseRangeAttack);
            }
        }
    }




    void Ray_BossMorayBodyPart::processNodeReached( Ray_EventBossMorayNodeReached* _nodeReached )
    {
        if (_nodeReached->getStartDash())
        {
            changeStateNextFrame(State_Dash_In);
        }
        else if (_nodeReached->getStopDash())
        {
            changeStateNextFrame(State_Dash_Out);
        }
    }




    void Ray_BossMorayBodyPart::getBoneCoord( const StringID& _boneID, Vec3d& _pos, f32& _angle )
    {
        if (_boneID.isValid())
        {
            u32 boneIndex = getAnimComponent()->getBoneIndex(_boneID);
            if (boneIndex != U32_INVALID)
            {
                AnimBoneDyn* bone = getAnimComponent()->getBone(boneIndex);
                _pos = bone->m_Pos.to3d(getActor()->getDepth());
                _angle = bone->getAngle();
            }
        }
        else
        {
            _pos = getActor()->getPos();
            _angle = getActor()->getAngle();
        }
    }



    IMPLEMENT_OBJECT_RTTI(Ray_BossMorayBodyPart_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_BossMorayBodyPart_Template)
        SERIALIZE_MEMBER("buboPath", m_buboPath);
        SERIALIZE_MEMBER("buboAttachBones", m_buboAttachBones);
        SERIALIZE_MEMBER("missilePath", m_missilePath);
        SERIALIZE_MEMBER("missileBone", m_missileBone);
        SERIALIZE_MEMBER("closeRangeAttackDistance", m_closeRangeAttackDistance);
    END_SERIALIZATION()

}
