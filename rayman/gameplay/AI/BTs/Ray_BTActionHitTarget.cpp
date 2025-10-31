
#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BTACTIONHITTARGET_H_
#include "rayman/gameplay/ai/BTs/Ray_BTActionHitTarget.h"
#endif // _ITF_RAY_BTACTIONHITTARGET_H_

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

#ifndef _ITF_RAY_AIBTFACTS_H_
#include "rayman/gameplay/ai/BTs/Ray_AIBTFacts.h"
#endif //_ITF_RAY_AIBTFACTS_H_

#ifndef _ITF_METRONOME_H_
#include "engine/sound/Metronome.h"
#endif //_ITF_METRONOME_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_RAY_GAMEMATERIAL_H_
#include "rayman/gameplay/Ray_GameMaterial.h"
#endif //_ITF_RAY_GAMEMATERIAL_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_BTActionHitTarget);

Ray_BTActionHitTarget::Ray_BTActionHitTarget()
: m_animComponent(NULL)
, m_physComponent(NULL)
, m_fxControllerComponent(NULL)
, m_currentAttack(U32_INVALID)
, m_previousAttack(U32_INVALID)
, m_timer(0.f)
, m_currentPunchDistance(0.f)
, m_currentPunchResult(PUNCHRELEASERESULT_NOIMPACT)
, m_preparingHit(bfalse)
, m_performingHit(bfalse)
, m_hitSomethingThisRound(bfalse)
, m_hitsReleased(bfalse)
, m_punchUp(bfalse)
, m_punchCharging(bfalse)
, m_comboHitNext(bfalse)
, m_hitFxBoneIndex(U32_INVALID)
, m_burstStage(0)
{
}

Ray_BTActionHitTarget::~Ray_BTActionHitTarget()
{
}

void Ray_BTActionHitTarget::init( BehaviorTree* _tree, const BTNode_Template* _template )
{
    Super::init(_tree,_template);

    m_animComponent = _tree->getActor()->GetComponent<AnimatedComponent>();
    m_physComponent = _tree->getActor()->GetComponent<PhysComponent>();
    m_fxControllerComponent = _tree->getActor()->GetComponent<FXControllerComponent>();

    ACTOR_REGISTER_EVENT_COMPONENT(m_tree->getActor(),ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301), m_tree);
    ACTOR_REGISTER_EVENT_COMPONENT(m_tree->getActor(),ITF_GET_STRINGID_CRC(AnimPolylineEvent,4083589773), m_tree);
}

