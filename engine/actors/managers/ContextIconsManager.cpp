#include "precompiled_engine.h"

#ifndef _ITF_CONTEXTICONSMANAGER_H_
#include "engine/actors/managers/ContextIconsManager.h"
#endif //_ITF_CONTEXTICONSMANAGER_H_

#ifndef _ITF_ALIASMANAGER_H_
#include "engine/aliasmanager/aliasmanager.h"
#endif //_ITF_ALIASMANAGER_H_

#ifndef _ITF_GAMEINTERFACE_H_
#include "engine/gameplay/GameInterface.h"
#endif //_ITF_GAMEINTERFACE_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_UIMENU_H_
#include "gameplay/components/UI/UIMenu.h"
#endif //_ITF_UIMENU_H_

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif //_ITF_UIMENUMANAGER_H_

#ifdef ITF_WINDOWS
#include <windows.h>
#endif // ITF_WINDOWS

#include <cctype>

namespace ITF {

namespace
{
    ITF_INLINE bbool IsBindingConfigured(const InputValue& binding)
    {
        return binding.inputType != Keyboard ||
            binding.inputValue != 0 ||
            binding.inputIndex != 0 ||
            binding.axisPosition != 0;
    }

    ITF_INLINE InputAdapter::ActionType GetActionForContextIcon(EContextIconType iconType)
    {
        switch (iconType)
        {
        case ContextIconType_Select: return InputAdapter::ActionJump;
        case ContextIconType_Back: return InputAdapter::ActionBack;
        case ContextIconType_Delete: return InputAdapter::ActionHit;
        case ContextIconType_RayHome: return InputAdapter::ActionShowMenu;
        default: return InputAdapter::ActionJump;
        }
    }

