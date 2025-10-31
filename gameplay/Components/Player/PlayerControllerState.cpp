#include "precompiled_gameplay.h"

#ifndef _ITF_PLAYERCONTROLLERCOMPONENT_H_
#include "gameplay/Components/Player/PlayerControllerComponent.h"
#endif //_ITF_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(PlayerControllerState)

    PlayerControllerState::PlayerControllerState()
        : m_actor(NULL)
        , m_parent(NULL)
        , m_animComponent(NULL)
        , m_characterPhys(NULL)
        , m_fxController(NULL)
    {
    }

    void PlayerControllerState::init(
        PlayerControllerComponent* _parent,
        AnimatedComponent* _animComponent,
        StickToPolylinePhysComponent* _characterPhys,
        FXControllerComponent* _fxController )
    {
        m_actor = _parent->GetActor();
        m_parent = _parent;
        m_animComponent = _animComponent;
        m_characterPhys = _characterPhys;
        m_fxController = _fxController;
    }

    void PlayerControllerState::onEnter()
    {
        m_animComponent->setAnim(getAnimAction());
    }

    bbool PlayerControllerState::isCurrentAnimFinished() const
    {
        return m_animComponent->isMainNodeFinished();

    }
}
