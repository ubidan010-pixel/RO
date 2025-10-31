#include "precompiled_gameplay.h"

#ifndef _ITF_UISLIDERCOMPONENT_H_
#include "gameplay/components/UI/UISliderComponent.h"
#endif //_ITF_UISLIDERCOMPONENT_H_

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif //_ITF_UIMENUMANAGER_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_UIMENU_H_
#include "gameplay/components/UI/UIMenu.h"
#endif //_ITF_UIMENU_H_

#ifndef _ITF_TEXTUREGRAPHICCOMPONENT2D_H_
#include "engine/actors/components/texturegraphiccomponent2D.h"
#endif //_ITF_TEXTUREGRAPHICCOMPONENT2D_H_

namespace ITF
{
///////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(UISliderComponent)
BEGIN_SERIALIZATION_CHILD(UISliderComponent)
END_SERIALIZATION()

///////////////////////////////////////////////////////////////////////////////////////////
UISliderComponent::UISliderComponent()
    : m_linkComponent(NULL)
    , m_isSliding(bfalse)
    , m_child1(NULL)
    , m_child2(NULL)
    , m_value(-1.0f)
    , m_valueUpdated(bfalse)
    , m_graphComponent(NULL)
{    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    UISliderComponent::~UISliderComponent()
    {
        clear();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UISliderComponent::clear()
    {
        UI_MENUMANAGER->removeUIComponentOfTheMenu(this);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UISliderComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        GetActor()->setAlwaysVisible(btrue);
        GetActor()->set2DActor(btrue);

        m_linkComponent = m_actor->GetComponent<LinkComponent>();

        if (m_linkComponent)
        {
            const LinkComponent::ChildrenList & children = m_linkComponent->getChildren();
            if (children.size() == 2)
            {
                if ( Pickable* pickable1 = SceneObjectPathUtils::getObjectFromAbsolutePath(children[0].getPath()) )
                {
                    Actor * actor = pickable1->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) ;
                    if (actor)
                    {
                        UISliderComponent * sliderComponent = actor->GetComponent<UISliderComponent>();
                        if (sliderComponent)
                        {
                            m_child1 = sliderComponent;
                        }
                    }
                }
                if ( Pickable* pickable2 = SceneObjectPathUtils::getObjectFromAbsolutePath(children[1].getPath()) )
                {
                    Actor * actor = pickable2->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) ;
                    if (actor)
                    {
                        UISliderComponent * sliderComponent = actor->GetComponent<UISliderComponent>();
                        if (sliderComponent)
                        {
                            m_child2 = sliderComponent;
                        }
                    }
                }
            }

            if( getTemplate()->getIsCursor() )
                setValue(0.5f);
        }

        m_graphComponent = GetActor()->GetComponent<TextureGraphicComponent2D>();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UISliderComponent::Update( f32 _deltaTime )
    {
        Super::Update(_deltaTime);

        if( !getTemplate()->getIsCursor() )
            return; 
        
        if ( !m_isSliding )
            return;

        if( !m_child1 || !m_child2 )
            return;

        // Mouse POS
        i32 x ,y;
        INPUT_ADAPTER->getMousePos(x, y);
        f32 screenWidth = (f32)GFX_ADAPTER->getScreenWidth();

        if (screenWidth <= 0.0f)
            return;

        f32 relativePos = x / screenWidth;
        if ( relativePos > m_child1->getRelativePosX() && relativePos < m_child2->getRelativePosX()  )
            m_relativePosX = relativePos;


        f32 posFromMin = m_relativePosX - m_child1->getRelativePosX() ;
        f32 width = m_child2->getRelativePosX() - m_child1->getRelativePosX();

        setValue( posFromMin / width, bfalse );
    }

    void UISliderComponent::onPressed()
    {
        m_isSliding = btrue;
    }

    void UISliderComponent::onReleased()
    {
        m_isSliding = bfalse;
    }

    void UISliderComponent::onActorClearComponents()
    {
        Super::onActorClearComponents();
        
    }

    void UISliderComponent::onRollover()
    {
        m_originalScale = GetActor()->getScale();
        GetActor()->setScale( m_originalScale * getTemplate()->getScaleOnSelected() );

        /*if(m_graphComponent)
        {
          m_graphComponent->setDrawColor( getTemplate()->getColorOnSelected().getAsU32() );
        }*/
    }

    void UISliderComponent::onRollout()
    {
        GetActor()->setScale( m_originalScale );

        if(m_graphComponent)
        {
            m_graphComponent->setDrawColor( COLOR_WHITE );
        }
    }

    void UISliderComponent::setValue(f32 val, bbool updatePosition)
    {
        val = Clamp ( val, 0.0f, 1.0f);
        if ( val == m_value )
            return ;

        if ( updatePosition && m_child2 && m_child1 )
        {
            f32 width = m_child2->getRelativePosX() - m_child1->getRelativePosX();
            m_relativePosX = width * val + m_child1->getRelativePosX();
        }
        
        m_value = val;
        m_valueUpdated = btrue;
    }

    void UISliderComponent::onAction( const StringID & action )
    {
        if(!m_isSliding)
            return ;

        if ( action == input_actionID_RightHold )
        {
            setValue( getValue() + getTemplate()->getSpeedCursor() );
        }
        else if ( action == input_actionID_LeftHold )
        {
            setValue( getValue() - getTemplate()->getSpeedCursor() );
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(UISliderComponent_Template)
    BEGIN_SERIALIZATION_CHILD(UISliderComponent_Template)    
    SERIALIZE_MEMBER("scaleOnSelected", m_scaleOnSelected);
    SERIALIZE_MEMBER("isCursor", m_isCursor);
    SERIALIZE_MEMBER("colorOnSelected",m_colorOnSelected);
    SERIALIZE_MEMBER("speedCursor",m_speedCursor);
END_SERIALIZATION()

UISliderComponent_Template::UISliderComponent_Template() 
:m_isCursor(bfalse)
,m_scaleOnSelected(1.0f)
,m_colorOnSelected(COLOR_WHITE)
,m_speedCursor(0.01f)
{
}

}
