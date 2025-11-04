#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_OPTIONMENUHELPER_H_
#include "rayman/gameplay/Managers/GameOptions/Ray_OptionMenuHelper.h"
#endif

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif

#ifndef _ITF_UIMENU_H_
#include "gameplay/components/UI/UIMenu.h"
#endif

#ifndef _ITF_TEXTUREGRAPHICCOMPONENT2D_H_
#include "engine/actors/components/texturegraphiccomponent2D.h"
#endif

#ifndef _ITF_RAY_GAMEOPTIONNAMES_H_
#include "rayman/gameplay/Managers/GameOptions/Ray_GameOptionNames.h"
#endif

#ifndef _ITF_RAY_GAMEOPTIONMANAGER_H_
#include "rayman/gameplay/Managers/GameOptions/Ray_GameOptionManager.h"
#endif

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif

namespace ITF
{
    struct HoverPair
    {
        StringID baseId;
        StringID hoverId;
    };

    static const HoverPair kHoverPairs[] = {
        {OPTIONMENU_LEFT_ARROW_LANGUAGE, OPTIONMENU_LEFT_ARROW_LANGUAGE_HOVER},
        {OPTIONMENU_LEFT_ARROW_RESOLUTION, OPTIONMENU_LEFT_ARROW_RESOLUTION_HOVER},
        {OPTIONMENU_LEFT_ARROW_START_WITH_HEART, OPTIONMENU_LEFT_ARROW_START_WITH_HEART_HOVER},
        {OPTIONMENU_LEFT_ARROW_VIBRATION, OPTIONMENU_LEFT_ARROW_VIBRATION_HOVER},
        {OPTIONMENU_LEFT_ARROW_RUN_BUTTON, OPTIONMENU_LEFT_ARROW_RUN_BUTTON_HOVER},
        {OPTIONMENU_RIGHT_ARROW_RESOLUTION, OPTIONMENU_RIGHT_ARROW_RESOLUTION_HOVER},
        {OPTIONMENU_RIGHT_ARROW_LANGUAGE, OPTIONMENU_RIGHT_ARROW_LANGUAGE_HOVER},
        {OPTIONMENU_RIGHT_ARROW_START_WITH_HEART, OPTIONMENU_RIGHT_ARROW_START_WITH_HEART_HOVER},
        {OPTIONMENU_RIGHT_ARROW_VIBRATION, OPTIONMENU_RIGHT_ARROW_VIBRATION_HOVER},
        {OPTIONMENU_RIGHT_ARROW_RUN_BUTTON, OPTIONMENU_RIGHT_ARROW_RUN_BUTTON_HOVER},
    };

    static i32 CycleIndex(i32 current, i32 count, i32 delta)
    {
        if (count <= 0) return 0;
        i32 next = (current + delta) % count;
        if (next < 0) next += count;
        return next;
    }

    static i32 GetListCountForOption(const StringID& optionId)
    {
        if (!RAY_GAMEMANAGER) return 0;
        const Ray_GameOption* opt = RAY_GAMEMANAGER->getGameOptionManager().getOption(optionId);
        if (!opt) return 0;
        switch (opt->getType())
        {
        case Ray_GameOption::OptionType_IntList:
            return static_cast<const Ray_GameOptionIntList*>(opt)->getOptions().size();
        case Ray_GameOption::OptionType_FloatList:
            return static_cast<const Ray_GameOptionFloatList*>(opt)->getOptions().size();
        case Ray_GameOption::OptionType_StringList:
            return static_cast<const Ray_GameOptionStringList*>(opt)->getOptions().size();
        default:
            return 0;
        }
    }

    Ray_OptionMenuHelper::Ray_OptionMenuHelper()
        : m_mainListener(nullptr)
          , m_menu(nullptr)
          , m_isActive(bfalse)
          , m_menuState(MenuState_Navigate)
          , m_currentEditingOption(StringID::Invalid)
          , m_currentFocusedOption(StringID::Invalid)
          , m_hasSnapshot(bfalse)
          , m_snapshotResolutionIndex(0)
          , m_snapshotStartWithHeartIndex(0)
          , m_snapshotVibrationMode(0)
          , m_snapshotRunButtonMode(0)
          , m_snapshotLanguageIndex(0)
          , m_snapshotWindowed(bfalse)
    {
    }

    Ray_OptionMenuHelper::~Ray_OptionMenuHelper()
    = default;

    void Ray_OptionMenuHelper::activateForOptionMenu(MenuItemActionListener* mainListener)
    {
        if (!UI_MENUMANAGER)
            return;

        m_isActive = btrue;
        m_mainListener = mainListener;
        UI_MENUMANAGER->setMenuListener(OPTION_MENU_NAME, this);
        m_menu = UI_MENUMANAGER->getMenu(OPTION_MENU_NAME);
        if (!m_menu)
            return;

        // for (const auto& p : kHoverPairs)
        // {
        //     if (UIComponent* c = m_menu->getUIComponentByID(p.hoverId))
        //     {
        //         if (Actor* a = c->GetActor()) a->disableDraw(btrue);
        //         c->setActive(bfalse);
        //     }
        // }
        initializeMenuState();

        // UpdateResolutionText();
        // UpdateLanguageText();
        // UpdateStartWithHeartText();
        // UpdateVibrationText();
        // UpdateRunButtonText();
        // UpdateWindowCheckboxVisual();

        // if (RAY_GAMEMANAGER)
        // {
        //     m_snapshotResolutionIndex = RAY_GAMEMANAGER->getResolutionIndex();
        //     m_snapshotLanguageIndex = RAY_GAMEMANAGER->getLanguageIndex();
        //     m_snapshotStartWithHeartIndex = RAY_GAMEMANAGER->getStartWithHeartIndex();
        //     m_snapshotVibrationMode = RAY_GAMEMANAGER->getVibrationMode();
        //     m_snapshotRunButtonMode = RAY_GAMEMANAGER->getRunButtonMode();
        //     m_snapshotWindowed = RAY_GAMEMANAGER->isWindowed();
        //     m_hasSnapshot = btrue;
        // }
    }

