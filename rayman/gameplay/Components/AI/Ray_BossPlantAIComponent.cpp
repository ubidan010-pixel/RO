#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BOSSPLANTAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BossPlantAIComponent.h"
#endif //_ITF_RAY_BOSSPLANTAICOMPONENT_H_

#ifndef _ITF_RAY_BOSSPLANTNODECOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BossPlantNodeComponent.h"
#endif //_ITF_RAY_BOSSPLANTNODECOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_RAY_STIMCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_StimComponent.h"
#endif //_ITF_RAY_STIMCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifndef _ITF_WORLD_MANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLD_MANAGER_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_BossPlantAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_BossPlantAIComponent)
    END_SERIALIZATION()


    BEGIN_VALIDATE_COMPONENT(Ray_BossPlantAIComponent)
        //VALIDATE_COMPONENT_PARAM("", m_stimComponent, "Ray_BossPlantAIComponent requires a Ray_StimComponent");
    END_VALIDATE_COMPONENT()


    Ray_BossPlantAIComponent::Ray_BossPlantAIComponent()
        : Super()
        , m_stimComponent(NULL)
        , m_cycleIndex(0)
        , m_justStarted(bfalse)
        , m_currentNetworkIndex(-1)
        , m_playRateMultiplier(1.f)
    {
    }

    Ray_BossPlantAIComponent::~Ray_BossPlantAIComponent()
    {
    }

    void Ray_BossPlantAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_stimComponent = m_actor->GetComponent<Ray_StimComponent>();

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventSetInitialNode,3466697001), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301), this);

        clear();
    }

    void Ray_BossPlantAIComponent::onCheckpointLoaded()
    {
        Super::onCheckpointLoaded();

        clear();
    }

    void Ray_BossPlantAIComponent::clear()
    {
        m_initialNode.invalidate();
        m_currentNode.invalidate();
        m_nextNode.invalidate();
    }

    void Ray_BossPlantAIComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( Ray_EventSetInitialNode* nodeEvent = _event->DynamicCast<Ray_EventSetInitialNode>(ITF_GET_STRINGID_CRC(Ray_EventSetInitialNode,3466697001)) )
        {
            processActivateEvent(nodeEvent);
        }
        else if ( AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
        {
            shakeCamera(animEvent);
        }
    }

    void Ray_BossPlantAIComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        bbool justStarted = m_justStarted;
        m_justStarted = bfalse;

        // current gives us the order type, next gives us the destination
        Ray_BossPlantNodeComponent* current = getNode(m_currentNode);
        Ray_BossPlantNodeComponent* next = getNode(m_nextNode);

        if (!current)
        {
            return;
        }

#ifndef ITF_FINAL
        cheatCycleNodes(current, next);
