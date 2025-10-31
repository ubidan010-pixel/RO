#include "precompiled_gameplay.h"

#ifndef _ITF_AIJUMPACTION_H_
#include "gameplay/ai/Actions/AIJumpAction.h"
#endif //_ITF_AIJUMPACTION_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(AIJumpAction_Template)

    BEGIN_SERIALIZATION_CHILD(AIJumpAction_Template)
        SERIALIZE_MEMBER("jumpForce", m_jumpForce);
        SERIALIZE_MEMBER("minXSpeed", m_minXSpeed);
        SERIALIZE_MEMBER("maxXSpeed", m_maxXSpeed);
        SERIALIZE_MEMBER("minXForce", m_minXForce);
        SERIALIZE_MEMBER("maxXForce", m_maxXForce);
    END_SERIALIZATION()

        AIJumpAction_Template::AIJumpAction_Template()
        : m_jumpForce(500.f)
        , m_minXSpeed(0.f)
        , m_maxXSpeed(0.f)
        , m_minXForce(0.f)
        , m_maxXForce(0.f)
    {
        m_animDefaultAction = ITF_GET_STRINGID_CRC(JUMP,2374255179);
    }

    IMPLEMENT_OBJECT_RTTI(AIJumpAction)

    BEGIN_VALIDATE_ACTION(AIJumpAction)
        VALIDATE_ACTION_PARAM("", m_physComponent, "AIJumpAction requires a StickToPolylinePhysComponent");
    END_VALIDATE_ACTION()


    AIJumpAction::AIJumpAction()
        : m_physComponent(NULL)
        , m_justStarted(bfalse)
    {
    }

    AIJumpAction::~AIJumpAction()
    {
    }

    void AIJumpAction::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();
    }

    void AIJumpAction::onActivate()
    {
        Super::onActivate();

        m_justStarted = btrue;

        Vec2d jumpDir = -m_aiComponent->getCurrentGravityDir();
        jumpDir *= getTemplate()->getJumpForce();
        m_physComponent->addForce(jumpDir);
    }

    void AIJumpAction::update( f32 _delta )
    {
        if ( m_justStarted )
        {
            m_justStarted = bfalse;
            return;
        }

        if ( m_physComponent->getStickedEdge() )
        {
            m_aiBehavior->onActionFinished();
        }
        else
        {
            if ( getTemplate()->getMinXSpeed() || getTemplate()->getMaxXSpeed())
            {
                if (getTemplate()->getMaxXForce()> 0.0f)
                {
                    const Vec2d& gravityDir = m_aiComponent->getCurrentGravityDir();
                    Vec2d gravityNorm = gravityDir.getPerpendicular();
                    Vec2d lookDir = AIUtils::getLookDir(m_actor,m_actor->isFlipped());

                    f32 xSpeed = gravityNorm.dot(m_physComponent->getSpeed());
                    bbool sameDir = ( lookDir.m_x >= 0.f && m_physComponent->getSpeed().m_x >= 0.f ) ||
                        ( lookDir.m_x <= 0.f && m_physComponent->getSpeed().m_x <= 0.f );
                    f32 forceX = 0.0f;
                    f32 absSpeedX = f32_Abs(xSpeed);

                    if ( !sameDir )
                    {
                        forceX = getTemplate()->getMaxXForce();
                    }
                    else if ( absSpeedX > getTemplate()->getMaxXSpeed() )
                    {
                        forceX = getTemplate()->getMinXForce();
                    }
                    else if ( absSpeedX < getTemplate()->getMinXSpeed() )
                    {
                        forceX = getTemplate()->getMaxXForce();
                    }
                    else
                    {
                        f32 t = (absSpeedX-getTemplate()->getMinXSpeed())/(getTemplate()->getMaxXSpeed()-getTemplate()->getMinXSpeed());
                        forceX = getTemplate()->getMinXForce() + ( (getTemplate()->getMaxXForce() - getTemplate()->getMinXForce()) * (1.f-t) );
                    }

                    Vec2d force = gravityNorm*forceX;

                    if ( lookDir.dot(gravityNorm) < 0.f )
                    {
                        force *= -1.f;
                    }

                    m_physComponent->addForce(force);
                }
            }
        }
    }

}