#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_HUNTERBOSSAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Shooter/Ray_HunterBossAIComponent.h"
#endif //_ITF_RAY_HUNTERBOSSAICOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_RAY_AISHOOTERATTACKBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Shooter/Ray_AIShooterAttackBehavior.h"
#endif //_ITF_RAY_AISHOOTERATTACKBEHAVIOR_H_

#ifndef _ITF_RAY_AIRECEIVEHITBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ray_AIReceiveHitBehavior.h"
#endif //_ITF_RAY_AIRECEIVEHITBEHAVIOR_H_

#ifndef _ITF_RAY_AIDEATHBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ray_AIDeathBehavior.h"
#endif //_ITF_RAY_AIDEATHBEHAVIOR_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_RAY_AIGROUNDATTACKBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundAttackBehavior.h"
#endif //_ITF_RAY_AIGROUNDATTACKBEHAVIOR_H_

#ifndef _ITF_RAY_AISHOOTERCLOSEATTACKBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Shooter/Ray_AIShooterCloseAttackBehavior.h"
#endif //_ITF_RAY_AISHOOTERCLOSEATTACKBEHAVIOR_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_AIHUNTERATTACKBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Shooter/Ray_AIHunterAttackBehavior.h"
#endif //_ITF_RAY_AIHUNTERATTACKBEHAVIOR_H_

#ifndef _ITF_RAY_HUNTERBOSSNODECOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_HunterBossNodeComponent.h"
#endif //_ITF_RAY_HUNTERBOSSNODECOMPONENT_H_

#ifndef _ITF_WORLD_MANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLD_MANAGER_H_

