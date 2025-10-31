#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIUTILS_H_
#include "Rayman/gameplay/AI/Utils/Ray_AIUtils.h"
#endif //_ITF_RAY_AIUTILS_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_RAY_GAMEMATERIAL_H_
#include "rayman/gameplay/Ray_GameMaterial.h"
#endif //_ITF_RAY_GAMEMATERIAL_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_RAY_FIXEDAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_FixedAIComponent.h"
#endif //_ITF_RAY_FIXEDAICOMPONENT_H_

#ifndef _ITF_RAY_AISCORELUMSBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Lums/Ray_AIScoreLumBehavior.h"
#endif //_ITF_RAY_AISCORELUMSBEHAVIOR_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_PERSISTENTGAMEDATA_H_
#include "rayman/gameplay/Ray_PersistentGameData.h"
#endif //_ITF_RAY_PERSISTENTGAMEDATA_H_

#ifndef _ITF_RAY_CAGEAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_CageAIComponent.h"
#endif //_ITF_RAY_CAGEAICOMPONENT_H_

#ifndef _ITF_RAY_SCORELUMAICOMPONENT_H_
#include "rayman/gameplay/Components/Lums/Ray_ScoreLumAIComponent.h"
#endif //_ITF_RAY_SCORELUMAICOMPONENT_H_

