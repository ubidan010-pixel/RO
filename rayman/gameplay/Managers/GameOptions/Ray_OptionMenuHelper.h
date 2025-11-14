#ifndef _ITF_RAY_OPTIONMENUHELPER_H_
#define _ITF_RAY_OPTIONMENUHELPER_H_

#include <initializer_list>
#include <vector>
#include <utility>
#ifndef _ITF_RAY_BASEMENUHELPER_H_
#include "rayman/gameplay/Managers/GameOptions/Ray_BaseMenuHelper.h"
#endif //_ITF_RAY_BASEMENUHELPER_H_

#ifndef _ITF_RAY_GAMEOPTIONNAMES_H_
#include "rayman/gameplay/Managers/GameOptions/Ray_GameOptionNames.h"
#endif

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif

namespace ITF
{
    class UIComponent;
    class UIMenu;
    class UIListOptionComponent;
    class UIFloatOptionComponent;
    class UIToggleOptionComponent;

    namespace Ray_OptionMenuHelperConstants
    {
        static const f32 INPUT_REPEAT_TIMEOUT = 0.2f;
        static const f32 INPUT_INITIAL_DELAY = 0.3f;
        static const f32 SLIDER_DEFAULT_STEP = 0.05f;
        static const f32 SLIDER_SPEED_MULTIPLIER = 3.0f;
        static const f32 FLOAT_EPSILON = 1e-4f;
    }

    class Ray_OptionMenuHelper : public Ray_BaseMenuHelper
    {
    public:
        enum EMenuState
        {
            MenuState_Navigate,
            MenuState_EditOption,
        };

        Ray_OptionMenuHelper();
        virtual ~Ray_OptionMenuHelper();
        void activateForOptionMenu(MenuItemActionListener* mainListener);
        void onMenuItemAction(UIComponent* _UIComponent) override;
        bbool onMenuItemOtherAction(UIComponent* _UIComponent, const StringID& _action) override;
        void UpdateMenuOnSelectionChange(UIComponent* uiComponent, bbool isSelected) override;
        void updateTimer();
        ObjectRef getNavigationOverrideTarget(UIComponent* current, f32 joyX, f32 joyY);

    protected:
        void onActivate() override;
        void onClose() override;

    private:
        bbool handleResetToDefault(const StringID& id);
        bbool handleAccept(const StringID& id);
        bbool handleCancel(const StringID& id);

        void initializeMenuState();
        void enterEditMode(UIComponent* component, const StringID& optionId);
        void exitEditMode();
        bbool isNavigating() const { return m_menuState == MenuState_Navigate; }
        bbool isEditing() const { return m_menuState == MenuState_EditOption; }
        bbool isOptionEditable(UIComponent* component) const;
        StringID getOptionIdForComponent(UIComponent* component) const;

        void hideAllArrows();
        void showArrowsForOption(const StringID& optionId);
        void hideArrowsForOption(const StringID& optionId);
        StringID getOptionIdFromComponent(const StringID& componentId) const;

        void setUIVisibilitySelectable(const StringID& id, bbool visible, bbool selectable) const;
        void setVisibilityFor(std::initializer_list<StringID> ids, bbool visible, bbool selectable) const;
        void ensureValidSelection() const;

        void loadOptionsFromSaveFile();
        StringID getOptionIdFromFriendlyName(const String8& friendlyName) const;
        void toggleOption(UIToggleOptionComponent* toggleComponent, const StringID& optionId);
        void adjustListOption(UIListOptionComponent* listComponent, const StringID& optionId, i32 direction);
        void adjustFloatOption(UIFloatOptionComponent* floatComponent, const StringID& optionId, i32 direction);
        void updateListOptionDisplay(UIListOptionComponent* listComponent, const StringID& optionId, i32 index) const;
        void applyOptionChange(const StringID& optionId) const;
        bbool processEditingInput(UIComponent* component, const StringID& action);
        void captureSnapshot();
        void restoreSnapshot();
        void refreshAllOptionVisuals();
        void UpdateResolutionText();
        void UpdateLanguageText();
        void UpdateStartWithHeartText();
        void UpdateRunButtonText();
        void UpdateVibrationText();
        void UpdateMurfyAssistToggle();
        void UpdateWindowCheckboxVisual();
        void UpdateMasterVolumeSlider();
        void UpdateMusicVolumeSlider();
        void UpdateSFXVolumeSlider();
        void UpdateIntensitySlider();
        UIComponent* findOptionComponent(const StringID& optionId) const;
        UIListOptionComponent* findListOptionComponent(const StringID& optionId) const;
        UIToggleOptionComponent* findToggleOptionComponent(const StringID& optionId) const;
        UIFloatOptionComponent* findFloatOptionComponent(const StringID& optionId) const;
        UIComponent* findComponentByFriendlyName(const char* friendlyName) const;
        enum ENavigationDirection
        {
            Navigation_Up,
            Navigation_Down,
            Navigation_Left,
            Navigation_Right
        };
        UIComponent* getNavigationTarget(UIComponent* current, ENavigationDirection direction) const;

        EMenuState m_menuState;
        StringID   m_currentEditingOption;
        UIComponent* m_currentEditingComponent;
        std::vector<std::pair<UIComponent*, bbool>> m_previousSelectionStates;

        f32 m_timer;
        f32 m_timeout;
        f32 m_timeoutJustPressed;
        bbool m_firstPressed;
        bbool m_hasSnapshot;
        i32 m_snapshotResolutionIndex;
        bbool m_snapshotWindowed;
        i32 m_snapshotLanguageIndex;
        i32 m_snapshotStartWithHeartIndex;
        i32 m_snapshotRunButtonMode;
        bbool m_snapshotMurfyAssist;
        i32 m_snapshotVibrationMode;
        f32 m_snapshotMasterVolume;
        f32 m_snapshotMusicVolume;
        f32 m_snapshotSFXVolume;
        f32 m_snapshotIntensity;

        static Ray_OptionMenuHelper* s_activeHelper;

    public:
        static Ray_OptionMenuHelper* getActiveHelper() { return s_activeHelper; }
        static bbool handleExternalEditingInput(UIComponent* component, const StringID& action)
        {
            return s_activeHelper ? s_activeHelper->processEditingInput(component, action) : bfalse;
        }
        static void requestExitEditMode(UIComponent* component);
    };
}

#endif //_ITF_RAY_OPTIONMENUHELPER_H_
