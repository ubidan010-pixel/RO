#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAYWMPANELCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Ray_WM_PanelComponent.h"
#endif //_ITF_RAYWMPANELCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_UITEXTBOXESCOMPONENT_H_
#include "gameplay/components/UI/UITextBoxesComponent.h"
#endif //_ITF_UITEXTBOXESCOMPONENT_H_

namespace ITF
{
    //---------------------------------------------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_WM_PanelComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_WM_PanelComponent)
        SERIALIZE_MEMBER("tag", m_tag);
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_WM_PanelComponent)
        VALIDATE_COMPONENT_PARAM("TextComponent",m_textComponent!=NULL, "Needs a text component");
    END_VALIDATE_COMPONENT()

    Ray_WM_PanelComponent::Ray_WM_PanelComponent()
    : m_textComponent(NULL)
    {
    }

    Ray_WM_PanelComponent::~Ray_WM_PanelComponent()
    {
    }

    void Ray_WM_PanelComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_textComponent = m_actor->GetComponent<UITextBoxesComponent>();

        getTemplate()->getElectoonPath().getString(m_electoonPath);
    }

    void Ray_WM_PanelComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        u32 price = RAY_GAMEMANAGER->getMinElectoons(m_tag);

        if (RAY_GAMEMANAGER->getElectoonsCount() < price)
        {
            m_prizeStr.setTextFormat("[actor:%s] %d",m_electoonPath.cStr(),price);

            m_textComponent->setText(getTemplate()->getBoneName(),m_prizeStr);
        }
        else
        {
            m_actor->disable();
        }
    }

    //---------------------------------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_WM_PanelComponent_Template)
        BEGIN_SERIALIZATION(Ray_WM_PanelComponent_Template)

        SERIALIZE_MEMBER("offset",m_offset);
    SERIALIZE_MEMBER("bone",m_boneName);
    SERIALIZE_MEMBER("electoonPath",m_electoonPath);

    END_SERIALIZATION()

        Ray_WM_PanelComponent_Template::Ray_WM_PanelComponent_Template()
        : m_offset(0.f,2.f)
    {
    }

    Ray_WM_PanelComponent_Template::~Ray_WM_PanelComponent_Template()
    {

    }



}

