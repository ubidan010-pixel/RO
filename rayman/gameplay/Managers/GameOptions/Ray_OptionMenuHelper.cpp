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

        for (const auto& p : kHoverPairs)
        {
            if (UIComponent* c = m_menu->getUIComponentByID(p.hoverId))
            {
                if (Actor* a = c->GetActor()) a->disableDraw(btrue);
                c->setActive(bfalse);
            }
        }

        UpdateResolutionText();
        UpdateLanguageText();
        UpdateStartWithHeartText();
        UpdateVibrationText();
        UpdateRunButtonText();
        UpdateWindowCheckboxVisual();
        UpdateArrowVisibilityByCount();
        ensureValidSelection();

        if (RAY_GAMEMANAGER)
        {
            m_snapshotResolutionIndex = RAY_GAMEMANAGER->getResolutionIndex();
            m_snapshotLanguageIndex = RAY_GAMEMANAGER->getLanguageIndex();
            m_snapshotStartWithHeartIndex = RAY_GAMEMANAGER->getStartWithHeartIndex();
            m_snapshotVibrationMode = RAY_GAMEMANAGER->getVibrationMode();
            m_snapshotRunButtonMode = RAY_GAMEMANAGER->getRunButtonMode();
            m_snapshotWindowed = RAY_GAMEMANAGER->isWindowed();
            m_hasSnapshot = btrue;
        }
    }

    void Ray_OptionMenuHelper::onMenuItemAction(UIComponent* component)
    {
        if (!component) return;

        const StringID id = component->getID();

        if (handleResetToDefault(id)) return;
        if (handleAccept(id)) return;
        if (handleCancel(id)) return;
        if (id == OPTIONMENU_CHECKBOX_WINDOW_OFF || id == OPTIONMENU_CHECKBOX_WINDOW_ON)
        {
            ToggleWindowCheckbox();
            return;
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
        for (const HoverPair& pair : kHoverPairs)
        {
            if (id == pair.baseId || id == pair.hoverId)
            {
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
                break;
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
