#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_RAY_POWERUPNAMES_H_
#include "rayman/gameplay/Ray_PowerUpNames.h"
#endif //_ITF_RAY_POWERUPNAMES_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StateCaughtInPipe)

    Ray_PlayerControllerComponent::StateCaughtInPipe::StateCaughtInPipe()
        : m_released(bfalse)
    {
    }

    Ray_PlayerControllerComponent::StateCaughtInPipe::~StateCaughtInPipe()
    {
    }


    void Ray_PlayerControllerComponent::StateCaughtInPipe::onEnter()
    {
        Super::onEnter();

        m_characterPhys->setDisabled(btrue);

        if (m_parent->getStance() == STANCE_HELICO)
        {
            m_parent->stopHelicoFx();
        }
        m_released = bfalse;
    }

    void Ray_PlayerControllerComponent::StateCaughtInPipe::onExit()
    {
        Super::onExit();

        m_parent->m_pipeRef = ITF_INVALID_OBJREF;
        m_characterPhys->setDisabled(bfalse);
        if (m_parent->getStance() == STANCE_HELICO)
        {
            m_parent->startHelicoFx();
        }
    }

    bbool Ray_PlayerControllerComponent::StateCaughtInPipe::checkStateChange( f32 _dt )
    {
        if (m_released)
        {
            m_released = bfalse;

            if ( m_parent->isPowerUpEnabled(Ray_PowerUp_Helicopter) &&
                 m_parent->isHoldingHelicopterAction() )
            {
                m_parent->changeState(&m_parent->m_stateHelicopter);
            }
            else
            {
                m_parent->changeState(&m_parent->m_stateFalling);
            }

            return btrue;
        }

        return bfalse;
    }

    void Ray_PlayerControllerComponent::StateCaughtInPipe::onEvent( Event* _event )
    {
        if (Ray_EventCaughtInPipe* eventPipe = _event->DynamicCast<Ray_EventCaughtInPipe>(ITF_GET_STRINGID_CRC(Ray_EventCaughtInPipe,1030315255)))
        {
            if (!eventPipe->getCatch())
            {
                m_released = btrue;
            }
        }
    }

}
