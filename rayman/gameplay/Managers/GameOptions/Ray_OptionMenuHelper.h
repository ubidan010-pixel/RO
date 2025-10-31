#ifndef _ITF_RAY_OPTIONMENUHELPER_H_
#define _ITF_RAY_OPTIONMENUHELPER_H_

#include <initializer_list>
#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif //_ITF_UIMENUMANAGER_H_

#ifndef _ITF_RAY_GAMEOPTIONNAMES_H_
#include "rayman/gameplay/Managers/GameOptions/Ray_GameOptionNames.h"
#endif

namespace ITF
{
    class UIComponent;
    class UIMenu;
    class Ray_OptionMenuHelper : public UIMenuManager::MenuItemActionListener
    {
    public:
        enum EOptionTab
        {
            OptionTab_General = 0,
            OptionTab_Controls = 1
        };

        Ray_OptionMenuHelper();
        virtual ~Ray_OptionMenuHelper();
        void activateForOptionMenu(MenuItemActionListener* mainListener);
        bbool isActive() const { return m_isActive; }
        void onMenuItemAction(UIComponent* _UIComponent) override;
        StringID onMenuPageAction(UIMenu* _menu, const StringID& _action, const StringID& _defaultAction) override;
        bbool onMenuItemOtherAction(UIComponent* _UIComponent, const StringID& _action) override;
        void UpdateMenuOnSelectionChange(UIComponent* uiComponent, bbool isSelected) override;

    private:
        bbool handleTabSwitch(const StringID& id);
        static bbool handleStartWithHeart(const StringID& id);
        static bbool handleRunButton(const StringID& id);
        static bbool handleResetToDefault(const StringID& id);
        bbool handleAccept(const StringID& id);
        bbool handleCancel(const StringID& id);

        void switchToTab(EOptionTab tab);
        void updateTabVisuals() const;
        void swapUIComponentActivation(const StringID& idToDisable,
                                       const StringID& idToEnable,
                                       bbool changeMenuSelection) const;
        void setGeneralItemsVisible(bbool visible) const;
        void setUIVisibilitySelectable(const StringID& id, bbool visible, bbool selectable, bbool changeMenuSelection = bfalse) const;
        void setUIVisibility(const StringID& id, bbool visible, bbool changeMenuSelection = bfalse) const;
        static void setTextByLineId(const StringID& itemId, const char* friendly, u32 lineId);
        void setVisibilityFor(std::initializer_list<StringID> ids, bbool visible, bbool selectable) const;
        void ensureValidSelection() const;
        void UpdateArrowVisibilityByCount() const;
        static void UpdateResolutionText();
        static void UpdateStartWithHeartText();
        static void UpdateVibrationText();
        static void UpdateRunButtonText();
        static void UpdateLanguageText();
        void UpdateWindowCheckboxVisual() const;
        void ToggleWindowCheckbox() const;
        void closeAndReturn();
        MenuItemActionListener* m_mainListener;
        UIMenu*    m_menu;
        const char* m_menuBaseName = OPTION_MENU_NAME;
        EOptionTab m_currentTab;
        bbool      m_isActive;

        bbool m_hasSnapshot;
        i32   m_snapshotResolutionIndex;
        i32   m_snapshotStartWithHeartIndex;
        i32   m_snapshotVibrationMode;
        i32   m_snapshotRunButtonMode;
        i32   m_snapshotLanguageIndex;
        bbool m_snapshotWindowed;
    };
}

#endif //_ITF_RAY_OPTIONMENUHELPER_H_
