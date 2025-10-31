#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BOSSMORAYAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BossMorayAIComponent.h"
#endif //_ITF_RAY_BOSSMORAYAICOMPONENT_H_

#ifndef _ITF_RAY_BOSSMORAYBODYPART_H_
#include "rayman/gameplay/Components/AI/Ray_BossMorayBodyPart.h"
#endif //_ITF_RAY_BOSSMORAYBODYPART_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_RAY_AIUTILS_H_
#include "Rayman/gameplay/AI/Utils/Ray_AIUtils.h"
#endif //_ITF_RAY_AIUTILS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

#ifndef _ITF_TWEENCOMPONENT_H_
#include "gameplay/Components/Common/TweenComponent.h"
#endif //_ITF_TWEENCOMPONENT_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_BossMorayAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_BossMorayAIComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_CONTAINER_OBJECT("sequences", m_sequences);
            SERIALIZE_MEMBER("finalCinematic", m_finalCinematic);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS(Ray_BossMorayAIComponent, Sequence)
        SERIALIZE_MEMBER("tweenSet", m_tweenSet);
        SERIALIZE_MEMBER("buboId", m_buboId);
        SERIALIZE_MEMBER("objectToTriggerOnEnter", m_objectToTriggerOnEnter);
        SERIALIZE_MEMBER("objectToTriggerOnExit", m_objectToTriggerOnExit);
        SERIALIZE_CONTAINER("missileRegions", m_missileRegions);
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_BossMorayAIComponent)
    END_VALIDATE_COMPONENT()


    Ray_BossMorayAIComponent::Ray_BossMorayAIComponent()
        : m_tweenComponent(NULL)
        , m_sequenceIndex(U32_INVALID)
        , m_starting(bfalse)

        , m_state(State_Node)
        , m_timer(0.f)
        , m_retractTargetDist(0.f)
        , m_retractDist(0.f)

        , m_wasInWater(bfalse)
    {
    }

    Ray_BossMorayAIComponent::~Ray_BossMorayAIComponent()
    {
    }

    void Ray_BossMorayAIComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);

        m_tweenComponent = m_actor->GetComponent<TweenComponent>();

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventBossBubonHit,1018185172), this);

        if (isRealBoss())
        {
            AIUtils::setAlwaysActive(m_actor, btrue);
        }

        m_trajectoryLengthMultiplier = 10.f;    // for retract (backpedal)

        if ( !getTemplate()->getSplashFX().isEmpty() )
        {
            SPAWNER->declareNeedsSpawnee(m_actor, &m_splashFX, getTemplate()->getSplashFX());
        }

        m_wasInWater = checkIsInWater();
    }

    /*
        0) We wait for a trigger
        1) We start a sequence (network)
            - trigger some stuff
            - start a tweening
        2) When the bubo is hit:
            - accelerate
            - trigger stuff
                - we get redirected to the closest exit
            - stop the current sequence
                - stop the tweening
        3) When the exit is reached, it sends us to a new network
            - start the new sequence -> see 1)
    */

    void Ray_BossMorayAIComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( _event->GetObjectClassCRC() == ITF_GET_STRINGID_CRC(EventTrigger,1343042510) )   // must be an exact match because Ray_EventNodeReached inherits from EventTrigger...
        {
            proccessTrigger();
        }
        if ( _event->IsClassCRC(ITF_GET_STRINGID_CRC(EventBossBubonHit,1018185172)) )
        {
            processBuboHit();
        }
    }

    void Ray_BossMorayAIComponent::proccessTrigger()
    {
        if (m_starting)
        {
            return;
        }

        m_sequenceIndex++;
        playMusic(m_sequenceIndex);

        if (m_sequenceIndex >= m_sequences.size())
        {
            return;
        }

        startCurrentSequence(bfalse);
    }

    void Ray_BossMorayAIComponent::processBuboHit()
    {
        stopCurrentSequence();

        if (m_sequenceIndex == m_sequences.size() - 1)
        {
            startDeath();
            playMusic(m_sequenceIndex + 1);
        }
    }


    void Ray_BossMorayAIComponent::startCurrentSequence( const bbool _trigger )
    {
        if (m_starting)
        {
            return;
        }
        m_starting = btrue;


        const Sequence& sequence = m_sequences[m_sequenceIndex];

        m_currentSpeedMultiplier = m_targetSpeedMultiplier = 1.f;
        m_currentSpeed = 0.f;

        if (sequence.m_tweenSet.isValid())
        {
            m_tweenComponent->playSet(sequence.m_tweenSet);
        }

        if (_trigger)
        {
            triggerObject(sequence.m_objectToTriggerOnEnter);
        }

        disableCollision(bfalse);

        if (m_state != State_Node)
        {
            changeState(State_Node);
        }


        m_starting = bfalse;
    }

    void Ray_BossMorayAIComponent::stopCurrentSequence()
    {
        const Sequence& sequence = m_sequences[m_sequenceIndex];

        m_tweenComponent->stopCurrentSet();

        triggerObject(sequence.m_objectToTriggerOnExit);

        disableCollision(btrue);

        changeState(State_NodeHit);
    }



    void Ray_BossMorayAIComponent::startDeath()
    {
        for (u32 i = 0; i < m_bodyParts.size(); i++)
        {
            if ( Ray_BossMorayBodyPart* part = m_bodyParts[i]->DynamicCast<Ray_BossMorayBodyPart>(ITF_GET_STRINGID_CRC(Ray_BossMorayBodyPart,4134625562)) )
            {
                part->startDeath();
            }
        }

        changeState(State_ShrinkAndShake);
    }


    const StringID& Ray_BossMorayAIComponent::getCurrentBubo()
    {
        if (m_sequenceIndex >= m_sequences.size())
        {
            return StringID::Invalid;
        }

        return m_sequences[m_sequenceIndex].m_buboId;
    }

    PolyLine* Ray_BossMorayAIComponent::getMissileRegion( u32 _regionIndex )
    {
        if (m_sequenceIndex >= m_sequences.size())
        {
            return NULL;
        }

        const Sequence& sequence = m_sequences[m_sequenceIndex];
        if (_regionIndex >= sequence.m_missileRegions.size())
        {
            return NULL;
        }

        Pickable* pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, sequence.m_missileRegions[_regionIndex]);
        if (!pickable)
        {
            return NULL;
        }

        if ( Frise* region = pickable->DynamicCast<Frise>(ITF_GET_STRINGID_CRC(Frise,2579114664)) )
        {
            return (PolyLine*)region->getVisualPoly().getObject();
        }

        return NULL;
    }


    void Ray_BossMorayAIComponent::triggerObject( const ObjectPath& _objectPath )
    {
        if ( Pickable* objectToTrigger = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, _objectPath) )
        {
            EventTrigger trigger;
            trigger.setSender(m_actor->getRef());
            trigger.setActivator(m_actor->getRef());
            trigger.setActivated(btrue);
            objectToTrigger->onEvent(&trigger);
        }
    }

    void Ray_BossMorayAIComponent::disableCollision( const bbool _disable )
    {
        for (u32 i = 0; i < m_bodyParts.size(); i++)
        {
            if ( BodyPart* bodyPart = m_bodyParts[i]->DynamicCast<BodyPart>(ITF_GET_STRINGID_CRC(BodyPart,3043470114)) )
            {
                if ( Actor* bodyPartActor = bodyPart->getActorRef().getActor() )
                {
                    AIUtils::disableCollision(bodyPartActor, _disable);
                }
            }
        }
    }

    Ray_BossMorayBodyPart* Ray_BossMorayAIComponent::getHead()
    {
        return m_bodyParts[0]->DynamicCast<Ray_BossMorayBodyPart>(ITF_GET_STRINGID_CRC(Ray_BossMorayBodyPart,4134625562));
    }



    void Ray_BossMorayAIComponent::Update( f32 _dt )
    {
        switch (m_state)
        {
        case State_Node:
            {
                Super::Update(_dt);
                updateSplashFX();
            }
            break;

        case State_NodeHit: // just got hit and fleeing to exit
            {
                if (getHead()->isPlayingHitAnim())
                {
                    // stop
                    m_currentSpeedMultiplier = m_targetSpeedMultiplier = 0.f;
                }
                else
                {
                    // flee to exit
                    m_targetSpeedMultiplier = 1.f;
                    m_targetSpeed = getTemplate()->getSpeedOnBuboHit();
                    m_acceleration = getTemplate()->getAccelerationOnBuboHit();
                }

                Super::Update(_dt);
                updateSplashFX();
            }
            break;

        case State_ShrinkAndShake:
            {
                updateShrinkAndShake(_dt);
            }
            break;

        case State_Retract:
            {
                updateRetract(_dt);
            }
            break;

        case State_FinalCine:
            {
                // nothing to do
            }
            break;
        }


#ifdef ITF_SUPPORT_DEBUGFEATURE
        cheatCycleSequence();
#endif // ITF_SUPPORT_DEBUGFEATURE
    }

    void Ray_BossMorayAIComponent::changeState( State _newState )
    {
        ITF_ASSERT(m_state != _newState);
        m_state = _newState;

        switch (m_state)
        {
        case State_ShrinkAndShake:
        case State_Retract:
            {
                // stop
                m_currentSpeedMultiplier = m_targetSpeedMultiplier = 0.f;
                m_currentSpeed = m_targetSpeed = 0.f;
            }
            break;

        case State_FinalCine:
            {
                if ( Pickable* cine = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, m_finalCinematic) )
                {
                    if ( Actor* cineActor = cine->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) )
                    {
                        Ray_AIUtils::showBossMedal(cineActor->getRef(), m_actor->getRef());
                    }
                }

                // disable just in case: the boss is supposed to be out of view, and we should never ever play the cine twice!
                m_actor->disable();
            }
            break;
        }
    }


    void Ray_BossMorayAIComponent::updateShrinkAndShake( const f32 _dt )
    {
        if ( !validateBodyParts(_dt) )
        {
            m_enclosingPolyline.deactivate();
            return;
        }


        // shrink & shake the body a bit

        m_timer = Min(m_timer + _dt, getTemplate()->getShrinkDuration());
        const f32 shrinkRatio = m_timer / getTemplate()->getShrinkDuration();
        const f32 shrinkSin = f32_Sin(shrinkRatio * MTH_PIBY2);  // 0 -> 1
        f32 shrinkDist = 0.f;

        // shrink
        for (u32 i = 1; i < m_bodyParts.size()-1; i++)
        {
            BodyPartBase* bodyPart = m_bodyParts[i];;

            const f32 initialSize = bodyPart->getTemplate()->getLengthOnTrajectory();
            const f32 size = Interpolate(initialSize, getTemplate()->getShrinkSize(), shrinkSin);

            bodyPart->setLengthOnTrajectory(size);
            shrinkDist += (initialSize - size);
        }


        positionBodyPartsOnTrajectory(shrinkDist);
        m_retractTargetDist = m_retractDist = shrinkDist;


        // shake
        for (u32 i = 0; i < m_bodyParts.size()-1; i++)
        {
            Ray_BossMorayBodyPart* bodyPart = m_bodyParts[i]->DynamicCast<Ray_BossMorayBodyPart>(ITF_GET_STRINGID_CRC(Ray_BossMorayBodyPart,4134625562));
            if (!bodyPart)
            {
                continue;
            }

            const f32 phase = (f32)(i) / (f32)(m_bodyParts.size()) * MTH_2PI;
            const f32 offset = sinf(phase + m_timer * getTemplate()->getShakeFrequency()) * getTemplate()->getShakeAmplitude();

            Actor* bodyPartActor = bodyPart->getActorRef().getActor();
            const Vec2d dir = AIUtils::getLookDir(bodyPartActor);
            bodyPartActor->set2DPos(bodyPartActor->get2DPos() + dir * offset);
        }


        if (m_timer >= getTemplate()->getShrinkDuration())
        {
            m_timer = 0.f;
            changeState(State_Retract);
        }
    }

    void Ray_BossMorayAIComponent::updateRetract( const f32 _dt )
    {
        if ( !validateBodyParts(_dt) )
        {
            m_enclosingPolyline.deactivate();
            return;
        }


        // wait a bit before retracting
        if (m_timer < getTemplate()->getRetractDelay())
        {
            m_timer += _dt;
            return;
        }


        // retract to the last node + offset
        const f32 finalDist = m_currentNodeDist + getTemplate()->getRetractOffset();
        m_retractTargetDist += (finalDist - m_retractTargetDist) * getTemplate()->getRetractTargetSmooth();
        m_retractDist += (m_retractTargetDist - m_retractDist) * getTemplate()->getRetractSmooth();
        positionBodyPartsOnTrajectory(m_retractDist);


        if (fabs(m_retractDist - finalDist) < getTemplate()->getRetractFinishedLimit())
        {
            // we're done :) !
            changeState(State_FinalCine);
        }
    }


    void Ray_BossMorayAIComponent::updateSplashFX()
    {
        if ( getTemplate()->getSplashFX().isEmpty() )
        {
            return;
        }

        bbool isInWater = checkIsInWater();

        if ( !m_wasInWater && isInWater )
        {
            Actor* splashFX = m_splashFX.getSpawnee(m_actor->getScene(),m_actor->getPos());

            if ( splashFX )
            {
                splashFX->setAngle(m_actor->getAngle());
                splashFX->setScale(m_actor->getScale());
            }
        }

        m_wasInWater = isInWater;
    }

    bbool Ray_BossMorayAIComponent::checkIsInWater() const
    {
        AIUtils::PolylineRegionList regionsList;

        AIUtils::getPolylineRegions(m_actor->getDepth(),m_actor->get2DPos(),regionsList);

        u32 numRegions = regionsList.size();

        for ( u32 i = 0; i < numRegions; i++ )
        {
            AIUtils::PolylineRegionData& data = regionsList[i];

            if ( data.m_regionId == s_RegionName_Swim && data.m_polyline )
            {
                return btrue;
            }
        }

        return bfalse;
    }


    void Ray_BossMorayAIComponent::playMusic( const u32 _sequenceIndex )
    {
        if (_sequenceIndex < getTemplate()->getMusics().size())
        {
            EventPlayMusic& music = const_cast<EventPlayMusic&>(getTemplate()->getMusics()[_sequenceIndex]);
            EVENTMANAGER->broadcastEvent(&music);
        }
    }