#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#include "rayman/gameplay/Ray_GameplayEvents.h"
#endif //_ITF_RAY_GAMEPLAYEVENTS_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_HunterBossAIComponent)
    BEGIN_SERIALIZATION_CHILD(Ray_HunterBossAIComponent)
    END_SERIALIZATION()

    Ray_HunterBossAIComponent::Ray_HunterBossAIComponent() 
        : Super()
        , m_idleBehavior(NULL)
        , m_attackBehavior(NULL)
        , m_holeAttackBehavior(NULL)
        , m_currentAttackBehavior(NULL)
        , m_receiveHitBehavior(NULL)
        , m_deathBehavior(NULL)
        , m_closeRangeBehavior(NULL)
        , m_crushedBehavior(NULL)
        , m_voidBehavior(NULL)
        , m_physComponent(NULL)
        , m_nodeComponent(NULL)
        , m_currentNode(U32_INVALID)
        , m_isHole(bfalse)
        , m_healthCounter(0)
    {
    }

    Ray_HunterBossAIComponent::~Ray_HunterBossAIComponent()
    {
        clear();
    }

    void Ray_HunterBossAIComponent::clear()
    {
        m_children.clear();
        m_currentNode = U32_INVALID;
        m_healthCounter = 0;
    }

    void Ray_HunterBossAIComponent::onCheckpointLoaded()
    {
        Super::onCheckpointLoaded();
    }

    void Ray_HunterBossAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_idleBehavior = createAiBehavior(getTemplate()->getIdleBehavior());
        m_attackBehavior = static_cast<Ray_AIHunterAttackBehavior *>(createAiBehavior(getTemplate()->getAttackBehavior()));
        m_holeAttackBehavior = static_cast<Ray_AIHunterAttackBehavior *>(createAiBehavior(getTemplate()->getHoleAttackBehavior()));
        m_receiveHitBehavior = static_cast<AIReceiveHitBehavior *>(createAiBehavior(getTemplate()->getReceiveHitBehavior()));
        m_deathBehavior = static_cast<Ray_AIDeathBehavior *>(createAiBehavior(getTemplate()->getDeathBehavior()));
        m_closeRangeBehavior = static_cast<Ray_AIShooterCloseAttackBehavior *>(createAiBehavior(getTemplate()->getCloseRangeBehavior()));
        m_crushedBehavior = createAiBehavior(getTemplate()->getCrushedBehavior());
        m_voidBehavior = createAiBehavior(getTemplate()->getVoidBehavior());

        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(PunchStim,200533519), this);
        if (m_crushedBehavior)
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608), this);
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventCrushed,3831882623), this);
        }
        m_nodeComponent = m_actor->GetComponent<Ray_HunterBossNodeComponent>();
        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();


        //build child list
        const LinkComponent::ChildrenList& children = m_nodeComponent->getChildren();
        u32 numChildren = children.size();
        m_children.reserve(getTemplate()->getMaxNodes());
        for (u32 i = 0; i < getTemplate()->getMaxNodes(); ++i)
        {
            m_children.push_back(ActorRef());
        }
        for (u32 i = 0; i < numChildren; ++i)
        {
            Pickable* child = SceneObjectPathUtils::getObjectFromRelativePath(GetActor(),children[i].getPath());
            if (child)
            {
                Ray_EventQueryHunterNode query;
                child->onEvent(&query);
                ITF_ASSERT(query.m_index < getTemplate()->getMaxNodes());
                {
                    m_children[query.m_index] = child->getRef();
                }
            }
        }
        if (numChildren)
            m_currentNode = 0;

        startVoid();
    }

    void Ray_HunterBossAIComponent::startIdle()
    {
        //choose node

        if (m_currentNode != U32_INVALID)
        {
            Actor * node = m_children[m_currentNode].getActor();
            ITF_ASSERT(node);

            node->onEvent(&m_nodeData);

            EventTeleport teleport(node->getPos(),node->getAngle(),btrue);
            teleport.setFlip(m_nodeData.m_flip);
            m_actor->onEvent(&teleport);
            m_isHole = m_nodeData.m_isHole;
            if (m_isHole)
            {
                m_physComponent->setDisabled(btrue);
                m_currentAttackBehavior = m_holeAttackBehavior;
            }
            else
            {
                m_physComponent->setDisabled(bfalse);
                m_currentAttackBehavior = m_attackBehavior;
            }
            m_currentAttackBehavior->setData(m_nodeData.m_numBulletsPerCycle,m_nodeData.m_numCycles);
            m_finishRadius = m_nodeData.m_finishRadius;
        }
        setBehavior(m_idleBehavior);
    }

    void Ray_HunterBossAIComponent::startVoid()
    {
        m_healthCounter = 0;
        unregisterObjects();
        // unregister the phantom
        m_phantomComponent->setDisabled(btrue);
        getNextNode();
        setBehavior(m_voidBehavior);
    }

    void Ray_HunterBossAIComponent::Update( f32 _deltaTime )
    {
//update aabb
        AABB aabb(m_actor->get2DPos());
        for (u32 i =0 ; i< m_children.size(); ++i)
        {
            Actor * actor = m_children[i].getActor();
            if (actor)
                aabb.grow(actor->getPos());
        }
        m_actor->growRelativeAABBFromAbsolute(aabb);

        Super::Update(_deltaTime);

        if ( getHealth() <= 0 )
        {
            if (m_currentBehavior != m_deathBehavior)
            {
                // die!
                setBehavior(m_deathBehavior);
                unregisterObjects();    // if the object is recycled, will re-register on unpause
            }
            return;
        }

        if (m_healthCounter >= getTemplate()->getHealthBeforeChange())
        {
            startVoid();
            return;
        }

        if (m_currentBehavior == m_receiveHitBehavior)
        {
            // nothing to do...
            updateIdle(_deltaTime);
        }
        else if ( m_crushedBehavior && m_currentBehavior == m_crushedBehavior )
        {
            // nothing to do ATM...
        }
        else if (m_currentBehavior == m_currentAttackBehavior)
        {
            updateAttack(_deltaTime);

        }
        else if (m_currentBehavior == m_closeRangeBehavior)
        {
            updateCloseRange(_deltaTime);
        }
        else if (m_currentBehavior == m_idleBehavior)
        {
            updateIdle(_deltaTime);
        }
    }

    ObjectRef Ray_HunterBossAIComponent::findEnemy(bbool & _closeRange) const
    {

        ObjectRef enemy = ITF_INVALID_OBJREF;
        if (getTemplate()->getUseRadius())
        {
            

            Actor * actor =  getClosestEnemy(getTemplate()->getFindEnemyRadius());
            if (actor)
            {
                if ((actor->get2DPos() - m_actor->get2DPos()).sqrnorm() < getTemplate()->getCloseRadius()*getTemplate()->getCloseRadius())
                    _closeRange = btrue;
                else
                    _closeRange = bfalse;
                enemy = actor->getRef();
            }

        }
        else
        {
            Actor * closeActor =  getClosestEnemy(getTemplate()->getDetectionCloseRange());
            if (closeActor)
            {
                _closeRange = btrue;
                enemy = closeActor->getRef();
            }
            else
            {
                Actor * actor =  getClosestEnemy(getTemplate()->getDetectionRange());
                if (actor)
                {
                    _closeRange = bfalse;
                    enemy = actor->getRef();
                }
            }

        }
        return enemy;
    }

    void Ray_HunterBossAIComponent::onEvent( Event* _event )
    {
        if ( PunchStim* hit = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)) )
        {
            processHit(hit);
        }
        else if ( EventInteractionQuery* interactionQuery = _event->DynamicCast<EventInteractionQuery>(ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608)) )
        {
            processInteractionQuery(interactionQuery);
        }
        else if ( EventCrushed* crushed = _event->DynamicCast<EventCrushed>(ITF_GET_STRINGID_CRC(EventCrushed,3831882623)) )
        {
            processCrushed(crushed);
        }
        Super::onEvent(_event);
    }

    void Ray_HunterBossAIComponent::processHit( PunchStim* _hit )
    {
        if ( getHealth() <= 0 )
            return;

        if ( !m_receiveHitBehavior->canReceiveHit(_hit))
            return;


        // notify the sender that he touched us
        Actor* sender = GETOBJECT(_hit->getSender())->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
        if (sender)
        {
            EventHitSuccessful hitSuccessful;
            hitSuccessful .setSender(m_actor->getRef());
            sender->onEvent(&hitSuccessful);
        }

        u32 health = getHealth();
        setBehavior(m_receiveHitBehavior);
        m_receiveHitBehavior->setData(_hit);
        u32 newHealth = getHealth();
        if (newHealth > 0)
        {
            m_healthCounter += (health-newHealth);
        }
    }

    void Ray_HunterBossAIComponent::onBehaviorFinished()
    {
        Super::onBehaviorFinished();

        if (m_currentBehavior == m_receiveHitBehavior || m_currentBehavior == m_currentAttackBehavior|| (m_closeRangeBehavior && m_currentBehavior == m_closeRangeBehavior))
        {
            // let Update decide what to do next
            if (m_isHole || (m_healthCounter >= getTemplate()->getHealthBeforeChange() && !m_isHole))
                startVoid();
        }
        else if ( m_currentBehavior == m_crushedBehavior )
        {
            onFinishedCrush();
        }
        else if (m_currentBehavior == m_voidBehavior)
        {
            onFinishedVoid();
        }
    }

    void Ray_HunterBossAIComponent::updateAnimInput()
    {
        Super::updateAnimInput();

        PhysActorForceArray windForces;
        Vec2d windForce = Vec2d::Zero;

        PHYSWORLD->getWindForces(m_actor->get2DPos(), Vec2d::Zero, m_actor->getDepth(), windForces);

        for ( u32 i = 0; i < windForces.size(); i++ )
        {
            windForce += windForces[i].m_force;
        }

        f32 force = windForce.dot(Vec2d::Right.Rotate(m_actor->isFlipped()?MTH_PI-m_actor->getAngle():m_actor->getAngle()));
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(windForce,2257032332),force);
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(isHole,3312447968),m_isHole);

    }

    void Ray_HunterBossAIComponent::startCrushed()
    {
        setBehavior(m_crushedBehavior);
    }
    void Ray_HunterBossAIComponent::processInteractionQuery( EventInteractionQuery* interactionQuery )
    {
        if ( getHealth() <= 0 )
        {
            ITF_ASSERT_MSG(0, "dead AIs should be unregistered!");
            return;
        }

        if ( RAY_GAMEMANAGER->getAllowNpcCrush() )
        {
            if ( m_currentBehavior != m_crushedBehavior )
            {
                interactionQuery->setInteraction(CharacterInteractionType_Crush);
            }
        }
    }
    void Ray_HunterBossAIComponent::processCrushed( EventCrushed* crushed )
    {
        ITF_ASSERT_MSG(m_crushedBehavior, "got crushed but we don't have a crushed behavior!");
        if (m_crushedBehavior)
        {
            receiveCrush(crushed);
            playLastHitFx();

            if ( getHealth() <= 0 )
            {
                // we're dead
                unregisterObjects();

                // unregister the phantom
                if ( m_phantomComponent )
                {
                    m_phantomComponent->setDisabled(btrue);
                }

                // Once the death behavior is done, we should pause the actor;
                // if the object is recycled, everything should re-register on unpause.
                // TODO: move all this crap to Ray_AIReceiveHitBehavior or Ray_AIComponent...
                // TODO: use an EventDead/EventReset or something?
            }
            startCrushed();
        }
    }
    void Ray_HunterBossAIComponent::onFinishedCrush()
    {
        startVoid();
    }

    Actor * Ray_HunterBossAIComponent::getClosestEnemy(f32 _radius) const
    {
        ActorList players;
        AIUtils::getPlayers( m_actor->getDepth(), players );
        f32 closest = F32_INFINITY;
        const f32 sqrRadius = _radius * _radius;
        Actor * closestActor = NULL;
        const Vec2d pos = m_actor->get2DPos();
        for (u32 i = 0; i < players.size(); i++)
        {
            Actor* player = players[i];
            f32 sqrdist = (player->get2DPos() - pos).sqrnorm() ;
            if ((sqrdist < closest) && (sqrdist < sqrRadius))
            {
                closestActor = player;
            }
        }
        return closestActor;
    }
    Actor * Ray_HunterBossAIComponent::getClosestEnemy(const AABB & _range) const
    {
        ActorList players;
        AIUtils::getPlayers( m_actor->getDepth(), players );
        f32 closest = F32_INFINITY;
        Actor * closestActor = NULL;
        const Vec2d pos = m_actor->get2DPos();
        const AABB aabb = AIUtils::getAbsoluteAABB(_range, pos, m_actor->isFlipped());
        for (u32 i = 0; i < players.size(); i++)
        {
            Actor* player = players[i];
            if (aabb.contains(player->get2DPos()))
            {
                f32 sqrdist = (player->get2DPos() - pos).sqrnorm() ;
                if (sqrdist < closest) 
                {
                    closestActor = player;
                }
            }
        }
        return closestActor;
    }

    void Ray_HunterBossAIComponent::getNextNode()
    {
        if (m_currentNode == U32_INVALID)
            return;
        u32 node = m_currentNode;
        m_currentNode = (m_currentNode+1) % getTemplate()->getMaxNodes();
        Actor * actor = m_children[m_currentNode].getActor();
        while (!actor)
        {
            m_currentNode = (m_currentNode+1) % getTemplate()->getMaxNodes();
            actor = m_children[m_currentNode].getActor();
            if (node == m_currentNode)
            {
                m_currentNode = U32_INVALID;
                return;
            }
        }
    }

    void Ray_HunterBossAIComponent::updateAttack(f32 _dt)
    {
        bbool closeRange = bfalse;
        ObjectRef enemy = findEnemy(closeRange);
        if (enemy != ITF_INVALID_OBJREF)
        {
            if (closeRange && m_closeRangeBehavior)
            {
                if (m_isHole)
                {
                    startVoid();
                }
                else
                {
                    m_closeRangeBehavior->setVictim(enemy);
                    setBehavior(m_closeRangeBehavior);            
                }
            }
        }
        else
        {
            startVoid();
        }
    }

    void Ray_HunterBossAIComponent::updateCloseRange(f32 _dt)
    {
        bbool closeRange = bfalse;
        ObjectRef enemy = findEnemy(closeRange);
        if (enemy != ITF_INVALID_OBJREF)
        {
            if (!closeRange)
            {
                startAttack(m_currentAttackBehavior,enemy);
            }
            else
            {
                if (m_isHole)
                {
                    startVoid();
                }
            }
        }
        else
        {
            startVoid();
        }
    }

    void Ray_HunterBossAIComponent::updateIdle(f32 _dt)
    {
        bbool closeRange = bfalse;
        ObjectRef enemy = findEnemy(closeRange);
        if (enemy != ITF_INVALID_OBJREF)
        {
            if (closeRange && m_closeRangeBehavior)
            {
                if (m_isHole)
                    startVoid();
                else
                {
                    m_closeRangeBehavior->setVictim(enemy);
                    setBehavior(m_closeRangeBehavior);            
                }
            }
            else
            {
                startAttack(m_currentAttackBehavior,enemy);
            }
        }
    }

    void Ray_HunterBossAIComponent::onFinishedVoid()
    {
        registerObjects();

        m_phantomComponent->setDisabled(bfalse);

        startIdle();
    }

    void Ray_HunterBossAIComponent::startAttack(Ray_AIHunterAttackBehavior * _attack, ObjectRef _victim)
    {
        if (_attack)
        {
            _attack->setVictim(_victim);
            setBehavior(_attack);            
        }
    }