    void Ray_OptionMenuHelper::onMenuItemAction(UIComponent* component)
    {
        return;
        if (!component) return;

        const StringID id = component->getID();

        // Handle action buttons first
        if (handleResetToDefault(id)) return;
        if (handleAccept(id)) return;
        if (handleCancel(id)) return;

        // Handle checkbox
        if (id == OPTIONMENU_CHECKBOX_WINDOW_OFF || id == OPTIONMENU_CHECKBOX_WINDOW_ON)
        {
            if (isNavigating())
            {
                // In navigate mode, press toggles checkbox
                ToggleWindowCheckbox();
            }
            return;
        }

        // If in edit mode, handle exit edit or arrow buttons
        if (isEditing())
        {
            // In edit mode, press on the option label/text again exits edit mode
            const StringID optionId = getOptionIdFromComponent(id);
            if (optionId != StringID::Invalid && optionId == m_currentEditingOption)
            {
                // Pressing the same option again exits edit mode
                exitEditMode();
                return;
            }
            // Arrow buttons will be handled below in the cycler loop
        }
        else if (isNavigating())
        {
            // In navigate mode, press on an option label/text enters edit mode
            const StringID optionId = getOptionIdFromComponent(id);
            if (optionId != StringID::Invalid)
            {
                if (isOptionEditable(optionId))
                {
                    enterEditMode(optionId);
                    return;
                }
                // For checkbox (window), toggle is handled above
            }
        }

        auto* gm = RAY_GAMEMANAGER;
        if (!gm) return;

        struct Cycler
        {
            StringID leftBase, leftHover, rightBase, rightHover, labelId;
            i32 (*count)();
            i32 (*get)();
            void (*set)(i32);
            const char* (*display)(i32);
            u32 (*lineId)(i32);
        };

        auto getStartIdx = []() -> i32 { return RAY_GAMEMANAGER->getStartWithHeartIndex(); };
        auto setStartIdx = [](i32 v) { RAY_GAMEMANAGER->setStartWithHeartIndex(v); };
        auto dispStart = [](i32 v) -> const char* { return RAY_GAMEMANAGER->getStartWithHeartDisplayName(v); };
        auto lineStart = [](i32 v) -> u32 { return RAY_GAMEMANAGER->getStartWithHeartLineId(v); };

        auto getRunMode = []() -> i32 { return RAY_GAMEMANAGER->getRunButtonMode(); };
        auto setRunMode = [](i32 v) { RAY_GAMEMANAGER->setRunButtonMode(v); };
        auto dispRun = [](i32 v) -> const char* { return RAY_GAMEMANAGER->getRunButtonDisplayName(v); };
        auto lineRun = [](i32 v) -> u32 { return RAY_GAMEMANAGER->getRunButtonLineId(v); };

        auto getLangIdx = []() -> i32 { return RAY_GAMEMANAGER->getLanguageIndex(); };
        auto setLangIdx = [](i32 v) { RAY_GAMEMANAGER->setLanguageIndex(v); };
        auto dispLang = [](i32 v) -> const char* { return RAY_GAMEMANAGER->getLanguageDisplayName(v); };
        auto lineLang = [](i32 v) -> u32 { return 0; };

        auto getResIdx = []() -> i32 { return RAY_GAMEMANAGER->getResolutionIndex(); };
        auto setResIdx = [](i32 v) { RAY_GAMEMANAGER->setResolutionIndex(v); };
        auto dispRes = [](i32 v) -> const char* { return RAY_GAMEMANAGER->getResolutionDisplayName(v); };
        auto lineRes = [](i32 v) -> u32 { return 0; };

        auto getVibMode = []() -> i32 { return RAY_GAMEMANAGER->getVibrationMode(); };
        auto setVibMode = [](i32 v) { RAY_GAMEMANAGER->setVibrationMode(v); };
        auto dispVib = [](i32 v) -> const char* { return RAY_GAMEMANAGER->getVibrationDisplayName(v); };
        auto lineVib = [](i32 v) -> u32 { return RAY_GAMEMANAGER->getVibrationLineId(v); };

        const Cycler cyclers[] = {
            {
                OPTIONMENU_LEFT_ARROW_RESOLUTION, OPTIONMENU_LEFT_ARROW_RESOLUTION_HOVER,
                OPTIONMENU_RIGHT_ARROW_RESOLUTION, OPTIONMENU_RIGHT_ARROW_RESOLUTION_HOVER,
                RESOLUTION_TEXT,
                []() -> i32 { return GetListCountForOption(OPTION_RESOLUTION); },
                getResIdx, setResIdx, dispRes, lineRes
            },
            {
                OPTIONMENU_LEFT_ARROW_LANGUAGE, OPTIONMENU_LEFT_ARROW_LANGUAGE_HOVER,
                OPTIONMENU_RIGHT_ARROW_LANGUAGE, OPTIONMENU_RIGHT_ARROW_LANGUAGE_HOVER,
                LANGUAGE_TEXT,
                []() -> i32 { return GetListCountForOption(OPTION_LANGUAGE); },
                getLangIdx, setLangIdx, dispLang, lineLang
            },
            {
                OPTIONMENU_LEFT_ARROW_START_WITH_HEART, OPTIONMENU_LEFT_ARROW_START_WITH_HEART_HOVER,
                OPTIONMENU_RIGHT_ARROW_START_WITH_HEART, OPTIONMENU_RIGHT_ARROW_START_WITH_HEART_HOVER,
                START_WITH_HEART,
                []() -> i32 { return START_WITH_HEART_CHOICES; },
                getStartIdx, setStartIdx, dispStart, lineStart
            },
            {
                OPTIONMENU_LEFT_ARROW_VIBRATION, OPTIONMENU_LEFT_ARROW_VIBRATION_HOVER,
                OPTIONMENU_RIGHT_ARROW_VIBRATION, OPTIONMENU_RIGHT_ARROW_VIBRATION_HOVER,
                VIBRATION_TEXT,
                []() -> i32 { return VIBRATION_CHOICES; },
                getVibMode, setVibMode, dispVib, lineVib
            },
            {
                OPTIONMENU_LEFT_ARROW_RUN_BUTTON, OPTIONMENU_LEFT_ARROW_RUN_BUTTON_HOVER,
                OPTIONMENU_RIGHT_ARROW_RUN_BUTTON, OPTIONMENU_RIGHT_ARROW_RUN_BUTTON_HOVER,
                RUN_BUTTON_NAME,
                []() -> i32 { return RUN_BUTTON_CHOICES; },
                getRunMode, setRunMode, dispRun, lineRun
            },
        };

        for (const auto& c : cyclers)
        {
            i32 delta;
            if (id == c.leftBase || id == c.leftHover) delta = -1;
            else if (id == c.rightBase || id == c.rightHover) delta = +1;
            else continue;

            // In edit mode, only allow changing value if we're editing this option
            if (isEditing())
            {
                const StringID optionId = getOptionIdFromComponent(c.labelId);
                if (m_currentEditingOption != optionId)
                {
                    // Not editing this option, ignore arrow press
                    return;
                }
            }

            const i32 count = c.count ? c.count() : 0;
            const i32 next = CycleIndex(c.get(), count, delta);
            c.set(next);
            setTextByLineId(c.labelId, c.display(next), c.lineId(next));
            return;
        }
    }

