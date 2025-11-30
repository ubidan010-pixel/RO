#ifndef _ITF_RAY_CONTROLSREMAPPINGMENUHELPER_H_
#define _ITF_RAY_CONTROLSREMAPPINGMENUHELPER_H_

#ifndef _ITF_RAY_BASEMENUHELPER_H_
#include "rayman/gameplay/Managers/GameOptions/Ray_BaseMenuHelper.h"
#endif //_ITF_RAY_BASEMENUHELPER_H_

#ifndef _ITF_ZINPUT_MANAGER_H
#include "engine/zinput/ZInputManager.h"
#endif //_ITF_ZINPUT_MANAGER_H

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

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
        static Ray_ControlsRemappingMenuHelper* getActiveHelper();
        void updateRemappingMode(f32 deltaTime);

    private:
        // Button handlers
        bbool handleAccept(const StringID& id);
        bbool handleCancel(const StringID& id);
        bbool handleIconAction(const StringID& id, UIComponent* component);
        void startRemappingMode(u32 playerIndex, ZInputManager::EGameAction action, UIComponent* component);
        bbool parseIconId(const StringID& id, u32& outPlayerIndex, ZInputManager::EGameAction& outAction);
        void clearIconDisplay(UIComponent* component);
        void restoreIconDisplay(UIComponent* component);
        void cancelRemappingMode(bbool restoreDisplay);
        void snapshotInputState();
        bbool detectPhysicalControl(u32& outPhysicalControl);
        void finalizeRemapping(u32 physicalControl);
        void onClose() override;

        static Ray_ControlsRemappingMenuHelper* s_activeHelper;
        bbool m_isRemappingMode;
        bbool m_isWaitingForRelease;
        u32 m_remappingPlayerIndex;
        ZInputManager::EGameAction m_remappingAction;
        UIComponent* m_remappingComponent;
        f32 m_remappingCooldown;
        f32 m_postRemapCooldown;
        InputAdapter::PressStatus m_lastButtonStates[JOY_MAX_BUT];
        f32 m_lastAxisStates[JOY_MAX_AXES];
    };
}

#endif //_ITF_RAY_CONTROLSREMAPPINGMENUHELPER_H_
