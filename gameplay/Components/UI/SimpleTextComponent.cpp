#include "precompiled_gameplay.h"

#ifndef _ITF_SIMPLETEXTCOMPONENT_H_
#include "gameplay/components/UI/SimpleTextComponent.h"
#endif //_ITF_SIMPLETEXTCOMPONENT_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(SimpleTextComponent)

BEGIN_SERIALIZATION_CHILD(SimpleTextComponent)

    BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_MEMBER("TextLabel", m_text);
        SERIALIZE_MEMBER("lineId", m_lineId);
        SERIALIZE_MEMBER("drawUsingEngine", m_drawUsingEngine);
        SERIALIZE_MEMBER("drawBoxWidth", m_drawBoxWidth);
        SERIALIZE_MEMBER("drawBoxHeight", m_drawBoxHeight);
        SERIALIZE_MEMBER("textSize", m_textSize);
        SERIALIZE_MEMBER("textColor", m_textColor);
        SERIALIZE_MEMBER("backgroundColor", m_backgroundColor);
        SERIALIZE_MEMBER("centerText", m_center);
    END_CONDITION_BLOCK()

END_SERIALIZATION()

SimpleTextComponent::SimpleTextComponent()
: Super()
, m_drawUsingEngine(btrue)
, m_drawBoxWidth(200.f)
, m_drawBoxHeight(100.f)
, m_textBox(NULL)
, m_textSize(25.0f)
, m_center(btrue)
, m_display(btrue)
, m_needUpdate(btrue)
{
    m_backgroundColor = Color(0.0f,0.0f,0.0f,0.0f);
    m_textColor = COLOR_BLACK;
}


SimpleTextComponent::~SimpleTextComponent()
{
}

void SimpleTextComponent::onBecomeActive()
{
    m_textBox =  m_actor->GetComponent<UITextBox>();
    m_needUpdate = btrue;
}

void SimpleTextComponent::Update(f32 _dt)
{
    if (m_textBox && m_needUpdate)
    {
        updateText();
        m_needUpdate = bfalse;
    }
}

void SimpleTextComponent::updateText()
{
    if (m_textBox)
    {
        TextInfo textInfo;
        textInfo.m_color = m_textColor.getAsU32();
        textInfo.m_size = m_textSize;
        textInfo.m_text.setText(m_text.cStr());
        textInfo.m_lineId = m_lineId;


        UITextBox::BoxPosition alignement = UITextBox::BoxPosition_TopLeft;
        if(m_center)
            alignement = UITextBox::BoxPosition_Center;

        m_textBox->setData(m_actor->getPos(),m_textSize,textInfo,m_drawBoxWidth * m_actor->getScale().m_x,m_drawBoxHeight* m_actor->getScale().m_y,0,Vec2d::Zero,Vec2d::One,m_backgroundColor.getAsU32(), btrue, alignement);
    }
}

void SimpleTextComponent::onEvent(Event * _event)
{
    if (EventActivate * eventActivate = _event->DynamicCast<EventActivate>(ITF_GET_STRINGID_CRC(EventActivate,2639596100)))
    {
        if ( m_textBox )
        {
            eventActivate->getActivated() ? m_textBox->show() : m_textBox->hide();
        }
    }
}

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(SimpleTextComponent_Template)
BEGIN_SERIALIZATION(SimpleTextComponent_Template)
END_SERIALIZATION()

}

