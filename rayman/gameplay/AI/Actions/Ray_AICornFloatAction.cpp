#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AICORNFLOATACTION_H_
#include "rayman/gameplay/ai/Actions/Ray_AICornFloatAction.h"
#endif //_ITF_RAY_AICORNFLOATACTION_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef ITF_CONSOLE_FINAL
#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_
#endif ITF_CONSOLE_FINAL

#ifndef _ITF_PHYSCOMPONENT_H_
#include "engine/actors/components/PhysComponent.h"
#endif //_ITF_PHYSCOMPONENT_H_

#ifndef _ITF_POLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/PolylineComponent.h"
#endif //_ITF_POLYLINECOMPONENT_H_

#ifndef _ITF_RAY_SUBANCHORCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_SubAnchorComponent.h"
#endif //_ITF_RAY_SUBANCHORCOMPONENT_H_

#ifndef _ITF_SEEDER_H_
#include "core/Seeder.h"
#endif //_ITF_SEEDER_H_

namespace ITF
{

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_AICornFloatAction)
BEGIN_VALIDATE_ACTION(Ray_AICornFloatAction)
    VALIDATE_ACTION_PARAM("", m_physComponent, "PhysComponent mandatory");
    VALIDATE_ACTION_PARAM("", m_polylineComponent, "PolylineComponent mandatory");
    VALIDATE_ACTION_PARAM("", m_subAnchorComponent, "Ray_SubAnchorComponent mandatory");
    VALIDATE_ACTION_PARAM("", m_animatedComponent, "AnimatedComponent mandatory");
END_VALIDATE_ACTION()

//------------------------------------------------------------------------------
Ray_AICornFloatAction::Ray_AICornFloatAction()
    : Super()
    , m_crouched(bfalse)
    , m_lowerPassed(bfalse)
    , m_floatCountdown(0.0f)
    , m_floatInterval(0.0f)
    , m_floatHeight(0.0f)
    , m_physComponent(NULL)
    , m_polylineComponent(NULL)
    , m_subAnchorComponent(NULL)
{
}

//------------------------------------------------------------------------------
Ray_AICornFloatAction::~Ray_AICornFloatAction()
{
}

//------------------------------------------------------------------------------
void Ray_AICornFloatAction::onActorLoaded(Pickable::HotReloadType _hotReload)
{   
    Super::onActorLoaded(_hotReload);

    // mandatory, validated
    m_physComponent = m_actor->GetComponent<PhysComponent>();
    m_polylineComponent = m_actor->GetComponent<PolylineComponent>();
    m_subAnchorComponent = m_actor->GetComponent<Ray_SubAnchorComponent>();
}

//------------------------------------------------------------------------------
void Ray_AICornFloatAction::onActivate()
{
    Super::onActivate();
    
    // reset physics
    m_physComponent->resetForces();
    m_physComponent->setSpeed(Vec2d::Zero);

    // reset float countdown    
    m_floatCountdown = 0.0f;
    m_lowerPassed = bfalse;
}

