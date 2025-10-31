#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIBLOWINGBIRDLOOKATBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/BlowingBird/Ray_AIBlowingBirdLookatBehavior.h"
#endif //_ITF_RAY_AIBLOWINGBIRDLOOKATBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_RAY_AIUTILS_H_
#include "Rayman/gameplay/AI/Utils/Ray_AIUtils.h"
#endif //_ITF_RAY_AIUTILS_H_

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_AIBlowingBirdLookatBehavior_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_AIBlowingBirdLookatBehavior_Template)
    SERIALIZE_OBJECT_WITH_FACTORY("uturn", m_uturn, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("attack", m_attack, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_MEMBER("beginDuration", m_beginDuration);
    SERIALIZE_MEMBER("rotationSpeed", m_rotationSpeed);
    SERIALIZE_MEMBER("minAngle", m_minAngle);
    SERIALIZE_MEMBER("maxAngle", m_maxAngle);
    END_SERIALIZATION()

    Ray_AIBlowingBirdLookatBehavior_Template::Ray_AIBlowingBirdLookatBehavior_Template()
    : m_uturn(NULL)
    , m_attack(NULL)
    , m_beginDuration(0.3f)
    , m_rotationSpeed(btrue,60.0f)
    , m_minAngle(btrue,0.0f)
    , m_maxAngle(btrue,0.0f)
    {
        
    }

    Ray_AIBlowingBirdLookatBehavior_Template::~Ray_AIBlowingBirdLookatBehavior_Template()
    {
        SF_DEL(m_uturn);
        SF_DEL(m_attack);

    }
    IMPLEMENT_OBJECT_RTTI(Ray_AIBlowingBirdLookatBehavior)

    Ray_AIBlowingBirdLookatBehavior::Ray_AIBlowingBirdLookatBehavior()
    : m_uturn(NULL)
    , m_attack(NULL)
    , m_angle(0.0f)
    , m_timer(0.0f)
    {
    }

    Ray_AIBlowingBirdLookatBehavior::~Ray_AIBlowingBirdLookatBehavior()
    {
    }

    void Ray_AIBlowingBirdLookatBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        m_uturn = createAiAction(getTemplate()->getUturn());
        m_attack = createAiAction(getTemplate()->getAttack());
   }

    void Ray_AIBlowingBirdLookatBehavior::update( f32 _delta  )
    {
        Super::update(_delta);
        if (m_currentAction == m_attack)
        {
            updateAttack(_delta);
        }
    }

 
    void Ray_AIBlowingBirdLookatBehavior::onActivate()
    {
        //Super::onActivate();
        m_timer = 0.0f;
        m_lookAtAngle = getTemplate()->getMinAngle();
        startAttack();
    }

    void Ray_AIBlowingBirdLookatBehavior::onActionFinished()
    {
        Super::onActionFinished();
        if (m_currentAction == m_uturn)
            onFinishedUturn();
        else if (m_currentAction == m_attack)
            onFinishedAttack();
    }



    void Ray_AIBlowingBirdLookatBehavior::startUturn()
    {
        if (m_uturn)
            setAction(m_uturn);
        else 
            onFinishedUturn();
    }

    void Ray_AIBlowingBirdLookatBehavior::onFinishedUturn()
    {
        m_actor->setFlipped(!m_actor->isFlipped());
        startAttack();
    }



    void Ray_AIBlowingBirdLookatBehavior::startAttack()
    {
        m_timer = getTemplate()->getBeginDuration();
        m_lookAtAngle = getTemplate()->getMinAngle();
        setAction(m_attack,btrue);
    }

    void Ray_AIBlowingBirdLookatBehavior::updateAttack( f32 _delta)
    {
        //wait until begin is finished
        if (m_timer == 0.0f)
        {
            f32 newAngle = m_lookAtAngle.ToRadians() + getTemplate()->getRotationSpeed().ToRadians() *_delta;

            newAngle = Min(newAngle,getTemplate()->getMaxAngle().ToRadians());

            bbool canUturn = btrue;
            if (m_lookAtAngle.ToRadians() != getTemplate()->getMaxAngle().ToRadians() && newAngle == getTemplate()->getMaxAngle().ToRadians())
            {
                canUturn = bfalse;
                m_timer = getTemplate()->getBeginDuration();
            }


            //check for flip;
            if (m_lookAtAngle.ToRadians() <= MTH_PIBY2 && newAngle > MTH_PIBY2)
            {
                m_actor->setFlipped(!m_actor->isFlipped());
            }

            m_lookAtAngle.SetRadians(newAngle);

            if (canUturn && m_lookAtAngle == getTemplate()->getMaxAngle())
            {
                startUturn();
            }
        }
        updateLookAt(_delta);
        m_timer = Max(0.0f,m_timer - _delta);
    }

    void Ray_AIBlowingBirdLookatBehavior::onFinishedAttack()
    {
        startUturn();
    }

    void Ray_AIBlowingBirdLookatBehavior::updateAnimInputs()
    {
        Super::updateAnimInputs(  );

        const f32 currentAngle = m_lookAtAngle.ToRadians();
        
        Angle angle(bfalse,currentAngle < MTH_PIBY2?currentAngle:MTH_PI-currentAngle); 
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(lookAtAngle,1473755958),angle.ToDegrees());

    }

    void Ray_AIBlowingBirdLookatBehavior::updateLookAt( f32 _dt)
    {

    }
}