    ITF_INLINE String8 KeyboardIconFromVirtualKey(u32 virtualKey)
    {
        String8 icon;
#ifdef ITF_WINDOWS
        switch (virtualKey)
        {
        case VK_SPACE: icon = "[icon:KEYBOARD_SPACE]"; return icon;
        case VK_ESCAPE: icon = "[icon:KEYBOARD_ESCAPE]"; return icon;
        case VK_RETURN: icon = "[icon:KEYBOARD_ENTER]"; return icon;
        case VK_BACK: icon = "[icon:KEYBOARD_BACKSPACE]"; return icon;
        case VK_DELETE: icon = "[icon:KEYBOARD_DELETE]"; return icon;
        case VK_SHIFT: icon = "[icon:KEYBOARD_SHIFT]"; return icon;
        case VK_LSHIFT: icon = "[icon:KEYBOARD_SHIFT]"; return icon;
        case VK_RSHIFT: icon = "[icon:KEYBOARD_SHIFT]"; return icon;
        case VK_CONTROL: icon = "[icon:KEYBOARD_CTRL]"; return icon;
        case VK_LCONTROL: icon = "[icon:KEYBOARD_CTRL]"; return icon;
        case VK_RCONTROL: icon = "[icon:KEYBOARD_CTRL]"; return icon;
        case VK_MENU: icon = "[icon:KEYBOARD_ALT]"; return icon;
        case VK_LMENU: icon = "[icon:KEYBOARD_ALT]"; return icon;
        case VK_RMENU: icon = "[icon:KEYBOARD_ALT]"; return icon;
        case VK_TAB: icon = "[icon:KEYBOARD_TAB]"; return icon;
        case VK_UP: icon = "[icon:KEYBOARD_ARROW_UP]"; return icon;
        case VK_DOWN: icon = "[icon:KEYBOARD_ARROW_DOWN]"; return icon;
        case VK_LEFT: icon = "[icon:KEYBOARD_ARROW_LEFT]"; return icon;
        case VK_RIGHT: icon = "[icon:KEYBOARD_ARROW_RIGHT]"; return icon;
        }

        if (virtualKey >= '0' && virtualKey <= '9')
        {
            icon.setTextFormat("[icon:KEYBOARD_%c]", char(virtualKey));
            return icon;
        }

        if (virtualKey >= 'A' && virtualKey <= 'Z')
        {
            icon.setTextFormat("[icon:KEYBOARD_%c]", char(std::toupper(static_cast<int>(virtualKey))));
            return icon;
        }
#else
        (void)virtualKey;
#endif
        return icon;
    }
}

#define CONTEXTICONSCONFIG_PATH GETPATH_ALIAS("contexticons")

//------------------------------------------------------------------------------
ContextIconsManager::ContextIconsManager()
    : m_template(NULL)
    , m_configPath()
    , m_left(ContextIcon_Invalid)
    , m_right(ContextIcon_Invalid)
    , m_topLeft(ContextIcon_Invalid)
    , m_topRight(ContextIcon_Invalid)
    , m_useRuntimeDepthRank(bfalse)
    , m_runtimeDepthRank(0)
    , iconBgSkipCine(nullptr)
    , iconProcessSkipCine(nullptr)
{
}

ContextIconsManager::ControllerIconLookup::ControllerIconLookup()
{
    for (auto& icon : buttonIcons)
    {
        icon = String8::emptyString;
    }
    for (auto& icon : axisNegativeIcons)
    {
        icon = String8::emptyString;
    }
    for (auto& icon : axisPositiveIcons)
    {
        icon = String8::emptyString;
    }
}

//------------------------------------------------------------------------------
ContextIconsManager::~ContextIconsManager()
{
    if (TEMPLATEDATABASE && !m_configPath.isEmpty())
    {
        TEMPLATEDATABASE->removeTemplateClient(m_configPath.getStringID(), this);
    }
}

//------------------------------------------------------------------------------
void ContextIconsManager::init()
{
    // load config file
    m_configPath = CONTEXTICONSCONFIG_PATH;
    TEMPLATEDATABASE->addTemplateClient(m_configPath.getStringID(), this);
    const ContextIconsManager_Template* config = TEMPLATEDATABASE->getTemplate<ContextIconsManager_Template>(this, m_configPath);
    if (!config)
    {
        ITF_FATAL_ERROR("Couldn't load context icons config: %s", m_configPath.getString8().cStr());
    }

    // store template
    m_template = const_cast<ContextIconsManager_Template*>(config);

    // safety checks
    bbool ok = btrue;
    if (m_template->getLineIds().size() != ContextIcon_Count)
    {
        ok = bfalse;
    }
    else if (m_template->getButtonNames().size() != InputAdapter::PadType_Count)
    {
        ok = bfalse;
    }
    else
    {
        for (u32 i=0; i<InputAdapter::PadType_Count; ++i)
        {
            if (m_template->getButtonNames()[i].size() != ContextIconType_Count)
            {
                ok = bfalse;
            }
        }
    }
    if (!ok)
    {
        ITF_FATAL_ERROR("Error loading context icons config: %s", m_configPath.getString8().cStr());
    }

    initializeControllerIconLookups();
    initializeDefaultKeyboardIcons();
}

//------------------------------------------------------------------------------
void ContextIconsManager::update()
{
    if (!isVisible())
    {
        return;
    }

    setupMenu();
}

//------------------------------------------------------------------------------
bbool ContextIconsManager::isVisible() const
{
    if (!GAMEMANAGER->isMenusLoaded())
    {
        return bfalse;
    }

    return UI_MENUMANAGER->isDisplayingPriority(GAMEINTERFACE->getContextIconsMenuPriority());
}

//------------------------------------------------------------------------------
void ContextIconsManager::show(
    EContextIcon _left,
    EContextIcon _right,
    EContextIcon _topLeft /*= ContextIcon_Invalid*/,
    EContextIcon _topRight /*= ContextIcon_Invalid*/
    )
{
    if (!GAMEMANAGER->isMenusLoaded())
    {
        return;
    }

    // init/change info
    m_left = _left;
    m_right = _right;
    m_topLeft = _topLeft;
    m_topRight = _topRight;

    // menu visible, don't touch it
    if (isVisible())
    {
        return;
    }

    // show context icons "menu"
    UI_MENUMANAGER->showMenuPage(GAMEINTERFACE->getContextIconsMenuPriority(), m_template->getMenuId(), btrue);

    // setup menu right away (fix for RO-13343)
    setupMenu();
}

//------------------------------------------------------------------------------
void ContextIconsManager::hide()
{
    if (!GAMEMANAGER->isMenusLoaded())
    {
        return;
    }

    // reset info
    m_left = ContextIcon_Invalid;
    m_right = ContextIcon_Invalid;
    m_topLeft = ContextIcon_Invalid;
    m_topRight = ContextIcon_Invalid;

    if (iconBgSkipCine && iconBgSkipCine->GetActor())
    {
        iconBgSkipCine->GetActor()->disable();
    }
    if (iconProcessSkipCine && iconProcessSkipCine->GetActor())
    {
        iconProcessSkipCine->GetActor()->disable();
    }

    // menu hidden, don't touch it
    if (!isVisible())
    {
        return;
    }

    UI_MENUMANAGER->showMenuPage(GAMEINTERFACE->getContextIconsMenuPriority(), m_template->getMenuId(), bfalse);
}

//------------------------------------------------------------------------------
void ContextIconsManager::changeLeftIcon(EContextIcon _icon)
{
    if (isVisible())
    {
        m_left = _icon;
    }
}

//------------------------------------------------------------------------------
void ContextIconsManager::changeRightIcon(EContextIcon _icon)
{
    if (isVisible())
    {
        m_right = _icon;
    }
}

//------------------------------------------------------------------------------
void ContextIconsManager::changeTopLeftIcon(EContextIcon _icon)
{
    if (isVisible())
    {
        m_topLeft = _icon;
    }
}

//------------------------------------------------------------------------------
void ContextIconsManager::changeTopRightIcon(EContextIcon _icon)
{
    if (isVisible())
    {
        m_topRight = _icon;
    }
}

//------------------------------------------------------------------------------
EContextIconType ContextIconsManager::getType(EContextIcon _icon) const
{
    if (_icon<0 || _icon>=ContextIcon_Count)
    {
        return ContextIconType_Invalid;
    }
    return s_iconsTypes[_icon];
}

//------------------------------------------------------------------------------
const EContextIconType ContextIconsManager::s_iconsTypes[ContextIcon_Count] =
{
    // ContextIcon_Select
    ContextIconType_Select,
    // ContextIcon_Continue
    ContextIconType_Select,
    // ContextIcon_Enter
    ContextIconType_Select,
    // ContextIcon_Skip
    ContextIconType_Back,
    // ContextIcon_Back
    ContextIconType_Back,
    // ContextIcon_Retry
    ContextIconType_Back,
    // ContextIcon_Delete
    ContextIconType_Delete,
    // ContextIcon_RayHome
    ContextIconType_RayHome,
    // ContextIcon_Controls
    ContextIconType_Delete,
    // ContextIcon_SpeedUp
    ContextIconType_Select,
};

//------------------------------------------------------------------------------
const String8& ContextIconsManager::getIconStr(u32 _padType, EContextIconType _context)
{
    ITF_ASSERT(_padType<InputAdapter::PadType_Count);
    ITF_ASSERT(_context<ContextIconType_Count && _context!=ContextIconType_Invalid);

    if(_context>=ContextIconType_Count || _context==ContextIconType_Invalid) _context = ContextIconType_Select;
    if(_padType>=InputAdapter::PadType_Count) _padType = 0;

    if(!m_template)
        return String8::emptyString;

    return m_template->getButtonNames()[_padType][_context];
}

String8 ContextIconsManager::resolveIconForPlayer(u32 playerIndex, EContextIconType iconType) const
{
    if (iconType <= ContextIconType_Invalid || iconType >= ContextIconType_Count)
    {
        return String8::emptyString;
    }

    const ControllerType primarySource = INPUT_ADAPTER->getPrimaryInputType(playerIndex);

    if (primarySource == Keyboard)
    {
        String8 keyboardIcon = resolveKeyboardIcon(playerIndex, iconType);
        if (!keyboardIcon.isEmpty() && keyboardIcon != "<N/A>")
        {
            return keyboardIcon;
        }
    }

    InputAdapter::PadType padType = INPUT_ADAPTER->getDebugInputPadType(playerIndex);
    if (padType == InputAdapter::Pad_Invalid)
    {
        padType = InputAdapter::Pad_GenericXBox;
    }

    String8 controllerIcon = resolveControllerIcon(playerIndex, padType, iconType);
    if (!controllerIcon.isEmpty() && controllerIcon != "<N/A>")
    {
        return controllerIcon;
    }

    if (primarySource != Keyboard)
    {
        String8 keyboardIcon = resolveKeyboardIcon(playerIndex, iconType);
        if (!keyboardIcon.isEmpty() && keyboardIcon != "<N/A>")
        {
            return keyboardIcon;
        }
    }

    if (padType != InputAdapter::Pad_GenericXBox)
    {
        controllerIcon = resolveControllerIcon(playerIndex, InputAdapter::Pad_GenericXBox, iconType);
        if (!controllerIcon.isEmpty() && controllerIcon != "<N/A>")
        {
            return controllerIcon;
        }
    }

    return String8::emptyString;
}

String8 ContextIconsManager::resolveKeyboardIcon(u32 playerIndex, EContextIconType iconType) const
{
    if (iconType <= ContextIconType_Invalid || iconType >= ContextIconType_Count)
    {
        return String8::emptyString;
    }

    const InputAdapter::ActionType action = GetActionForContextIcon(iconType);
    for (u32 bindingIndex = 0; bindingIndex < InputAdapter::MAX_BINDINGS_PER_ACTION; ++bindingIndex)
    {
        const ITF::InputValue& binding = INPUT_ADAPTER->GetInputValue(playerIndex, action, bindingIndex);
        if (!IsBindingConfigured(binding))
            continue;
        if (binding.inputType != Keyboard)
            continue;

        String8 icon = KeyboardIconFromVirtualKey(binding.inputValue);
        if (!icon.isEmpty())
        {
            return icon;
        }
    }

    const String8& fallback = m_defaultKeyboardIcons[iconType];
    if (!fallback.isEmpty())
    {
        return fallback;
    }
    return String8::emptyString;
}

String8 ContextIconsManager::resolveControllerIcon(u32 playerIndex, InputAdapter::PadType padType, EContextIconType iconType) const
{
    if (iconType <= ContextIconType_Invalid || iconType >= ContextIconType_Count)
    {
        return String8::emptyString;
    }

    if (padType <= InputAdapter::Pad_Invalid || padType >= InputAdapter::PadType_Count)
    {
        return String8::emptyString;
    }

    const InputAdapter::ActionType action = GetActionForContextIcon(iconType);
    const size_t padIndex = static_cast<size_t>(padType);
    const ControllerIconLookup& lookup = m_controllerIconLookups[padIndex];

    for (u32 bindingIndex = 0; bindingIndex < InputAdapter::MAX_BINDINGS_PER_ACTION; ++bindingIndex)
    {
        const ITF::InputValue& binding = INPUT_ADAPTER->GetInputValue(playerIndex, action, bindingIndex);
        if (!IsBindingConfigured(binding))
            continue;
        if (binding.inputType == Keyboard)
            continue;

        String8 icon = resolveIconForInputValue(lookup, binding);
        if (!icon.isEmpty())
        {
            return icon;
        }
    }

    return String8::emptyString;
}

String8 ContextIconsManager::resolveIconForInputValue(const ControllerIconLookup& lookup, const ITF::InputValue& value) const
{
    if (!IsBindingConfigured(value))
    {
        return String8::emptyString;
    }

    switch (value.inputType)
    {
    case ControllerButton:
        if (value.inputValue < lookup.buttonIcons.size())
        {
            return lookup.buttonIcons[value.inputValue];
        }
        break;
    case ControllerAxis:
        if (value.inputValue < JOY_MAX_AXES)
        {
            if (value.axisPosition == 0)
            {
                const String8& negativeIcon = lookup.axisNegativeIcons[value.inputValue];
                if (!negativeIcon.isEmpty())
                {
                    return negativeIcon;
                }
            }
            else
            {
                const String8& positiveIcon = lookup.axisPositiveIcons[value.inputValue];
                if (!positiveIcon.isEmpty())
                {
                    return positiveIcon;
                }
            }
        }
        break;
    default:
        break;
    }

    return String8::emptyString;
}

String8 ContextIconsManager::fetchTemplateIcon(InputAdapter::PadType padType, EContextIconType iconType) const
{
    if (!m_template)
    {
        return String8::emptyString;
    }

    if (padType <= InputAdapter::Pad_Invalid || padType >= InputAdapter::PadType_Count)
    {
        return String8::emptyString;
    }

    if (iconType <= ContextIconType_Invalid || iconType >= ContextIconType_Count)
    {
        return String8::emptyString;
    }

    const ITF_VECTOR<ITF_VECTOR<String8>>& buttonRows = m_template->getButtonNames();
    const size_t padIndex = static_cast<size_t>(padType);
    if (padIndex >= buttonRows.size())
    {
        return String8::emptyString;
    }

    const ITF_VECTOR<String8>& column = buttonRows[padIndex];
    const size_t iconIndex = static_cast<size_t>(iconType);
    if (iconIndex >= column.size())
    {
        return String8::emptyString;
    }

    return column[iconIndex];
}

void ContextIconsManager::initializeDefaultKeyboardIcons()
{
    for (auto& icon : m_defaultKeyboardIcons)
    {
        icon = String8::emptyString;
    }

    m_defaultKeyboardIcons[ContextIconType_Select] = "[icon:KEYBOARD_ENTER]";
    m_defaultKeyboardIcons[ContextIconType_Back] = "[icon:KEYBOARD_ESCAPE]";
    m_defaultKeyboardIcons[ContextIconType_Delete] = "[icon:KEYBOARD_DELETE]";
    m_defaultKeyboardIcons[ContextIconType_RayHome] = "[icon:KEYBOARD_ESCAPE]";
}

void ContextIconsManager::initializeControllerIconLookups()
{
    for (auto& lookup : m_controllerIconLookups)
    {
        lookup = ControllerIconLookup();
    }

    auto assignButton = [&](InputAdapter::PadType pad, u32 buttonIndex, const char* tag)
    {
        if (!tag)
            return;
        if (pad <= InputAdapter::Pad_Invalid || pad >= InputAdapter::PadType_Count)
            return;
        if (buttonIndex >= JOY_MAX_BUT)
            return;
        const size_t padIndex = static_cast<size_t>(pad);
        m_controllerIconLookups[padIndex].buttonIcons[buttonIndex] = tag;
    };

    auto assignAxis = [&](InputAdapter::PadType pad, u32 axisIndex, const char* negativeTag, const char* positiveTag)
    {
        if (pad <= InputAdapter::Pad_Invalid || pad >= InputAdapter::PadType_Count)
            return;
        if (axisIndex >= JOY_MAX_AXES)
            return;
        const size_t padIndex = static_cast<size_t>(pad);
    m_controllerIconLookups[padIndex].axisNegativeIcons[axisIndex] = negativeTag ? String8(negativeTag) : String8();
    m_controllerIconLookups[padIndex].axisPositiveIcons[axisIndex] = positiveTag ? String8(positiveTag) : String8();
    };

    // Generic Xbox
    assignButton(InputAdapter::Pad_GenericXBox, m_joyButton_A, "[icon:XBOX_BUTTON_A]");
    assignButton(InputAdapter::Pad_GenericXBox, m_joyButton_B, "[icon:XBOX_BUTTON_B]");
    assignButton(InputAdapter::Pad_GenericXBox, m_joyButton_X, "[icon:XBOX_BUTTON_X]");
    assignButton(InputAdapter::Pad_GenericXBox, m_joyButton_Y, "[icon:XBOX_BUTTON_Y]");
    assignButton(InputAdapter::Pad_GenericXBox, m_joyButton_LB, "[icon:XBOX_BUTTON_LB]");
    assignButton(InputAdapter::Pad_GenericXBox, m_joyButton_RB, "[icon:XBOX_BUTTON_RB]");
    assignButton(InputAdapter::Pad_GenericXBox, m_joyButton_Back, "[icon:XBOX_BUTTON_BACK]");
    assignButton(InputAdapter::Pad_GenericXBox, m_joyButton_Start, "[icon:XBOX_BUTTON_START]");
    assignButton(InputAdapter::Pad_GenericXBox, m_joyButton_DPadL, "[icon:XBOX_DPAD]");
    assignButton(InputAdapter::Pad_GenericXBox, m_joyButton_DPadR, "[icon:XBOX_DPAD]");
    assignButton(InputAdapter::Pad_GenericXBox, m_joyButton_DPadU, "[icon:XBOX_DPAD]");
    assignButton(InputAdapter::Pad_GenericXBox, m_joyButton_DPadD, "[icon:XBOX_DPAD]");
    assignButton(InputAdapter::Pad_GenericXBox, m_joyButton_ThumbLeft, "[icon:XBOX_STICK]");
    assignButton(InputAdapter::Pad_GenericXBox, m_joyButton_ThumbRight, "[icon:XBOX_STICK]");
    assignAxis(InputAdapter::Pad_GenericXBox, m_joyTrigger_Left, nullptr, "[icon:XBOX_BUTTON_LT]");
    assignAxis(InputAdapter::Pad_GenericXBox, m_joyTrigger_Right, nullptr, "[icon:XBOX_BUTTON_RT]");

    // Xbox 360
    assignButton(InputAdapter::Pad_X360, m_joyButton_A, "[icon:X360_BUTTON_A]");
    assignButton(InputAdapter::Pad_X360, m_joyButton_B, "[icon:X360_BUTTON_B]");
    assignButton(InputAdapter::Pad_X360, m_joyButton_X, "[icon:X360_BUTTON_X]");
    assignButton(InputAdapter::Pad_X360, m_joyButton_Y, "[icon:X360_BUTTON_Y]");
    assignButton(InputAdapter::Pad_X360, m_joyButton_LB, "[icon:X360_BUTTON_LB]");
    assignButton(InputAdapter::Pad_X360, m_joyButton_RB, "[icon:X360_BUTTON_RB]");
    assignButton(InputAdapter::Pad_X360, m_joyButton_Back, "[icon:X360_BUTTON_BACK]");
    assignButton(InputAdapter::Pad_X360, m_joyButton_Start, "[icon:X360_BUTTON_START]");
    assignButton(InputAdapter::Pad_X360, m_joyButton_DPadL, "[icon:X360_DPAD]");
    assignButton(InputAdapter::Pad_X360, m_joyButton_DPadR, "[icon:X360_DPAD]");
    assignButton(InputAdapter::Pad_X360, m_joyButton_DPadU, "[icon:X360_DPAD]");
    assignButton(InputAdapter::Pad_X360, m_joyButton_DPadD, "[icon:X360_DPAD]");
    assignButton(InputAdapter::Pad_X360, m_joyButton_ThumbLeft, "[icon:X360_STICK]");
    assignButton(InputAdapter::Pad_X360, m_joyButton_ThumbRight, "[icon:X360_STICK]");
    assignAxis(InputAdapter::Pad_X360, m_joyTrigger_Left, nullptr, "[icon:X360_BUTTON_LT]");
    assignAxis(InputAdapter::Pad_X360, m_joyTrigger_Right, nullptr, "[icon:X360_BUTTON_RT]");

    // PlayStation 3
    assignButton(InputAdapter::Pad_PS3, m_joyButton_A, "[icon:PS3_BUTTON_CROSS]");
    assignButton(InputAdapter::Pad_PS3, m_joyButton_B, "[icon:PS3_BUTTON_CIRCLE]");
    assignButton(InputAdapter::Pad_PS3, m_joyButton_X, "[icon:PS3_BUTTON_SQUARE]");
    assignButton(InputAdapter::Pad_PS3, m_joyButton_Y, "[icon:PS3_BUTTON_TRIANGLE]");
    assignButton(InputAdapter::Pad_PS3, m_joyButton_LB, "[icon:PS3_BUTTON_L1]");
    assignButton(InputAdapter::Pad_PS3, m_joyButton_RB, "[icon:PS3_BUTTON_R1]");
    assignButton(InputAdapter::Pad_PS3, m_joyButton_Back, "[icon:PS3_BUTTON_SELECT]");
    assignButton(InputAdapter::Pad_PS3, m_joyButton_Start, "[icon:PS3_BUTTON_START]");
    assignButton(InputAdapter::Pad_PS3, m_joyButton_DPadL, "[icon:PS3_DPAD]");
    assignButton(InputAdapter::Pad_PS3, m_joyButton_DPadR, "[icon:PS3_DPAD]");
    assignButton(InputAdapter::Pad_PS3, m_joyButton_DPadU, "[icon:PS3_DPAD]");
    assignButton(InputAdapter::Pad_PS3, m_joyButton_DPadD, "[icon:PS3_DPAD]");
    assignButton(InputAdapter::Pad_PS3, m_joyButton_ThumbLeft, "[icon:PS3_STICK]");
    assignButton(InputAdapter::Pad_PS3, m_joyButton_ThumbRight, "[icon:PS3_STICK]");
    assignAxis(InputAdapter::Pad_PS3, m_joyTrigger_Left, nullptr, "[icon:PS3_BUTTON_L2]");
    assignAxis(InputAdapter::Pad_PS3, m_joyTrigger_Right, nullptr, "[icon:PS3_BUTTON_R2]");

    // PlayStation 5
    assignButton(InputAdapter::Pad_PS5, m_joyButton_A, "[icon:PS5_BUTTON_CROSS]");
    assignButton(InputAdapter::Pad_PS5, m_joyButton_B, "[icon:PS5_BUTTON_CIRCLE]");
    assignButton(InputAdapter::Pad_PS5, m_joyButton_X, "[icon:PS5_BUTTON_SQUARE]");
    assignButton(InputAdapter::Pad_PS5, m_joyButton_Y, "[icon:PS5_BUTTON_TRIANGLE]");
    assignButton(InputAdapter::Pad_PS5, m_joyButton_LB, "[icon:PS5_BUTTON_L1]");
    assignButton(InputAdapter::Pad_PS5, m_joyButton_RB, "[icon:PS5_BUTTON_R1]");
    assignButton(InputAdapter::Pad_PS5, m_joyButton_Back, "[icon:PS5_BUTTON_SHARE]");
    assignButton(InputAdapter::Pad_PS5, m_joyButton_Start, "[icon:PS5_BUTTON_OPTIONS]");
    assignButton(InputAdapter::Pad_PS5, m_joyButton_DPadL, "[icon:PS5_DPAD_LEFT]");
    assignButton(InputAdapter::Pad_PS5, m_joyButton_DPadR, "[icon:PS5_DPAD_RIGHT]");
    assignButton(InputAdapter::Pad_PS5, m_joyButton_DPadU, "[icon:PS5_DPAD_UP]");
    assignButton(InputAdapter::Pad_PS5, m_joyButton_DPadD, "[icon:PS5_DPAD_DOWN]");
    assignButton(InputAdapter::Pad_PS5, m_joyButton_ThumbLeft, "[icon:PS5_STICK_L]");
    assignButton(InputAdapter::Pad_PS5, m_joyButton_ThumbRight, "[icon:PS5_STICK_R]");
    assignButton(InputAdapter::Pad_PS5, m_joyButton_TouchPad, "[icon:PS5_TOUCHPAD]");
    assignAxis(InputAdapter::Pad_PS5, m_joyTrigger_Left, nullptr, "[icon:PS5_BUTTON_L2]");
    assignAxis(InputAdapter::Pad_PS5, m_joyTrigger_Right, nullptr, "[icon:PS5_BUTTON_R2]");

    // Switch dual/pro controllers (ABXY layout)
    InputAdapter::PadType switchPads[] = { InputAdapter::Pad_NX_Joycon_Dual, InputAdapter::Pad_NX_Pro };
    for (InputAdapter::PadType pad : switchPads)
    {
        assignButton(pad, m_joyButton_A, "[icon:SWITCH_BUTTON_A]");
        assignButton(pad, m_joyButton_B, "[icon:SWITCH_BUTTON_B]");
        assignButton(pad, m_joyButton_X, "[icon:SWITCH_BUTTON_X]");
        assignButton(pad, m_joyButton_Y, "[icon:SWITCH_BUTTON_Y]");
        assignButton(pad, m_joyButton_LB, "[icon:SWITCH_BUTTON_L]");
        assignButton(pad, m_joyButton_RB, "[icon:SWITCH_BUTTON_R]");
        assignButton(pad, m_joyButton_Back, "[icon:SWITCH_BUTTON_MINUS]");
        assignButton(pad, m_joyButton_Start, "[icon:SWITCH_BUTTON_PLUS]");
        assignButton(pad, m_joyButton_DPadL, "[icon:SWITCH_PRO_DPAD_LEFT]");
        assignButton(pad, m_joyButton_DPadR, "[icon:SWITCH_PRO_DPAD_RIGHT]");
        assignButton(pad, m_joyButton_DPadU, "[icon:SWITCH_PRO_DPAD_UP]");
        assignButton(pad, m_joyButton_DPadD, "[icon:SWITCH_PRO_DPAD_DOWN]");
        assignButton(pad, m_joyButton_ThumbLeft, "[icon:SWITCH_BUTTON_LSTICK]");
        assignButton(pad, m_joyButton_ThumbRight, "[icon:SWITCH_BUTTON_RSTICK]");
        assignAxis(pad, m_joyTrigger_Left, nullptr, "[icon:SWITCH_BUTTON_ZL]");
        assignAxis(pad, m_joyTrigger_Right, nullptr, "[icon:SWITCH_BUTTON_ZR]");
    }

    // Switch single joycon
    assignButton(InputAdapter::Pad_NX_Joycon, m_joyButton_DPadR, "[icon:SWITCH_JOYCON_DPAD_RIGHT]");
    assignButton(InputAdapter::Pad_NX_Joycon, m_joyButton_DPadL, "[icon:SWITCH_JOYCON_DPAD_LEFT]");
    assignButton(InputAdapter::Pad_NX_Joycon, m_joyButton_DPadU, "[icon:SWITCH_JOYCON_DPAD_UP]");
    assignButton(InputAdapter::Pad_NX_Joycon, m_joyButton_DPadD, "[icon:SWITCH_JOYCON_DPAD_DOWN]");
    assignButton(InputAdapter::Pad_NX_Joycon, m_joyButton_B, "[icon:SWITCH_BUTTON_SR]");
    assignButton(InputAdapter::Pad_NX_Joycon, m_joyButton_A, "[icon:SWITCH_BUTTON_SL]");
    assignButton(InputAdapter::Pad_NX_Joycon, m_joyButton_LeftSL, "[icon:SWITCH_BUTTON_SL]");
    assignButton(InputAdapter::Pad_NX_Joycon, m_joyButton_LeftSR, "[icon:SWITCH_BUTTON_SR]");
    assignButton(InputAdapter::Pad_NX_Joycon, m_joyButton_RightSL, "[icon:SWITCH_BUTTON_SL]");
    assignButton(InputAdapter::Pad_NX_Joycon, m_joyButton_RightSR, "[icon:SWITCH_BUTTON_SR]");

    // Wii controllers
    assignButton(InputAdapter::Pad_WiiSideWay, m_joyButton_A, "[icon:WII_BUTTON_2]");
    assignButton(InputAdapter::Pad_WiiSideWay, m_joyButton_B, "[icon:WII_BUTTON_1]");
    assignButton(InputAdapter::Pad_WiiSideWay, m_joyButton_Home, "[icon:WII_BUTTON_HOME]");
    assignButton(InputAdapter::Pad_WiiSideWay, m_joyButton_TriggerLeft, "[icon:WII_BUTTON_MINUS]");
    assignButton(InputAdapter::Pad_WiiSideWay, m_joyButton_TriggerRight, "[icon:WII_BUTTON_PLUS]");

    assignButton(InputAdapter::Pad_WiiNunchuk, m_joyButton_A, "[icon:WII_BUTTON_A]");
    assignButton(InputAdapter::Pad_WiiNunchuk, m_joyButton_B, "[icon:WII_BUTTON_B]");
    assignButton(InputAdapter::Pad_WiiNunchuk, m_joyButton_C, "[icon:WII_BUTTON_C]");
    assignButton(InputAdapter::Pad_WiiNunchuk, m_joyButton_Z, "[icon:WII_BUTTON_Z]");
    assignButton(InputAdapter::Pad_WiiNunchuk, m_joyButton_Home, "[icon:WII_BUTTON_HOME]");
    assignButton(InputAdapter::Pad_WiiNunchuk, m_joyButton_TriggerLeft, "[icon:WII_BUTTON_MINUS]");
    assignButton(InputAdapter::Pad_WiiNunchuk, m_joyButton_TriggerRight, "[icon:WII_BUTTON_PLUS]");

    assignButton(InputAdapter::Pad_WiiClassic, m_joyButton_A, "[icon:WII_CLASSIC_BUTTON_B]");
    assignButton(InputAdapter::Pad_WiiClassic, m_joyButton_B, "[icon:WII_CLASSIC_BUTTON_A]");
    assignButton(InputAdapter::Pad_WiiClassic, m_joyButton_X, "[icon:WII_CLASSIC_BUTTON_X]");
    assignButton(InputAdapter::Pad_WiiClassic, m_joyButton_Y, "[icon:WII_CLASSIC_BUTTON_Y]");
    assignButton(InputAdapter::Pad_WiiClassic, m_joyButton_LB, "[icon:WII_CLASSIC_BUTTON_L]");
    assignButton(InputAdapter::Pad_WiiClassic, m_joyButton_RB, "[icon:WII_CLASSIC_BUTTON_R]");
    assignButton(InputAdapter::Pad_WiiClassic, m_joyButton_Back, "[icon:WII_BUTTON_MINUS]");
    assignButton(InputAdapter::Pad_WiiClassic, m_joyButton_Start, "[icon:WII_BUTTON_PLUS]");
    assignAxis(InputAdapter::Pad_WiiClassic, m_joyTrigger_Left, nullptr, "[icon:WII_CLASSIC_BUTTON_ZL]");
    assignAxis(InputAdapter::Pad_WiiClassic, m_joyTrigger_Right, nullptr, "[icon:WII_CLASSIC_BUTTON_ZR]");

    // Vita / CTR fallback to Xbox icons
    assignButton(InputAdapter::Pad_Vita, m_joyButton_A, "[icon:X360_BUTTON_A]");
    assignButton(InputAdapter::Pad_Vita, m_joyButton_B, "[icon:X360_BUTTON_B]");
    assignButton(InputAdapter::Pad_Vita, m_joyButton_X, "[icon:X360_BUTTON_X]");
    assignButton(InputAdapter::Pad_Vita, m_joyButton_Y, "[icon:X360_BUTTON_Y]");
    assignButton(InputAdapter::Pad_Vita, m_joyButton_LB, "[icon:X360_BUTTON_LB]");
    assignButton(InputAdapter::Pad_Vita, m_joyButton_RB, "[icon:X360_BUTTON_RB]");
    assignButton(InputAdapter::Pad_Vita, m_joyButton_Back, "[icon:X360_BUTTON_BACK]");
    assignButton(InputAdapter::Pad_Vita, m_joyButton_Start, "[icon:X360_BUTTON_START]");
    assignAxis(InputAdapter::Pad_Vita, m_joyTrigger_Left, nullptr, "[icon:X360_BUTTON_LT]");
    assignAxis(InputAdapter::Pad_Vita, m_joyTrigger_Right, nullptr, "[icon:X360_BUTTON_RT]");

    assignButton(InputAdapter::Pad_CTR, m_joyButton_A, "[icon:X360_BUTTON_A]");
    assignButton(InputAdapter::Pad_CTR, m_joyButton_B, "[icon:X360_BUTTON_B]");
    assignButton(InputAdapter::Pad_CTR, m_joyButton_X, "[icon:X360_BUTTON_X]");
    assignButton(InputAdapter::Pad_CTR, m_joyButton_Y, "[icon:X360_BUTTON_Y]");
    assignButton(InputAdapter::Pad_CTR, m_joyButton_LB, "[icon:X360_BUTTON_LB]");
    assignButton(InputAdapter::Pad_CTR, m_joyButton_RB, "[icon:X360_BUTTON_RB]");
    assignButton(InputAdapter::Pad_CTR, m_joyButton_Back, "[icon:X360_BUTTON_BACK]");
    assignButton(InputAdapter::Pad_CTR, m_joyButton_Start, "[icon:X360_BUTTON_START]");
    assignAxis(InputAdapter::Pad_CTR, m_joyTrigger_Left, nullptr, "[icon:X360_BUTTON_LT]");
    assignAxis(InputAdapter::Pad_CTR, m_joyTrigger_Right, nullptr, "[icon:X360_BUTTON_RT]");
}

//------------------------------------------------------------------------------
void ContextIconsManager::setupMenu()
{
    // retrieve UI menu pointer
    UIMenu* uiMenu = UI_MENUMANAGER->getMenu(m_template->getMenuId());

    // warn if not found
    ITF_WARNING(
        NULL, uiMenu,
        "Can't find UIMenu '%s'",
        m_template->getMenuId().getDebugString()
        );
    if (!uiMenu)
    {
        return;
    }

    // get left icon UI components
    static const String8 s_icon_left = "icon_left";
    static const String8 s_text_left = "text_left";
    UIComponent* iconLeft = uiMenu->getUIComponentByFriendly(s_icon_left);
    UIComponent* textLeft = uiMenu->getUIComponentByFriendly(s_text_left);
    ITF_WARNING(NULL, iconLeft, "Can't find UIComponent '%s'", s_icon_left.cStr());
    ITF_WARNING(NULL, textLeft, "Can't find UIComponent '%s'", s_text_left.cStr());
    if (!(iconLeft && textLeft))
    {
        return;
    }

    // get right icon UI components
    static const String8 s_text_right = "text_right";
    static const String8 s_icon_right = "icon_right";
    UIComponent* iconRight = uiMenu->getUIComponentByFriendly(s_icon_right);
    UIComponent* textRight = uiMenu->getUIComponentByFriendly(s_text_right);
    ITF_WARNING(NULL, iconRight, "Can't find UIComponent '%s'", s_icon_right.cStr());
    ITF_WARNING(NULL, textRight, "Can't find UIComponent '%s'", s_text_right.cStr());
    if (!(iconRight && textRight))
    {
        return;
    }

    // get top-left icon UI components
    static const String8 s_icon_top_left = "icon_top_left";
    static const String8 s_text_top_left = "text_top_left";
    UIComponent* iconTopLeft = uiMenu->getUIComponentByFriendly(s_icon_top_left);
    UIComponent* textTopLeft = uiMenu->getUIComponentByFriendly(s_text_top_left);
    ITF_WARNING(NULL, iconTopLeft, "Can't find UIComponent '%s'", s_icon_top_left.cStr());
    ITF_WARNING(NULL, textTopLeft, "Can't find UIComponent '%s'", s_text_top_left.cStr());
    if (!(iconTopLeft && textTopLeft))
    {
        return;
    }

    // get top_right icon UI components
    static const String8 s_icon_top_right = "icon_top_right";
    static const String8 s_text_top_right = "text_top_right";
    UIComponent* iconTopRight = uiMenu->getUIComponentByFriendly(s_icon_top_right);
    UIComponent* textTopRight = uiMenu->getUIComponentByFriendly(s_text_top_right);
    ITF_WARNING(NULL, iconTopRight, "Can't find UIComponent '%s'", s_icon_top_right.cStr());
    ITF_WARNING(NULL, textTopRight, "Can't find UIComponent '%s'", s_text_top_right.cStr());
    if (!(iconTopRight && textTopRight))
    {
        return;
    }

    // get skip icon UI components
    static const String8 icon_bg_skip_cine = "icon_bg_skip_cine";
    static const String8 icon_progress_skip_cine = "icon_progress_skip_cine";
    if (!iconBgSkipCine)
        iconBgSkipCine = uiMenu->getUIComponentByID(icon_bg_skip_cine);
    if (!iconProcessSkipCine)
        iconProcessSkipCine = uiMenu->getUIComponentByID(icon_progress_skip_cine);
    if (!(iconBgSkipCine && iconProcessSkipCine))
    {
        return;
    }

    // setup
    setupIcon(m_left, iconLeft, textLeft);
    setupIcon(m_right, iconRight, textRight);
    setupIcon(m_topLeft, iconTopLeft, textTopLeft);
    setupIcon(m_topRight, iconTopRight, textTopRight);
}

//------------------------------------------------------------------------------
void ContextIconsManager::setupIcon(EContextIcon _icon, UIComponent* _iconUI, UIComponent* _textUI)
{
    if (_icon == ContextIcon_Invalid)
    {
        _iconUI->GetActor()->disable();
        _textUI->GetActor()->disable();
    }
    else
    {
        // get index of the player in control
        // NB[LaurentCou]: assumes the main player is always the one concerned,
        // we'll have to adapt this for the pause menu which should be controlled
        // by whoever opened it
        u32 mainPlayer = GAMEMANAGER->getMainIndexPlayer();

        // invert display for valid/back but only *internally*, otherwise the
        // sequence player skipping gets inverted twice...
        // => fix for RO-17619 & RO-17620
        // NOTE[LaurentCou]: this could be my last contribution to Rayman
        // Origins, and that's for the Japanese HD version so:
        // "GO RAYMAN GO, KICK MARIO'S BUTT!!!" :)
        EContextIconType iconType = s_iconsTypes[_icon];
        if (SYSTEM_ADAPTER->isBackAndSelectButtonsInverted())
        {
            if (iconType == ContextIconType_Select)
                iconType = ContextIconType_Back;
            else if (iconType == ContextIconType_Back)
                iconType = ContextIconType_Select;
        }
        if (iconType < 0 || iconType >= ContextIconType_Count)
        {
            iconType = ContextIconType_Select;
        }

        String8 content = resolveIconForPlayer(mainPlayer, iconType);

        if (content.isEmpty() || content == "<N/A>")
        {
            InputAdapter::PadType padType = INPUT_ADAPTER->getDebugInputPadType(mainPlayer);
            if (padType == InputAdapter::Pad_Invalid)
            {
                padType = InputAdapter::Pad_GenericXBox;
            }
            content = fetchTemplateIcon(padType, iconType);

            if (content.isEmpty() || content == "<N/A>")
            {
                content = fetchTemplateIcon(InputAdapter::Pad_GenericXBox, iconType);
            }
        }

        if ((content.isEmpty() || content == "<N/A>") && iconType >= 0 && iconType < ContextIconType_Count)
        {
            const String8& keyboardFallback = m_defaultKeyboardIcons[iconType];
            if (!keyboardFallback.isEmpty())
            {
                content = keyboardFallback;
            }
        }

        if (content.isEmpty())
        {
            content = "<N/A>";
        }

        _iconUI->forceContent(content);

        LocalisationId lineId = m_template->getLineIds()[_icon];
        _textUI->setLineId(lineId);
        _textUI->fillContent();

        _iconUI->GetActor()->enable();
        _textUI->GetActor()->enable();
    }

    // apply runtime deth rank
    if (m_useRuntimeDepthRank)
    {
        _iconUI->setRuntimeDepthRank(m_runtimeDepthRank);
        _textUI->setRuntimeDepthRank(m_runtimeDepthRank);
    }
    else
    {
        _iconUI->resetRuntimeDepthRank();
        _textUI->resetRuntimeDepthRank();
    }
    m_iconData[_icon]={_iconUI,_textUI};
}
void ContextIconsManager::setSkipProgressPercent(f32 _percent)
{
    _percent = Clamp(_percent, 0.f, 100.f);
    if (!iconBgSkipCine || !iconProcessSkipCine)
        return;

    iconBgSkipCine->GetActor()->enable();
    iconBgSkipCine->forceContent("[icon:SKIP_BG]");

    if (_percent > 0.0f)
    {
        iconProcessSkipCine->GetActor()->enable();
        i32 progressFrame = Clamp((i32)_percent, 1, 100);
        String8 progressIconName;
        progressIconName.setTextFormat("[icon:SKIP_PROGRESS_%d]", progressFrame);
        iconProcessSkipCine->forceContent(progressIconName);
    }
    else
    {
        iconProcessSkipCine->GetActor()->disable();
    }

    if (m_useRuntimeDepthRank)
    {
        iconBgSkipCine->setRuntimeDepthRank(m_runtimeDepthRank);
        iconProcessSkipCine->setRuntimeDepthRank(m_runtimeDepthRank);
    }
    else
    {
        iconBgSkipCine->resetRuntimeDepthRank();
        iconProcessSkipCine->resetRuntimeDepthRank();
    }
}
void ContextIconsManager::highlightText(EContextIcon _contextIcon, bbool _isSelected)
{
    auto iconData = m_iconData[_contextIcon];
    if (iconData.textUI != nullptr)
    {
        iconData.textUI->setIsSelected(_isSelected);
    }
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(ContextIconsManager_Template)
BEGIN_SERIALIZATION(ContextIconsManager_Template)
    SERIALIZE_MEMBER("menuId", m_menuId);
    SERIALIZE_CONTAINER("lineIds", m_lineIds);
    SERIALIZE_CONTAINER("buttonNames", m_buttonNames);
END_SERIALIZATION()

//------------------------------------------------------------------------------
ContextIconsManager_Template::ContextIconsManager_Template()
    : Super()
    , m_menuId()
    , m_lineIds()
    , m_buttonNames()
{
}

//------------------------------------------------------------------------------
ContextIconsManager_Template::ContextIconsManager_Template(const Path& _path)
    : Super(_path)
    , m_menuId()
    , m_lineIds()
    , m_buttonNames()
{
}

//------------------------------------------------------------------------------
ContextIconsManager_Template::~ContextIconsManager_Template()
{
}


} // namespace ITF
