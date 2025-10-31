#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AILIVINGSTONEROAMBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/LivingStone/Ray_AILivingStoneRoamBehavior.h"
#endif //_ITF_RAY_AILIVINGSTONEROAMBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_AIFALLACTION_H_
#include "gameplay/ai/Actions/AIFallAction.h"
#endif //_ITF_AIFALLACTION_H_

#ifndef _ITF_AIPLAYANIMACTION_H_
#include "gameplay/ai/Actions/AIPlayAnimAction.h"
#endif //_ITF_AIPLAYANIMACTION_H_

#ifndef _ITF_AIJUMPACTION_H_
#include "gameplay/ai/Actions/AIJumpAction.h"
#endif //_ITF_AIJUMPACTION_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_WAYPOINTSMANAGER_H_
#include "gameplay/managers/WaypointsManager.h"
#endif //_ITF_WAYPOINTSMANAGER_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMATERIAL_H_
#include "rayman/gameplay/Ray_GameMaterial.h"
#endif //_ITF_RAY_GAMEMATERIAL_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AILivingStoneRoamBehavior_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AILivingStoneRoamBehavior_Template)
        // ACTIONS
        SERIALIZE_OBJECT_WITH_FACTORY("fall", m_fall, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("defaultPause", m_defaultPause, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("attack", m_attack, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_MEMBER("uturnDelay2", m_uturnDelay);
        SERIALIZE_OBJECT("attackRange", m_attackRange);

    END_SERIALIZATION()

        Ray_AILivingStoneRoamBehavior_Template::Ray_AILivingStoneRoamBehavior_Template()
        : m_fall(NULL)
        , m_defaultPause(NULL)
        , m_uturnDelay(0.3f)
        , m_attack(NULL)
    {

    }

    Ray_AILivingStoneRoamBehavior_Template::~Ray_AILivingStoneRoamBehavior_Template()
    {
        SF_DEL(m_fall);
        SF_DEL(m_defaultPause);
        SF_DEL(m_attack);
    }
    IMPLEMENT_OBJECT_RTTI(Ray_AILivingStoneRoamBehavior)
    BEGIN_VALIDATE_BEHAVIOR(Ray_AILivingStoneRoamBehavior)
        //VALIDATE_BEHAVIOR_PARAM("fall", m_fall, "this action is mandatory");
    END_VALIDATE_BEHAVIOR()

    Ray_AILivingStoneRoamBehavior::Ray_AILivingStoneRoamBehavior()
        : Super()
        , m_fall(NULL)
        , m_defaultPause(NULL)
        , m_attack(NULL)
        , m_blockingEdgeIndex(U32_INVALID)
        , m_timer(0.0f)
        , m_phantomComponent(NULL)
    {
    }

    Ray_AILivingStoneRoamBehavior::~Ray_AILivingStoneRoamBehavior()
    {
    }

    void Ray_AILivingStoneRoamBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {

        m_fall = static_cast<AIIdleAction*>(createAiAction(getTemplate()->getFall()));
        m_defaultPause = static_cast<AIIdleAction*>(createAiAction(getTemplate()->getDefaultPause()));
        m_attack = createAiAction(getTemplate()->getAttack());

        Super::onActorLoaded(_hotReload);



        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventBlockedByPolyline,2950236768), m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventTriggerChangePage,3503198481), m_aiComponent);

        m_phantomComponent = m_actor->GetComponent<PhantomComponent>();
    }


    void Ray_AILivingStoneRoamBehavior::onActivate()
    {
        Super::onActivate();

        m_blockingPoly.invalidate();
        m_blockingEdgeIndex = U32_INVALID;
        m_blockingContact = Vec2d::Zero;
        startIdle();
    }

    void Ray_AILivingStoneRoamBehavior::update( f32 _delta  )
    {
        if ( m_currentAction == m_idle )
        {
            updateIdle(_delta);
        }
        else if ( m_currentAction == m_move )
        {
            updateMove(_delta);
        }
        else if (m_currentAction == m_fall)
        {
            PolyLine* poly;
            const PolyLineEdge* edge;

            AIUtils::getPolyLine(m_blockingPoly,m_blockingEdgeIndex,poly,edge);

            if ( poly && edge )
            {
                const Ray_GameMaterial_Template* mat = (const Ray_GameMaterial_Template*)(World::getGameMaterial(edge->getGameMaterial()));

                if ( mat && mat->getDangerousLevel() > 0 )
                {
                    m_aiComponent->setHealth(0);
                    m_aiComponent->onBehaviorFinished();
                    return;
                }
            }
        }

        m_blockingPoly.invalidate();
        m_blockingEdgeIndex = U32_INVALID;
        m_blockingContact = Vec2d::Zero;

        Super::update(_delta);

        m_obstacle.invalidate();
        m_timer = Max(0.0f,m_timer - _delta);
    }

    void Ray_AILivingStoneRoamBehavior::onActionFinished()
    {
        Super::onActionFinished();
        if ( m_currentAction == m_fall )
        {
            onActionFinishedFall();
        }
        else if ( m_currentAction == m_defaultPause )
        {
            onActionFinishedPause();
        }
        else if ( m_currentAction == m_attack)
        {
            onActionFinishedAttack();
        }
    }

    void Ray_AILivingStoneRoamBehavior::onEvent( Event* _event )
    {
        Super::onEvent(_event);
        if ( EventBlockedByPolyline* blockedEvent = _event->DynamicCast<EventBlockedByPolyline>(ITF_GET_STRINGID_CRC(EventBlockedByPolyline,2950236768)) )
        {
            processEventBlockedByPolyline(blockedEvent);
        }
        else if ( Ray_EventTriggerChangePage * onChangePage = _event->DynamicCast<Ray_EventTriggerChangePage>(ITF_GET_STRINGID_CRC(Ray_EventTriggerChangePage,3503198481)))
        {
            m_obstacle = onChangePage->getSender();
        }
    }

    void Ray_AILivingStoneRoamBehavior::updateIdle( f32 _dt )
    {
        PolyLine* poly = m_physComponent->getStickedPolyline();

        // If there is no floor we fall
        if ( !poly )
        {
            setAction(m_fall);
            return;
        }

        if (checkAttack())
            return ;

        if (m_timer == 0.0f)
        {
            // just move forward
            startMove(m_actor->isFlipped());
        }
    }


    bbool Ray_AILivingStoneRoamBehavior::updateMove( f32 _dt )
    {
        PolyLine* poly = m_physComponent->getStickedPolyline();

        if ( !poly )
        {
            // No floor, we fall
            startFall();
            return bfalse;
        }

        Actor * obstacle = m_obstacle.getActor();
        if (obstacle)
        {
            const Vec2d dirContact = obstacle->get2DPos()-m_actor->get2DPos();
            const Vec2d lookDir = AIUtils::getLookDir(m_actor,m_actor->isFlipped());
            if (dirContact.dot(lookDir) > 0.0f)
            {
                if (m_timer == 0.0f)
                {
                    m_timer = getTemplate()->getUturnDelay();
                    performUTurnImmediate();
                    return btrue;
                }
                else
                {
                    startIdle();
                    return btrue;
                }
            }
        }

        if (checkBlockingContacts())
        {
            return bfalse;
        }

        if (m_blockingPoly.isValid() 
            && m_blockingEdgeIndex != U32_INVALID 
            && checkBlockingContact(m_blockingPoly,m_blockingEdgeIndex,m_blockingContact))
        {
            return bfalse;
        }

        u32 edgeIndex = m_physComponent->getStickedEdgeIndex();
        if (checkPolylineObstacles(poly, edgeIndex))
        {
            return bfalse;
        }


        if (checkUturn())
        {
            return bfalse;
        }

        if (checkAttack())
            return bfalse;


        return btrue;
    }

    void Ray_AILivingStoneRoamBehavior::startAnimPause( )
    {
        setAction(m_defaultPause);
    }

    void Ray_AILivingStoneRoamBehavior::onActionFinishedFall()
    {
        startIdle();
    }

    void Ray_AILivingStoneRoamBehavior::onActionFinishedPause()
    {
        m_timer = getTemplate()->getUturnDelay();
        performUTurnImmediate();
    }


    void Ray_AILivingStoneRoamBehavior::onActionFinishedUturn()
    {
        Super::onActionFinishedUturn();

        startMove(m_actor->isFlipped());
    }

    void Ray_AILivingStoneRoamBehavior::onActionFinishedMove()
    {
        startAnimPause();
    }

    void Ray_AILivingStoneRoamBehavior::startIdle()
    {
        setAction(m_idle);
    }

    void Ray_AILivingStoneRoamBehavior::startMove( bbool _walkLeft)
    {
        m_move->setWalkLeft(_walkLeft);
        setAction(m_move, btrue);
    }

     void Ray_AILivingStoneRoamBehavior::startFall()
    {
        setAction(m_fall);
    }

 
    void Ray_AILivingStoneRoamBehavior::updateAnimInputs()
    {
        Super::updateAnimInputs();

    }

    bbool Ray_AILivingStoneRoamBehavior::checkBlockingContacts()
    {
        Vec2d lookDir = AIUtils::getLookDir(m_actor, m_actor->isFlipped());

        const PhysContactsContainer& contacts = m_physComponent->getContacts();
        const u32 contactCount = contacts.size();
        for (u32 i = 0; i < contactCount; i++)
        {
            const SCollidableContact& contact = contacts[i];

            if (checkBlockingContact(contact.m_collidableUserData,contact.m_edgeIndex,contact.m_contactPoint))
                return btrue;
        }

        return bfalse;
    }

    void Ray_AILivingStoneRoamBehavior::processEventBlockedByPolyline( EventBlockedByPolyline* _blockedEvent )
    {
        BaseObject* obj = GETOBJECT(_blockedEvent->getPolyline());
        if ( obj )
        {
            // remember the blocking and process it on update
            m_blockingPoly = _blockedEvent->getPolyline();
            m_blockingEdgeIndex = _blockedEvent->getEdge();
            m_blockingContact = _blockedEvent->getContact();
#ifndef ITF_FINAL
            //DebugDraw::edge(m_blockingEdge, m_blockingPoly->getDepth(), Color::red(), 1.f);
#endif // ITF_FINAL
        }
    }

    // If there is a hole: jump down if it's not too deep, otherwise pause & u-turn.
    bbool Ray_AILivingStoneRoamBehavior::checkHole( PolyLine* poly )
    {
        f32 holeDepth;
        bbool hasHole = AIUtils::hasHoleInFront(
            m_actor, m_actor->isFlipped(),
            poly, m_physComponent->getStickedEdgeIndex(),
            getTemplate()->getHoleDetectionRange(),
            m_physComponent->getRadius(),
            m_aiComponent->getCurrentGravityDir(),
            holeDepth);

        if ( hasHole )
        {
            startAnimPause();

            return btrue;
        }

        return bfalse;
    }

    ITF::bbool Ray_AILivingStoneRoamBehavior::checkUturn()
    {

        PhysContactsContainer contacts;

        PHYSWORLD->collidePhantoms(m_actor->get2DPos(),m_actor->get2DPos(),m_actor->getAngle(),
            m_phantomComponent->getShape(),m_actor->getDepth(),ECOLLISIONFILTER_CHARACTERS,contacts);

        ActorList list;


        for ( u32 i = 0; i < contacts.size(); i++ )
        {
            SCollidableContact& contact = contacts[i];

            if ( contact.m_collidableUserData == m_actor->getRef() )
            {
                continue;
            }

            BaseObject* obj = contact.m_collidableUserData.getObject();
            if (!obj)
            {
                continue;
            }


            if ( Actor* actor = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) )
            {
                if (FACTION_MANAGER->hasInteraction(m_aiComponent->getFaction(), AIUtils::getFaction(actor), INTERACTION_ALLY))
                {
                    const Vec2d dirContact = actor->get2DPos()-m_actor->get2DPos();
                    const Vec2d lookDir = AIUtils::getLookDir(m_actor,m_actor->isFlipped());
                    if (dirContact.dot(lookDir) > 0.0f)
                    {
                        if (m_timer == 0.0f)
                        {
                            m_timer = getTemplate()->getUturnDelay();
                            performUTurnImmediate();
                            return btrue;
                        }
                        else
                        {
                            startIdle();
                            return btrue;
                        }
                    }
                }

            }
        }
        return bfalse;
    }

    ITF::bbool Ray_AILivingStoneRoamBehavior::checkPolylineObstacles( PolyLine* poly, u32 edgeIndex )
    {
         AIUtils::EdgeType edgeType = AIUtils::EdgeType_Invalid;
        f32 distanceToEdge = F32_INFINITY;
        f32 edgeLength = 0.0f;
        bbool up = bfalse;
        u32 nextEdgeIndex = AIUtils::getNextEdgeInfo(
            m_actor->get2DPos(),
            poly,edgeIndex,
            m_actor->isFlipped(),
            m_aiComponent->getCurrentGravityDir(),
            edgeType,
            distanceToEdge,
            edgeLength,
            up);



        //    no edge
        if (nextEdgeIndex == U32_INVALID && distanceToEdge < getTemplate()->getHoleDetectionRange())
        {
            startAnimPause();
            return btrue;
        }
        //    edge
        else if (edgeType == AIUtils::EdgeType_Wall || edgeType == AIUtils::EdgeType_Ceiling)
        {
            //hole 
            if (!up && distanceToEdge < getTemplate()->getHoleDetectionRange())
            {
                if (edgeLength > getTemplate()->getHoleJumpDepth())
                {
                    startAnimPause();
                    return btrue;
                }
            }
            //wall
            else if (up && distanceToEdge < getTemplate()->getObstacleDetectionRange())
            {
                if (edgeLength > getTemplate()->getObstacleJumpHeight())
                {
                    startAnimPause();
                    return btrue;
                }
            }
        }
        return bfalse;
    }

    ITF::bbool Ray_AILivingStoneRoamBehavior::checkBlockingContact(ObjectRef _poly, u32 _edgeIndex, Vec2d _contact)
    {

        Vec2d lookDir = AIUtils::getLookDir(m_actor, m_actor->isFlipped());

        //ignore self
        if ( _poly == m_actor->getRef() )
            return bfalse;

        BaseObject* obj = GETOBJECT(_poly);

        PolyLine* polyLine = obj?obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747)):NULL;
        if (polyLine)
        {
            if (polyLine->getOwnerActor() == m_actor)
                return bfalse;

            if (polyLine->getOwnerActor() && m_actor->getBinding()->isDescendant(polyLine->getOwnerActor()->getRef()))
                return bfalse;
        }

        PolyLine* contactPoly(NULL);
        const PolyLineEdge* contactEdge(NULL);
        AIUtils::getPolyLine(_poly, _edgeIndex, contactPoly, contactEdge);

        if (contactPoly && contactEdge)
        {
            f32 dist = 0.f;
            Vec2d edgeToPos = _contact - contactEdge->getPos();
            f32 startPos = contactEdge->m_normalizedVector.dot(edgeToPos);

            startPos = Clamp(startPos,0.f,contactEdge->m_length);

            if ( m_actor->isFlipped() )
            {
                dist += startPos;
            }
            else
            {
                dist += contactEdge->m_length - startPos;
            }

            //it should not be possible to contact with a wall considered a hole...

            AIUtils::EdgeType type = AIUtils::getEdgeType(*contactEdge);

            if ((_contact - m_actor->get2DPos()).dot(lookDir) > 0.0f
                && (type == AIUtils::EdgeType_Ceiling || type == AIUtils::EdgeType_Wall)
                && dist > getTemplate()->getObstacleJumpHeight())
            {
                startAnimPause();
                return btrue;
            }
        }

        return bfalse;
    }
    void Ray_AILivingStoneRoamBehavior::onActionFinishedBounceToLayer()
    {
        Super::onActionFinishedBounceToLayer(  );
        startIdle();
    }

    void Ray_AILivingStoneRoamBehavior::onActionFinishedBounce()
    {
        Super::onActionFinishedBounce(  );
        startIdle();
    }

    void Ray_AILivingStoneRoamBehavior::startAttack()
    {
        setAction(m_attack);
    }

    void Ray_AILivingStoneRoamBehavior::onActionFinishedAttack()
    {
        startIdle();
    }

    ITF::bbool Ray_AILivingStoneRoamBehavior::checkAttack()
    {        
        if (AIUtils::getClosestEnemy(m_actor,m_actor->isFlipped(),getTemplate()->getAttackRange()))
        {
            startAttack();
            return btrue;
        }
        return bfalse;
    }

#ifdef ITF_SUPPORT_EDITOR
    void Ray_AILivingStoneRoamBehavior::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        AABB aabb = getTemplate()->getAttackRange();
        if (m_actor->isFlipped())
            aabb.FlipHorizontaly(bfalse);
        DebugDraw::AABB(m_actor->getPos(),0.0f,aabb,Color::white(),0.0f,1.0f,"AttackRange");
    }
#endif // ITF_SUPPORT_EDITOR
}