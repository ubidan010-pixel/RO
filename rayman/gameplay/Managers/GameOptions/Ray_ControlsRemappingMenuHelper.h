#ifndef _ITF_RAY_CONTROLSREMAPPINGMENUHELPER_H_
#define _ITF_RAY_CONTROLSREMAPPINGMENUHELPER_H_

#ifndef _ITF_RAY_BASEMENUHELPER_H_
#include "rayman/gameplay/Managers/GameOptions/Ray_BaseMenuHelper.h"
#endif //_ITF_RAY_BASEMENUHELPER_H_

#ifndef _ITF_ZINPUT_MANAGER_H
#include "engine/zinput/ZInputManager.h"
#endif //_ITF_ZINPUT_MANAGER_H

#ifndef _ITF_INPUTDEVICE_H_
#include "engine/zinput/ZInputDevice.h"
#endif //_ITF_INPUTDEVICE_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#if defined(ITF_WINDOWS)
#include <vector>
#include <utility>
#endif

namespace ITF
{
    class UIComponent;
    class UIListOptionComponent;

    class Ray_ControlsRemappingMenuHelper : public Ray_BaseMenuHelper
    {
    public:
        Ray_ControlsRemappingMenuHelper();
        virtual ~Ray_ControlsRemappingMenuHelper();

        void activateForControlsRemappingMenu(MenuItemActionListener* mainListener);
        void onMenuItemAction(UIComponent* _UIComponent) override;
        ObjectRef getNavigationOverrideTarget(UIComponent* current, f32 joyX, f32 joyY) override;
        
        static Ray_ControlsRemappingMenuHelper* getActiveHelper();
        void updateRemappingMode(f32 deltaTime);

        static bbool handleExternalEditingInput(UIComponent* component, const StringID& action);

#if defined(ITF_WINDOWS)
        bbool processEditingInput(UIComponent* component, const StringID& action);
#endif

    private:
        bbool handleAccept(const StringID& id);
        bbool handleCancel(const StringID& id);
        bbool handleIconAction(const StringID& id, UIComponent* component);
        bbool handleResetToDefault(const StringID& id);
        bbool parseResetButtonId(const StringID& id, u32& outPlayerIndex);
        void startRemappingMode(u32 playerIndex, ZInputManager::EGameAction action, UIComponent* component);
        bbool parseIconId(const StringID& id, u32& outPlayerIndex, ZInputManager::EGameAction& outAction);
        void clearIconDisplay(UIComponent* component);
        void restoreIconDisplay(UIComponent* component);
        void cancelRemappingMode(bbool restoreDisplay);
        bbool detectPhysicalControl(u32& outPhysicalControl, EInputSourceType& outSource);
        void finalizeRemapping(u32 physicalControl);
        void onClose() override;
        EInputSourceType getActiveSourceForReset(u32 playerIndex) const;

#if defined(ITF_WINDOWS)
        bbool handleControllerTypeAction(const StringID& id, UIComponent* component);
        void enterControllerTypeEditMode(UIListOptionComponent* component);
        void exitControllerTypeEditMode();
        void adjustControllerType(UIListOptionComponent* listComponent, i32 direction);
        void updateControllerTypeDisplay(UIListOptionComponent* listComponent, i32 index);
        UIListOptionComponent* findControllerTypeComponent() const;
        bbool isControllerTypeEditing() const { return m_isEditingControllerType; }
#endif

        static Ray_ControlsRemappingMenuHelper* s_activeHelper;

        bbool m_isRemappingMode;
        bbool m_isWaitingForRelease;
        u32 m_remappingPlayerIndex;
        ZInputManager::EGameAction m_remappingAction;
        UIComponent* m_remappingComponent;
        f32 m_remappingCooldown;
        f32 m_postRemapCooldown;
        EInputSourceType m_remappingSource;

#if defined(ITF_WINDOWS)
        bbool m_isEditingControllerType;
        UIListOptionComponent* m_editingControllerTypeComponent;
        f32 m_controllerTypeInputTimer;
        f32 m_controllerTypeFirstPressTimer;
        bbool m_controllerTypeFirstPressed;
        std::vector<std::pair<UIComponent*, bbool>> m_previousSelectionStates;
#endif
    };
}

#endif //_ITF_RAY_CONTROLSREMAPPINGMENUHELPER_H_
