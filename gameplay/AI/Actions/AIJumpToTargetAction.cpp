

#include "precompiled_gameplay.h"

#ifndef _ITF_AIJUMPTOTARGETACTION_H_
#include "gameplay/ai/Actions/AIJumpToTargetAction.h"
#endif //_ITF_AIJUMPTOTARGETACTION_H_

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/animatedcomponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(AIJumpToTargetAction_Template)
    BEGIN_SERIALIZATION_CHILD(AIJumpToTargetAction_Template)

        SERIALIZE_MEMBER("duration",m_duration);
        SERIALIZE_MEMBER("height0",m_height0);
        SERIALIZE_MEMBER("heught1",m_height1);

    END_SERIALIZATION()

    AIJumpToTargetAction_Template::AIJumpToTargetAction_Template()
    : m_duration(1.f)
    , m_height0(2.f)
    , m_height1(1.f)
    {
        m_animDefaultAction = ITF_GET_STRINGID_CRC(JUMP,2374255179);
    }

    IMPLEMENT_OBJECT_RTTI(AIJumpToTargetAction)

    AIJumpToTargetAction::AIJumpToTargetAction()
    : m_physComponent(NULL)
    , m_jumpCounter(0.f)
    , m_targetPos(Vec3d::Zero)
    , m_startPos(Vec3d::Zero)
    , m_targetActor(ITF_INVALID_OBJREF)
    {
    }

    AIJumpToTargetAction::~AIJumpToTargetAction()
    {
    }

    void AIJumpToTargetAction::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();

        ITF_ASSERT(m_physComponent);

    }

    void AIJumpToTargetAction::onActivate()
    {
        Super::onActivate();

        m_physComponent->setDisabled(btrue);
        m_prevPos = m_actor->get2DPos();
        m_startPos = m_actor->getPos();
        m_jumpCounter = 0.f;

        if ( m_animatedComponent )
        {
            m_animatedComponent->resetTree();
        }
    }

    void AIJumpToTargetAction::onDeactivate()
    {
        Super::onActivate();

        m_physComponent->setDisabled(bfalse);
    }

    void AIJumpToTargetAction::update( f32 _delta )
    {
        f32 duration = getTemplate()->getDuration();

        m_jumpCounter = Min(m_jumpCounter+_delta,duration);

        Vec3d newP = getTargetPos();

        m_actor->setPos(newP);

        Vec2d newP2D = newP.truncateTo2D();
        Vec2d dif = newP2D - m_prevPos;

        m_physComponent->setSpeed(dif/LOGICDT);

        m_prevPos = newP2D;

        if ( m_jumpCounter == duration )
        {
            m_aiBehavior->onActionFinished();
        }
    }

    Vec3d AIJumpToTargetAction::getTargetPos() const
    {
        Vec3d targetP;

        if ( m_targetActor != ITF_INVALID_OBJREF )
        {
            Actor* target = AIUtils::getActor(m_targetActor);

            if ( target )
            {
                targetP = target->getPos();
            }
            else
            {
                targetP = m_targetPos;
            }
        }
        else
        {
            targetP = m_targetPos;
        }

        Vec3d bPoint0 = ( targetP + m_startPos ) * 0.5f;
        bPoint0 += Vec3d::Up * getTemplate()->getHeight0();
        Vec3d bPoint1 = targetP + (Vec3d::Up*getTemplate()->getHeight1());

        f32 t = m_jumpCounter / getTemplate()->getDuration();

        Vec3d pA = Interpolate(m_startPos,bPoint0,t);
        Vec3d pB = Interpolate(bPoint0,bPoint1,t);
        Vec3d pC = Interpolate(bPoint1,targetP,t);
        Vec3d pD = Interpolate(pA,pB,t);
        Vec3d pE = Interpolate(pB,pC,t);
        Vec3d currentP = Interpolate(pD,pE,t);

        return currentP;
    }
}