#endif

        if (!next)
        {
            // no next node: loop forever
            ITF_ASSERT(justStarted || !m_animComponent->getIsLooped());
            if ( !justStarted && m_animComponent->isSubAnimFinished() )
            {
                if ( Ray_BossPlantNodeComponent* initial = getNode(m_initialNode) )
                {
                    // a new initial node was queued, play it now
                    setInitialNode(initial, btrue);
                    return;
                }

                m_animComponent->resetCurTime();
            }

            return;
        }



        // walk to next node
        ITF_ASSERT(justStarted || !m_animComponent->getIsLooped());
        if ( !justStarted && m_animComponent->isSubAnimFinished() )
        {
            if ( Ray_BossPlantNodeComponent* initial = getNode(m_initialNode) )
            {
                // a new initial node was queued, play it now
                setInitialNode(initial, btrue);
                return;
            }


            bbool destinationReached;
            if (current->isStationary())
            {
                destinationReached = btrue;
            }
            else
            {
                Vec2d vecToNext = next->GetActor()->get2DPos() - current->GetActor()->get2DPos();
                u32 cycleCount = current->getCycleCount(vecToNext);

                m_cycleIndex++;
                destinationReached = (m_cycleIndex >= cycleCount);
            }

            if (destinationReached)
            {
                setCurrentNode(next);
            }
            else
            {
                m_animComponent->resetCurTime();
            }
        }
        else
        {
            if ( !current->isStationary() )
            {
                // adjust playRate multiplier according to distance
                Actor* target = AIUtils::getLeadLivePlayer(m_actor);
                if (target)
                {
#ifndef ITF_FINAL
                    //DebugDraw::arrow2D(m_actor->get2DPos() + m_targetEvaluationOffset, target->get2DPos(), m_actor->getDepth());
#endif // ITF_FINAL

                    f32 targetPlayRateMultiplier(1.f);

                    f32 sqrDist = (target->get2DPos() - m_actor->get2DPos() - getTemplate()->getTargetEvaluationOffset()).sqrnorm();
                    if (sqrDist < getTemplate()->getPlayRateMultiplierMinDistance() * getTemplate()->getPlayRateMultiplierMinDistance())
                    {
                        // player is too close, slow down
                        targetPlayRateMultiplier = getTemplate()->getPlayRateMultiplierMinValue();
                    }
                    else if (sqrDist > getTemplate()->getPlayRateMultiplierMaxDistance() * getTemplate()->getPlayRateMultiplierMaxDistance())
                    {
                        // player is too far, accelerate
                        targetPlayRateMultiplier = getTemplate()->getPlayRateMultiplierMaxValue();
                    }

                    if (m_playRateMultiplier < targetPlayRateMultiplier)
                    {
                        m_playRateMultiplier += getTemplate()->getPlayRateMultiplierAcceleration() * _dt;
                        m_playRateMultiplier = Min(m_playRateMultiplier, targetPlayRateMultiplier);
                    }
                    else if (m_playRateMultiplier > targetPlayRateMultiplier)
                    {
                        m_playRateMultiplier -= getTemplate()->getPlayRateMultiplierDeceleration() * _dt;
                        m_playRateMultiplier = Max(m_playRateMultiplier, targetPlayRateMultiplier);
                    }

                    m_animComponent->setPlayRate(current->getAnimPlayRate() * m_playRateMultiplier);
                }

                Vec2d animPos = m_animComponent->getRootDeltaCumulated(btrue);
                const SubAnim* curAnim = m_animComponent->getCurSubAnim(0);
                if (curAnim->getIsReversed() && curAnim->getDefaultFlip())
                {
                    animPos = current->getCycleVector() + animPos;
                }
                else if (curAnim->getIsReversed())
                {
                    animPos = current->getCycleVector() - animPos;
                }
                else if (curAnim->getDefaultFlip())
                {
                    animPos *= -1;
                }

                m_actor->set2DPos(
                    current->GetActor()->get2DPos() +
                    current->getCycleVector() * (f32)m_cycleIndex +
                    animPos
                );
            }
            //else
            //{
            //    m_actor->set2DPos( current->GetActor()->get2DPos() );
            //}
        }

#ifndef ITF_FINAL
        //debugNodes();