    bbool Ray_OptionMenuHelper::handleResetToDefault(const StringID& id)
    {
        if (id != OPTIONMENU_RESET_TO_DEFAULT_BUTTON) return bfalse;
        auto* gm = RAY_GAMEMANAGER;
        if (!gm) return btrue;
        gm->getGameOptionManager().resetOption(OPTION_RESOLUTION);
        gm->getGameOptionManager().resetOption(OPTION_LANGUAGE);
        gm->getGameOptionManager().resetOption(OPTION_START_WITH_HEART);
        gm->getGameOptionManager().resetOption(OPTION_VIBRATIONS);
        gm->getGameOptionManager().resetOption(OPTION_RUN_BUTTON);
        UpdateResolutionText();
        UpdateLanguageText();
        UpdateStartWithHeartText();
        UpdateVibrationText();
        UpdateRunButtonText();
        return btrue;
    }

    void Ray_OptionMenuHelper::closeAndReturn()
    {
        m_isActive = bfalse;
        m_menu = nullptr;
        m_hasSnapshot = bfalse;
        if (UI_MENUMANAGER)
        {
            UI_MENUMANAGER->showPreviousMenu();
            UI_MENUMANAGER->setMenuListener(UI_MENUMANAGER->getCurrentMenuID(), m_mainListener);
        }
    }

    bbool Ray_OptionMenuHelper::handleAccept(const StringID& id)
    {
        if (id != OPTIONMENU_ACCEPT_BUTTON) return bfalse;
        if (RAY_GAMEMANAGER)
        {
            RAY_GAMEMANAGER->applyDisplayOptions();
            RAY_GAMEMANAGER->saveGameOptions();
        }
        closeAndReturn();
        return btrue;
    }

    bbool Ray_OptionMenuHelper::handleCancel(const StringID& id)
    {
        if (id != OPTIONMENU_CANCEL_BUTTON) return bfalse;
        if (RAY_GAMEMANAGER && m_hasSnapshot)
        {
            RAY_GAMEMANAGER->setResolutionIndex(m_snapshotResolutionIndex);
            RAY_GAMEMANAGER->setLanguageIndex(m_snapshotLanguageIndex);
            RAY_GAMEMANAGER->setStartWithHeartIndex(m_snapshotStartWithHeartIndex);
            RAY_GAMEMANAGER->setVibrationMode(m_snapshotVibrationMode);
            RAY_GAMEMANAGER->setRunButtonMode(m_snapshotRunButtonMode);
            RAY_GAMEMANAGER->setWindowed(m_snapshotWindowed);
            UpdateResolutionText();
            UpdateLanguageText();
            UpdateStartWithHeartText();
            UpdateVibrationText();
            UpdateRunButtonText();
            UpdateWindowCheckboxVisual();
        }
        closeAndReturn();
        return btrue;
    }


    StringID Ray_OptionMenuHelper::onMenuPageAction(UIMenu* menu, const StringID& action,
                                                    const StringID& defaultAction)
    {
        if (!UI_MENUMANAGER) return defaultAction;

        // If in edit mode, lock navigation - only allow left/right for changing values
        // Block up/down navigation
        if (isEditing())
        {
            // Check if action is up/down navigation
            // If so, block it. Only allow left/right which are handled by arrow buttons
            // For now, we'll let the default handler decide, but we could block specific actions here
            // The actual locking is done by making non-arrow components non-selectable
        }

        return UI_MENUMANAGER->onMenuPageAction_Default1ButtonMenu(menu, action, defaultAction);
    }

    bbool Ray_OptionMenuHelper::onMenuItemOtherAction(UIComponent* component, const StringID& action)
    {
        return bfalse;
    }

    void Ray_OptionMenuHelper::UpdateMenuOnSelectionChange(UIComponent* uiComponent, bbool isSelected)
    {
        if (!uiComponent) return;

        const StringID id = uiComponent->getID();

        // If in edit mode, lock navigation - only allow arrow buttons
        if (isEditing())
        {
            // Only allow hover effects for arrows of the current editing option
            for (const HoverPair& pair : kHoverPairs)
            {
                if (id == pair.baseId || id == pair.hoverId)
                {
                    const StringID optionId = getOptionIdFromComponent(id);
                    if (optionId == m_currentEditingOption)
                    {
                        // Allow hover effect for arrows of current editing option
                        if (isSelected)
                        {
                            const bbool changeSelection = (id == pair.baseId) ? btrue : bfalse;
                            swapUIComponentActivation(pair.baseId, pair.hoverId, changeSelection);
                        }
                        else
                        {
                            if (id == pair.hoverId)
                            {
                                swapUIComponentActivation(pair.hoverId, pair.baseId, bfalse);
                            }
                        }
                    }
                    break;
                }
            }
            return;
        }

        // In navigate mode, handle focus changes
        if (isNavigating())
        {
            // Check if this is an option label or text
            const StringID optionId = getOptionIdFromComponent(id);
            if (optionId != StringID::Invalid)
            {
                if (isSelected)
                {
                    // Unfocus previous option
                    if (m_currentFocusedOption != StringID::Invalid && m_currentFocusedOption != optionId)
                    {
                        setOptionFocus(m_currentFocusedOption, bfalse);
                    }

                    // Focus new option
                    setOptionFocus(optionId, btrue);
                    m_currentFocusedOption = optionId;
                }
            }

            // Handle arrow hover effects (only show when not navigating)
            // Arrows are hidden in navigate mode, so this shouldn't trigger
            // But keep for safety
            for (const HoverPair& pair : kHoverPairs)
            {
                if (id == pair.baseId || id == pair.hoverId)
                {
                    // Arrows should be hidden in navigate mode
                    break;
                }
            }
        }
    }