#ifdef ITF_SUPPORT_EDITOR
    void Ray_HunterBossAIComponent::drawEdit( ActorDrawEditInterface* _drawInterface , u32 _flags) const
    {
        Super::drawEdit(_drawInterface,_flags);

        if (getTemplate()->getUseRadius())
        {
            GFX_ADAPTER->drawDBGCircle(m_actor->get2DPos(),getTemplate()->getCloseRadius(),Color::blue());
            GFX_ADAPTER->drawDBGCircle(m_actor->get2DPos(),getTemplate()->getFindEnemyRadius(),Color::red());
        }
        else
        {
            AABB aabb(getTemplate()->getDetectionCloseRange());
            bbool flip = m_actor->isFlipped(); 
            if (flip)
                aabb.FlipHorizontaly(bfalse);
            DebugDraw::AABB(m_actor->getPos(),0.0f,aabb,Color::blue(),0.f,1.f,"DetectionCloseRange");
            aabb = getTemplate()->getDetectionRange();
            if (flip)
                aabb.FlipHorizontaly(bfalse);
            DebugDraw::AABB(m_actor->getPos(),0.0f,aabb,Color::red(),0.f,1.f,"DetectionRange");
        }
    }
#endif // ITF_SUPPORT_EDITOR

    //----------------------------------------------------------------------------------------------------------------------------------------

        IMPLEMENT_OBJECT_RTTI(Ray_HunterBossAIComponent_Template)
        BEGIN_SERIALIZATION_CHILD(Ray_HunterBossAIComponent_Template)

            SERIALIZE_OBJECT_WITH_FACTORY("idleBehavior", m_idleBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
            SERIALIZE_OBJECT_WITH_FACTORY("attackBehavior", m_attackBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
            SERIALIZE_OBJECT_WITH_FACTORY("holeAttackBehavior", m_holeAttackBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
            SERIALIZE_OBJECT_WITH_FACTORY("receiveHitBehavior", m_receiveHitBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
            SERIALIZE_OBJECT_WITH_FACTORY("deathBehavior", m_deathBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
            SERIALIZE_OBJECT_WITH_FACTORY("closeRangeBehavior", m_closeRangeBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
            SERIALIZE_OBJECT_WITH_FACTORY("crushedBehavior", m_crushedBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
            SERIALIZE_OBJECT_WITH_FACTORY("voidBehavior", m_voidBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
            SERIALIZE_MEMBER("findEnemyRadius", m_findEnemyRadius);
            SERIALIZE_MEMBER("closeRadius", m_closeRadius);
            SERIALIZE_MEMBER("useRadius", m_useRadius);
            SERIALIZE_OBJECT("detectionRange", m_detectionRange);
            SERIALIZE_OBJECT("detectionCloseRange", m_detectionCloseRange);
            SERIALIZE_MEMBER("healthBeforeChange", m_healthBeforeChange);
            SERIALIZE_MEMBER("maxNodes", m_maxNodes);

        END_SERIALIZATION()

        Ray_HunterBossAIComponent_Template::Ray_HunterBossAIComponent_Template()
        : m_findEnemyRadius(4.f)
        , m_closeRadius(1.0f)
        , m_detectionRange(Vec2d(-2, -2), Vec2d(5, 2))     // 5 meters front, 2 behind, 2 above & below
        , m_detectionCloseRange(Vec2d(-2, -2), Vec2d(5, 2))     // 5 meters front, 2 behind, 2 above & below
        , m_useRadius(btrue)
        , m_maxNodes(20)
        , m_healthBeforeChange(50)
        , m_idleBehavior(NULL)
        , m_attackBehavior(NULL)
        , m_holeAttackBehavior(NULL)
        , m_currentAttackBehavior(NULL)
        , m_receiveHitBehavior(NULL)
        , m_deathBehavior(NULL)
        , m_closeRangeBehavior(NULL)
        , m_crushedBehavior(NULL)
        , m_voidBehavior(NULL)
    {
    }

        Ray_HunterBossAIComponent_Template::~Ray_HunterBossAIComponent_Template()
        {
            SF_DEL(m_idleBehavior);
            SF_DEL(m_attackBehavior);
            SF_DEL(m_holeAttackBehavior);
            SF_DEL(m_currentAttackBehavior);
            SF_DEL(m_receiveHitBehavior);
            SF_DEL(m_deathBehavior);
            SF_DEL(m_closeRangeBehavior);
            SF_DEL(m_crushedBehavior);
            SF_DEL(m_voidBehavior);
        }
} // namespace ITF