#endif // ITF_FINAL
    }


    void Ray_BossPlantAIComponent::processActivateEvent( Ray_EventSetInitialNode* _nodeEvent )
    {
        if (_nodeEvent->getGetNodeFromSender())
        {
            Actor* sender = static_cast<ActorRef>(_nodeEvent->getSender()).getActor();
            LinkComponent* link = sender ? sender->GetComponent<LinkComponent>() : NULL;
            ITF_ASSERT(link);
            if (link)
            {
                const LinkComponent::ChildrenList& children = link->getChildren();
                u32 numChildren = children.size();
                for (u32 i = 0; i < numChildren; i++)
                {
                    if ( Ray_BossPlantNodeComponent* node = getNode(children[i].getPath()) )
                    {
                        setInitialNode(node, _nodeEvent->getInterruptCurrentNode());
                        return;
                    }
                }
            }
        }
        else
        {
            LinkComponent* link = m_actor->GetComponent<LinkComponent>();
            ITF_ASSERT(link);
            if (link)
            {
                FixedArray<Ray_BossPlantNodeComponent*, 8> nodes;

                const LinkComponent::ChildrenList& children = link->getChildren();
                u32 numChildren = children.size();
                for (u32 i = 0; i < numChildren; i++)
                {
                    if ( Ray_BossPlantNodeComponent* node = getNode(children[i].getPath()) )
                    {
                        nodes.push_back(node);
                    }
                }

                const u32 nodeCount = nodes.size();
                if (nodeCount)
                {
                    m_currentNetworkIndex++;
                    if (m_currentNetworkIndex >= (i32)nodeCount)
                    {
                        m_currentNetworkIndex = 0;
                    }

                    Ray_BossPlantNodeComponent* node = nodes[m_currentNetworkIndex];
                    ITF_WARNING(NULL, 0, "starting network %d - %s", m_currentNetworkIndex, node->GetActor()->getUserFriendly().cStr());
                    setInitialNode(node, _nodeEvent->getInterruptCurrentNode());
                    return;
                }
            }
        }

        ITF_WARNING(m_actor, 0, "could not get the initial node from the Ray_EventSetInitialNode");
    }

    void Ray_BossPlantAIComponent::setInitialNode( Ray_BossPlantNodeComponent* _node, bbool _doItNow )
    {
        if ( _doItNow || !getNode(m_currentNode) )
        {
            // start now
            m_initialNode.invalidate();
            m_playRateMultiplier = 1.f;
            setCurrentNode(_node);
        }
        else
        {
            // start on next isAnimFinished
            m_initialNode = _node->GetActor()->getRef();
        }
    }

    void Ray_BossPlantAIComponent::setCurrentNode( Ray_BossPlantNodeComponent* _node )
    {
        m_currentNode.invalidate();
        m_nextNode.invalidate();


        m_currentNode = (ActorRef)_node->GetActor()->getRef();

        if ( Ray_BossPlantNodeComponent* next = _node->getNextNode() )
        {
            m_nextNode = next->GetActor()->getRef();
        }

        startNode(_node);
    }

    void Ray_BossPlantAIComponent::startNode( Ray_BossPlantNodeComponent* _node )
    {
        _node->onNodeReached(m_actor);

        if (!_node->isStationary())
        {
            // warp to node
            m_actor->set2DPos(_node->GetActor()->get2DPos());
            m_actor->onForceMove();

            if (m_stimComponent)
            {
                // orient stim
                Vec2d cycleVector = _node->getCycleVector();
                f32 angle = cycleVector.normalize().getAngle();
                m_stimComponent->setFixedAngle( angle );
            }
        }

        m_cycleIndex = 0;

        if (_node->getAnim().isValid())
        {
            m_justStarted = btrue;
            m_animComponent->resetCurTime();
            m_animComponent->setAnim(_node->getAnim());
            m_animComponent->setPlayRate(_node->getAnimPlayRate());
        }
        else
        {
            // node with no anim (purely an event node) -> go to next node instantly
            if ( Ray_BossPlantNodeComponent* next = getNode(m_nextNode) )
            {
                setCurrentNode(next);
            }
        }
    }

    Ray_BossPlantNodeComponent* Ray_BossPlantAIComponent::getNode( ActorRef _nodeActor )
    {
        Actor* actor = _nodeActor.getActor();
        return actor ? actor->GetComponent<Ray_BossPlantNodeComponent>() : NULL;
    }

    Ray_BossPlantNodeComponent* Ray_BossPlantAIComponent::getNode( const ObjectPath& _objectPath )
    {
        if ( Pickable* child = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, _objectPath) )
        {
            if ( Actor* actorChild = child->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) )
            {
                if ( Ray_BossPlantNodeComponent* node = actorChild->GetComponent<Ray_BossPlantNodeComponent>() )
                {
                    return node;
                }
            }
        }

        return NULL;
    }

