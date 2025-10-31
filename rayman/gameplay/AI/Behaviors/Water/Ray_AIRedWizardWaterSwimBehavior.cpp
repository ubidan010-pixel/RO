#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIREDWIZARDWATERSWIMBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Water/Ray_AIRedWizardWaterSwimBehavior.h"
#endif //_ITF_RAY_AIREDWIZARDWATERSWIMBEHAVIOR_H_

#ifndef _ITF_RAY_AIWATERFLOATACTION_H_
#include "rayman/gameplay/ai/Actions/Water/Ray_AIWaterFloatAction.h"
#endif //_ITF_RAY_AIWATERFLOATACTION_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_RAY_AIGROUNDCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_GroundAIComponent.h"
#endif //_ITF_RAY_AIGROUNDCOMPONENT_H_

#ifndef _ITF_AIJUMPACTION_H_
#include "gameplay/ai/Actions/AIJumpAction.h"
#endif //_ITF_AIJUMPACTION_H_


namespace ITF 
{
IMPLEMENT_OBJECT_RTTI(Ray_AIRedWizardWaterSwimBehavior_Template)

BEGIN_SERIALIZATION_CHILD(Ray_AIRedWizardWaterSwimBehavior_Template)
    SERIALIZE_MEMBER("minSpeed", m_pushMinSpeed);
    SERIALIZE_MEMBER("maxSpeed", m_pushMaxSpeed);
    SERIALIZE_MEMBER("minForce", m_pushMinForce);
    SERIALIZE_MEMBER("maxForce", m_pushMaxForce);
    SERIALIZE_MEMBER("moveForce", m_moveForce);
    SERIALIZE_OBJECT_WITH_FACTORY("swim", m_swim, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("jump", m_jump, ACTORSMANAGER->getAIActionsFactory());

END_SERIALIZATION()

Ray_AIRedWizardWaterSwimBehavior_Template::Ray_AIRedWizardWaterSwimBehavior_Template()
: m_swim(NULL)
, m_jump(NULL)
, m_pushMinSpeed(0.f)
, m_pushMaxSpeed(3.f)
, m_pushMinForce(50.f)
, m_pushMaxForce(250.f)
, m_moveForce(50.0f)
{

}

Ray_AIRedWizardWaterSwimBehavior_Template::~Ray_AIRedWizardWaterSwimBehavior_Template()
{
    SF_DEL(m_swim);
    SF_DEL(m_jump);

}
IMPLEMENT_OBJECT_RTTI(Ray_AIRedWizardWaterSwimBehavior)

BEGIN_VALIDATE_BEHAVIOR(Ray_AIRedWizardWaterSwimBehavior)
END_VALIDATE_BEHAVIOR()

Ray_AIRedWizardWaterSwimBehavior::Ray_AIRedWizardWaterSwimBehavior()
: m_swim(NULL)
, m_jump(NULL)
, m_timer(0.0f)
, m_timeToMove(0.5f)
, m_groundAiComponent(NULL)
{
}

Ray_AIRedWizardWaterSwimBehavior::~Ray_AIRedWizardWaterSwimBehavior()
{
}

void Ray_AIRedWizardWaterSwimBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    m_swim = createAiAction(getTemplate()->getSwim());
    m_jump = static_cast<AIJumpAction*>(createAiAction(getTemplate()->getJump()));

    Super::onActorLoaded(_hotReload);

    m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();

    ITF_ASSERT(m_physComponent);

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventPolylineBlockingContact,1992742979),m_aiComponent);
    m_groundAiComponent = m_aiComponent->DynamicCast<Ray_GroundAIComponent>(ITF_GET_STRINGID_CRC(Ray_GroundAIComponent,2230428483));
    ITF_ASSERT(m_groundAiComponent);
}