    void Ray_OptionMenuHelper::UpdateArrowVisibilityByCount() const
    {
        auto hideArrowsIfSingle = [this](const StringID& optionId,
                                         const StringID& leftBase, const StringID& leftHover,
                                         const StringID& rightBase, const StringID& rightHover)
        {
            const i32 count = GetListCountForOption(optionId);
            const bbool show = (count > 1) ? btrue : bfalse;
            setUIVisibilitySelectable(leftBase, show, show);
            setUIVisibilitySelectable(rightBase, show, show);
            if (!show)
            {
                setUIVisibility(leftHover, bfalse);
                setUIVisibility(rightHover, bfalse);
            }
        };

        hideArrowsIfSingle(OPTION_RESOLUTION,
                           OPTIONMENU_LEFT_ARROW_RESOLUTION, OPTIONMENU_LEFT_ARROW_RESOLUTION_HOVER,
                           OPTIONMENU_RIGHT_ARROW_RESOLUTION, OPTIONMENU_RIGHT_ARROW_RESOLUTION_HOVER);

        hideArrowsIfSingle(OPTION_LANGUAGE,
                           OPTIONMENU_LEFT_ARROW_LANGUAGE, OPTIONMENU_LEFT_ARROW_LANGUAGE_HOVER,
                           OPTIONMENU_RIGHT_ARROW_LANGUAGE, OPTIONMENU_RIGHT_ARROW_LANGUAGE_HOVER);

        hideArrowsIfSingle(OPTION_START_WITH_HEART,
                           OPTIONMENU_LEFT_ARROW_START_WITH_HEART, OPTIONMENU_LEFT_ARROW_START_WITH_HEART_HOVER,
                           OPTIONMENU_RIGHT_ARROW_START_WITH_HEART, OPTIONMENU_RIGHT_ARROW_START_WITH_HEART_HOVER);

        hideArrowsIfSingle(OPTION_VIBRATIONS,
                           OPTIONMENU_LEFT_ARROW_VIBRATION, OPTIONMENU_LEFT_ARROW_VIBRATION_HOVER,
                           OPTIONMENU_RIGHT_ARROW_VIBRATION, OPTIONMENU_RIGHT_ARROW_VIBRATION_HOVER);

        hideArrowsIfSingle(OPTION_RUN_BUTTON,
                           OPTIONMENU_LEFT_ARROW_RUN_BUTTON, OPTIONMENU_LEFT_ARROW_RUN_BUTTON_HOVER,
                           OPTIONMENU_RIGHT_ARROW_RUN_BUTTON, OPTIONMENU_RIGHT_ARROW_RUN_BUTTON_HOVER);
    }

    void Ray_OptionMenuHelper::setUIVisibilitySelectable(const StringID& id, bbool visible, bbool selectable,
                                                         bbool changeMenuSelection) const
    {
        if (!m_menu) return;
        if (UIComponent* c = m_menu->getUIComponentByID(id))
        {
            if (Actor* a = c->GetActor()) a->disableDraw(visible ? bfalse : btrue);
            c->setActive(visible);
            c->setCanBeSelected(selectable);
            if (!selectable && c->getIsSelected())
            {
                c->setIsSelected(bfalse);
            }
            if (changeMenuSelection) c->setIsSelected(visible && selectable);
        }
    }

    void Ray_OptionMenuHelper::setUIVisibility(const StringID& id, bbool visible, bbool changeMenuSelection) const
    {
        if (!m_menu) return;
        if (UIComponent* c = m_menu->getUIComponentByID(id))
        {
            setUIVisibilitySelectable(id, visible, c->getCanBeSelected(), changeMenuSelection);
        }
    }

    void Ray_OptionMenuHelper::setVisibilityFor(std::initializer_list<StringID> ids, bbool visible,
                                                bbool selectable) const
    {
        for (const auto& id : ids)
        {
            setUIVisibilitySelectable(id, visible, selectable);
        }
    }

    void Ray_OptionMenuHelper::UpdateResolutionText()
    {
        if (!RAY_GAMEMANAGER) return;
        const i32 index = RAY_GAMEMANAGER->getResolutionIndex();
        setTextByLineId(RESOLUTION_TEXT,
                        RAY_GAMEMANAGER->getResolutionDisplayName(index),
                        0);
    }

    void Ray_OptionMenuHelper::UpdateRunButtonText()
    {
        if (!RAY_GAMEMANAGER) return;
        const i32 mode = RAY_GAMEMANAGER->getRunButtonMode();
        setTextByLineId(RUN_BUTTON_NAME,
                        RAY_GAMEMANAGER->getRunButtonDisplayName(mode),
                        RAY_GAMEMANAGER->getRunButtonLineId(mode));
    }

    void Ray_OptionMenuHelper::UpdateStartWithHeartText()
    {
        if (!RAY_GAMEMANAGER) return;
        const i32 index = RAY_GAMEMANAGER->getStartWithHeartIndex();
        setTextByLineId(START_WITH_HEART,
                        RAY_GAMEMANAGER->getStartWithHeartDisplayName(index),
                        RAY_GAMEMANAGER->getStartWithHeartLineId(index));
    }

    void Ray_OptionMenuHelper::UpdateVibrationText()
    {
        if (!RAY_GAMEMANAGER) return;
        const i32 mode = RAY_GAMEMANAGER->getVibrationMode();
        setTextByLineId(VIBRATION_TEXT,
                        RAY_GAMEMANAGER->getVibrationDisplayName(mode),
                        RAY_GAMEMANAGER->getVibrationLineId(mode));
    }

    void Ray_OptionMenuHelper::UpdateLanguageText()
    {
        if (!RAY_GAMEMANAGER) return;
        const i32 index = RAY_GAMEMANAGER->getLanguageIndex();
        setTextByLineId(LANGUAGE_TEXT,
                        RAY_GAMEMANAGER->getLanguageDisplayName(index),
                        0);
    }

