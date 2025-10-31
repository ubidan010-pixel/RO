#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIRESTOREANGLEBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Ray_AIRestoreAngleBehavior.h"
#endif //_ITF_RAY_AIRESTOREANGLEBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIRestoreAngleBehavior)

    BEGIN_VALIDATE_BEHAVIOR(Ray_AIRestoreAngleBehavior)
        VALIDATE_BEHAVIOR_PARAM("angleSpeed", getTemplate()->getAngleSpeed() >= 0.0f && getTemplate()->getAngleSpeed() <= MTH_PI, "angleSpeed must be between 0 and PI"); 
    END_VALIDATE_BEHAVIOR()

    //*****************************************************************************

    Ray_AIRestoreAngleBehavior::Ray_AIRestoreAngleBehavior()
    : m_initialAngle(0.0f)
    {
        // none
    }

    //*****************************************************************************

    Ray_AIRestoreAngleBehavior::~Ray_AIRestoreAngleBehavior()
    {
        // none
    }

    //*****************************************************************************

    void Ray_AIRestoreAngleBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_initialAngle = m_actor->getAngle();
    }

    //*****************************************************************************

    void Ray_AIRestoreAngleBehavior::update(f32 _dt)
    {
        Super::update(_dt);

        f32 actorAngle = m_actor->getAngle();

        if (actorAngle != m_initialAngle)
        {
            f32 angleSpeed = getTemplate()->getAngleSpeed();
            NormalizeAnglePI(actorAngle);

            if (actorAngle >= angleSpeed && actorAngle <= MTH_PI)
            {
                m_actor->setAngle(actorAngle - angleSpeed);
            }
            else if (actorAngle >= -MTH_PI && actorAngle <= -angleSpeed)
            {
                m_actor->setAngle(actorAngle + angleSpeed);
            }
            else
            {
                m_actor->setAngle(m_initialAngle);
            } 
        }   
    }

#ifdef ITF_SUPPORT_EDITOR
    // ****************************************************************************
    void Ray_AIRestoreAngleBehavior::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;
    }
#endif // ITF_SUPPORT_EDITOR

    //*****************************************************************************
    // Template
    //*****************************************************************************

    IMPLEMENT_OBJECT_RTTI(Ray_AIRestoreAngleBehavior_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_AIRestoreAngleBehavior_Template)
        SERIALIZE_MEMBER("angleSpeed", m_angleSpeed);
    END_SERIALIZATION()

    //*****************************************************************************

    Ray_AIRestoreAngleBehavior_Template::Ray_AIRestoreAngleBehavior_Template()
    : m_angleSpeed(0.1f)
    {
        // none
    }

    //*****************************************************************************

    Ray_AIRestoreAngleBehavior_Template::~Ray_AIRestoreAngleBehavior_Template()
    {
        // none
    }


}