void Ray_AIRedWizardWaterSwimBehavior::update( f32 _delta  )
{
    ActorList players;
    AIUtils::getPlayers(m_actor->getDepth(), players);

    Actor* closestPlayer = AIUtils::getClosestActor( players,
        m_actor, m_actor->isFlipped(),
        10.0f );
    Vec2d dir = Vec2d::Zero;

    if (closestPlayer)
    {

        dir = closestPlayer->get2DPos() - m_actor->get2DPos();
        if (dir.sqrnorm() < 4.0f)
            dir = Vec2d::Zero;
        else
           dir.normalize();
    }

    bbool bdir = dir != Vec2d::Zero;
    bbool bcdir = m_currentDir != Vec2d::Zero;

    if (m_timer == 0.0f)
    {
        if (bdir != bcdir)
            m_timer = m_timeToMove;
        m_currentDir = dir;
    }

    if (m_currentAction != m_jump)
    {
        if (tryJump())
        {
            startJump();
        }
        else
        {
            if (!m_groundAiComponent->UpdatePhysicSwim(_delta,bfalse,m_currentDir))
                m_aiComponent->onBehaviorFinished();
        }
    }

    m_timer = Max(0.0f,m_timer - _delta);

    Super::update(_delta);
}

void Ray_AIRedWizardWaterSwimBehavior::onActivate()
{
    m_groundAiComponent->setStance(Ray_GroundAIComponent::Stance_Swim);

    startSwim();
}

void Ray_AIRedWizardWaterSwimBehavior::onEvent( Event * _event)
{
    if ( EventPolylineBlockingContact* blockedEvent = _event->DynamicCast<EventPolylineBlockingContact>(ITF_GET_STRINGID_CRC(EventPolylineBlockingContact,1992742979)) )
    {
        processBlockingContact(blockedEvent);
    }
}

void Ray_AIRedWizardWaterSwimBehavior::processBlockingContact( EventPolylineBlockingContact* _blockedEvent )
{
    if ( getTemplate()->getPushMinSpeed() != getTemplate()->getPushMaxSpeed() )
    {
        Vec2d pushSpeed = _blockedEvent->getSpeed();
        f32 speedMod = pushSpeed.norm();
        speedMod = Clamp(speedMod,getTemplate()->getPushMinSpeed(),getTemplate()->getPushMaxSpeed());
        f32 t = ( speedMod - getTemplate()->getPushMinSpeed() ) / ( getTemplate()->getPushMaxSpeed() - getTemplate()->getPushMinSpeed() );
        Vec2d force = pushSpeed;

        force.normalize();
        force *= Interpolate(getTemplate()->getPushMinForce(),getTemplate()->getPushMaxForce(),t);

        m_physComponent->addForce(force);
    }
    else if ( getTemplate()->getPushMaxForce() )
    {
        Vec2d force = _blockedEvent->getSpeed();

        force.normalize();
        force *= getTemplate()->getPushMaxForce();

        m_physComponent->addForce(force);
    }
}

void Ray_AIRedWizardWaterSwimBehavior::onActionFinished()
{
    Super::onActionFinished();
    if (m_currentAction == m_swim)
    {
        onFinishedSwim();
    }
    else if (m_currentAction == m_jump)
    {
        m_aiComponent->onBehaviorFinished();
    }

}

void Ray_AIRedWizardWaterSwimBehavior::startSwim()
{
    setAction(m_swim);
}

void Ray_AIRedWizardWaterSwimBehavior::onDeactivate()
{
    Super::onDeactivate();
    m_groundAiComponent->setStance(Ray_GroundAIComponent::Stance_Normal);

}

void Ray_AIRedWizardWaterSwimBehavior::startJump()
{
    m_groundAiComponent->setStance(Ray_GroundAIComponent::Stance_Normal);
    m_actor->setAngle(0.0f);
    if (m_currentDir.dot(Vec2d::Right) < 0.0f != m_actor->isFlipped())
    {
        m_actor->setFlipped(!m_actor->isFlipped());
    }
    setAction(m_jump);
    //m_aiComponent->onBehaviorFinished();
}


void Ray_AIRedWizardWaterSwimBehavior::onFinishedSwim()
{
    m_aiComponent->onBehaviorFinished();
}

bbool Ray_AIRedWizardWaterSwimBehavior::tryJump()
{

    

    if (m_groundAiComponent->getSwimmingIsOnSurface() && m_currentDir != Vec2d::Zero)
    {
        PolyLine* poly;
        const PolyLineEdge* closestEdge;

        AIUtils::getPolyLine(m_groundAiComponent->getSwimingPolyline(),m_groundAiComponent->getSwimmingClosestEdge(),poly,closestEdge);

        if ( !poly || !closestEdge )
        {
            return bfalse;
        }
        if (m_currentDir.dot(closestEdge->m_normalizedVector.getPerpendicular())>0.0f)
            return btrue;

    }
    return bfalse;
}
}

