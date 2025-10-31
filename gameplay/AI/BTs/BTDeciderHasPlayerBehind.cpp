
#include "precompiled_gameplay.h"

#ifndef _ITF_BTDECIDERHASPLAYERBEHIND_H_
#include "gameplay/ai/BTs/BTDeciderHasPlayerBehind.h"
#endif // _ITF_BTDECIDERHASPLAYERBEHIND_H_

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_AIBTFACTS_H_
#include "gameplay/ai/BTs/AIBTFacts.h"
#endif //_ITF_AIBTFACTS_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(BTDeciderHasPlayerBehind);

BTDeciderHasPlayerBehind::BTDeciderHasPlayerBehind()
{
}

BTDeciderHasPlayerBehind::~BTDeciderHasPlayerBehind()
{
}

bbool BTDeciderHasPlayerBehind::validate( Actor* _actor ) const
{
    bbool retVal = Super::validate(_actor);

    if ( !retVal )
    {
        return bfalse;
    }

    if ( !getTemplate()->getFactDir().isValid() )
    {
        ITF_WARNING_PARAM(_actor,"factDir",0,"%s: the fact to retrieve the direction of the level is not set",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    return btrue;
}

bbool BTDeciderHasPlayerBehind::decide( f32 _dt )
{
    const DepthRange depthRange(m_tree->getActor()->getDepth());
    const u32 uPlayerCount = GAMEMANAGER->getMaxPlayerCount();
    f32 radiusCheckSq = getTemplate()->getRadius()*getTemplate()->getRadius();
    bbool checkAll = getTemplate()->getCheckAll();
    bbool invertCheck = getTemplate()->getInvert();
    u32 numPlayersOnRightSide = 0;
    u32 numPlayersValid = 0;
    f32 minDistFrontSq = 0.f;
    f32 minDistBehindSq = 0.f;
    ObjectRef closestBehind = ITF_INVALID_OBJREF;
    ObjectRef closestFront = ITF_INVALID_OBJREF;
    Vec2d levelDir;
    Vec2d dirToActor;

    getLevelDir(levelDir);

    for (u32 i = 0; i < uPlayerCount; ++i)
    {
        Actor* pActor = GAMEMANAGER->getActivePlayer(i);

        if ( !pActor || depthRange != DepthRange(pActor->getDepth()) )
        {
            continue;
        }

        EventQueryIsDead query;
        pActor->onEvent(&query);
        if (query.getIsDead())
        {
            continue;
        }

        numPlayersValid++;

        dirToActor = pActor->get2DPos() - m_tree->getActor()->get2DPos();
        f32 distSq = dirToActor.sqrnorm();

        f32 dirDist = levelDir.dot(dirToActor);

        if ( dirDist > 0.f )
        {
            bbool isValid = bfalse;

            if ( invertCheck || distSq < radiusCheckSq )
            {
                if ( getTemplate()->getMargin() )
                {
                    if ( invertCheck )
                    {
                        if ( dirDist >= getTemplate()->getMargin() )
                        {
                            isValid = btrue;
                        }
                    }
                }
                else
                {
                    isValid = btrue;
                }
            }

            if ( isValid )
            {
                numPlayersOnRightSide++;
            }

            if ( closestFront == ITF_INVALID_OBJREF ||
                 distSq < minDistFrontSq )
            {
                closestFront = pActor->getRef();
                minDistFrontSq = distSq;
            }
        }
        else
        {
            bbool isValid = bfalse;

            if ( !invertCheck || distSq < radiusCheckSq )
            {
                if ( getTemplate()->getMargin() )
                {
                    if ( !invertCheck )
                    {
                        if ( (-dirDist) >= getTemplate()->getMargin() )
                        {
                            isValid = btrue;
                        }
                    }
                }
                else
                {
                    isValid = btrue;
                }
            }

            if ( isValid )
            {
                numPlayersOnRightSide++;
            }

            if ( closestBehind == ITF_INVALID_OBJREF ||
                 distSq < minDistBehindSq )
            {
                closestBehind = pActor->getRef();
                minDistBehindSq = distSq;
            }
        }
    }

    m_tree->getBlackBoard().setFact(AIFact_infrontPlayer,closestFront);
    m_tree->getBlackBoard().setFact(AIFact_behindPlayer,closestBehind);

    bbool retOk = Super::decide(_dt);

    return retOk &&
           ( ( !checkAll && numPlayersOnRightSide ) ||
             ( checkAll && numPlayersOnRightSide == numPlayersValid ) );
}

void BTDeciderHasPlayerBehind::getLevelDir( Vec2d& _levelDir ) const
{
    Vec2d targetPos = Vec2d::Zero;
    ObjectRef targetActor = ITF_INVALID_OBJREF;

    m_tree->getBlackBoard().getFact(getTemplate()->getFactDir(),targetActor);

    if ( targetActor != ITF_INVALID_OBJREF )
    {
        Actor* actor = AIUtils::getActor(targetActor);

        if ( actor )
        {
            targetPos = actor->get2DPos();
            _levelDir = targetPos - m_tree->getActor()->get2DPos();
            _levelDir.normalize();
        }
        else
        {
            _levelDir = AIUtils::getLookDir(m_tree->getActor(),m_tree->getActor()->isFlipped());
        }
    }
    else
    {
        _levelDir = AIUtils::getLookDir(m_tree->getActor(),m_tree->getActor()->isFlipped());
    }
}

IMPLEMENT_OBJECT_RTTI(BTDeciderHasPlayerBehind_Template);

BEGIN_SERIALIZATION_CHILD(BTDeciderHasPlayerBehind_Template)

    SERIALIZE_MEMBER("radius",m_radius);
    SERIALIZE_MEMBER("margin",m_margin);
    SERIALIZE_MEMBER("invert",m_invert);
    SERIALIZE_MEMBER("checkAll",m_checkAll);
    SERIALIZE_MEMBER("factDir",m_factDir);

END_SERIALIZATION()

}