#ifdef ITF_SUPPORT_DEBUGFEATURE
    void Ray_BossMorayAIComponent::cheatCycleSequence()
    {
        if (!isRealBoss())
        {
            return;
        }

        InputAdapter::PressStatus buttons[JOY_MAX_BUT];
        INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentEngine, 0, buttons, JOY_MAX_BUT);

        if (buttons[m_joyButton_ThumbLeft] == InputAdapter::Pressed)
        {
            if (buttons[m_joyButton_DPadL] == InputAdapter::JustPressed)
            {
                // go to prev sequence
                stopCurrentSequence();

                m_sequenceIndex--;
                if (m_sequenceIndex >= m_sequences.size())
                {
                    m_sequenceIndex = m_sequences.size()-1;
                }

                startCurrentSequence(btrue);

                // force dispatch missiles
                EventBossBubonHit event;
                event.setSender(m_actor->getRef());
                for (u32 i = 0; i < m_bodyParts.size(); i++)
                {
                    m_bodyParts[i]->onEvent(&event);
                }

                String s;s.setTextFormat("go to seq %d / %d", m_sequenceIndex+1, m_sequences.size());
                DebugInfo::get().addDebugInfoDisplay(s, 1.f);
            }
            else if (buttons[m_joyButton_DPadR] == InputAdapter::JustPressed)
            {
                // go to next sequence
                stopCurrentSequence();

                m_sequenceIndex++;
                if (m_sequenceIndex >= m_sequences.size())
                {
                    m_sequenceIndex = 0;
                }

                startCurrentSequence(btrue);

                // force dispatch missiles
                EventBossBubonHit event;
                event.setSender(m_actor->getRef());
                for (u32 i = 0; i < m_bodyParts.size(); i++)
                {
                    m_bodyParts[i]->onEvent(&event);
                }

                String s;s.setTextFormat("go to seq %d / %d", m_sequenceIndex+1, m_sequences.size());
                DebugInfo::get().addDebugInfoDisplay(s, 1.f);
            }

        }
    }
