#include "precompiled_gameplay_war.h"

#ifndef _ITF_WAR_AIPERFORMHITSTIMACTION_H_
#include "war/gameplay/ai/Actions/War_AIPerformHitStimAction.h"
#endif //_ITF_WAR_AIPERFORMHITSTIMACTION_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(War_AIPerformHitStimAction)

    BEGIN_SERIALIZATION_CHILD(War_AIPerformHitStimAction)
        SERIALIZE_FUNCTION(clear, ESerialize_Template_Load);
        SERIALIZE_OBJECT_WITH_FACTORY("stimShape", m_stimShape, PHYSWORLD->getShapesFactory(), ESerialize_PropertyEdit|ESerialize_Template);
    END_SERIALIZATION()

    War_AIPerformHitStimAction::War_AIPerformHitStimAction()
        : Super()

        , m_stimShape(NULL)
    {
    }

    War_AIPerformHitStimAction::~War_AIPerformHitStimAction()
    {
    }

    void War_AIPerformHitStimAction::clear()
    {
        SF_DEL(m_stimShape);
    }

    void War_AIPerformHitStimAction::performHit( AnimGameplayEvent* _animEvent )
    {
        War_HitStim* stim = STIMSMANAGER->requestStim<War_HitStim>(m_stimShape);

#if defined(ITF_FINAL)
        if(stim)
#endif // ITF_FINAL
        {
            const Vec2d& stimPosition = m_actor->get2DPos();

            stim->setPrevPos(stimPosition);
            stim->setPos(stimPosition);
            stim->setAngle(m_actor->getAngle());
            stim->setDepth(m_actor->getDepth());
            stim->setSender(m_actor->getRef());

            stim->setLevel(0);
            stim->setReceivedHitType(WAR_RECEIVEDHITTYPE_NORMALHIT);
            stim->setFxPos(m_actor->getPos());

            STIMSMANAGER->sendStim(stim);
        }
    }

}