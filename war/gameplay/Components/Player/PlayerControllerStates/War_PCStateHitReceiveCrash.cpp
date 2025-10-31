#include "precompiled_gameplay_war.h"

#ifndef _ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_
#include "war/gameplay/Components/Player/War_PlayerControllerComponent.h"
#endif //_ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(War_PlayerControllerComponent::StateHitReceiveCrash)

////////////////////
// Receive Hit Crash State
////////////////////
void War_PlayerControllerComponent::StateHitReceiveCrash::onEnter()
{
    Super::onEnter();

    m_characterPhys->setDisabled(btrue);

    const BaseObject* obj = GETOBJECT(m_crashPoly);

    if ( obj )
    {
        const PolyLine* poly = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747));

        if ( poly )
        {
            const PolyLineEdge& edge = poly->getEdgeAt(m_crashEdge);

            Vec2d dir = Vec2d::Right.Rotate(m_crashTargetAngle);

            bbool lookRight = dir.dot(edge.m_normalizedVector.getPerpendicular()) < 0.f;

            m_parent->setLookRight(lookRight);
        }
    }
}

void War_PlayerControllerComponent::StateHitReceiveCrash::onExit()
{
    m_characterPhys->setDisabled(bfalse);
}

bbool War_PlayerControllerComponent::StateHitReceiveCrash::checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes )
{
    if ( isCurrentAnimFinished() )
    {
        m_parent->changeState(&m_parent->m_stateFalling);
        return btrue;
    }

    return bfalse;
}

void War_PlayerControllerComponent::StateHitReceiveCrash::update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes)
{
    const BaseObject* obj = GETOBJECT(m_crashPoly);

    if ( obj )
    {
        const PolyLine* poly = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747));

        if ( poly )
        {
            const PolyLineEdge& edge = poly->getEdgeAt(m_crashEdge);
            Vec3d magicBox;

            ITF_ASSERT(m_animComponent);
            if (m_animComponent && m_animComponent->getMagicBox(magicBox))
            {
                Vec2d pos = edge.getPos() + ( edge.m_normalizedVector * m_crashDist );

                m_actor->setPos(pos.to3d(&m_parent->GetActor()->getPos().m_z) - magicBox);
                m_actor->setAngle(m_crashTargetAngle);
            }
        }
    }
}

void War_PlayerControllerComponent::StateHitReceiveCrash::setCrashData( ObjectRef _crashPoly, u32 _crashEdge, f32 _crashDist, f32 _crashTargetAngle )
{
    m_crashPoly = _crashPoly;
    m_crashEdge = _crashEdge;
    m_crashDist = _crashDist;
    m_crashTargetAngle = _crashTargetAngle;
}

}