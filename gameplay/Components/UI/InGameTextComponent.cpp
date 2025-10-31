#include "precompiled_gameplay.h"

#ifndef _ITF_INGAMETEXTCOMPONENT_H_
#include "gameplay/Components/UI/InGameTextComponent.h"
#endif //_ITF_INGAMETEXTCOMPONENT_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif //_ITF_UIMENUMANAGER_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_UITEXTBOXESCOMPONENT_H_
#include "gameplay/components/UI/UITextBoxesComponent.h"
#endif //_ITF_UITEXTBOXESCOMPONENT_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

namespace ITF {

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(InGameTextComponent)
BEGIN_SERIALIZATION_CHILD(InGameTextComponent)
END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(InGameTextComponent)

    VALIDATE_COMPONENT_PARAM("AnimatedComponent", m_animComponent, "Animated component mandatory");
    VALIDATE_COMPONENT_PARAM("UITextBox", m_textBoxComponent, "UITextBox component mandatory");

END_VALIDATE_COMPONENT()

InGameTextComponent::InGameTextComponent()
: m_animComponent(NULL)
, m_textBoxComponent(NULL)
, m_locId(LocalisationId::Invalid)
, m_currentScaleX(0.f)
, m_targetScaleX(0.f)
, m_velScaleX(0.f)
, m_updateText(bfalse)
, m_appear(bfalse)
{
}

InGameTextComponent::~InGameTextComponent()
{
}

void InGameTextComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_animComponent = m_actor->GetComponent<AnimatedComponent>();

    if ( m_animComponent )
    {
        m_animComponent->setUpdateAnimInput(this);
        m_animComponent->setRank2D(UI_TEXTMANAGER->getAnimDepthRank());
    }

    m_textBoxComponent = m_actor->GetComponent<UITextBoxesComponent>();
}

void InGameTextComponent::onBecomeActive()
{
    Super::onBecomeActive();

    m_animComponent->enableDraw(m_locId!=LocalisationId::Invalid);

    if ( m_updateText )
    {
        prepareText();
        m_currentScaleX = m_targetScaleX;
        m_velScaleX = 0.f;
    }
}

void InGameTextComponent::setText( const LocalisationId& _id )
{
    if ( m_locId != _id )
    {
        m_locId = _id;

        if ( m_locId != LocalisationId::Invalid )
        {
            m_updateText = btrue;

            if ( m_actor->isActive() )
            {
                prepareText();
            }
        }
        else
        {
            m_animComponent->enableDraw(bfalse);
        }
    }
}

void InGameTextComponent::Update( f32 _dt )
{
    Super::Update(_dt);

    updateScale(_dt);
}

void InGameTextComponent::updateScale( f32 _dt )
{
    f32 dif = m_targetScaleX - m_currentScaleX;
    f32 f = (dif*getTemplate()->getScaleK()) - (m_velScaleX*getTemplate()->getScaleD());

    m_velScaleX += f*_dt;
    m_currentScaleX += m_velScaleX*_dt;
}

void InGameTextComponent::prepareText()
{
    u32 numChars = UITextBox::calculateNumOfCharactersFromLoc(m_locId);

    // HACK[LaurentCou]: this heuristic doesn't work very well with Japanese
    // ideograms, let's act like there was a bit more characters in play
    // (fixes RO-17623, RO-17628, RO-17630 & RO-17632)
    if (LOCALISATIONMANAGER->getCurrentLanguage() == ITF_LANGUAGE_JAPANESE)
    {
        numChars = u32(numChars * 1.75f);
    }
    
    numChars = Clamp(numChars,getTemplate()->getMinScaleNumCharacters(),getTemplate()->getMaxScaleNumCharacters());

    f32 t = static_cast<f32>(numChars-getTemplate()->getMinScaleNumCharacters()) / static_cast<f32>(getTemplate()->getMaxScaleNumCharacters()-getTemplate()->getMinScaleNumCharacters());

    m_targetScaleX = Interpolate(0.f,1.f,t);
    m_updateText = bfalse;

    m_textBoxComponent->setLineId(getTemplate()->getTextBone(),m_locId);
    m_animComponent->enableDraw(btrue);
}

void InGameTextComponent::playAppear()
{
    m_appear = btrue;
    m_animComponent->resetTree();
}

void InGameTextComponent::playDisappear()
{
    m_appear = bfalse;
    m_animComponent->resetTree();
}

void InGameTextComponent::updateAnimInput()
{
    m_animComponent->setInput(ITF_GET_STRINGID_CRC(scaleX,1292250801),m_currentScaleX);
    m_animComponent->setInput(ITF_GET_STRINGID_CRC(visible,3088086709),m_appear);
}

ITF::bbool InGameTextComponent::isDisappearFinished()
{
    return !m_appear && m_animComponent->isMainNodeFinished();
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(InGameTextComponent_Template)
BEGIN_SERIALIZATION(InGameTextComponent_Template)

    SERIALIZE_MEMBER("textBone",m_textBone);
    SERIALIZE_MEMBER("scaleK",m_scaleXK);
    SERIALIZE_MEMBER("scaleD",m_scaleXD);
    SERIALIZE_MEMBER("minScaleNumCharacters",m_minScaleNumCharacters);
    SERIALIZE_MEMBER("maxScaleNumCharacters",m_maxScaleNumCharacters);

END_SERIALIZATION()

//------------------------------------------------------------------------------
InGameTextComponent_Template::InGameTextComponent_Template()
: m_scaleXK(500.f)
, m_scaleXD(50.f)
, m_minScaleNumCharacters(100)
, m_maxScaleNumCharacters(200)
{
}

//------------------------------------------------------------------------------
InGameTextComponent_Template::~InGameTextComponent_Template()
{
}

} // namespace ITF