    void Ray_OptionMenuHelper::UpdateWindowCheckboxVisual() const
    {
        if (!m_menu || !RAY_GAMEMANAGER) return;
        if (RAY_GAMEMANAGER->isWindowed())
        {
            swapUIComponentActivation(OPTIONMENU_CHECKBOX_WINDOW_OFF, OPTIONMENU_CHECKBOX_WINDOW_ON, btrue);
        }
        else
        {
            swapUIComponentActivation(OPTIONMENU_CHECKBOX_WINDOW_ON, OPTIONMENU_CHECKBOX_WINDOW_OFF, btrue);
        }
    }

    void Ray_OptionMenuHelper::ToggleWindowCheckbox() const
    {
        if (!RAY_GAMEMANAGER) return;
        const bbool current = RAY_GAMEMANAGER->isWindowed();
        RAY_GAMEMANAGER->setWindowed(current ? bfalse : btrue);
        UpdateWindowCheckboxVisual();
        const bbool nowWindowed = RAY_GAMEMANAGER->isWindowed();
        if (m_menu)
        {
            const StringID idToSelect = nowWindowed ? OPTIONMENU_CHECKBOX_WINDOW_ON : OPTIONMENU_CHECKBOX_WINDOW_OFF;
            if (UIComponent* c = m_menu->getUIComponentByID(idToSelect))
            {
                c->setIsSelected(btrue);
            }
        }
        ensureValidSelection();
    }

    void Ray_OptionMenuHelper::swapUIComponentActivation(const StringID& idToDisable,
                                                         const StringID& idToEnable,
                                                         bbool changeMenuSelection) const
    {
        if (!m_menu) return;

        if (UIComponent* c = m_menu->getUIComponentByID(idToDisable))
        {
            if (Actor* a = c->GetActor()) a->disableDraw(btrue);
            c->setActive(bfalse);
            c->setCanBeSelected(bfalse);
            if (changeMenuSelection) c->setIsSelected(bfalse);
        }
        if (UIComponent* c = m_menu->getUIComponentByID(idToEnable))
        {
            if (Actor* a = c->GetActor()) a->disableDraw(bfalse);
            c->setActive(btrue);
            c->setCanBeSelected(btrue);
            if (changeMenuSelection) c->setIsSelected(btrue);
        }
    }

    void Ray_OptionMenuHelper::setTextByLineId(const StringID& itemId, const char* friendly, u32 lineId)
    {
        if (!UI_MENUMANAGER) return;
        UI_MENUMANAGER->changeMenuItemFriendlyByID(OPTION_MENU_NAME, itemId, String(friendly ? friendly : ""));
    }

    void Ray_OptionMenuHelper::initializeMenuState()
    {
        if (!m_menu) return;

        m_menuState = MenuState_Navigate;
        m_currentEditingOption = StringID::Invalid;
        m_currentFocusedOption = StringID::Invalid;

        // Hide all arrows initially
        hideAllArrows();

        // Set all options to unfocused (yellow color)
        setAllOptionsUnfocused();

        // Ensure all option labels are selectable (these are the main navigation targets)
        const StringID optionLabels[] = {
            RESOLUTION_LABEL, LANGUAGE_LABEL, HEARTS_LABEL,
            RUN_LABEL, VIBRATION_LABEL, WINDOW_LABEL
        };

        for (const StringID& labelId : optionLabels)
        {
            if (UIComponent* comp = m_menu->getUIComponentByID(labelId))
            {
                comp->setCanBeSelected(btrue);
            }
        }

        // Make text values and arrows non-selectable in navigate mode
        // They will only be selectable when in edit mode
        const StringID textIds[] = {
            RESOLUTION_TEXT, LANGUAGE_TEXT, START_WITH_HEART,
            RUN_BUTTON_NAME, VIBRATION_TEXT
        };

        for (const StringID& textId : textIds)
        {
            if (UIComponent* comp = m_menu->getUIComponentByID(textId))
            {
                comp->setCanBeSelected(bfalse);
            }
        }

        // Hide and disable all arrows initially
        hideAllArrows();

        // Make action buttons selectable
        if (UIComponent* acceptComp = m_menu->getUIComponentByID(OPTIONMENU_ACCEPT_BUTTON))
            acceptComp->setCanBeSelected(btrue);
        if (UIComponent* cancelComp = m_menu->getUIComponentByID(OPTIONMENU_CANCEL_BUTTON))
            cancelComp->setCanBeSelected(btrue);
        if (UIComponent* resetComp = m_menu->getUIComponentByID(OPTIONMENU_RESET_TO_DEFAULT_BUTTON))
            resetComp->setCanBeSelected(btrue);

        // Focus on resolution by default - select the label
        m_currentFocusedOption = RESOLUTION_LABEL;
        setOptionFocus(RESOLUTION_LABEL, btrue);

        // Select resolution label component
        if (UIComponent* resComp = m_menu->getUIComponentByID(RESOLUTION_LABEL))
        {
            resComp->setIsSelected(btrue);
            resComp->onRollover();
        }
        else
        {
            ensureValidSelection();
        }
    }

    void Ray_OptionMenuHelper::setMenuState(EMenuState state)
    {
        m_menuState = state;
    }

