
#include "precompiled_gameplay.h"

#ifndef _ITF_BTACTIONJUMPTOTARGET_H_
#include "gameplay/ai/BTs/BTActionJumpTotarget.h"
#endif // _ITF_BTACTIONJUMPTOTARGET_H_

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

#ifndef _ITF_GROUNDAICONTROLLERCOMPONENT_H_
#include "gameplay/Components/AI/GroundAIControllerComponent.h"
#endif //_ITF_GROUNDAICONTROLLERCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(BTActionJumpToTarget);

BTActionJumpToTarget::BTActionJumpToTarget()
: m_aiController(NULL)
, m_jumpCounter(0.f)
, m_prevPos(Vec2d::Zero)
{
}

void BTActionJumpToTarget::init( class BehaviorTree* _tree, const class BTNode_Template* _template )
{
    Super::init(_tree,_template);

    m_aiController = m_tree->getActor()->GetComponent<GroundAIControllerComponent>();
}

bbool BTActionJumpToTarget::validate( Actor* _actor ) const
{
    bbool retValue = Super::validate(_actor);

    if ( !retValue )
    {
        return bfalse;
    }

    if ( !m_aiController )
    {
        ITF_WARNING_PARAM(_actor,"groundAiController",m_aiController!=NULL,"%s: physics component is mandatory",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    if ( !getTemplate()->getFactTargetActor().isValid() && !getTemplate()->getFactTargetPos().isValid() )
    {
        ITF_WARNING_PARAM(_actor,"factTargetActor/factTargetPos",0,"%s: the fact to retrieve the position is not set",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    return btrue;
}

bbool BTActionJumpToTarget::decide( f32 _dt )
{
    bbool actorExists = getTemplate()->getFactTargetActor().isValid() && m_tree->getBlackBoard().factExists(getTemplate()->getFactTargetActor());
    bbool posExists = getTemplate()->getFactTargetPos().isValid() && m_tree->getBlackBoard().factExists(getTemplate()->getFactTargetPos());

    if ( !actorExists && !posExists )
    {
        return bfalse;
    }

    Vec3d targetP = getTargetPos();

    f32 distToTargetSq = ( m_tree->getActor()->getPos() - targetP ).sqrnorm();

    return distToTargetSq > (0.1f*0.1f);
}

void BTActionJumpToTarget::onActivate()
{
    Super::onActivate();

    if ( getTemplate()->getAnim().isValid() )
    {
        AnimLightComponent* animLight = m_tree->getActor()->GetComponent<AnimLightComponent>();

        if ( animLight )
        {
            animLight->setAnim(getTemplate()->getAnim());

            AnimatedComponent* animated = animLight->DynamicCast<AnimatedComponent>(ITF_GET_STRINGID_CRC(AnimatedComponent,1654726928));

            if ( animated )
            {
                animated->resetTree();
            }
        }
    }

    m_prevTarget = getTargetPos();
}

bbool BTActionJumpToTarget::update( f32 _dt )
{
    Super::update(_dt);

    Vec3d targetP = getTargetPos();

    if ( targetP == Vec3d::Zero )
    {
        return bfalse;
    }

    bbool finished = m_tree->getActor()->getPos().IsEqual(m_prevTarget,MTH_EPSILON);

    m_aiController->jumpToTarget(targetP);
    m_prevTarget = targetP;

    return !finished;
}

Vec3d BTActionJumpToTarget::getTargetPos() const
{
    Vec3d targetP = Vec3d::Zero;
    ObjectRef targetActor = ITF_INVALID_OBJREF;

    if ( getTemplate()->getFactTargetActor().isValid() )
    {
        m_tree->getBlackBoard().getFact(getTemplate()->getFactTargetActor(),targetActor);
    }

    if ( targetActor != ITF_INVALID_OBJREF )
    {
        Actor* target = AIUtils::getActor(targetActor);

        if ( target )
        {
            targetP = target->getPos();
        }
        else
        {
            if ( getTemplate()->getFactTargetPos().isValid() )
            {
                Vec2d target2d;
                m_tree->getBlackBoard().getFact(getTemplate()->getFactTargetPos(),target2d);
                targetP = target2d.to3d(m_tree->getActor()->getPos().m_z);
            }
        }
    }
    else
    {
        if ( getTemplate()->getFactTargetPos().isValid() )
        {
            Vec2d target2d;
            m_tree->getBlackBoard().getFact(getTemplate()->getFactTargetPos(),target2d);
            targetP = target2d.to3d(m_tree->getActor()->getPos().m_z);
        }
    }

    return targetP;
}

IMPLEMENT_OBJECT_RTTI(BTActionJumpToTarget_Template);

BEGIN_SERIALIZATION_CHILD(BTActionJumpToTarget_Template)

    SERIALIZE_MEMBER("anim",m_anim);
    SERIALIZE_MEMBER("factTargetActor",m_factTargetActor);
    SERIALIZE_MEMBER("factTargetPos",m_factTargetPos);

END_SERIALIZATION()

BTActionJumpToTarget_Template::BTActionJumpToTarget_Template()
{
}

}