//------------------------------------------------------------------------------
void Ray_AICornFloatAction::update(f32 _dt)
{
    Super::update(_dt);
    
    // if too much weight: play crouch anim and move equilibrium to lower pos
    f32 weight = m_polylineComponent->getMaxUserWeight();
    bbool tooMuchWeight = (weight >= getTemplate()->getWeightThreshold());
    if (tooMuchWeight)
    {
        m_crouched = btrue;
        
        Vec2d equilibrium = m_subAnchorComponent->getPos(ITF_GET_STRINGID_CRC(lowerPos,1531896678)).truncateTo2D();
        
        if (m_actor->getPos().m_y < equilibrium.m_y)
        {
            m_lowerPassed = btrue;
        }
        if (m_lowerPassed)
        {
            // lower pos spring
            Vec2d dirToEquilibrium = equilibrium - m_actor->get2DPos();
            Vec2d springForce = dirToEquilibrium * getTemplate()->getLowerSpringConstant();
            m_physComponent->addForce(springForce);

            // counter gravity
            const Vec2d& gravity = m_aiComponent->getCurrentGravity();
            m_physComponent->addForce(-gravity);

            // lower pos friction
            Vec2d currentSpeed = m_physComponent->getSpeed();
            Vec2d frictionForce = -currentSpeed * getTemplate()->getLowerFrictionConstant();
            m_physComponent->addForce(frictionForce);
        }
        else
        {
            // friction only
            Vec2d currentSpeed = m_physComponent->getSpeed();
            Vec2d frictionForce = -currentSpeed * getTemplate()->getFrictionConstant();
            m_physComponent->addForce(frictionForce);
        }
    }
    else
    {
        m_crouched = weight > 0.0f;
        m_lowerPassed = bfalse;

        // update floating
        m_floatCountdown  = Max(0.0f, m_floatCountdown -_dt);
        if (m_floatCountdown  == 0.0f)
        {
            Seeder& seeder = Seeder::getSharedSeeder();
            m_floatCountdown  = m_floatInterval = seeder.GetFloat(
                getTemplate()->getFloatIntervalMin(),
                getTemplate()->getFloatIntervalMax());
            m_floatHeight = seeder.GetFloat(
                getTemplate()->getFloatHeightMin(),
                getTemplate()->getFloatHeightMax());
        }
        f32 t = (m_floatInterval - m_floatCountdown ) / m_floatInterval;
        Vec2d equilibrium = m_actor->getWorldInitialPos().truncateTo2D();
        equilibrium.m_y += m_floatHeight * 0.5f * (f32_Cos(MTH_2PI * t) - 1.0f);
        
        // spring
        Vec2d dirToEquilibrium = equilibrium - m_actor->get2DPos();
        Vec2d springForce = dirToEquilibrium * getTemplate()->getSpringConstant();
        m_physComponent->addForce(springForce);

        // counter gravity
        const Vec2d& gravity = m_aiComponent->getCurrentGravity();
        m_physComponent->addForce(-gravity);

        // friction
        Vec2d currentSpeed = m_physComponent->getSpeed();
        Vec2d frictionForce = -currentSpeed * getTemplate()->getFrictionConstant();
        m_physComponent->addForce(frictionForce);
    }
}

//------------------------------------------------------------------------------
void Ray_AICornFloatAction::updateAnimInputs()
{
    Super::updateAnimInputs();

    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(Crouched,1896639299), m_crouched);
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_AICornFloatAction_Template)
BEGIN_SERIALIZATION_CHILD(Ray_AICornFloatAction_Template)
    SERIALIZE_MEMBER("floatHeightMin", m_floatHeightMin);
    SERIALIZE_MEMBER("floatIntervalMin", m_floatIntervalMin);
    SERIALIZE_MEMBER("floatIntervalMax", m_floatIntervalMax);
    SERIALIZE_MEMBER("floatHeightMin", m_floatHeightMin);
    SERIALIZE_MEMBER("floatHeightMax", m_floatHeightMax);
    SERIALIZE_MEMBER("springConstant", m_springConstant);
    SERIALIZE_MEMBER("frictionConstant", m_frictionConstant);
    SERIALIZE_MEMBER("weightThreshold", m_weightThreshold);
    SERIALIZE_MEMBER("lowerSpringConstant", m_lowerSpringConstant);
    SERIALIZE_MEMBER("lowerFrictionConstant", m_lowerFrictionConstant);
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_AICornFloatAction_Template::Ray_AICornFloatAction_Template()
    : Super()
    , m_floatHeightMin(1.0f)
    , m_floatHeightMax(2.0f)
    , m_floatIntervalMin(1.0f)
    , m_floatIntervalMax(2.0f)
    , m_springConstant(1.0f)
    , m_frictionConstant(1.0f)
    , m_weightThreshold(1.0f)
    , m_lowerSpringConstant(1.0f)
    , m_lowerFrictionConstant(1.0f)
{
    m_animDefaultAction = ITF_GET_STRINGID_CRC(Float,3473554049);
}

} // namespace ITF