    void Ray_OptionMenuHelper::enterEditMode(const StringID& optionId)
    {
        if (m_menuState == MenuState_EditOption && m_currentEditingOption == optionId)
        {
            // Already editing this option, exit edit mode
            exitEditMode();
            return;
        }

        m_menuState = MenuState_EditOption;
        m_currentEditingOption = optionId;

        // Show arrows for this option
        showArrowsForOption(optionId);

        // Focus on left arrow of the option being edited
        StringID leftArrowId = StringID::Invalid;
        if (optionId == RESOLUTION_LABEL) leftArrowId = OPTIONMENU_LEFT_ARROW_RESOLUTION;
        else if (optionId == LANGUAGE_LABEL) leftArrowId = OPTIONMENU_LEFT_ARROW_LANGUAGE;
        else if (optionId == HEARTS_LABEL) leftArrowId = OPTIONMENU_LEFT_ARROW_START_WITH_HEART;
        else if (optionId == VIBRATION_LABEL) leftArrowId = OPTIONMENU_LEFT_ARROW_VIBRATION;
        else if (optionId == RUN_LABEL) leftArrowId = OPTIONMENU_LEFT_ARROW_RUN_BUTTON;

        if (leftArrowId != StringID::Invalid && m_menu)
        {
            if (UIComponent* arrowComp = m_menu->getUIComponentByID(leftArrowId))
            {
                if (arrowComp->getActive() && arrowComp->getCanBeSelected())
                {
                    arrowComp->setIsSelected(btrue);
                    arrowComp->onRollover();
                }
            }
        }

        // Lock other options - make them non-selectable
        // Only allow arrows of current editing option to be selectable
        if (m_menu)
        {
            // Make all option labels non-selectable (including the one being edited)
            // The label itself can still be pressed to exit edit mode, but not selectable for navigation
            const StringID optionLabels[] = {
                RESOLUTION_LABEL, LANGUAGE_LABEL, HEARTS_LABEL,
                RUN_LABEL, VIBRATION_LABEL, WINDOW_LABEL
            };

            for (const StringID& labelId : optionLabels)
            {
                // Make label non-selectable (even the one being edited)
                // It can still receive press action to exit edit mode
                if (UIComponent* comp = m_menu->getUIComponentByID(labelId))
                {
                    comp->setCanBeSelected(bfalse);
                }

                // Make corresponding text non-selectable (always non-selectable in edit mode)
                StringID textId = StringID::Invalid;
                if (labelId == RESOLUTION_LABEL) textId = RESOLUTION_TEXT;
                else if (labelId == LANGUAGE_LABEL) textId = LANGUAGE_TEXT;
                else if (labelId == HEARTS_LABEL) textId = START_WITH_HEART;
                else if (labelId == RUN_LABEL) textId = RUN_BUTTON_NAME;
                else if (labelId == VIBRATION_LABEL) textId = VIBRATION_TEXT;

                if (textId != StringID::Invalid)
                {
                    if (UIComponent* comp = m_menu->getUIComponentByID(textId))
                    {
                        comp->setCanBeSelected(bfalse);
                    }
                }
            }

            // Make action buttons non-selectable in edit mode
            // (User can still exit by pressing again on the option)
            if (UIComponent* acceptComp = m_menu->getUIComponentByID(OPTIONMENU_ACCEPT_BUTTON))
                acceptComp->setCanBeSelected(bfalse);
            if (UIComponent* cancelComp = m_menu->getUIComponentByID(OPTIONMENU_CANCEL_BUTTON))
                cancelComp->setCanBeSelected(bfalse);
            if (UIComponent* resetComp = m_menu->getUIComponentByID(OPTIONMENU_RESET_TO_DEFAULT_BUTTON))
                resetComp->setCanBeSelected(bfalse);
        }
    }

    void Ray_OptionMenuHelper::exitEditMode()
    {
        if (m_menuState != MenuState_EditOption) return;

        // Hide arrows for current editing option
        if (m_currentEditingOption != StringID::Invalid)
        {
            hideArrowsForOption(m_currentEditingOption);
        }

        // Unlock all options - make labels selectable again, but keep text values non-selectable
        if (m_menu)
        {
            const StringID optionLabels[] = {
                RESOLUTION_LABEL, LANGUAGE_LABEL, HEARTS_LABEL,
                RUN_LABEL, VIBRATION_LABEL, WINDOW_LABEL
            };

            for (const StringID& labelId : optionLabels)
            {
                // Make label selectable (these are the navigation targets)
                if (UIComponent* comp = m_menu->getUIComponentByID(labelId))
                {
                    comp->setCanBeSelected(btrue);
                }

                // Keep text values non-selectable (they're just display, not navigation targets)
                StringID textId = StringID::Invalid;
                if (labelId == RESOLUTION_LABEL) textId = RESOLUTION_TEXT;
                else if (labelId == LANGUAGE_LABEL) textId = LANGUAGE_TEXT;
                else if (labelId == HEARTS_LABEL) textId = START_WITH_HEART;
                else if (labelId == RUN_LABEL) textId = RUN_BUTTON_NAME;
                else if (labelId == VIBRATION_LABEL) textId = VIBRATION_TEXT;

                if (textId != StringID::Invalid)
                {
                    if (UIComponent* comp = m_menu->getUIComponentByID(textId))
                    {
                        comp->setCanBeSelected(bfalse); // Keep text non-selectable
                    }
                }
            }

            // Make action buttons selectable again
            if (UIComponent* acceptComp = m_menu->getUIComponentByID(OPTIONMENU_ACCEPT_BUTTON))
                acceptComp->setCanBeSelected(btrue);
            if (UIComponent* cancelComp = m_menu->getUIComponentByID(OPTIONMENU_CANCEL_BUTTON))
                cancelComp->setCanBeSelected(btrue);
            if (UIComponent* resetComp = m_menu->getUIComponentByID(OPTIONMENU_RESET_TO_DEFAULT_BUTTON))
                resetComp->setCanBeSelected(btrue);
        }

        m_menuState = MenuState_Navigate;
        StringID previousEditingOption = m_currentEditingOption;
        m_currentEditingOption = StringID::Invalid;

        // Ensure focus is back on the option we were editing
        if (previousEditingOption != StringID::Invalid && m_menu)
        {
            // Focus back on the label of the option we were editing
            if (UIComponent* labelComp = m_menu->getUIComponentByID(previousEditingOption))
            {
                if (labelComp->getActive() && labelComp->getCanBeSelected())
                {
                    labelComp->setIsSelected(btrue);
                    labelComp->onRollover();
                    m_currentFocusedOption = previousEditingOption;
                }
            }
            else
            {
                ensureValidSelection();
            }
        }
        else if (m_currentFocusedOption != StringID::Invalid)
        {
            ensureValidSelection();
        }
    }

    bbool Ray_OptionMenuHelper::isOptionEditable(const StringID& optionId) const
    {
        // Options that can be edited (have arrow buttons)
        return optionId == RESOLUTION_LABEL ||
               optionId == LANGUAGE_LABEL ||
               optionId == HEARTS_LABEL ||
               optionId == RUN_LABEL ||
               optionId == VIBRATION_LABEL;
    }

