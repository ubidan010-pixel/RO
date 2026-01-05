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
    class Actor;
    class UIComponent;
    class UIListOptionComponent;

    class Ray_ControlsRemappingMenuHelper : public Ray_BaseMenuHelper
    {
    public:
        Ray_ControlsRemappingMenuHelper();
        virtual ~Ray_ControlsRemappingMenuHelper();

        void activateForControlsRemappingMenu(MenuItemActionListener* mainListener);
        void onMenuItemAction(UIComponent* _UIComponent) override;
        StringID onMenuPageAction(UIMenu* _menu, const StringID& _action, const StringID& _defaultAction) override;
        void UpdateMenuOnSelectionChange(UIComponent* uiComponent, bbool isSelected) override;
        ObjectRef getNavigationOverrideTarget(UIComponent* current, f32 joyX, f32 joyY) override;
        ObjectRef getFocusOverrideTargetForInputPlayer(UIComponent* current, u32 inputPlayer) const;
        UIComponent* getSelectedComponentForPlayer(u32 inputPlayer) const;
        bbool isNavigationLockedForPlayer(u32 inputPlayer) const;

        bbool isNavigationLocked() const
        {
            for (u32 i = 0; i < 4; ++i)
            {
                if (isNavigationLockedForPlayer(i))
                    return btrue;
            }
#if defined(ITF_WINDOWS)
            if (m_isEditingControllerType)
                return btrue;
#endif
            return bfalse;
        }

        static Ray_ControlsRemappingMenuHelper* getActiveHelper();
        void updateRemappingMode(f32 deltaTime);

        static bbool handleExternalEditingInput(UIComponent* component, const StringID& action)
        {
#if defined(ITF_WINDOWS)
            if (!s_activeHelper)
                return bfalse;

            return s_activeHelper->processEditingInput(component, action);
#else
            (void)component; (void)action;
            return bfalse;
#endif
        }

#if defined(ITF_WINDOWS)
        bbool processEditingInput(UIComponent* component, const StringID& action);
#endif

    private:
        void showContextIcons() override;

        enum ExitDecision
        {
            ExitDecision_None = 0,
            ExitDecision_Save,
        };

        bbool isPlayerParticipatingForExit(u32 playerIndex) const;
        void resetExitDecisionsAndStatusUI();
        void setExitDecisionForPlayer(u32 playerIndex, ExitDecision decision);
        bbool areAllParticipatingPlayersReadyToExit() const;
        void applyExitDecisionsAndClose();

        void captureRemappingSnapshot();
        void restoreRemappingSnapshot();
        void restoreRemappingSnapshotForPlayer(u32 playerIndex);
        bbool handleIconAction(const StringID& id, UIComponent* component);
        void startRemappingMode(u32 playerIndex, ZInputManager::EGameAction action, UIComponent* component);
        bbool parseIconId(const StringID& id, u32& outPlayerIndex, ZInputManager::EGameAction& outAction);
        void clearIconDisplay(u32 playerIndex, UIComponent* component);
        void restoreIconDisplay(u32 playerIndex, UIComponent* component);
        void cancelRemappingMode(u32 playerIndex, bbool restoreDisplay);
        bbool detectPhysicalControl(u32 playerIndex, u32& outPhysicalControl, EInputSourceType& outSource);
        void finalizeRemapping(u32 playerIndex, u32 physicalControl);
        void onClose() override;
        EInputSourceType getActiveSourceForReset(u32 playerIndex) const;
        bbool tryGetSelectedPlayerIndex(u32& outPlayerIndex);
        bbool tryGetIconInfoFromComponent(UIComponent* component, u32& outPlayerIndex,
                                          ZInputManager::EGameAction& outAction) const;
        UIComponent* findIconComponent(u32 playerIndex, ZInputManager::EGameAction action) const;
        static u32 getActionIndex(ZInputManager::EGameAction action);
        static ZInputManager::EGameAction getActionByIndex(u32 index);

#if defined(ITF_WINDOWS)
        bbool handleControllerTypeAction(const StringID& id, UIComponent* component);
        void enterControllerTypeEditMode(UIListOptionComponent* component);
        void exitControllerTypeEditMode();
        void adjustControllerType(UIListOptionComponent* listComponent, i32 direction);
        void updateControllerTypeDisplay(UIListOptionComponent* listComponent, i32 index);
        UIListOptionComponent* findControllerTypeComponent() const;
        bbool isControllerTypeEditing() const { return m_isEditingControllerType; }
        void updateControllerTypeEditing(f32 deltaTime);
#endif

        static Ray_ControlsRemappingMenuHelper* s_activeHelper;
        bbool m_isRemappingModeByPlayer[4];
        bbool m_isWaitingForReleaseByPlayer[4];
        ZInputManager::EGameAction m_remappingActionByPlayer[4];
        UIComponent* m_remappingComponentByPlayer[4];
        f32 m_remappingCooldownByPlayer[4];
        f32 m_postRemapCooldownByPlayer[4];
        EInputSourceType m_remappingSourceByPlayer[4];
        ObjectRef m_selectedComponentRefByPlayer[4];
        bbool m_hasSelectedComponentByPlayer[4];

        bbool m_hasCommittedChanges;

        ExitDecision m_exitDecisionByPlayer[4];
        Actor* m_readyStatusActorByPlayer[4];

        bbool m_hasSnapshot;
        bbool m_hasSnapshotGamepad[4];
        bbool m_hasSnapshotKeyboard[4];
        ITF_VECTOR<u32> m_snapshotGamepad[4];
        ITF_VECTOR<u32> m_snapshotKeyboard[4];
        LocalisationId m_previousIconLineIdByPlayer[4];

#if defined(ITF_WINDOWS)
        bbool m_hasSnapshotPCControlMode;
        i32 m_snapshotPCControlMode;
        bbool m_hasSnapshotPCControlModeEdit;
        i32 m_snapshotPCControlModeEdit;
        bbool m_isEditingControllerType;
        UIListOptionComponent* m_editingControllerTypeComponent;
        f32 m_controllerTypeChangeCooldown;
        std::vector<std::pair<UIComponent*, bbool>> m_previousSelectionStates;
#endif
        bbool m_hasLastActionByPlayer[4];
        ZInputManager::EGameAction m_lastActionByPlayer[4];
        u32 m_lastSelectedPlayerIndex;
    };
}

#endif //_ITF_RAY_CONTROLSREMAPPINGMENUHELPER_H_
