#include "precompiled_gameplay.h"

#ifndef _ITF_UIGAMEOPTIONCOMPONENT_H_
#include "gameplay/components/UI/UIGameOptionComponent.h"
#endif //_ITF_UIGAMEOPTIONCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_OBJECTPATH_H_
#include "core/ObjectPath.h"
#endif //_ITF_OBJECTPATH_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UIGameOptionComponent)
    BEGIN_SERIALIZATION_CHILD(UIGameOptionComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("labelPath", m_labelPath);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIGameOptionComponent::UIGameOptionComponent()
    : Super()
    , m_labelActor(NULL)
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIGameOptionComponent::~UIGameOptionComponent()
    {
        clear();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIGameOptionComponent::clear()
    {
        m_labelActor = NULL;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIGameOptionComponent::resolveLabelActor()
    {
        m_labelActor = NULL;

        if (m_labelPath.isEmpty())
        {
            return;
        }

        ObjectPath labelPath;
        ITF_STDSTRING pathStr = m_labelPath.cStr();
        labelPath.fromString(pathStr);

        if (!labelPath.isValid())
        {
            return;
        }

        Pickable* pickable = NULL;
        if (labelPath.getIsAbsolute())
        {
            pickable = SceneObjectPathUtils::getObjectFromAbsolutePath(labelPath);
        }
        else
        {
            pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, labelPath);
        }

        if (pickable)
        {
            m_labelActor = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor, 2546623115));
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIGameOptionComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        resolveLabelActor();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIGameOptionComponent::Update(f32 _deltaTime)
    {
        Super::Update(_deltaTime);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIGameOptionComponent::onActorClearComponents()
    {
        Super::onActorClearComponents();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIGameOptionComponent::onBecomeActive()
    {
        Super::onBecomeActive();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIGameOptionComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIGameOptionComponent::onPressed()
    {
        Super::onPressed();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIGameOptionComponent::onReleased()
    {
        Super::onReleased();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIGameOptionComponent::onRollover()
    {
        Super::onRollover();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIGameOptionComponent::onRollout()
    {
        Super::onRollout();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIGameOptionComponent::onAction(const StringID & action)
    {
        Super::onAction(action);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UIGameOptionComponent_Template)
    BEGIN_SERIALIZATION(UIGameOptionComponent_Template)
    END_SERIALIZATION()

    UIGameOptionComponent_Template::UIGameOptionComponent_Template()
    : Super()
    {
    }

    UIGameOptionComponent_Template::~UIGameOptionComponent_Template()
    {
    }
}