    void Ray_OptionMenuHelper::setOptionFocus(const StringID& optionId, bbool focused)
    {
        if (!m_menu) return;

        // Map option labels to their corresponding text IDs
        struct OptionMapping
        {
            StringID labelId;
            StringID textId;
        };

        static const OptionMapping mappings[] = {
            {RESOLUTION_LABEL, RESOLUTION_TEXT},
            {LANGUAGE_LABEL, LANGUAGE_TEXT},
            {HEARTS_LABEL, START_WITH_HEART},
            {RUN_LABEL, RUN_BUTTON_NAME},
            {VIBRATION_LABEL, VIBRATION_TEXT},
            {WINDOW_LABEL, StringID::Invalid}, // Checkbox, no text
        };

        for (const auto& mapping : mappings)
        {
            if (mapping.labelId == optionId)
            {
                setOptionTextColor(optionId, mapping.textId, focused);
                break;
            }
        }
    }

    void Ray_OptionMenuHelper::setAllOptionsUnfocused()
    {
        // Set all options to unfocused (yellow) state
        setOptionFocus(RESOLUTION_LABEL, bfalse);
        setOptionFocus(LANGUAGE_LABEL, bfalse);
        setOptionFocus(HEARTS_LABEL, bfalse);
        setOptionFocus(RUN_LABEL, bfalse);
        setOptionFocus(VIBRATION_LABEL, bfalse);
        setOptionFocus(WINDOW_LABEL, bfalse);
    }

    void Ray_OptionMenuHelper::setOptionTextColor(const StringID& labelId, const StringID& textId, bbool isFocused)
    {
        if (!m_menu) return;

        // Set label color (yellow = unfocused, white = focused)
        // Using alpha/visibility as a proxy for color since we may not have direct color control
        // Focused = fully visible (white), Unfocused = slightly dimmed (yellow equivalent)
        const f32 focusedAlpha = 1.0f;
        const f32 unfocusedAlpha = 0.7f; // Dimmed to simulate yellow

        if (UIComponent* labelComp = m_menu->getUIComponentByID(labelId))
        {
            if (Actor* actor = labelComp->GetActor())
            {
                // Try to find UITextBox component to set alpha
                if (UITextBox* textBox = actor->GetComponent<UITextBox>())
                {
                    textBox->setTextAlpha(isFocused ? focusedAlpha : unfocusedAlpha);
                }
            }
        }

        // Set text value color if it exists
        if (textId != StringID::Invalid)
        {
            if (UIComponent* textComp = m_menu->getUIComponentByID(textId))
            {
                if (Actor* actor = textComp->GetActor())
                {
                    if (UITextBox* textBox = actor->GetComponent<UITextBox>())
                    {
                        textBox->setTextAlpha(isFocused ? focusedAlpha : unfocusedAlpha);
                    }
                }
            }
        }
    }

    void Ray_OptionMenuHelper::hideAllArrows()
    {
        setVisibilityFor({
            OPTIONMENU_LEFT_ARROW_RESOLUTION,
            OPTIONMENU_RIGHT_ARROW_RESOLUTION,
            OPTIONMENU_LEFT_ARROW_LANGUAGE,
            OPTIONMENU_RIGHT_ARROW_LANGUAGE,
            OPTIONMENU_LEFT_ARROW_START_WITH_HEART,
            OPTIONMENU_RIGHT_ARROW_START_WITH_HEART,
            OPTIONMENU_LEFT_ARROW_VIBRATION,
            OPTIONMENU_RIGHT_ARROW_VIBRATION,
            OPTIONMENU_LEFT_ARROW_RUN_BUTTON,
            OPTIONMENU_RIGHT_ARROW_RUN_BUTTON,
        }, bfalse, bfalse);

        // Hide hover arrows too
        setUIVisibility(OPTIONMENU_LEFT_ARROW_RESOLUTION_HOVER, bfalse);
        setUIVisibility(OPTIONMENU_RIGHT_ARROW_RESOLUTION_HOVER, bfalse);
        setUIVisibility(OPTIONMENU_LEFT_ARROW_LANGUAGE_HOVER, bfalse);
        setUIVisibility(OPTIONMENU_RIGHT_ARROW_LANGUAGE_HOVER, bfalse);
        setUIVisibility(OPTIONMENU_LEFT_ARROW_START_WITH_HEART_HOVER, bfalse);
        setUIVisibility(OPTIONMENU_RIGHT_ARROW_START_WITH_HEART_HOVER, bfalse);
        setUIVisibility(OPTIONMENU_LEFT_ARROW_VIBRATION_HOVER, bfalse);
        setUIVisibility(OPTIONMENU_RIGHT_ARROW_VIBRATION_HOVER, bfalse);
        setUIVisibility(OPTIONMENU_LEFT_ARROW_RUN_BUTTON_HOVER, bfalse);
        setUIVisibility(OPTIONMENU_RIGHT_ARROW_RUN_BUTTON_HOVER, bfalse);
    }

    void Ray_OptionMenuHelper::showArrowsForOption(const StringID& optionId)
    {
        if (optionId == RESOLUTION_LABEL)
        {
            const i32 count = GetListCountForOption(OPTION_RESOLUTION);
            if (count > 1)
            {
                setUIVisibilitySelectable(OPTIONMENU_LEFT_ARROW_RESOLUTION, btrue, btrue);
                setUIVisibilitySelectable(OPTIONMENU_RIGHT_ARROW_RESOLUTION, btrue, btrue);
            }
        }
        else if (optionId == LANGUAGE_LABEL)
        {
            const i32 count = GetListCountForOption(OPTION_LANGUAGE);
            if (count > 1)
            {
                setUIVisibilitySelectable(OPTIONMENU_LEFT_ARROW_LANGUAGE, btrue, btrue);
                setUIVisibilitySelectable(OPTIONMENU_RIGHT_ARROW_LANGUAGE, btrue, btrue);
            }
        }
        else if (optionId == HEARTS_LABEL)
        {
            setUIVisibilitySelectable(OPTIONMENU_LEFT_ARROW_START_WITH_HEART, btrue, btrue);
            setUIVisibilitySelectable(OPTIONMENU_RIGHT_ARROW_START_WITH_HEART, btrue, btrue);
        }
        else if (optionId == VIBRATION_LABEL)
        {
            setUIVisibilitySelectable(OPTIONMENU_LEFT_ARROW_VIBRATION, btrue, btrue);
            setUIVisibilitySelectable(OPTIONMENU_RIGHT_ARROW_VIBRATION, btrue, btrue);
        }
        else if (optionId == RUN_LABEL)
        {
            setUIVisibilitySelectable(OPTIONMENU_LEFT_ARROW_RUN_BUTTON, btrue, btrue);
            setUIVisibilitySelectable(OPTIONMENU_RIGHT_ARROW_RUN_BUTTON, btrue, btrue);
        }
    }

