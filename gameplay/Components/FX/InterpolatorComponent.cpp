#include "precompiled_gameplay.h"

#ifndef _ITF_INTERPOLATORCOMPONENT_H_
#include "gameplay/Components/FX/InterpolatorComponent.h"
#endif //_ITF_INTERPOLATORCOMPONENT_H_

#ifndef _ITF_WORLD_MANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLD_MANAGER_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(InterpolatorComponent_Template)

    BEGIN_SERIALIZATION_ABSTRACT(InterpolatorComponent_Template)
        SERIALIZE_MEMBER("input", m_input);
        SERIALIZE_MEMBER("useMainCharacter", m_useMainCharacter);
    END_SERIALIZATION()

    InterpolatorComponent_Template::InterpolatorComponent_Template()
        : m_useMainCharacter(btrue)
    {
    }

    InterpolatorComponent_Template::~InterpolatorComponent_Template()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(InterpolatorComponent)

    BEGIN_SERIALIZATION_ABSTRACT_CHILD(InterpolatorComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(InterpolatorComponent)
    END_VALIDATE_COMPONENT()

    InterpolatorComponent::InterpolatorComponent()
        : Super()
        , m_linkComponent(NULL)
        , m_inputValue(0.0f)
    {
    }

    InterpolatorComponent::~InterpolatorComponent()
    {
    }

    void InterpolatorComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_linkComponent = m_actor->GetComponent<LinkComponent>();
    }

    void InterpolatorComponent::Update( f32 _dt )
    {
        if ( !canUpdate() )
            return;

        Super::Update(_dt);

        m_inputValue = getInputValue(_dt);

        EventSetFloatInput onSetInput;
        onSetInput.setInputName(getTemplate()->getInput());
        onSetInput.setInputValue(m_inputValue);

        m_actor->onEvent(&onSetInput);

        if (m_linkComponent)
        {
            m_linkComponent->sendEventToChildren(&onSetInput);
        }
    }

    Vec2d InterpolatorComponent::getInputPos() const
    {
        Vec2d input;

        if (getTemplate()->getUseMainCharacter())
        {
            Actor* mainPlayer = AIUtils::getLeadLivePlayer(m_actor);
            if (mainPlayer)
                input = mainPlayer->get2DPos();
        }
        else
        {
            Vec3d pos = Vec3d(GFX_ADAPTER->getScreenWidth() * 0.5f, GFX_ADAPTER->getScreenHeight() * 0.5f,0.0f);
            Vec3d pos3d;
            GFX_ADAPTER->compute2DTo3D(pos,pos3d);
            input = pos3d.truncateTo2D();
        }

        return input;
    }

}