bbool Ray_BTActionHitTarget::validate( Actor* _actor ) const
{
    bbool retVal = Super::validate(_actor);

    if ( !retVal )
    {
        return bfalse;
    }

    if ( !m_animComponent )
    {
        ITF_WARNING_PARAM(_actor,"AnimatedComponent",m_animComponent!=NULL,"%s: anim component is mandatory",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    if ( !m_physComponent )
    {
        ITF_WARNING_PARAM(_actor,"PhysComponent",m_physComponent!=NULL,"%s: physic component is mandatory",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    u32 numAttacks = getTemplate()->getNumAttacks();

    if ( numAttacks == 0 )
    {
        ITF_WARNING_PARAM(_actor,"attacks",0,"%s: no attacks set",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    for ( u32 i = 0; i < numAttacks; i++ )
    {
        if ( !getTemplate()->getAttack(i).m_anim.isValid() )
        {
            ITF_WARNING_PARAM(_actor,"anim",0,"%s: attack %d doesn't have an anim set",StringID(GetObjectClassCRC()).getDebugString(),i);
            return bfalse;
        }
    }

    return btrue;
}

bbool Ray_BTActionHitTarget::decide( f32 _dt )
{
    return btrue;
}

void Ray_BTActionHitTarget::onActivate()
{
    Super::onActivate();

    chooseAttack();

    m_preparingHit = bfalse;
    m_performingHit = bfalse;
    m_hitsReleased = bfalse;
    m_hitSomethingThisRound = bfalse;
    m_currentPunchResult = PUNCHRELEASERESULT_NOIMPACT;
    m_currentPunchDistance = 1.f;
    m_animComponent->resetCurTime();
    m_hitQueue.clear();
    m_memorizedHits.clear();
    m_shapePoints.clear();

    m_tree->getBlackBoard().setFact(Ray_AIFact_attackInProgress,true);
}

void Ray_BTActionHitTarget::onDeactivate()
{
    Super::onActivate();

    m_tree->getBlackBoard().removeFact(Ray_AIFact_attackInProgress);
}

bbool Ray_BTActionHitTarget::update( f32 _dt )
{
    Super::update(_dt);

    const Ray_BTActionHitTarget_Template::AttackData& currentAttack = getTemplate()->getAttack(m_currentAttack);
    bbool continueAttack;

    switch(currentAttack.m_punchType)
    {
        case PUNCHTYPE_CHARGE:
            continueAttack = updateChargePunch(_dt,currentAttack.m_level,currentAttack.m_pushBackRadius,currentAttack.m_duration);
            break;
        case PUNCHTYPE_CROUCHKICK:
            continueAttack = updateCrouchKick(_dt,currentAttack.m_level,currentAttack.m_pushBackRadius,currentAttack.m_duration);
            break;
        default:
            continueAttack = updateRepeating(_dt,currentAttack.m_level,currentAttack.m_pushBackRadius,currentAttack.m_duration);
            break;
    }

    if ( m_preparingHit )
    {
        fillHitQueue(btrue,getBaseHitDir());
    }

    if ( m_performingHit )
    {
        m_hitQueue.clear();
        fillHitQueue(bfalse,getBaseHitDir());

        PunchStim* stim = setupPunchStim();
        releaseHitsFromQueue(stim);
        STIMSMANAGER->recycleStim(stim);
    }

    m_timer += _dt;

    return continueAttack;
}

void Ray_BTActionHitTarget::clearHit()
{
    m_comboHitNext = bfalse;
    m_hitsReleased = bfalse;
    m_performingHit = bfalse;
    m_preparingHit = bfalse;
    m_hitQueue.clear();
    m_memorizedHits.clear();
    m_currentPunchDistance = 1.f;
    m_currentPunchResult = PUNCHRELEASERESULT_NOIMPACT;
    m_hitSomethingThisRound = bfalse;
}

bbool Ray_BTActionHitTarget::updateChargePunch( f32 _dt, u32 _level, f32 _push, f32 _duration )
{
    if ( m_animComponent->isMainNodeFinished() )
    {
        if ( m_punchCharging )
        {
            m_punchCharging = bfalse;
        }
        else
        {
            return bfalse;
        }
    }

    return btrue;
}

bbool Ray_BTActionHitTarget::updateCrouchKick( f32 _dt, u32 _level, f32 _push, f32 _duration )
{
    if ( m_animComponent->isMainNodeFinished() )
    {
        return bfalse;
    }

    return btrue;
}

bbool Ray_BTActionHitTarget::updateRepeating( f32 _dt, u32 _level, f32 _push, f32 _duration )
{
    if ( m_comboHitNext )
    {
        if ( m_timer < getTemplate()->getAttack(m_currentAttack).m_duration )
        {
            // restart the punch
            clearHit();
            m_burstStage = (m_burstStage+1)%2;
            m_animComponent->resetTree();
        }
        else if ( m_animComponent->isMainNodeFinished() )
        {
            return bfalse;
        }
    }

    return btrue;
}

void Ray_BTActionHitTarget::chooseAttack()
{
    m_previousAttack = m_currentAttack;

    u32 numAttacks = getTemplate()->getNumAttacks();

    if ( numAttacks > 1 )
    {
        static const u32 numTries = 4;
        u32 currentTries = 0;

        do
        {
            m_currentAttack = Seeder::getSharedSeeder().GetU32(numAttacks-1);
            currentTries++;
        } while ( m_currentAttack == m_previousAttack && currentTries < numTries );
    }
    else
    {
        m_currentAttack = 0;
    }

    if ( getTemplate()->getAttack(m_currentAttack).m_punchType == PUNCHTYPE_CHARGE )
    {
        m_punchCharging = btrue;
    }
    else
    {
        m_punchCharging = bfalse;
    }

    m_punchUp = bfalse;
    m_timer = 0.f;
    m_animComponent->setAnim(getTemplate()->getAttack(m_currentAttack).m_anim);
    m_animComponent->resetTree();
}

void Ray_BTActionHitTarget::processPerformHit()
{
    PunchStim* stim = setupPunchStim();

    releaseHitsFromQueue(stim);

    STIMSMANAGER->recycleStim(stim);
}

void Ray_BTActionHitTarget::fillHitQueue( bbool _updateAnimValues, const Vec2d& _hitDir )
{
    if ( !m_shapePoints.size() )
    {
        return;
    }

    m_shape.setFlipped(bfalse);

    // We only calculate collision when we are setting the animation values
    calculateHitQueue(_hitDir,_updateAnimValues,m_hitQueue);

    if ( _updateAnimValues )
    {
        updateAnimValuesFromQueue();
    }
}

void Ray_BTActionHitTarget::updateAnimValuesFromQueue()
{
    // Use the information on the queue to setup the animation
    if ( m_hitQueue.size() )
    {
        HitQueueInfo& hitInfo = m_hitQueue[m_hitQueue.size()-1];

        if ( hitInfo.m_blocks )
        {
            m_currentPunchDistance = hitInfo.m_animDist;
        }
        else
        {
            m_currentPunchDistance = 1.f;
        }

        if ( hitInfo.m_fail )
        {
            m_currentPunchResult = PUNCHRELEASERESULT_FAIL;
        }
        else
        {
            m_currentPunchResult = PUNCHRELEASERESULT_IMPACT;
        }
    }
    else
    {
        m_currentPunchDistance = 1.f;
        m_currentPunchResult = PUNCHRELEASERESULT_NOIMPACT;
    }
}

void Ray_BTActionHitTarget::calculateHitShapeFromEvent( AnimPolylineEvent* _animPolyEvent )
{
    u32 numPoints = _animPolyEvent->getNumPoints();

    if ( numPoints > 1 )
    {
        const Vec2d* points = _animPolyEvent->getPoints();
        calculateHitShapeFromPolyLine(points,numPoints);
    }
}

void Ray_BTActionHitTarget::calculateHitShapeFromPolyLine( const Vec2d* _points, u32 _numPoints )
{
    Vec2d p;
    bbool flipped = m_tree->getActor()->isFlipped();

    m_shapePoints.clear();

    const Vec2d& myPos = m_tree->getActor()->get2DPos();
    f32 myAngle = m_tree->getActor()->getAngle();

    if ( flipped )
    {
        for ( i32 i = static_cast<i32>(_numPoints)-1; i >= 0; i-- )
        {
            p = ( _points[i] - myPos ).Rotate(-myAngle);

            m_shapePoints.push_back(p);
        }
    }
    else
    {
        for ( u32 i = 0; i < _numPoints; i++ )
        {
            p = ( _points[i] - myPos ).Rotate(-myAngle);

            m_shapePoints.push_back(p);
        }
    }

    m_shape.setPoints(m_shapePoints);
}

void Ray_BTActionHitTarget::calculateHitQueue( const Vec2d& _hitDir, bbool _calculateCollision, HitQueue& _queue )
{
    PhysContactsContainer allContacts, contacts;
    u32 collisionIndex = 0;
    Actor* myActor = m_tree->getActor();
    const Vec2d& myPos = myActor->get2DPos();
    f32 myAngle = myActor->getAngle();
    f32 myDepth = myActor->getDepth();

    PHYSWORLD->checkEncroachment(myPos,myPos,myAngle,
        &m_shape,ECOLLISIONFILTER_ENVIRONMENT,myDepth,allContacts);
    collisionIndex = allContacts.size();
    PHYSWORLD->collidePhantoms(myPos,myPos,myAngle,
        &m_shape,myDepth,ECOLLISIONFILTER_ALL,allContacts);

    hitQueueApplyFilter(allContacts,contacts,collisionIndex);

    u32 numContacts = allContacts.size();

    if ( numContacts )
    {
        AIUtils::SortedContactInfoArray sortedContacts;
        f32 shapeStart, shapeEnd;
        AIUtils::sortContactsOnShape(myActor,_hitDir,m_shape,contacts,
            sortedContacts,shapeStart,shapeEnd);

        numContacts = sortedContacts.size();
        bbool hitBlocked;
        HitQueueInfo tempHit;
        bbool needSort = _queue.size() > 0;
        u32 lastHitIndex = needSort ? _queue.size()-1 : U32_INVALID;
        f32 blockingHitDist = needSort ? _queue[lastHitIndex].m_dist : 0.f;
        f32 animStart = m_physComponent->getRadius();
        f32 animEnd = Max(animStart,shapeEnd);
        f32 animRange = animEnd - animStart;
        PolyLine* poly;
        const PolyLineEdge* edge;

        for ( u32 i = 0; i < numContacts; i++ )
        {
            AIUtils::SortedContactInfo& contact = sortedContacts[i];
            SCollidableContact& closestContact = contact.m_contacts[0].m_contact;
            f32 contactDist = contact.m_contacts[0].m_dist;

            if ( _calculateCollision )
            {
                if ( blockingHitDist && contactDist > blockingHitDist )
                {
                    // a previous hit was blocking, if this hit is close we send it, otherwise we stop
                    f32 delta = contactDist - blockingHitDist;
                    if ( delta > 0.1f )
                    {
                        // Flush the hits after this
                        _queue.resize(lastHitIndex+1);
                        break;
                    }
                }
            }

            hitBlocked = bfalse;

            tempHit.m_fail = bfalse;
            tempHit.m_blocks = bfalse;
            tempHit.m_dist = contactDist;
            tempHit.m_pos = closestContact.m_contactPoint;
            tempHit.m_dir = _hitDir;
            tempHit.m_closestEdgeIndex = closestContact.m_edgeIndex;
            tempHit.m_closestPoly = closestContact.m_edgeIndex != U32_INVALID ? closestContact.m_collidableUserData : ITF_INVALID_OBJREF;
            tempHit.m_receiver = contact.m_actor;

            f32 animDist = Clamp(contactDist,animStart,animEnd);

            if ( animRange )
            {
                tempHit.m_animDist = ( animDist - animStart ) / animRange;
            }
            else
            {
                tempHit.m_animDist = 0.f;
            }

            tempHit.m_contacts.clear();

            for (u32 i = 0; i < contact.m_contacts.size() && !tempHit.m_contacts.full(); i++)
            {
                tempHit.m_contacts.push_back(contact.m_contacts[i].m_contact);
            }

            if ( _calculateCollision )
            {
                if ( contact.m_actor != ITF_INVALID_OBJREF )
                {
                    Actor* receiver = AIUtils::getActor(contact.m_actor);

                    if ( receiver )
                    {
                        EventQueryBlocksHits queryBlock;
                        queryBlock.setContacts(&tempHit.m_contacts);

                        receiver->onEvent(&queryBlock);

                        if ( queryBlock.getCanBlockHits() )
                        {
                            hitBlocked = btrue;
                        }
                    }
                }
            }

            if ( contact.m_polyline != ITF_INVALID_OBJREF )
            {
                AIUtils::getPolyLine(contact.m_polyline,closestContact.m_edgeIndex,poly,edge);

                if ( poly->m_isCollisionActive && edge )
                {
                    // If it's a polyline of the environment and it's solid we will stop hitting
                    // Also, if we are performing a crush attack we stop when we hit any environment polyline
                    const Ray_GameMaterial_Template* mat = (const Ray_GameMaterial_Template*)(World::getGameMaterial(edge->getGameMaterial()));

                    if ( mat )
                    {
                        tempHit.m_contactMaterial = mat->getFile().getStringID();
                    }

                    if ( _calculateCollision )
                    {
                        bbool isFail = mat && mat->getDangerousLevel();
                        bbool isSolid = !mat || mat->isSolid();

                        if ( isFail )
                        {
                            tempHit.m_fail = btrue;
                        }

                        if ( isSolid && 
                            _hitDir.dot(edge->m_normalizedVector.getPerpendicular()) <= 0.f &&
                            contact.m_actor == ITF_INVALID_OBJREF )
                        {
                            hitBlocked = btrue;
                        }
                    }
                }
            }

            bbool hitBlocks = tempHit.m_fail || hitBlocked;

            if ( hitBlocks )
            {
                blockingHitDist = tempHit.m_dist;
                tempHit.m_blocks = btrue;
            }

            u32 queueSize = _queue.size();

            for ( u32 queueIndex = 0; queueIndex < queueSize; queueIndex++ )
            {
                const HitQueueInfo& hitInfo = _queue[queueIndex];

                if ( hitInfo.m_receiver == tempHit.m_receiver &&
                    hitInfo.m_closestEdgeIndex == tempHit.m_closestEdgeIndex &&
                    hitInfo.m_closestPoly == tempHit.m_closestPoly )
                {
                    _queue.eraseKeepOrder(queueIndex);
                    break;
                }
            }

            if ( _queue.full() )
            {
                continue;
            }

            if ( !needSort )
            {
                lastHitIndex = _queue.size();
                _queue.push_back(tempHit);
            }
            else
            {
                bbool inserted = bfalse;

                for ( u32 queueIndex = 0; queueIndex < _queue.size(); queueIndex++ )
                {
                    HitQueueInfo hitInfo = _queue[queueIndex];

                    if ( hitInfo.m_dist > tempHit.m_dist)
                    {
                        inserted = btrue;
                        _queue.insert(tempHit,queueIndex);

                        if ( hitBlocks )
                        {
                            lastHitIndex = queueIndex;
                        }

                        break;
                    }
                }

                if ( !inserted )
                {
                    lastHitIndex = _queue.size();
                    _queue.push_back(tempHit);
                }
            }
        }
    }
}

void Ray_BTActionHitTarget::hitQueueApplyFilter( const PhysContactsContainer& _allContacts, PhysContactsContainer& _filteredContacts, u32& _collisionIndex )
{
    PolyLine* poly(NULL);
    const PolyLineEdge* edge(NULL);
    bbool hitFriendly = bfalse;
    bbool hitOtherActor = m_hitSomethingThisRound;
    u32 numContacts = _allContacts.size();
    u32 newCollisionIndex = _collisionIndex;
    Actor* myActor = m_tree->getActor();
    u32 faction = U32_INVALID;
    m_tree->getBlackBoard().getFact(Ray_AIFact_faction,faction);

    for ( u32 i = 0; i < numContacts && !_filteredContacts.full(); i++ )
    {
        const SCollidableContact& contact = _allContacts[i];

        if ( contact.m_collidableUserData == myActor->getRef() )
        {
            if ( i < _collisionIndex )
            {
                newCollisionIndex--;
            }

            continue;
        }

        if ( contact.m_edgeIndex != U32_INVALID )
        {
            AIUtils::getPolyLine(contact.m_collidableUserData, contact.m_edgeIndex, poly, edge);

            if ( poly && edge )
            {
                if ( poly->m_isCollisionActive )
                {
                    const GameMaterial_Template* mat = World::getGameMaterial(edge->getGameMaterial());

                    if ( mat && mat->getNoStim() )
                    {
                        if ( i < _collisionIndex )
                        {
                            newCollisionIndex--;
                        }

                        continue;
                    }
                }

                if ( i >= _collisionIndex )
                {
                    Actor* ownerActor = poly->getOwnerActor();

                    if ( ownerActor )
                    {
                        if ( !AIUtils::isEnemy(faction,AIUtils::getFaction(ownerActor)) )
                        {
                            hitFriendly = btrue;
                        }
                        else
                        {
                            hitOtherActor = btrue;
                        }
                    }
                }
            }
        }
        else
        {
            if ( i >= _collisionIndex )
            {
                Actor* ownerActor = AIUtils::getActor(contact.m_collidableUserData);

                if ( ownerActor && !AIUtils::isEnemy(faction,AIUtils::getFaction(ownerActor)) )
                {
                    hitFriendly = btrue;
                }
                else
                {
                    hitOtherActor = btrue;
                }
            }
        }

        _filteredContacts.push_back(contact);
    }

    numContacts = _filteredContacts.size();

    if ( !numContacts )
    {
        return;
    }

    // If we hit a friendly and a non-friendly we remove the friendly
    if ( hitOtherActor && hitFriendly )
    {
        u32 index = 0;

        while ( index < numContacts )
        {
            const SCollidableContact& contact = _filteredContacts[index];

            if ( contact.m_edgeIndex != U32_INVALID )
            {
                AIUtils::getPolyLine(contact.m_collidableUserData, contact.m_edgeIndex, poly, edge);

                if ( poly && edge )
                {
                    Actor* ownerActor = poly->getOwnerActor();

                    if ( ownerActor )
                    {
                        if ( !AIUtils::isEnemy(faction,AIUtils::getFaction(ownerActor)) )
                        {
                            if ( index < newCollisionIndex )
                            {
                                newCollisionIndex--;
                            }

                            _filteredContacts.eraseNoOrder(index);
                            numContacts--;
                            continue;
                        }
                    }
                }
            }
            else
            {
                Actor* ownerActor = AIUtils::getActor(contact.m_collidableUserData);

                if ( ownerActor && !AIUtils::isEnemy(faction,AIUtils::getFaction(ownerActor)) )
                {
                    if ( index < newCollisionIndex )
                    {
                        newCollisionIndex--;
                    }

                    _filteredContacts.eraseNoOrder(index);
                    numContacts--;
                    continue;
                }
            }

            index++;
        }
    }

    m_hitSomethingThisRound = hitOtherActor;
}

void Ray_BTActionHitTarget::releaseHitsFromQueue( PunchStim* _stim )
{
    m_hitsReleased = btrue;

    u32 numHits = m_hitQueue.size();

    if ( !numHits )
    {
        return;
    }

    bbool isMemorized;
    u32 numMemorized = m_memorizedHits.size();

    for ( u32 i = 0; i < numHits; i++ )
    {
        HitQueueInfo& info = m_hitQueue[i];

        isMemorized = bfalse;

        for ( u32 memorizedIndex = 0; memorizedIndex < numMemorized; memorizedIndex++ )
        {
            const MemorizedHitInfo& memInfo = m_memorizedHits[memorizedIndex];

            if ( memInfo.m_receiver == info.m_receiver &&
                 memInfo.m_edgeIndex == info.m_closestEdgeIndex &&
                 memInfo.m_poly == info.m_closestPoly )
            {
                isMemorized = btrue;
            }
        }

        if ( isMemorized )
        {
            continue;
        }

        Actor* receiver = AIUtils::getActor(info.m_receiver);

        if ( receiver )
        {
            bbool hasPhantom = bfalse;

            _stim->resetContacts();

            for (u32 i = 0; i < info.m_contacts.size(); i++)
            {
                const SCollidableContact& c = info.m_contacts[i];
                BaseObject* obj = GETOBJECT(c.m_collidable);

                if ( obj )
                {
                    hasPhantom |= (obj->DynamicCast<PhysPhantom>(ITF_GET_STRINGID_CRC(PhysPhantom,126559815)) != NULL);
                }

                _stim->addContact(c);
            }

            _stim->setDirection(info.m_dir);
            receiver->onEvent(_stim);
        }
        else
        {
            if ( m_fxControllerComponent )
            {
                static StringID s_hitWall = "HitWallFx";
                const GameMaterial_Template* mat = World::getGameMaterial(info.m_contactMaterial);
                m_fxControllerComponent->playMaterialFX(s_hitWall,mat);
            }
        }

        if ( !m_memorizedHits.full() )
        {
            MemorizedHitInfo memorized;

            memorized.m_edgeIndex = info.m_closestEdgeIndex;
            memorized.m_poly = info.m_closestPoly;
            memorized.m_receiver = info.m_receiver;

            m_memorizedHits.push_back(memorized);
        }
    }

    m_hitQueue.clear();
}

Vec2d Ray_BTActionHitTarget::getBaseHitDir() const
{
    f32 angle = m_tree->getActor()->getAngle();

    switch(getTemplate()->getAttack(m_currentAttack).m_punchType)
    {
    case PUNCHTYPE_CRUSH:
        angle += -MTH_PIBY2;
        break;
    case PUNCHTYPE_UPPERKICK:
        angle += MTH_PIBY2;
        break;
    case PUNCHTYPE_CHARGE:
    case PUNCHTYPE_REPEATING:
        if ( !m_tree->getActor()->isFlipped() )
        {
            if ( m_punchUp )
            {
                angle += MTH_PIBY2;
            }
        }
        else
        {
            angle += MTH_PI;

            if ( m_punchUp )
            {
                angle -= MTH_PIBY2;
            }
        }
        break;
    default:
        if ( m_tree->getActor()->isFlipped() )
        {
            angle += MTH_PI;
        }
        break;
    }

    return Vec2d::Right.Rotate(angle);
}

PunchStim* Ray_BTActionHitTarget::setupPunchStim()
{
    PunchStim* stim = NULL;

    stim = STIMSMANAGER->requestStim<PunchStim>(NULL);

    if(stim)
    {
        Actor* myActor = m_tree->getActor();

        stim->setDepth(myActor->getDepth());
        stim->setSender(myActor->getRef());
        stim->setLevel(getTemplate()->getAttack(m_currentAttack).m_level);
        stim->setAngle(myActor->getAngle());
        stim->setPushBackDistance(getTemplate()->getAttack(m_currentAttack).m_pushBackRadius);
        u32 faction = U32_INVALID;
        m_tree->getBlackBoard().getFact(Ray_AIFact_faction,faction);
        stim->setFaction(faction);

        setReceiveHitTypeOnStim(stim);

        stim->setPos(myActor->get2DPos());
        stim->setPrevPos(myActor->get2DPos());

        Vec3d fxPos = getHitFxPos(myActor->getPos());
        stim->setFxPos(fxPos);
    }

    return stim;
}

void Ray_BTActionHitTarget::setReceiveHitTypeOnStim( PunchStim* _stim ) const
{
    EPunchType punchType = getTemplate()->getAttack(m_currentAttack).m_punchType;

    if (punchType == PUNCHTYPE_CROUCHKICK)
    {
        _stim->setReceivedHitType(RECEIVEDHITTYPE_EJECTY);
    }
    else if (punchType == PUNCHTYPE_UPPERKICK || m_punchUp)
    {
        _stim->setReceivedHitType(RECEIVEDHITTYPE_UPPUNCH);
    }
    else
    {
        _stim->setReceivedHitType(RECEIVEDHITTYPE_FRONTPUNCH);
    }
}

Vec3d Ray_BTActionHitTarget::getHitFxPos( const Vec3d& _default )
{
    if ( m_hitFxBoneIndex == U32_INVALID )
    {
        if ( getTemplate()->getHitFxBoneName().isValid() )
        {
            m_hitFxBoneIndex =  m_animComponent->getBoneIndex(getTemplate()->getHitFxBoneName());

            if ( m_hitFxBoneIndex != U32_INVALID )
            {
                return getBonePos(m_hitFxBoneIndex,_default);
            }
        }
    }
    else
    {
        return getBonePos(m_hitFxBoneIndex,_default);
    }

    return _default;
}

Vec3d Ray_BTActionHitTarget::getBonePos( u32 _index, const Vec3d& _default ) const
{
    AnimMeshScene* meshScene = m_animComponent->getAnimMeshScene();

    if ( meshScene )
    {
        SafeArray<AnimBoneDyn> & boneDynList = meshScene->m_AnimInfo.getWorkingBoneList();
        if ( _index < boneDynList.size() )
        {
            return boneDynList[_index].m_Pos.to3d(m_tree->getActor()->getPos().m_z);
        }
    }

    return _default;
}

void Ray_BTActionHitTarget::onEvent( Event* _event )
{
    Super::onEvent(_event);

    if ( AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
    {
        if (animEvent->getName() == MRK_Perform_Hit)
        {
            m_hitQueue.clear();
            processPerformHit();
            m_preparingHit = bfalse;
            m_performingHit = bfalse;
            m_memorizedHits.clear();
        }
        else if ( animEvent->getName() == MRK_Perform_Hit_Start )
        {
            m_hitQueue.clear();
            processPerformHit();
            m_preparingHit = bfalse;
            m_performingHit = btrue;
        }
        else if ( animEvent->getName() == MRK_Perform_Hit_Stop )
        {
            m_preparingHit = bfalse;
            m_performingHit = bfalse;
            m_memorizedHits.clear();
        }
        else if (animEvent->getName() == MRK_ComboHit_Next)
        {
            m_comboHitNext = btrue;
        }
    }
    else if ( AnimPolylineEvent* animPolyEvent = _event->DynamicCast<AnimPolylineEvent>(ITF_GET_STRINGID_CRC(AnimPolylineEvent,4083589773)) )
    {
        if (animPolyEvent->getName() == MRK_Prepare_Hit )
        {
            m_hitQueue.clear();
            calculateHitShapeFromEvent(animPolyEvent);
            fillHitQueue(btrue,getBaseHitDir());
            m_preparingHit = bfalse;
            m_performingHit = bfalse;
        }
        else if (animPolyEvent->getName() == MRK_Prepare_Hit_Start )
        {
            m_hitQueue.clear();
            calculateHitShapeFromEvent(animPolyEvent);
            fillHitQueue(btrue,getBaseHitDir());
            m_preparingHit = btrue;
            m_performingHit = bfalse;
        }
        else if (animPolyEvent->getName() == MRK_Prepare_Hit_Stop )
        {
            m_preparingHit = bfalse;
            m_performingHit = bfalse;
        }
        else if (animPolyEvent->getName() == MRK_Perform_Hit)
        {
            m_hitQueue.clear();
            calculateHitShapeFromEvent(animPolyEvent);
            fillHitQueue(bfalse,getBaseHitDir());
            processPerformHit();
            m_preparingHit = bfalse;
            m_performingHit = bfalse;
            m_memorizedHits.clear();
        }
        else if ( animPolyEvent->getName() == MRK_Perform_Hit_Start )
        {
            m_hitQueue.clear();
            calculateHitShapeFromEvent(animPolyEvent);
            fillHitQueue(bfalse,getBaseHitDir());
            processPerformHit();
            m_preparingHit = bfalse;
            m_performingHit = btrue;
        }
        else if ( animPolyEvent->getName() == MRK_Perform_Hit_Stop )
        {
            m_preparingHit = bfalse;
            m_performingHit = bfalse;
            m_memorizedHits.clear();
        }
    }
}

void Ray_BTActionHitTarget::updateAnimInput()
{
    Super::updateAnimInput();

    m_animComponent->setInput(ITF_GET_STRINGID_CRC(CurrentPunchReleaseResult,2626363589),static_cast<u32>(m_currentPunchResult));
    m_animComponent->setInput(ITF_GET_STRINGID_CRC(CurrentPunchDistance,4131479995),m_currentPunchDistance);
    m_animComponent->setInput(ITF_GET_STRINGID_CRC(CurrentPunchCharging,2266672517),m_punchCharging);
    m_animComponent->setInput(ITF_GET_STRINGID_CRC(CurrentPunchUp,2820806605),m_punchUp);
    m_animComponent->setInput(ITF_GET_STRINGID_CRC(CurrentPunchType,3418379187),static_cast<u32>(getTemplate()->getAttack(m_currentAttack).m_punchType));
    m_animComponent->setInput(ITF_GET_STRINGID_CRC(PunchBurstStage,1704106144),m_burstStage);
}

IMPLEMENT_OBJECT_RTTI(Ray_BTActionHitTarget_Template);

BEGIN_SERIALIZATION_CHILD(Ray_BTActionHitTarget_Template)

    SERIALIZE_CONTAINER_OBJECT("attacks",m_attacks);
    SERIALIZE_MEMBER("fxBoneName",m_hitFXBoneName);

END_SERIALIZATION()

Ray_BTActionHitTarget_Template::Ray_BTActionHitTarget_Template()
{
}

BEGIN_SERIALIZATION_SUBCLASS(Ray_BTActionHitTarget_Template,AttackData)

    SERIALIZE_PUNCHTYPE("punchType",m_punchType);
    SERIALIZE_MEMBER("level",m_level);
    SERIALIZE_MEMBER("pushBackRadius",m_pushBackRadius);
    SERIALIZE_MEMBER("duration",m_duration);
    SERIALIZE_MEMBER("anim",m_anim);

END_SERIALIZATION()

Ray_BTActionHitTarget_Template::AttackData::AttackData()
: m_punchType(PUNCHTYPE_REPEATING)
, m_level(0)
, m_pushBackRadius(0.f)
, m_duration(0.f)
{
}

}