    void Ray_OptionMenuHelper::hideArrowsForOption(const StringID& optionId)
    {
        if (optionId == RESOLUTION_LABEL)
        {
            setUIVisibilitySelectable(OPTIONMENU_LEFT_ARROW_RESOLUTION, bfalse, bfalse);
            setUIVisibilitySelectable(OPTIONMENU_RIGHT_ARROW_RESOLUTION, bfalse, bfalse);
            setUIVisibility(OPTIONMENU_LEFT_ARROW_RESOLUTION_HOVER, bfalse);
            setUIVisibility(OPTIONMENU_RIGHT_ARROW_RESOLUTION_HOVER, bfalse);
        }
        else if (optionId == LANGUAGE_LABEL)
        {
            setUIVisibilitySelectable(OPTIONMENU_LEFT_ARROW_LANGUAGE, bfalse, bfalse);
            setUIVisibilitySelectable(OPTIONMENU_RIGHT_ARROW_LANGUAGE, bfalse, bfalse);
            setUIVisibility(OPTIONMENU_LEFT_ARROW_LANGUAGE_HOVER, bfalse);
            setUIVisibility(OPTIONMENU_RIGHT_ARROW_LANGUAGE_HOVER, bfalse);
        }
        else if (optionId == HEARTS_LABEL)
        {
            setUIVisibilitySelectable(OPTIONMENU_LEFT_ARROW_START_WITH_HEART, bfalse, bfalse);
            setUIVisibilitySelectable(OPTIONMENU_RIGHT_ARROW_START_WITH_HEART, bfalse, bfalse);
            setUIVisibility(OPTIONMENU_LEFT_ARROW_START_WITH_HEART_HOVER, bfalse);
            setUIVisibility(OPTIONMENU_RIGHT_ARROW_START_WITH_HEART_HOVER, bfalse);
        }
        else if (optionId == VIBRATION_LABEL)
        {
            setUIVisibilitySelectable(OPTIONMENU_LEFT_ARROW_VIBRATION, bfalse, bfalse);
            setUIVisibilitySelectable(OPTIONMENU_RIGHT_ARROW_VIBRATION, bfalse, bfalse);
            setUIVisibility(OPTIONMENU_LEFT_ARROW_VIBRATION_HOVER, bfalse);
            setUIVisibility(OPTIONMENU_RIGHT_ARROW_VIBRATION_HOVER, bfalse);
        }
        else if (optionId == RUN_LABEL)
        {
            setUIVisibilitySelectable(OPTIONMENU_LEFT_ARROW_RUN_BUTTON, bfalse, bfalse);
            setUIVisibilitySelectable(OPTIONMENU_RIGHT_ARROW_RUN_BUTTON, bfalse, bfalse);
            setUIVisibility(OPTIONMENU_LEFT_ARROW_RUN_BUTTON_HOVER, bfalse);
            setUIVisibility(OPTIONMENU_RIGHT_ARROW_RUN_BUTTON_HOVER, bfalse);
        }
    }

    StringID Ray_OptionMenuHelper::getOptionIdFromComponent(const StringID& componentId) const
    {
        // Map component IDs to option IDs
        if (componentId == RESOLUTION_LABEL ||
            componentId == RESOLUTION_TEXT ||
            componentId == OPTIONMENU_LEFT_ARROW_RESOLUTION ||
            componentId == OPTIONMENU_RIGHT_ARROW_RESOLUTION)
            return RESOLUTION_LABEL;

        if (componentId == LANGUAGE_LABEL ||
            componentId == LANGUAGE_TEXT ||
            componentId == OPTIONMENU_LEFT_ARROW_LANGUAGE ||
            componentId == OPTIONMENU_RIGHT_ARROW_LANGUAGE)
            return LANGUAGE_LABEL;

        if (componentId == HEARTS_LABEL ||
            componentId == START_WITH_HEART ||
            componentId == OPTIONMENU_LEFT_ARROW_START_WITH_HEART ||
            componentId == OPTIONMENU_RIGHT_ARROW_START_WITH_HEART)
            return HEARTS_LABEL;

        if (componentId == RUN_LABEL ||
            componentId == RUN_BUTTON_NAME ||
            componentId == OPTIONMENU_LEFT_ARROW_RUN_BUTTON ||
            componentId == OPTIONMENU_RIGHT_ARROW_RUN_BUTTON)
            return RUN_LABEL;

        if (componentId == VIBRATION_LABEL ||
            componentId == VIBRATION_TEXT ||
            componentId == OPTIONMENU_LEFT_ARROW_VIBRATION ||
            componentId == OPTIONMENU_RIGHT_ARROW_VIBRATION)
            return VIBRATION_LABEL;

        if (componentId == WINDOW_LABEL ||
            componentId == OPTIONMENU_CHECKBOX_WINDOW_OFF ||
            componentId == OPTIONMENU_CHECKBOX_WINDOW_ON)
            return WINDOW_LABEL;

        return StringID::Invalid;
    }

    void Ray_OptionMenuHelper::ensureValidSelection() const
    {
        if (!m_menu) return;
        UIComponent* selected = m_menu->getUIComponentSelected();
        if (selected && selected->getActive() && selected->getCanBeSelected())
        {
            return;
        }
        ObjectRefList list = m_menu->getUIComponentsList();
        for (auto i : list)
        {
            if (UIComponent* c = UI_MENUMANAGER->getUIComponent(i))
            {
                if (c->getActive() && c->getCanBeSelected())
                {
                    if (selected)
                    {
                        selected->setIsSelected(bfalse);
                        selected->onRollout();
                    }
                    c->setIsSelected(btrue);
                    c->onRollover();
                    break;
                }
            }
        }
    }
}
