#include "precompiled_gameplay.h"

#ifndef _ITF_SPEEDINPUTPROVIDERCOMPONENT_H_
#include "gameplay/Components/FX/SpeedInputProviderComponent.h"
#endif //_ITF_SPEEDINPUTPROVIDERCOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(SpeedInputProviderComponent_Template)

    BEGIN_SERIALIZATION(SpeedInputProviderComponent_Template)
        SERIALIZE_MEMBER("speedInput", m_speedInput);
        SERIALIZE_MEMBER("speedXInput", m_speedXInput);
        SERIALIZE_MEMBER("speedYInput", m_speedYInput);
        SERIALIZE_MEMBER("speedZInput", m_speedZInput);
    END_SERIALIZATION()

    SpeedInputProviderComponent_Template::SpeedInputProviderComponent_Template()
    {
    }

    SpeedInputProviderComponent_Template::~SpeedInputProviderComponent_Template()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(SpeedInputProviderComponent)

    BEGIN_SERIALIZATION_CHILD(SpeedInputProviderComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(SpeedInputProviderComponent)
    END_VALIDATE_COMPONENT()


    SpeedInputProviderComponent::SpeedInputProviderComponent()
        : m_speed(0.f)
    {
    }

    SpeedInputProviderComponent::~SpeedInputProviderComponent()
    {
    }

    void SpeedInputProviderComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_linkComponent = m_actor->GetComponent<LinkComponent>();
    }

    void SpeedInputProviderComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        m_prevPos = m_actor->getPos();
    }

    void SpeedInputProviderComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        const Vec3d& pos = m_actor->getPos();
        Vec3d delta = (pos - m_prevPos);
        m_prevPos = pos;

        const f32 oneOverDt = 1.f / _dt;
        m_speed = delta.norm() * oneOverDt;

        sendInputEvent(getTemplate()->getSpeedInput(), m_speed);
        sendInputEvent(getTemplate()->getSpeedXInput(), delta.m_x * oneOverDt);
        sendInputEvent(getTemplate()->getSpeedYInput(), delta.m_y * oneOverDt);
        sendInputEvent(getTemplate()->getSpeedZInput(), delta.m_z * oneOverDt);
    }

    void SpeedInputProviderComponent::sendInputEvent( const StringID& _input, const f32 _value )
    {
        if (!_input.isValid())
        {
            return;
        }

        EventSetFloatInput onSetInput;
        onSetInput.setInputName(_input);
        onSetInput.setInputValue(_value);

        m_actor->onEvent(&onSetInput);

        if (m_linkComponent)
        {
            m_linkComponent->sendEventToChildren(&onSetInput);
        }
    }

#ifdef ITF_SUPPORT_EDITOR
    void SpeedInputProviderComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface,_flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        DebugDraw::text("Speed: %.2f", m_speed);
    }

    void SpeedInputProviderComponent::onEditorMove(bbool _modifyInitialPos)
    {
        Super::onEditorMove(_modifyInitialPos);

        if (!GetActor()->isUpdateAllowed() && GetActor()->getLastUpdatedFrame() < CURRENTFRAME)  // avoid double job pushing --> crash
            Update(LOGICDT);
    }
#endif // ITF_SUPPORT_EDITOR
};