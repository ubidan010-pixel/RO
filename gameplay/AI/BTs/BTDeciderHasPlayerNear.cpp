
#include "precompiled_gameplay.h"

#ifndef _ITF_BTDECIDERHASPLAYERNEAR_H_
#include "gameplay/ai/BTs/BTDeciderHasPlayerNear.h"
#endif // _ITF_BTDECIDERHASPLAYERNEAR_H_

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_DETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/DetectorComponent.h"
#endif //_ITF_DETECTORCOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(BTDeciderHasPlayerNear);

BTDeciderHasPlayerNear::BTDeciderHasPlayerNear()
{
}

BTDeciderHasPlayerNear::~BTDeciderHasPlayerNear()
{
}

bbool BTDeciderHasPlayerNear::decide( f32 _dt )
{
    ObjectRef closest;
    bbool useRadius = btrue;
 
    if ( getTemplate()->getDetectionArea().isValid() )
    {
        ObjectRef detectionAreaRef = ITF_INVALID_OBJREF;

        m_tree->getBlackBoard().getFact(getTemplate()->getDetectionArea(),detectionAreaRef);

        Actor* detectionActor = AIUtils::getActor(detectionAreaRef);

        if ( detectionActor )
        {
            useRadius = bfalse;
            closest = checkDetectionArea(detectionActor);
        }
    }
    
    if ( useRadius )
    {
        closest = checkRadius();
    }

    if ( getTemplate()->getFact().isValid() )
    {
        if ( closest == ITF_INVALID_OBJREF )
        {
            m_tree->getBlackBoard().removeFact(getTemplate()->getFact());
        }
        else
        {
            m_tree->getBlackBoard().setFact(getTemplate()->getFact(),closest);
        }
    }

    return Super::decide(_dt) && closest != ITF_INVALID_OBJREF;
}

ObjectRef BTDeciderHasPlayerNear::checkRadius()
{
    const DepthRange depthRange(m_tree->getActor()->getDepth());
    const u32 uPlayerCount = GAMEMANAGER->getMaxPlayerCount();
    f32 radiusCheckSq = getTemplate()->getRadius()*getTemplate()->getRadius();
    f32 minDistSq = 0.f;
    ObjectRef closest = ITF_INVALID_OBJREF;

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

        f32 distSq = ( pActor->get2DPos() - m_tree->getActor()->get2DPos() ).sqrnorm();

        if ( distSq < radiusCheckSq )
        {
            if ( closest == ITF_INVALID_OBJREF ||
                distSq < minDistSq )
            {
                closest = pActor->getRef();
                minDistSq = distSq;
            }
        }
    }

    return closest;
}

ObjectRef BTDeciderHasPlayerNear::checkDetectionArea( Actor* _detectionActor )
{
    ObjectRef closest = ITF_INVALID_OBJREF;

    if ( _detectionActor )
    {
        DetectorComponent* detectComponent = _detectionActor->GetComponent<DetectorComponent>();

        if ( detectComponent )
        {
            const ActorRefList& actorsInside = detectComponent->getActorsInside();
            u32 numActors = actorsInside.size();
            f32 minDistSq = 0;

            for ( u32 i = 0; i < numActors; i++ )
            {
                ActorRef refActor = actorsInside[i];

                if ( !GAMEMANAGER->isPlayerActor(refActor) )
                {
                    continue;
                }

                Actor* pActor = refActor.getActor();

                f32 distSq = ( pActor->get2DPos() - m_tree->getActor()->get2DPos() ).sqrnorm();

                if ( closest == ITF_INVALID_OBJREF ||
                     distSq < minDistSq )
                {
                    closest = pActor->getRef();
                    minDistSq = distSq;
                }
            }
        }
    }

    return closest;
}

IMPLEMENT_OBJECT_RTTI(BTDeciderHasPlayerNear_Template);

BEGIN_SERIALIZATION_CHILD(BTDeciderHasPlayerNear_Template)

    SERIALIZE_MEMBER("fact",m_fact);
    SERIALIZE_MEMBER("radius",m_radius);
    SERIALIZE_MEMBER("detectionArea",m_detectionArea);

END_SERIALIZATION()

}
