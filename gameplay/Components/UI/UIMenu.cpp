#include "precompiled_gameplay.h"

#ifndef _ITF_UIMENU_H_
#include "gameplay/components/UI/UIMenu.h"
#endif //_ITF_UIMENU_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif // _ITF_SCENE_H_

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif //_ITF_UIMENUMANAGER_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    UIMenu::UIMenu(const StringID &_menuID)
    : m_worldMenuRef(ITF_INVALID_OBJREF), m_AlwaysStaySelected(bfalse), m_forcePadIndex(U32_INVALID), m_active(btrue)
    , m_padTypeAllowed(U32_INVALID)
    {  
        m_menuID = _menuID;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIMenu::~UIMenu()
    {    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIMenu::addUIComponent(ObjectRef _ref)
    {
        UIComponent* pUIComponent  = UIMenuManager::getUIComponent(_ref);
        if (pUIComponent)
        {
            if (pUIComponent->getIsDefaultSelected())
            {
                // Unselect all the others UIComponents
                UIComponent* pCurrentUIComponent;
                for (u32 i = 0; i < m_UIComponentsList.size(); i++)
                {
                    pCurrentUIComponent  = UIMenuManager::getUIComponent(m_UIComponentsList[i]);
                    if (pCurrentUIComponent)
                        pCurrentUIComponent->setIsSelected(bfalse);
                }
            }
            else
            {
                // Choose one UIComponent selected by default (the first UIComponent added)
                UIComponent* pCurrentUIComponent;
                bbool bFirstItem = btrue;
                for (u32 i = 0; i < m_UIComponentsList.size(); i++)
                {
                    // Is it the first UIComponent of the menu?
                    pCurrentUIComponent = UIMenuManager::getUIComponent(m_UIComponentsList[i]);
                    if (pCurrentUIComponent)
                    {
                        bFirstItem = bfalse;
                        break;
                    }
                }
                if (bFirstItem)
                {
                    pUIComponent->setIsSelected(btrue);
                    pUIComponent->onRollover();
                    Actor* actor = pUIComponent->GetActor();
                    if (actor)
                    {
                        SafeArray<UIComponent*> siblings = actor->GetComponents<UIComponent>();
                        for (u32 j = 0; j < siblings.size(); ++j)
                        {
                            if (siblings[j] != pUIComponent)
                            {
                                siblings[j]->setIsSelected(btrue);
                                siblings[j]->onRollover();
                            }
                        }
                    }
                }
            }
            m_UIComponentsList.push_back(_ref);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIMenu::removeUIComponent (UIComponent*  _uiComponent)
    {
        ITF_ASSERT(_uiComponent!=NULL);
        if(!_uiComponent)
            return; 

        ObjectRef _ref = _uiComponent->GetActor()->getRef();
        if (_ref != ITF_INVALID_OBJREF)
        {
            i32 index = m_UIComponentsList.find(_ref);
            if (index != -1) // Component found
            {
                if (_uiComponent && _uiComponent->getIsSelected())
                {
                    m_UIComponentsList.eraseKeepOrder(index);

                    // Select randomly a UIComponent of the same menu (the first encountered)
                    UIComponent* pCurrentUIComponent;
                    for (u32 i = 0; i < m_UIComponentsList.size(); i++)
                    {
                        pCurrentUIComponent  = UIMenuManager::getUIComponent(m_UIComponentsList[i]);
                        if (pCurrentUIComponent)
                        {
                            pCurrentUIComponent->setIsSelected(btrue);
                            break;
                        }
                    }
                }
                else
                    m_UIComponentsList.eraseKeepOrder(index);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIComponent* UIMenu::getUIComponentSelected ( ) const
    {
        for (u32 i = 0; i < m_UIComponentsList.size(); i++)
        {
            UIComponent* pUIComponent = UIMenuManager::getUIComponent(m_UIComponentsList[i]);
            if (pUIComponent && pUIComponent->getIsSelected())
                return pUIComponent;
        }
        return NULL;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIMenu::reinitMenuSelection ( )
    {
        if ( m_AlwaysStaySelected ) return ; 

        bbool wasSomethingSelected=bfalse;
        for (u32 i = 0; i < m_UIComponentsList.size(); i++)
        {
            UIComponent * pUIComponent = UIMenuManager::getUIComponent(m_UIComponentsList[i]);
            ITF_ASSERT(pUIComponent!=NULL);
            if (pUIComponent)
            {
                if (pUIComponent->getIsDefaultSelected())
                {
                    ITF_ASSERT_MSG(!wasSomethingSelected, "several menu items are flagged as selected by default");
                    wasSomethingSelected=btrue;
                    pUIComponent->setIsSelected(btrue);
                    pUIComponent->onRollover();
                    Actor* actor = pUIComponent->GetActor();
                    if (actor)
                    {
                        SafeArray<UIComponent*> siblings = actor->GetComponents<UIComponent>();
                        for (u32 j = 0; j < siblings.size(); ++j)
                        {
                            if (siblings[j] != pUIComponent)
                            {
                                siblings[j]->setIsSelected(btrue);
                                siblings[j]->onRollover();
                            }
                        }
                    }
                }
                else 
                    pUIComponent->setIsSelected(bfalse);
            }
        }
        if (!wasSomethingSelected && m_UIComponentsList.size()>0)
        {
            UIComponent * pUIComponent = UIMenuManager::getUIComponent(m_UIComponentsList[0]);
            ITF_ASSERT(pUIComponent!=NULL);
            if (pUIComponent)
                pUIComponent->setIsSelected(btrue);

        }

    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    UIComponent* UIMenu::getUIComponentByID(const StringID& _id)
    {
        u32 size = m_UIComponentsList.size();
        for (u32 i=0; i<size; i++)
        {
            ActorRef actorRef = m_UIComponentsList[i];
            if (Actor* actor = actorRef.getActor())
            {
                if (UIComponent* uiComponent = actor->GetComponent<UIComponent>())
                {
                    if (uiComponent->getID() == _id)
                    {
                        return uiComponent;
                    }
                }
            }
        }
        return NULL;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    UIComponent* UIMenu::getUIComponentByFriendly(const String8& _friendly)
    {
        // retrieve given component
        u32 size = m_UIComponentsList.size();
        for (u32 i=0; i<size; i++)
        {
            ActorRef actorRef = m_UIComponentsList[i];
            if (Actor* actor = actorRef.getActor())
            {
                if (UIComponent* uiComponent = actor->GetComponent<UIComponent>())
                {
                    if (uiComponent->getFriendly().equals(_friendly, false))
                    {
                        return uiComponent;
                    }
                }
            }
        }

        // warn if not found
        ITF_WARNING(
            NULL, bfalse,
            "Can't find UIComponent with friendly '%s'",
            _friendly.cStr()
            );
        return NULL;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    UITextBox* UIMenu::getUITextBoxByFriendly(const String8& _friendly)
    {
        // retrieve given component
        u32 size = m_UIComponentsList.size();
        for (u32 i=0; i<size; i++)
        {
            ActorRef actorRef = m_UIComponentsList[i];
            if (Actor* actor = actorRef.getActor())
            {
                if (UIComponent* uiComponent = actor->GetComponent<UIComponent>())
                {
                    if (uiComponent->getFriendly().equals(_friendly, false))
                    {
                        return actor->GetComponent<UITextBox>();
                    }
                }
            }
        }

        // warn if not found
        ITF_WARNING(
            NULL, bfalse,
            "Can't find UIComponent with friendly '%s'",
            _friendly.cStr()
            );
        return NULL;
    }

    bbool UIMenu::isPadAllowed( u32 _padIndex )
    {
        if(m_padTypeAllowed == U32_INVALID)
            return btrue;

        return ( m_padTypeAllowed == (u32)INPUT_ADAPTER->getPadType(_padIndex));
    }

    void UIMenu::setPadTypeAllowed( u32 _padType )
    {
        m_padTypeAllowed = _padType;
    }
}
