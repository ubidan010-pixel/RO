#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIBOULDERSENTBACKBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Boulder/Ray_AIBoulderSentBackBehavior.h"
#endif //_ITF_RAY_AIBOULDERSENTBACKBEHAVIOR_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIBoulderSentBackBehavior)

    BEGIN_VALIDATE_BEHAVIOR(Ray_AIBoulderSentBackBehavior)
    END_VALIDATE_BEHAVIOR()

    //------------------------------------------------------------------
    Ray_AIBoulderSentBackBehavior::Ray_AIBoulderSentBackBehavior()
    : m_physComponent(NULL)
    , m_behaviorTimer(-1.0f)
    , m_speedValue(0.f)
    , m_direction(Vec2d::Zero)
    , m_speed(Vec2d::Zero)
    {
    }

    //------------------------------------------------------------------
    Ray_AIBoulderSentBackBehavior::~Ray_AIBoulderSentBackBehavior()
    {
    }

    //------------------------------------------------------------------
    void Ray_AIBoulderSentBackBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        //get components
        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();
    }

    //------------------------------------------------------------------
    void Ray_AIBoulderSentBackBehavior::update(f32 _dt)
    {
        Super::update(_dt);
        
        updateActorAngle(_dt);
        updateFlight(_dt);

        //update behavior timer
        m_behaviorTimer += _dt;
    }

    //------------------------------------------------------------------
    void Ray_AIBoulderSentBackBehavior::onActivate()
    {
        if( m_physComponent )
        {
            m_physComponent->setDisabled(btrue);
        }
        //if direction has not been set take current actor direction
        if( m_direction == Vec2d::Zero )
        {
            const f32 angle = m_actor->getAngle();
            m_direction = Vec2d::Right.Rotate(angle);
        }
        //compute starting speed
        ITF_ASSERT(m_speedValue > 0.f);
        m_speed = m_direction * m_speedValue;
        //reset behavior timer
        m_behaviorTimer = 0.0f;
    }

    //------------------------------------------------------------------
    void Ray_AIBoulderSentBackBehavior::onDeactivate()
    {
        Super::onDeactivate();
        if( m_physComponent )
        {
            m_physComponent->setDisabled(btrue);
        }
        m_behaviorTimer = -1.0f;
    }

    //------------------------------------------------------------------
    // State management
    //------------------------------------------------------------------

    //------------------------------------------------------------------
    void Ray_AIBoulderSentBackBehavior::updateFlight(const f32 _dt)
    {
        Vec2d boulderPosition = m_actor->get2DPos();
        
        //add friction
        m_speed -= m_speed * (getTemplate()->getFriction() * _dt);
        //add gravity
        m_speed -= Vec2d::Up * (getTemplate()->getGravity() * _dt);

        //update position
        boulderPosition += m_speed * _dt;
        m_actor->set2DPos(boulderPosition);
    }

    //------------------------------------------------------------------
    void Ray_AIBoulderSentBackBehavior::updateActorAngle(const f32 _dt)
    {
        if( m_speed != Vec2d::Zero )
        {
            const f32 ANGULAR_SPEED = MTH_PI;

            const f32 actorAngle = m_actor->getAngle();
            Vec2d speed = m_speed;
            Angle targetAngle(bfalse, speed.normalize().getAngle());
            Angle currentAngle(bfalse, actorAngle);
            Angle diff = Angle::GetShortestDif(currentAngle, targetAngle);

            const f32 angleDiff = diff.ToRadians();
            f32 angleMove = ANGULAR_SPEED * _dt;
            if( angleDiff < 0.0f )
            {
                angleMove *= -1.0f;
            }

            if( f32_Abs(angleDiff) < f32_Abs(angleMove) )
            {
                angleMove = angleDiff;
            }

            m_actor->setAngle( actorAngle + angleMove );
        }
    }


    IMPLEMENT_OBJECT_RTTI(Ray_AIBoulderSentBackBehavior_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AIBoulderSentBackBehavior_Template)
        SERIALIZE_MEMBER("gravity", m_gravity);
        SERIALIZE_MEMBER("friction", m_friction);
    END_SERIALIZATION()

    Ray_AIBoulderSentBackBehavior_Template::Ray_AIBoulderSentBackBehavior_Template()
    : m_gravity(9.8f)
    , m_friction(5.0f)
    {
    }

    Ray_AIBoulderSentBackBehavior_Template::~Ray_AIBoulderSentBackBehavior_Template()
    {        
    }
}

