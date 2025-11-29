#include "precompiled_gameplay.h"

#ifndef _ITF_UIACTIONBUTTONCOMPONENT_H_
#include "gameplay/components/UI/UIActionButtonComponent.h"
#endif //_ITF_UIACTIONBUTTONCOMPONENT_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UIActionButtonComponent)
    BEGIN_SERIALIZATION_CHILD(UIActionButtonComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("highlightBgPath", m_highlightBgPath);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIActionButtonComponent::UIActionButtonComponent()
    : Super()
    , m_highlightBgPath()
    , m_highlightBgActor(nullptr)
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIActionButtonComponent::~UIActionButtonComponent()
    {
        clear();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIActionButtonComponent::clear()
    {
        m_highlightBgPath.clear();
        m_highlightBgActor = nullptr;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIActionButtonComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        if (!m_highlightBgPath.isEmpty() && m_actor)
        {
            Scene* scene = m_actor->getScene();
            if (scene)
            {
                m_highlightBgActor = scene->getActorFromUserFriendly(m_highlightBgPath);
                if (m_highlightBgActor)
                {
                    m_highlightBgActor->disable();
                }
                else
                {
                    LOG("[UIActionButtonComponent] Highlight actor '%s' not found", m_highlightBgPath.cStr());
                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIActionButtonComponent::onActorClearComponents()
    {
        Super::onActorClearComponents();
        m_highlightBgActor = nullptr;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIActionButtonComponent::onBecomeActive()
    {
        Super::onBecomeActive();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIActionButtonComponent::onBecomeInactive()
    {
        // Hide highlight when becoming inactive
        setHighlightVisible(bfalse);
        Super::onBecomeInactive();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIActionButtonComponent::onPressed()
    {
        Super::onPressed();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIActionButtonComponent::onReleased()
    {
        Super::onReleased();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIActionButtonComponent::onRollover()
    {
        Super::onRollover();
        setHighlightVisible(btrue);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIActionButtonComponent::onRollout()
    {
        Super::onRollout();
        setHighlightVisible(bfalse);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIActionButtonComponent::onAction(const StringID & action)
    {
        Super::onAction(action);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIActionButtonComponent::setHighlightVisible(bbool visible)
    {
        if (!m_highlightBgActor)
        {
            if (!m_highlightBgPath.isEmpty() && m_actor)
            {
                Scene* scene = m_actor->getScene();
                if (scene)
                {
                    m_highlightBgActor = scene->getActorFromUserFriendly(m_highlightBgPath);
                }
            }
            
            if (!m_highlightBgActor)
                return;
        }

        if (visible)
            m_highlightBgActor->enable();
        else
            m_highlightBgActor->disable();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UIActionButtonComponent_Template)
    BEGIN_SERIALIZATION_CHILD(UIActionButtonComponent_Template)
    END_SERIALIZATION()

    UIActionButtonComponent_Template::UIActionButtonComponent_Template()
    : Super()
    {
    }

    UIActionButtonComponent_Template::~UIActionButtonComponent_Template()
    {
    }
}
