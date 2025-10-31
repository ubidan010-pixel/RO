#include "precompiled_gameplay.h"

#ifndef _ITF_CONSTANTMOVEMENTCOMPONENT_H_
#include "gameplay/Components/Common/ConstantMovementComponent.h"
#endif //_ITF_CONSTANTMOVEMENTCOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(ConstantMovementComponent)

    BEGIN_SERIALIZATION_CHILD(ConstantMovementComponent)
    END_SERIALIZATION()


    BEGIN_VALIDATE_COMPONENT(ConstantMovementComponent)
    END_VALIDATE_COMPONENT()


    ConstantMovementComponent::ConstantMovementComponent()
    {
    }

    ConstantMovementComponent::~ConstantMovementComponent()
    {
    }

    void ConstantMovementComponent::Update( f32 _dt )
    {
        Super::Update(_dt);


        f32 angle = m_actor->getAngle();
        angle += getTemplate()->getWorldAngularSpeed().ToRadians() * _dt;
        m_actor->setAngle(angle);


        Vec3d pos = m_actor->getPos();
        pos += getWorldSpeed() * _dt;

        Vec2d localDelta =  getTemplate()->getLocalSpeed().Rotate(angle) * _dt;
        pos.m_x += localDelta.m_x;
        pos.m_y += localDelta.m_y;

        m_actor->setPos(pos);

#ifndef ITF_FINAL
        //DebugDraw::circle(m_actor->getPos(), 0.1f, Color::black(), 1.f);
#endif // ITF_FINAL
    }

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(ConstantMovementComponent_Template)
    BEGIN_SERIALIZATION(ConstantMovementComponent_Template)

        SERIALIZE_MEMBER("localSpeed", m_localSpeed);
        SERIALIZE_MEMBER("worldSpeed", m_worldSpeed);
        SERIALIZE_MEMBER("worldAngularSpeed", m_worldAngularSpeed);

    END_SERIALIZATION()

    ConstantMovementComponent_Template::ConstantMovementComponent_Template()
    : m_localSpeed(Vec2d::Zero)
    , m_worldSpeed(Vec3d::Zero)
    , m_worldAngularSpeed(bfalse,0.f)
    {
    }
};