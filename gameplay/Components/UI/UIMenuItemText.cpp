#include "precompiled_gameplay.h"

#ifndef _ITF_UIMENUITEMTEXT_H_
#include "gameplay/components/UI/UIMenuItemText.h"
#endif //_ITF_UIMENUITEMTEXT_H_

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif //_ITF_UIMENUMANAGER_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UIMenuItemText)
    BEGIN_SERIALIZATION_CHILD(UIMenuItemText)
    SERIALIZE_MEMBER("lineId",m_lineId);
    END_SERIALIZATION()

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIMenuItemText::UIMenuItemText()
    : Super()
    , m_timer(0.0f)
    {    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIMenuItemText::~UIMenuItemText()
    {
        clear();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIMenuItemText::clear()
    {
        UI_MENUMANAGER->removeUIComponentOfTheMenu(this);
    }


    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIMenuItemText::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_actor->setAlwaysVisible(btrue);

        m_textBox = m_actor->GetComponent<UITextBox>();

        if(!m_textBox)
            m_isTextBoxActive = bfalse;

        m_textChanged = btrue;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIMenuItemText::Update( f32 _deltaTime )
    {
        if(m_isTextBoxActive)
        {
            u32 color = getColor();

            f32 initSize = getTemplate()->getInitialFontHeight();
            f32 finalScale = 1.f;
            if (getIsSelected())
            {
                f32 select = getTemplate()->getFontHeightSelected();
                if (select < 0)
                    select = initSize;
                
                finalScale = select / initSize;
            }

            TextInfo textInfo(
                m_content.cStr(),
                color,
                initSize,
                m_friendly,
                m_lineId
                );

            if (getIsSelected())
            {
                f32 curScale = 1.0f + getTemplate()->getIdleSelectedScale() * f32_Sin(MTH_2PI * getTemplate()->getIdleSelectedPulseFrequency() * m_timer);
                m_actor->setScale(Vec2d::One * curScale * finalScale);
            }
            else
            {
                m_actor->setScale(Vec2d::One * finalScale);
            }

            m_textBox->setNoScaleWithActor(bfalse);

            m_textBox->setData(m_actor->getPos(), initSize, textInfo, 
                0.0f, 0.0f, // NB[LaurentCou]: let the text box handle its size
                getDepthRank(), Vec2d::Zero, Vec2d::One, COLOR_WHITE, bfalse, 
                UITextBox::BoxPosition_Center, getShadowOffset(), getShadowColor(), getTemplate()->getLineSpacingFactor(), ResourceID::Invalid, m_fontIndex,
                getTemplate()->getTextMode(), getTemplate()->getTextModeY());

            // NB[LaurentCou]: refresh text box size on resolution change (RO-13880)
            // NB2[LaurentCou]: if there's an anim, the UIComponent scales the actor for us,
            // no need to do this
            if (!m_animLightComponent)
            {
                m_textBox->scaleBox();
            }
        }  

        m_timer += _deltaTime;
        Super::Update(_deltaTime);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIMenuItemText::Draw2D()
    {
        Super::Draw2D();
        
        if (m_fontIndex!= U32_INVALID)
        {
            if(!m_isTextBoxActive)
            {
                GFX_ADAPTER->drawText(GFX_ADAPTER->getFontByIndex(m_fontIndex), getDepthRank(), getColor(), m_actor->getPos().m_x, m_actor->getPos().m_y, m_actor->getPos().m_z, getShadowOffset(), getShadowColor(), m_content, 0, getTemplate()->getTextMode(), m_fontHeight);
            }
        }
        else 
        {
#ifdef ITF_SUPPORT_CHEAT
            String textSelect = m_content;
            if (m_bSelected)
                textSelect += " [SELECTED]";

            GFX_ADAPTER->drawDBGText(textSelect, m_actor->getPos().m_x, m_actor->getPos().m_y);
#endif //ITF_SUPPORT_CHEAT
        }  
    }


    void UIMenuItemText::onRollover()
    {
        m_timer = 0.0f;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UIMenuItemText_Template)
    BEGIN_SERIALIZATION_CHILD(UIMenuItemText_Template)
    SERIALIZE_MEMBER("idleSelectedScale", m_idleSelectedScale);
    SERIALIZE_MEMBER("idleSelectedPulseFrequency", m_idleSelectedPulseFrequency);
   END_SERIALIZATION()
}
