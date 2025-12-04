#include "precompiled_gameplay.h"

#ifndef _ITF_UISELECTABLEFRIENDLYCOMPONENT_H_
#include "gameplay/components/UI/UISelectableFriendlyComponent.h"
#endif //_ITF_UISELECTABLEFRIENDLYCOMPONENT_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(UISelectableFriendlyComponent)
    BEGIN_SERIALIZATION_CHILD(UISelectableFriendlyComponent)
    END_SERIALIZATION()

    UISelectableFriendlyComponent::UISelectableFriendlyComponent()
        : Super()
        , m_lastSelected(bfalse)
    {
    }

    UISelectableFriendlyComponent::~UISelectableFriendlyComponent()
    {
    }

    void UISelectableFriendlyComponent::applyFriendlyFromSelection()
    {
        const bbool isSelected = (getIsSelected() || m_forceSelected);
        // const String8& targetFriendly = isSelected && !m_selectedFriendly.isEmpty()
        //                                     ? m_selectedFriendly
        //                                     : m_defaultFriendly;
        const String8& targetFriendly = m_selectedFriendly;
        if (targetFriendly.isEmpty())
        {
            return;
        }
        if (targetFriendly == m_friendly)
        {
            return;
        }
        setFriendly(targetFriendly);
        m_content = targetFriendly;   
        m_textChanged = btrue;
        m_isContentFill = bfalse;
        if (UI_TEXTMANAGER && UI_TEXTMANAGER->isReady())
        {
            fillContent();
            m_isContentFill = btrue;
        }
    }

    void UISelectableFriendlyComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_defaultFriendly = getTemplate()->getDefaultFriendly();
        m_selectedFriendly = getTemplate()->getSelectedFriendly();
        m_lastSelected = getIsSelected() || m_forceSelected;

        applyFriendlyFromSelection();
    }

    void UISelectableFriendlyComponent::Update(f32 _deltaTime)
    {
        const bbool isSelectedNow = getIsSelected() || m_forceSelected;
        if (isSelectedNow != m_lastSelected)
        {
            m_lastSelected = isSelectedNow;
            applyFriendlyFromSelection();
        }

        Super::Update(_deltaTime);
    }

    IMPLEMENT_OBJECT_RTTI(UISelectableFriendlyComponent_Template)
    BEGIN_SERIALIZATION_CHILD(UISelectableFriendlyComponent_Template)
        SERIALIZE_MEMBER("friendlyDefault", m_defaultFriendly);
        SERIALIZE_MEMBER("friendlySelected", m_selectedFriendly);
    END_SERIALIZATION()
}
