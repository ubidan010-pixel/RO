#ifndef _ITF_RAY_CONTROLSREMAPPINGMENUHELPER_H_
#define _ITF_RAY_CONTROLSREMAPPINGMENUHELPER_H_

#ifndef _ITF_RAY_BASEMENUHELPER_H_
#include "rayman/gameplay/Managers/GameOptions/Ray_BaseMenuHelper.h"
#endif //_ITF_RAY_BASEMENUHELPER_H_

#ifndef _ITF_ZINPUT_MANAGER_H
#include "engine/zinput/ZInputManager.h"
#endif //_ITF_ZINPUT_MANAGER_H

namespace ITF
{
    class UIComponent;

    class Ray_ControlsRemappingMenuHelper : public Ray_BaseMenuHelper
    {
    public:
        Ray_ControlsRemappingMenuHelper();
        virtual ~Ray_ControlsRemappingMenuHelper();
        void activateForControlsRemappingMenu(MenuItemActionListener* mainListener);
        void onMenuItemAction(UIComponent* _UIComponent) override;

    private:
        // Button handlers
        bbool handleAccept(const StringID& id);
        bbool handleCancel(const StringID& id);
        bbool handleIconAction(const StringID& id, UIComponent* component);
        void startRemappingMode(u32 playerIndex, ZInputManager::EGameAction action, UIComponent* component);
        bbool parseIconId(const StringID& id, u32& outPlayerIndex, ZInputManager::EGameAction& outAction);
        void clearIconDisplay(UIComponent* component);
        
        bbool m_isRemappingMode;
        u32 m_remappingPlayerIndex;
        ZInputManager::EGameAction m_remappingAction;
        UIComponent* m_remappingComponent; 
    };
}

#endif //_ITF_RAY_CONTROLSREMAPPINGMENUHELPER_H_
