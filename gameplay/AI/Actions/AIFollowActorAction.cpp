#include "precompiled_gameplay.h"

#ifndef _ITF_AIFOLLOWACTORACTION_H_
#include "gameplay/ai/Actions/AIFollowActorAction.h"
#endif //_ITF_AIFOLLOWACTORACTION_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

#ifndef _ITF_PHYSCOMPONENT_H_
#include "engine/actors/components/physcomponent.h"
#endif //_ITF_PHYSCOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(AIFollowActorAction_Template)

    BEGIN_SERIALIZATION_CHILD(AIFollowActorAction_Template)
        SERIALIZE_MEMBER("aiFollowActorRelativPos",m_relativPosToActor);
        SERIALIZE_MEMBER("aiFollowActorRelativPosNext",m_relativPosToFollower);
        SERIALIZE_MEMBER("aiFollowActorAcceleration",m_acceleration);
        SERIALIZE_MEMBER("aiFollowActorFriction",m_friction);
        SERIALIZE_MEMBER("aiFollowActorDoFlip",m_doFlip);
    END_SERIALIZATION()

        AIFollowActorAction_Template::AIFollowActorAction_Template()
        : m_relativPosToActor(Vec3d::Zero)
        , m_relativPosToFollower(Vec3d::Zero)
        , m_acceleration(0.f)
        , m_friction(0.f)
        , m_doFlip(btrue)
    {
    }

    IMPLEMENT_OBJECT_RTTI(AIFollowActorAction)


    AIFollowActorAction::AIFollowActorAction()
        : m_followActor(ITF_INVALID_OBJREF)
        , m_followAnimLight(NULL)
        , m_physComponent(NULL)
    {
    }

    AIFollowActorAction::~AIFollowActorAction()
    {
    }

    void AIFollowActorAction::onActorLoaded(Pickable::HotReloadType _hotReload)
    {   
        Super::onActorLoaded(_hotReload);

        m_relativPos = getTemplate()->getRelativPosToActor();
        m_physComponent = m_actor->GetComponent<PhysComponent>();

        ITF_ASSERT(m_physComponent);
    }

    void AIFollowActorAction::onActivate()
    {
        Super::onActivate();

        m_lastPos = m_actor->getPos();
    }

    void AIFollowActorAction::update( f32 _delta )
    {
        if (!m_followAnimLight || !m_physComponent || _delta <= 0.f)
            return;

        Vec3d   curPos = m_actor->getPos();
        m_lastPos = curPos;

        Vec2d gravity = m_aiComponent->getCurrentGravity() * m_physComponent->getGravityMultiplier();
        Vec3d direction = getFollowPos() - curPos;

        m_physComponent->addForce(  direction.truncateTo2D()*getTemplate()->getAcceleration()  +          // distance force
                                    m_physComponent->getSpeed()*(getTemplate()->getFriction() * -1.f / _delta) - gravity);  // friction force

        if ( getTemplate()->getFlip() )
        {
            m_actor->setFlipped(direction.dot(Vec3d::Right) < 0.0f);
        }
    }

    Vec3d AIFollowActorAction::getFollowPos() const
    {
        Actor * followActor     = getFollowActor();
        Vec3d   newRelativPos   = m_relativPos;

        Actor * actor = ActorRef(m_followActor).getActor();
        bbool   isFlipped       = actor && actor->isFlipped();

        if (isFlipped)
            newRelativPos.m_x = -newRelativPos.m_x;
        return followActor->getPos() + newRelativPos;
    }

    void AIFollowActorAction::setFollowActor(ObjectRef _actor) 
    {
        m_followActor               = ITF_INVALID_OBJREF;
        m_followAnimLight           = NULL;

        BaseObject* obj = GETOBJECT(_actor);
        if (!obj)
            return;
        Actor* objActor = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
        if (!objActor)
            return;
        m_followAnimLight = objActor->GetComponent<AnimLightComponent>();
        if (!m_followAnimLight)
            return;
        m_followActor = _actor;
    }

    Actor* AIFollowActorAction::getFollowActor() const
    {
        BaseObject* obj = GETOBJECT(m_followActor);

        if ( obj )
        {
            return obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
        }

        return NULL;
    }

    void AIFollowActorAction::setRelativPosToActor()
    {
        m_relativPos = getTemplate()->getRelativPosToActor();  
    }

    void AIFollowActorAction::setRelativPosToFollower()
    {
        m_relativPos = getTemplate()->getRelativPosToFollower();
    }
}