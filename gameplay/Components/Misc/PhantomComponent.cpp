#include "precompiled_gameplay.h"

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_ANIMATIONSKELETON_H_
#include "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_

#ifndef _ITF_ANIMATIONPOLYLINE_H_
#include "engine/animation/AnimationPolyline.h"
#endif // _ITF_ANIMATIONPOLYLINE_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(PhantomComponent)

    BEGIN_SERIALIZATION_CHILD(PhantomComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(PhantomComponent)
    END_VALIDATE_COMPONENT()

    PhantomComponent::PhantomComponent()
        : Super()
        , m_disabled(bfalse)
        , m_phantom(NULL)
        , m_registered(bfalse)
    {
    }

    PhantomComponent::~PhantomComponent()
    {
        clear();
    }

    void PhantomComponent::clear()
    {
        unregisterPhantom();
        PHYSWORLD->deallocPhantom(m_phantom);
    }

    void PhantomComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryPhysShape,2080285933),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventDisableCollision,3761196874),this);
    }

    void PhantomComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        if ( m_disabled )
        {
            return;
        }

        registerPhantom();
    }

    void PhantomComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();

        unregisterPhantom();
    }

    void PhantomComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( EventQueryPhysShape* queryShape = _event->DynamicCast<EventQueryPhysShape>(ITF_GET_STRINGID_CRC(EventQueryPhysShape,2080285933)) )
        {
            if (m_phantom)
            {
                queryShape->setPhysShape(m_phantom->getShape());
                queryShape->setPosition(m_phantom->getPos());
                queryShape->setAngle(m_phantom->getAngle());
            }
        }
        else if ( EventDisableCollision* collision = _event->DynamicCast<EventDisableCollision>(ITF_GET_STRINGID_CRC(EventDisableCollision,3761196874)) )
        {
            m_disabled = collision->getDisable();
        }
    }

    void PhantomComponent::createPhantom()
    {
        PhysPhantomInitCommonInfo initInfo;

        SET_OWNER(initInfo, m_actor->getRef());
        initInfo.m_shape = getShape();
        initInfo.m_pos = m_actor->get2DPos();   // don't use getShapePos here as it may not be available; we'll move the phantom on update anyway
        initInfo.m_angle = m_actor->getAngle();
        initInfo.m_userData = m_actor->getRef();
        initInfo.m_collisionGroup = getTemplate()->getCollisionGroup();

        m_phantom = PHYSWORLD->allocPhantom(initInfo);

        if ( !m_disabled &&
             m_actor->isActive() &&
             m_actor->isEnabled() )
        {
            registerPhantom();
        }
    }

    void PhantomComponent::registerPhantom()
    {
        if ( !m_registered && m_phantom && isValid() )
        {
            PHYSWORLD->insertPhantom(m_phantom, m_actor->getDepth());
            m_registered = btrue;
        }
    }

    void PhantomComponent::unregisterPhantom()
    {
        if ( m_registered && m_phantom )
        {
            PHYSWORLD->removePhantom(m_phantom);
            m_registered = bfalse;
        }
    }



    void PhantomComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        if ( isValid() && !m_disabled )
        {
            if ( !m_phantom )
            {
                createPhantom();
            }
            registerPhantom();

            f32 z = m_actor->getDepth();
            m_phantom->setPos3D(getShapePos().to3d(z));
            m_phantom->setAngle(m_actor->getAngle());
            m_phantom->updateAABB();
        }
        else
        {
            unregisterPhantom();
        }
    }

#ifdef ITF_SUPPORT_DEBUGFEATURE
    void PhantomComponent::drawDebug() const
    {
        if (!m_disabled)
        {
            //Super::drawDebug();

            DebugDraw::shape(getShapePos(), m_actor->getDepth(), m_actor->getAngle(), getShape(),
                Color::white(), 0.f, "phantom", 65 * MTH_DEGTORAD);
        }
    }
#endif // ITF_SUPPORT_DEBUGFEATURE

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(PhantomComponent_Template)
    BEGIN_SERIALIZATION_CHILD(PhantomComponent_Template)

        SERIALIZE_ENUM_BEGIN("collisionGroup", m_collisionGroup);
            SERIALIZE_ENUM_VAR(ECOLLISIONGROUP_NONE);
            SERIALIZE_ENUM_VAR(ECOLLISIONGROUP_POLYLINE);
            SERIALIZE_ENUM_VAR(ECOLLISIONGROUP_CHARACTER);
            SERIALIZE_ENUM_VAR(ECOLLISIONGROUP_ITEMS);
        SERIALIZE_ENUM_END();

    END_SERIALIZATION()

    PhantomComponent_Template::PhantomComponent_Template()
    : m_collisionGroup(ECOLLISIONGROUP_NONE)
    {
    }
}