#endif // ITF_SUPPORT_DEBUGFEATURE


    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_BossMorayAIComponent_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_BossMorayAIComponent_Template)
        SERIALIZE_MEMBER("speedOnBuboHit", m_speedOnBuboHit);
        SERIALIZE_MEMBER("accelerationOnBuboHit", m_accelerationOnBuboHit);

        SERIALIZE_MEMBER("shrinkDuration", m_shrinkDuration);
        SERIALIZE_MEMBER("shrinkSize", m_shrinkSize);
        SERIALIZE_MEMBER("shakeFrequency", m_shakeFrequency);
        SERIALIZE_MEMBER("shakeAmplitude", m_shakeAmplitude);

        SERIALIZE_MEMBER("retractDelay", m_retractDelay);
        SERIALIZE_MEMBER("retractSpeed", m_retractSpeed);
        SERIALIZE_MEMBER("retractTargetSmooth", m_retractTargetSmooth);
        SERIALIZE_MEMBER("retractSmooth", m_retractSmooth);
        SERIALIZE_MEMBER("retractOffset", m_retractOffset);
        SERIALIZE_MEMBER("retractFinishedLimit", m_retractFinishedLimit);

        SERIALIZE_MEMBER("splashFX", m_splashFX);
        SERIALIZE_CONTAINER_OBJECT("musics", m_musics);
    END_SERIALIZATION()

};
