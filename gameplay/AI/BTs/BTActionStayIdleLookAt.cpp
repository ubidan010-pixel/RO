
#include "precompiled_gameplay.h"

#ifndef _ITF_BTACTIONSTAYIDLELOOKAT_H_
#include "gameplay/ai/BTs/BTActionStayIdleLookAt.h"
#endif // _ITF_BTACTIONSTAYIDLELOOKAT_H_

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(BTActionStayIdleLookAt);

BTActionStayIdleLookAt::BTActionStayIdleLookAt()
: m_doingUTurn(bfalse)
{
}

bbool BTActionStayIdleLookAt::validate( Actor* _actor ) const
{
    bbool retVal = Super::validate(_actor);

    if ( !retVal )
    {
        return bfalse;
    }

    if ( !getTemplate()->getPickableFactName().isValid() &&
         !getTemplate()->getPosFactName().isValid() )
    {
        ITF_WARNING_PARAM(_actor,"Facts",0,"%s: needs to set a pickable fact or a position fact to look at",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    return btrue;
}

void BTActionStayIdleLookAt::onActivate()
{
    Super::onActivate();

    m_doingUTurn = bfalse;
}

bbool BTActionStayIdleLookAt::update( f32 _dt )
{
    if ( !decide(_dt) )
    {
        return bfalse;
    }

    Super::update(_dt);

    if ( m_doingUTurn )
    {
        if ( m_animComponent->isSubAnimFinished() )
        {
            m_doingUTurn = bfalse;
            m_tree->getActor()->setFlipped(!m_tree->getActor()->isFlipped());
        }
    }
    else
    {
        lookAt();
    }

    return btrue;
}

void BTActionStayIdleLookAt::lookAt()
{
    Vec2d lookAtTargetPos;
    ObjectRef lookAtTargetPickable = ITF_INVALID_OBJREF;

    if ( getTemplate()->getPickableFactName().isValid() && m_tree->getBlackBoard().getFact(getTemplate()->getPickableFactName(),lookAtTargetPickable) )
    {
        BaseObject* obj = GETOBJECT(lookAtTargetPickable);

        if ( obj && obj->DynamicCast<Pickable>(ITF_GET_STRINGID_CRC(Pickable,594660361)) )
        {
            Pickable* pick = static_cast<Pickable*>(obj);

            lookAtTargetPos = pick->get2DPos();
        }
        else if ( getTemplate()->getPosFactName().isValid() && !m_tree->getBlackBoard().getFact(getTemplate()->getPosFactName(),lookAtTargetPos) )
        {
            return;
        }
    }
    else if ( getTemplate()->getPosFactName().isValid() && !m_tree->getBlackBoard().getFact(getTemplate()->getPosFactName(),lookAtTargetPos) )
    {
        return;
    }

    Vec2d dirToTarget = lookAtTargetPos - m_tree->getActor()->get2DPos();
    f32 xDir = m_physComponent->getCurrentGravityDir().getPerpendicular().dot(dirToTarget);
    bbool lookLeft = xDir < 0.f;

    if ( lookLeft != m_tree->getActor()->isFlipped() )
    {
        m_doingUTurn = btrue;
    }
}

void BTActionStayIdleLookAt::updateAnimInput()
{
    Super::updateAnimInput();

    if ( AnimatedComponent* animComponent = m_animComponent->DynamicCast<AnimatedComponent>(ITF_GET_STRINGID_CRC(AnimatedComponent,1654726928)) )
    {
        animComponent->setInput(ITF_GET_STRINGID_CRC(performingUTurn,1740082645),m_doingUTurn);
    }
}

IMPLEMENT_OBJECT_RTTI(BTActionStayIdleLookAt_Template);

BEGIN_SERIALIZATION_CHILD(BTActionStayIdleLookAt_Template)

    SERIALIZE_MEMBER("pickableFact",m_pickableFact);
    SERIALIZE_MEMBER("posFact",m_posFact);

END_SERIALIZATION()

BTActionStayIdleLookAt_Template::BTActionStayIdleLookAt_Template()
{
}

}