namespace ITF
{

namespace Ray_AIUtils
{

const PolyLineEdge * getSquashingPolyline(const Actor * _actor,ObjectRef _currentStickedPolylineRef, const PolyLineEdge* _currentEdge, f32 _radius, f32 _penetrationRadius )
{
    PhysShapeCircle circle(_penetrationRadius);
    PhysContactsContainer contacts;

    PHYSWORLD->checkEncroachment(_actor->get2DPos(),_actor->get2DPos(),_actor->getAngle(),
        &circle,ECOLLISIONFILTER_ENVIRONMENT,_actor->getDepth(),contacts);

    if ( contacts.empty() )
    {
        return NULL;
    }

    bbool solidPenetrating = bfalse;
    PolyLine* crashingPoly = NULL;
    const PolyLineEdge* crashingEdge = NULL;
    u32 crashingEdgeIndex = U32_INVALID;

    for ( u32 index = 0; index < contacts.size(); index++ )
    {
        const SCollidableContact& contact = contacts[index];

        if ( contact.m_edgeIndex == U32_INVALID )
        {
            // If it's not a polyline we pass
            continue;
        }

        PolyLine* polyLine;
        const PolyLineEdge* edge;

        AIUtils::getPolyLine(contact.m_collidableUserData,contact.m_edgeIndex,
            polyLine,edge);

        if ( !polyLine || !edge )
        {
            continue;
        }

        if (polyLine && polyLine->getOwnerActor() == _actor)
            continue;

        const Ray_GameMaterial_Template* mat = static_cast<const Ray_GameMaterial_Template*>(World::getGameMaterial(edge->getGameMaterial()));
        bbool isSolid = !mat || mat->isSolid();

        if ( !isSolid && _currentEdge != edge )
        {
            // If it's not solid and we are not sticked to it we ignore it
            continue;
        }

        if ( mat && mat->isNoSquashDamage() )
        {
            // Skip these edges
            continue;
        }

        if (polyLine->getOwnerActor())
        {
            EventQueryCanStickOrCollide queryCanStick;
            queryCanStick.setSender(_actor->getRef());
            polyLine->getOwnerActor()->onEvent(&queryCanStick);
            if (!queryCanStick.canCollide())
                return nullptr;
        }

        Vec2d edgeNormal = edge->m_normalizedVector.getPerpendicular();
        bbool movingPolyLine = polyLine->m_physShape && polyLine->m_physShape->DynamicCast<PhysShapeMovingPolyline>(ITF_GET_STRINGID_CRC(PhysShapeMovingPolyline,309826108));

        /*
        if ( !movingPolyLine )
        {
            if ( contact.m_normal.dot(edgeNormal) > 0 )
            {
                // If the normal of the edge goes in the direction of the normal then we skip it
                continue;
            }
        }
        */

        // A solid polyline must be penetrating our collision circle
        if ( isSolid && !solidPenetrating )
        {
            bbool penetrates = bfalse;

            if ( movingPolyLine )
            {
                if ( polyLine->getRef() == _currentStickedPolylineRef )
                {
                    continue;
                }

                const PhysShapeMovingPolyline* movingPoly = static_cast<const PhysShapeMovingPolyline*>(polyLine->m_physShape);
                const PolyLineEdge& prevEdge = movingPoly->getPreviousPolyline()->getEdgeAt(contact.m_edgeIndex);
                Vec2d vecToPosA = _actor->get2DPos() - prevEdge.getPos();
                Vec2d vecToPosB = _actor->get2DPos() - edge->getPos();

                f32 distA = vecToPosA.dot(prevEdge.m_normalizedVector);
                f32 distB = vecToPosB.dot(edge->m_normalizedVector);

                distA = Clamp(distA,0.f,prevEdge.m_length);
                distB = Clamp(distB,0.f,edge->m_length);

                Vec2d pA = prevEdge.getPos() + ( prevEdge.m_normalizedVector * distA );
                Vec2d pB = edge->getPos() + ( edge->m_normalizedVector * distB );

                vecToPosA = pA - _actor->get2DPos();
                vecToPosB = pB - _actor->get2DPos();
                f32 distToPointSq = vecToPosB.sqrnorm();

                vecToPosA.normalize();

                if ( vecToPosA.dot(prevEdge.m_normalizedVector.getPerpendicular()) < -MTH_EPSILON )
                {
                    vecToPosB.normalize();

                    f32 dotProd = vecToPosA.dot(vecToPosB);

                    if ( dotProd < -MTH_EPSILON )
                    {
                        penetrates = btrue;
                    }
                }
                
                if ( penetrates == bfalse )
                {
                    if ( distToPointSq <= (_penetrationRadius*_penetrationRadius) )
                    {
                        penetrates = btrue;
                    }
                }
            }
            else
            {
                penetrates = btrue;
            }

            if ( penetrates )
            {
                solidPenetrating = btrue;
                crashingEdge = edge;
                crashingPoly = polyLine;
                crashingEdgeIndex = contact.m_edgeIndex;
                break;
            }
        }
    }
    return crashingEdge;
}


void getSoftCollSweepInfo(Actor * _actor, f32 _radius, PhysSweepInfo & _info, AABB & _aabb)
{
    PhysShapeCircle thisPlayerShape;
    thisPlayerShape.setRadius(_radius);
    thisPlayerShape.calculateAABB(_actor->get2DPos(), _actor->get2DPos(), _actor->getAngle(), _aabb);
    PhysCollisionSolver::calculateSweepInfo(_actor->get2DPos(),_actor->get2DPos(),_actor->getAngle(),&thisPlayerShape,_info);
}

bbool hasSoftColl(Actor * _to, f32 _toRadius, Actor * _from, f32 _fromRadius)
{

    PhysSweepInfo toInfo,fromInfo;
    AABB toAABB, fromAABB;

    getSoftCollSweepInfo(_to, _toRadius,toInfo, toAABB);
    getSoftCollSweepInfo(_from, _fromRadius ,fromInfo, fromAABB);

    PhysContactsContainer contacts;
    PhysCollisionSolver::collide(toInfo,fromInfo,contacts);

    if ( contacts.size() > 0 )
    {
        return btrue;
    }
    return bfalse;
}

Vec2d getSoftCollForce( Actor* _to, const Vec2d& _fromPos, const f32 _fromRadius, const f32 _exitSpeed, const f32 _exitForce )
{
    const PhysComponent* toPhys = _to->GetComponent<PhysComponent>();
    if (!toPhys)
    {
        return Vec2d::Zero;
    }


    Ray_EventQueryAIData queryTo;
    _to->onEvent(&queryTo);

    if (queryTo.getSoftCollisionRadius() == 0.0f)
    {
        return Vec2d::Zero;
    }


    const f32 totalRadius = queryTo.getSoftCollisionRadius() + _fromRadius;
    const f32 sqrTotalRadius = sqr(totalRadius);

    const Vec2d toPlayer = _to->get2DPos() - _fromPos;
    const f32 sqrDistToPlayer = toPlayer.sqrnorm();
    if (sqrDistToPlayer > sqrTotalRadius)
    {
        return Vec2d::Zero;
    }

    const Vec2d dir = toPlayer / sqrtf(sqrDistToPlayer);

    const f32 distRatio = 1.0f - sqrDistToPlayer/sqrTotalRadius;
    const f32 speedDotP = toPhys->getSpeed().dot(dir);
    const f32 speedRatio = speedDotP < 0.0f ? 1.0f : 1.0f - Clamp(speedDotP/_exitSpeed,0.0f,1.0f);

    return dir * _exitForce * distRatio * speedRatio;
}

Ray_ScoreLumAIComponent* fetchScoreLumComponent()
{
    Actor *actor = (Actor*)Ray_AIScoreLumBehavior::getInstance().getObject();
    if (actor)
    {
        Ray_ScoreLumAIComponent *component = actor->GetComponent<Ray_ScoreLumAIComponent>();
        return component;
    }
    return NULL;
}

Ray_AIScoreLumBehavior* fetchScoreLumBehavior()
{
    Actor *actor = (Actor*)Ray_AIScoreLumBehavior::getInstance().getObject();
    if (actor)
    {
        Ray_FixedAIComponent *component = actor->GetComponent<Ray_ScoreLumAIComponent>();
        if (!component)
        {
            //compatibility fallback
            component = actor->GetComponent<Ray_FixedAIComponent>();
        }
        if (component)
        {
            AIBehavior *behavior0 = component->getCurrentBehavior();
            if (behavior0)
            {
                Ray_AIScoreLumBehavior *behavior = behavior0->DynamicCast<Ray_AIScoreLumBehavior>(ITF_GET_STRINGID_CRC(Ray_AIScoreLumBehavior,2156335494));
                if (behavior)
                {
                    return behavior;
                }
            }
        }
    }
    return NULL;
}

Vec2d getRadialConstrainedDir(const Vec2d& _from, const Vec2d& _to)
{
    Vec2d dir = _to - _from;
    dir.normalize();
    
    f32 angle = 0.0f;
    if (!dir.IsEqual(Vec2d::Zero))
    {
        angle = dir.getAngle();
    }

    f32 constrainedAngle = 0.0f;
    if (angle < MTH_PIBY8)
    {
        constrainedAngle = 0;
    }
    else if (angle < MTH_PIBY2)
    {
        constrainedAngle = MTH_PIBY4;
    }
    else if (angle < (7.0f * MTH_PIBY8))
    {
        constrainedAngle = 3.0f * MTH_PIBY4;
    }
    else if (angle < (9.0f * MTH_PIBY8))
    {
        constrainedAngle = MTH_PI;
    }
    else if (angle < (3.0f * MTH_PIBY2))
    {
        constrainedAngle = 5.0f * MTH_PIBY4;
    }
    else if (angle < (15.0f * MTH_PIBY8))
    {
        constrainedAngle = 7.0f * MTH_PIBY4;
    }
    else
    {
        ITF_ASSERT(angle <= MTH_2PI);
        constrainedAngle = 0;
    }

    Vec2d constrainedDir = Vec2d::Right.Rotate(constrainedAngle);
    constrainedDir.normalize();
    
    return constrainedDir;
}

void sendRadialConstrainedPunchStim(const Actor* _from, Actor* _to)
{
    Vec2d punchDir = getRadialConstrainedDir(_from->get2DPos(), _to->get2DPos());

    PunchStim punch;
    punch.setLevel(1);
    punch.setReceivedHitType(RECEIVEDHITTYPE_HURTBOUNCE);
    punch.setDirection(punchDir);
    punch.setFxPos(_from->getPos());
    punch.setDepth(_from->getDepth());
    punch.setSender(_from->getRef());
    punch.setAngle(_from->getAngle());
    punch.setPos(_from->getPos().truncateTo2D());
    punch.setPrevPos(_from->getPos().truncateTo2D());
    punch.setFaction(AIUtils::getFaction(_from));
    _to->onEvent(&punch);    
}

bbool setupNoseShape(
    const Actor* _actor,
    const AnimLightComponent* _animComponent,
    const StringID& _nosePolyline,
    PhysShapePolygon& _noseShape,
    PhysShapePolygon::PointsList& _noseShapePoints,
    PhysShapePolygon::PointsList& _prevNosePoints
    )
{
    u32 numPoints;
    const Vec2d* points = _animComponent->getCurrentPolylinePointBuffer(_nosePolyline, &numPoints);

    ITF_ASSERT(points);
    if (!points)
    {
        return bfalse;
    }

    ITF_ASSERT(numPoints > 1);
    if ( numPoints <= 1 )
    {
        return bfalse;
    }

    if (_prevNosePoints.size() == 0)
    {
        for (u32 i=0; i<numPoints; ++i)
        {
            _prevNosePoints.push_back(points[i]);
        }
        return bfalse;
    }

    _noseShapePoints.clear();

    if ( !_actor->isFlipped() )
    {
        for (u32 i=0; i<numPoints; ++i)
        {
            _noseShapePoints.push_back( ( points[i] - _actor->get2DPos() ).Rotate(-_actor->getAngle()) );
        }
        for (u32 i=0; i<numPoints; ++i)
        {
            _noseShapePoints.push_back( ( _prevNosePoints[numPoints-i-1] - _actor->get2DPos() ).Rotate(-_actor->getAngle()) );
        }
    }
    else
    {
        for (u32 i=0; i<numPoints; ++i)
        {
            _noseShapePoints.push_back( ( _prevNosePoints[i] - _actor->get2DPos() ).Rotate(-_actor->getAngle()) );
        }
        for (u32 i=0; i<numPoints; ++i)
        {
            _noseShapePoints.push_back( ( points[numPoints-i-1] - _actor->get2DPos() ).Rotate(-_actor->getAngle()) );
        }
    }

    _noseShape.setPoints(_noseShapePoints);

    for (u32 i=0; i<numPoints; ++i)
    {
        _prevNosePoints[i] = points[i];
    }
    
    return btrue;
}

bbool isBubblePrize(const Actor* _actor)
{
    // NB[LaurentCou]: not really efficient, if this becomes bottleneck we could
    // use a custom faction or some other kind of tag to distinguish bubble
    // prizes from the common crowd
    if (const AIComponent* aiComponent = _actor->GetComponent<AIComponent>())
    {
        return (NULL != aiComponent->getBehaviorFromName(ITF_GET_STRINGID_CRC(BubblePrizeBehavior,1770039327)));
    }

    return bfalse;
}

u32 computeElectoonMask(const StringID &_missionType, const Ray_PersistentGameData_Level *_level, Actor *_ignoredCage, u32 _ignoreChallenges, bbool & _isComplete)
{
    const Ray_GameManagerConfig_Template::MissionConfig *config = RAY_GAMEMANAGER->findMissionConfigByType(_missionType, btrue);
    u32 mask = 0;
    _isComplete = bfalse;

    i32 ignoredCageIndex = -1;
    if (_ignoredCage)
    {
        Ray_CageAIComponent *cageComponent = _ignoredCage->GetComponent<Ray_CageAIComponent>();
        if (cageComponent)
        {
            ignoredCageIndex = cageComponent->getCageIndex();
        }
    }

    u32 lumAttackIndex=0;
    if (config)
    {
        //iterate medal slots and set related mask
        const ITF_VECTOR<Ray_GameManagerConfig_Template::MedalSlotConfig> &slots = config->m_medalSlots;
        ITF_VECTOR<Ray_GameManagerConfig_Template::MedalSlotConfig>::const_iterator iter;
        u32 slotMask = 1;
        u32 unlockedSlotCount = 0;
        for (iter=slots.begin(); iter!=slots.end(); iter++, slotMask = (slotMask<<1))
        {
            const Ray_GameManagerConfig_Template::MedalSlotConfig &slot = *iter;
            switch(slot.m_type)
            {
            case Ray_GameManagerConfig_Template::MedalSlotConfig::Cage:
                {
                    //Cages : set mask
                    if (slot.m_value != ignoredCageIndex && _level->isCageBroken((u32)slot.m_value))
                    {
                        mask |= slotMask;
                        unlockedSlotCount++;
                    }
                } break;
            case Ray_GameManagerConfig_Template::MedalSlotConfig::Boss:
                {
                    if (_level->isChallengeWon(CHALLENGE_BEATENBOSS) && !(_ignoreChallenges&CHALLENGE_BEATENBOSS))
                    {
                        mask |= slotMask;
                        unlockedSlotCount++;
                    }
                }break;
            case Ray_GameManagerConfig_Template::MedalSlotConfig::LumAttack:
                {
                    EChallengeType challenge = (lumAttackIndex==0)?CHALLENGE_LUMATTACK1:CHALLENGE_LUMATTACK2;
                    if (_level->isChallengeWon(challenge) && !(_ignoreChallenges&(u32)challenge))
                    {
                        mask |= slotMask;
                        unlockedSlotCount++;
                    }
                    lumAttackIndex++;
                } break;
            case Ray_GameManagerConfig_Template::MedalSlotConfig::TimeAttack:
                {
                    if (_level->isChallengeWon(CHALLENGE_TIMEATTACK) && !(_ignoreChallenges&CHALLENGE_TIMEATTACK))
                    {
                        mask |= slotMask;
                        unlockedSlotCount++;
                    }
                } break;
            }
        }

        _isComplete = (unlockedSlotCount==slots.size());
    }

    return mask;
}

void showBossMedal( ActorRef _cineRef, ActorRef _sender )
{
    {
        Ray_EventShowElectoonMedal showMedal;

        showMedal.m_forBrokenCage       = bfalse;
        showMedal.m_playVictorySequence = bfalse;
        showMedal.m_forBeatenBoss       = btrue;
        showMedal.m_playEndSequence     = btrue;
        showMedal.m_autoEndLevel        = btrue;
        showMedal.m_forLumAndTimeAttacks = bfalse;
        showMedal.m_dontSetLumAttack1   = btrue;
        showMedal.m_dontSetLumAttack2   = btrue;
        showMedal.m_dontSetTimeAttack   = btrue;
        showMedal.m_customCineRef       = _cineRef;

        showMedal.setSender(_sender);

        EVENTMANAGER->broadcastEvent(&showMedal);
    }
}

} // namespace Ray_AIUtils

} // namespace ITF
