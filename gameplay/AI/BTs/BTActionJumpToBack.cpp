
#include "precompiled_gameplay.h"

#ifndef _ITF_BTACTIONJUMPTOBACK_H_
#include "gameplay/ai/BTs/BTActionJumpToBack.h"
#endif // _ITF_BTACTIONJUMPTOBACK_H_

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

#ifndef _ITF_GROUNDAICONTROLLERCOMPONENT_H_
#include "gameplay/Components/AI/GroundAIControllerComponent.h"
#endif //_ITF_GROUNDAICONTROLLERCOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(BTActionJumpToBack);

BTActionJumpToBack::BTActionJumpToBack()
: m_physComponent(NULL)
, m_animComponent(NULL)
, m_aiController(NULL)
, m_startPos(Vec3d::Zero)
, m_timeCounter(0.f)
{
}

void BTActionJumpToBack::init( BehaviorTree* _tree, const BTNode_Template* _template )
{
    Super::init(_tree,_template);

    m_physComponent = _tree->getActor()->GetComponent<StickToPolylinePhysComponent>();
    m_animComponent = _tree->getActor()->GetComponent<AnimLightComponent>();
    m_aiController = _tree->getActor()->GetComponent<GroundAIControllerComponent>();
}

bbool BTActionJumpToBack::validate( Actor* _actor ) const
{
    bbool retVal = Super::validate(_actor);

    if ( !retVal )
    {
        return bfalse;
    }

    if ( !m_animComponent )
    {
        ITF_WARNING_PARAM(_actor,"AnimLightComponent",m_animComponent!=NULL,"%s: anim component is mandatory",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    if ( !m_aiController )
    {
        ITF_WARNING_PARAM(_actor,"AIController",m_aiController!=NULL,"%s: ground ai controller component is mandatory",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    if ( !getTemplate()->getAnim().isValid() )
    {
        ITF_WARNING_PARAM(_actor,"anim",getTemplate()->getAnim().isValid(),"%s: no animation was set for the jump",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    return btrue;
}

void BTActionJumpToBack::onActivate()
{
    Super::onActivate();

    m_timeCounter = 0.f;
    m_startPos = m_tree->getActor()->getPos();

    if ( m_physComponent )
    {
        m_physComponent->setSpeed(Vec2d::Zero);
    }

    m_aiController->setDisablePhysics(btrue);

    if ( getTemplate()->getAnim().isValid() )
    {
        m_animComponent->setAnim(getTemplate()->getAnim());
    }
}

void BTActionJumpToBack::onDeactivate()
{
    Super::onDeactivate();

    if ( m_timeCounter < getTemplate()->getTime() )
    {
        Vec3d finalPos = m_startPos;

        finalPos.m_z += getTemplate()->getZOffset();

        m_tree->getActor()->setPos(finalPos);
    }

    m_aiController->setDisablePhysics(bfalse);
}

bbool BTActionJumpToBack::update( f32 _dt )
{
    Super::update(_dt);

    f32 totalTime = getTemplate()->getTime();
    m_timeCounter = Min(m_timeCounter+_dt,totalTime);
    
    Vec3d newPos = m_startPos;

    if ( m_timeCounter )
    {
        f32 halfTime = totalTime * 0.5f;
        f32 t = m_timeCounter / totalTime;
        f32 halfT = m_timeCounter / halfTime;
        f32 val = MTH_PIBY2*halfT;

        val = f32_Sin(val);

        newPos.m_y = m_startPos.m_y + (getTemplate()->getHeight()*val);
        newPos.m_z = m_startPos.m_z + (getTemplate()->getZOffset()*t);
    }
    else
    {
        newPos.m_z += getTemplate()->getZOffset();
    }

    m_tree->getActor()->setPos(newPos);

    return m_timeCounter < totalTime;
}

IMPLEMENT_OBJECT_RTTI(BTActionJumpToBack_Template);

BEGIN_SERIALIZATION_CHILD(BTActionJumpToBack_Template)

    SERIALIZE_MEMBER("anim",m_anim);
    SERIALIZE_MEMBER("time",m_time);
    SERIALIZE_MEMBER("zOffset",m_zOffset);
    SERIALIZE_MEMBER("height",m_height);

END_SERIALIZATION()

BTActionJumpToBack_Template::BTActionJumpToBack_Template()
: m_time(1.f)
, m_zOffset(0.1f)
, m_height(2.f)
{
}

}