#ifndef ITF_FINAL
    void Ray_BossPlantAIComponent::debugNodes()
    {
#ifdef DEVELOPER_BEN
        Ray_BossPlantNodeComponent* current = getNode(m_currentNode);
        Ray_BossPlantNodeComponent* next = getNode(m_nextNode);

        String s;

        s.setTextFormat("current    %s", current ? current->GetActor()->getUserFriendly().cStr() : "-");
        GFX_ADAPTER->drawDBGText(s);
        s.setTextFormat("next       %s", next ? next->GetActor()->getUserFriendly().cStr() : "-");
        GFX_ADAPTER->drawDBGText(s);

        if (current)
        {
            s.setTextFormat("anim       %s", current->getAnim().getDebugString());
            GFX_ADAPTER->drawDBGText(s);

            s.setTextFormat("root pos   %.2f", m_animComponent->getRootDeltaCumulated(btrue).norm());
            GFX_ADAPTER->drawDBGText(s);

            s.setTextFormat("frame      %.2f", m_animComponent->getAnimFrameInfo()[0].m_fFrame);
            GFX_ADAPTER->drawDBGText(s);

            f32 targetDist = -1.f;
            if ( Actor* target = AIUtils::getLeadLivePlayer(m_actor) )
                targetDist = (target->get2DPos() - m_actor->get2DPos()).norm();
            s.setTextFormat("playRate   %.2f * %.2f     (target dist: %.2fm)", current->getAnimPlayRate(), m_playRateMultiplier, targetDist);
            GFX_ADAPTER->drawDBGText(s);
        }
#endif // DEVELOPER_BEN
    }
#endif // ITF_FINAL

    void Ray_BossPlantAIComponent::shakeCamera( AnimGameplayEvent* _animEvent )
    {
        if (_animEvent->getName() == MRK_Cam_Shake_00)
        {
            CAMERACONTROLLERMANAGER->addShake(ITF_GET_STRINGID_CRC(Quake0,2087111994));
        }
        else if (_animEvent->getName() == MRK_Cam_Shake_01)
        {
            CAMERACONTROLLERMANAGER->addShake(ITF_GET_STRINGID_CRC(Quake1,3041659572));
        }
        else if (_animEvent->getName() == MRK_Cam_Shake_02)
        {
            CAMERACONTROLLERMANAGER->addShake(ITF_GET_STRINGID_CRC(Quake2,3890011725));
        }
        else if (_animEvent->getName() == MRK_Cam_Shake_03)
        {
            CAMERACONTROLLERMANAGER->addShake(ITF_GET_STRINGID_CRC(Quake3,4080307277));
        }
    }

    void Ray_BossPlantAIComponent::cheatCycleNodes( Ray_BossPlantNodeComponent* _current,  Ray_BossPlantNodeComponent* _next )
    {
        InputAdapter::PressStatus buttons[JOY_MAX_BUT];
        INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentEngine, 0, buttons, JOY_MAX_BUT);

        if (buttons[m_joyButton_ThumbLeft] == InputAdapter::Pressed)
        {
            if (buttons[m_joyButton_DPadL] == InputAdapter::JustPressed)
            {
                // go to prev node if any
                if ( Ray_BossPlantNodeComponent* prev = _current->getPrevNode() )
                {
                    setCurrentNode(prev);
                }
            }
            else if (buttons[m_joyButton_DPadR] == InputAdapter::JustPressed)
            {
                // go to next node if any
                if (_next)
                {
                    setCurrentNode(_next);
                }
            }
        }
    }

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_BossPlantAIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_BossPlantAIComponent_Template)
        SERIALIZE_MEMBER("targetEvaluationOffset", m_targetEvaluationOffset);
        SERIALIZE_MEMBER("playRateMultiplierMinDistance", m_playRateMultiplierMinDistance);
        SERIALIZE_MEMBER("playRateMultiplierMaxDistance", m_playRateMultiplierMaxDistance);
        SERIALIZE_MEMBER("playRateMultiplierMinValue", m_playRateMultiplierMinValue);
        SERIALIZE_MEMBER("playRateMultiplierMaxValue", m_playRateMultiplierMaxValue);
        SERIALIZE_MEMBER("playRateMultiplierAcceleration", m_playRateMultiplierAcceleration);
        SERIALIZE_MEMBER("playRateMultiplierDeceleration", m_playRateMultiplierDeceleration);
    END_SERIALIZATION()

    Ray_BossPlantAIComponent_Template::Ray_BossPlantAIComponent_Template()
    : m_targetEvaluationOffset(Vec2d::Zero)
    , m_playRateMultiplierMinDistance(5.f)
    , m_playRateMultiplierMaxDistance(10.f)
    , m_playRateMultiplierMinValue(0.7f)
    , m_playRateMultiplierMaxValue(1.3f)
    , m_playRateMultiplierAcceleration(1.f)
    , m_playRateMultiplierDeceleration(1.f)
    {
    }
};