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

#ifndef _ITF_RAY_BASEMENUHELPER_H_
#include "rayman/gameplay/Managers/GameOptions/Ray_BaseMenuHelper.h"
#endif //_ITF_RAY_BASEMENUHELPER_H_

#ifndef _ITF_ZPAD_BASE_H_
#include "engine/zinput/ZPad_Base.h"
#endif //_ITF_ZPAD_BASE_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

namespace ITF {

#define CONTEXTICONSCONFIG_PATH GETPATH_ALIAS("contexticons")

const ContextIconsManager::ButtonIconMapping ContextIconsManager::s_iconMapping_X360 =
    { "X360_BUTTON_A", "X360_BUTTON_B", "X360_BUTTON_X", "X360_BUTTON_Y", "X360_BUTTON_RB", "X360_BUTTON_LB" };

const ContextIconsManager::ButtonIconMapping ContextIconsManager::s_iconMapping_XboxSeries =
    { "XBOX_BUTTON_A", "XBOX_BUTTON_B", "XBOX_BUTTON_X", "XBOX_BUTTON_Y", "X360_BUTTON_RB", "X360_BUTTON_LB" };

const ContextIconsManager::ButtonIconMapping ContextIconsManager::s_iconMapping_PS3 =
    { "PS3_BUTTON_CROSS", "PS3_BUTTON_CIRCLE", "PS3_BUTTON_SQUARE", "PS3_BUTTON_TRIANGLE", "PS3_BUTTON_R1", "PS3_BUTTON_R2" };

const ContextIconsManager::ButtonIconMapping ContextIconsManager::s_iconMapping_PS5 =
    { "PS5_BUTTON_CROSS", "PS5_BUTTON_CIRCLE", "PS5_BUTTON_SQUARE", "PS5_BUTTON_TRIANGLE", "PS5_BUTTON_R1", "PS5_BUTTON_R2" };

const ContextIconsManager::ButtonIconMapping ContextIconsManager::s_iconMapping_Switch_Joycon =
    { "SWITCH_JOYCON_DPAD_DOWN", "SWITCH_JOYCON_DPAD_RIGHT", "SWITCH_JOYCON_DPAD_LEFT", "SWITCH_JOYCON_DPAD_UP", "SWITCH_BUTTON_R", "SWITCH_BUTTON_L" };

const ContextIconsManager::ButtonIconMapping ContextIconsManager::s_iconMapping_Switch_Pro =
    { "SWITCH_BUTTON_B", "SWITCH_BUTTON_A", "SWITCH_BUTTON_Y", "SWITCH_BUTTON_X", "SWITCH_BUTTON_R", "SWITCH_BUTTON_L" };

const ContextIconsManager::ButtonIconMapping ContextIconsManager::s_iconMapping_Wii_Sideway =
    { "WII_BUTTON_2", "WII_BUTTON_1", "WII_BUTTON_MINUS", nullptr };

const ContextIconsManager::ButtonIconMapping ContextIconsManager::s_iconMapping_Wii_Nunchuk =
    { "WII_BUTTON_A", "WII_BUTTON_B", "WII_BUTTON_MINUS", nullptr };

const ContextIconsManager::ButtonIconMapping ContextIconsManager::s_iconMapping_Wii_Classic =
    { "WII_CLASSIC_BUTTON_B", "WII_CLASSIC_BUTTON_A", "WII_CLASSIC_BUTTON_Y", "WII_CLASSIC_BUTTON_X" };

const ContextIconsManager::ButtonIconMapping ContextIconsManager::s_iconMapping_Keyboard =
    { "KEYBOARD_SPACE", "KEYBOARD_BACKSPACE", "KEYBOARD_S", "KEYBOARD_W", "KEYBOARD_E", "KEYBOARD_ENTER" };

const ContextIconsManager::ButtonIconMapping ContextIconsManager::s_iconMapping_Fallback =
    { "BUTTON_SOUTH", "BUTTON_EAST", "BUTTON_WEST", "BUTTON_NORTH", "BUTTON_R_SHOULDER", "BUTTON_L_SHOULDER" };

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
    if (!GAMEMANAGER->isMenusLoaded() || !canShowIcon())
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
    for (auto iconData : m_iconData) {
        if (iconData.second.textUI != nullptr)
        {
            iconData.second.textUI->setIsSelected(false);
        }
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
bbool ContextIconsManager::canShowIcon() const
{
    if (!UI_MENUMANAGER)
    {
        return bfalse;
    }

    UIMenuManager::MenuItemActionListener* listener = UI_MENUMANAGER->getCurrentMenuActionListener();
    if (!listener)
    {
        return btrue;
    }
    return !listener->isBaseMenuHelper();
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
const EPhysicalButtonAction ContextIconsManager::s_iconToActionMap[ContextIcon_Count] =
{
    // ContextIcon_Select
    PhysicalButtonAction_Confirm,
    // ContextIcon_Continue
    PhysicalButtonAction_Confirm,
    // ContextIcon_Enter
    PhysicalButtonAction_Confirm,
    // ContextIcon_Skip
    PhysicalButtonAction_Back,
    // ContextIcon_Back
    PhysicalButtonAction_Back,
    // ContextIcon_Retry
    PhysicalButtonAction_Back,
    // ContextIcon_Delete
    PhysicalButtonAction_Delete,
    // ContextIcon_RayHome
    PhysicalButtonAction_HomeMap,
    // ContextIcon_Controls
    PhysicalButtonAction_Delete,
    // ContextIcon_SpeedUp
    PhysicalButtonAction_Confirm,
    // ContextIcon_Upload
    PhysicalButtonAction_Upload, // Upload (RT)
    //ContextIcon_Download
    PhysicalButtonAction_Download, // Download (LT)
    // ContextIcon_Confirm
    PhysicalButtonAction_Delete,
    // ContextIcon_Cancel
    PhysicalButtonAction_Back,
};

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
    // ContextIcon_Upload
    ContextIconType_Upload,
    // ContextIcon_Download
    ContextIconType_Download,
    // ContextIcon_Confirm
    ContextIconType_Confirm,
    // ContextIcon_Cancel
    ContextIconType_Cancel,
};

//------------------------------------------------------------------------------
EPhysicalButtonAction ContextIconsManager::getPhysicalButtonAction(EContextIcon _icon) const
{
    if (_icon < 0 || _icon >= ContextIcon_Count)
    {
        return PhysicalButtonAction_Invalid;
    }
    return s_iconToActionMap[_icon];
}

//------------------------------------------------------------------------------
u32 ContextIconsManager::getPhysicalButtonForAction(EPhysicalButtonAction _action, bbool _isInverted) const
{
    // Default mapping: South = Confirm, East = Back
    // When inverted (NX/OUNCE/PS5_SIEJ): South = Back, East = Confirm

    switch (_action)
    {
    case PhysicalButtonAction_Confirm:
        return _isInverted ? ZPad_Base::BUTTON_FACE_EAST : ZPad_Base::BUTTON_FACE_SOUTH;

    case PhysicalButtonAction_Back:
        return _isInverted ? ZPad_Base::BUTTON_FACE_SOUTH : ZPad_Base::BUTTON_FACE_EAST;

    case PhysicalButtonAction_Delete:
    case PhysicalButtonAction_HomeMap:
        return ZPad_Base::BUTTON_FACE_WEST;
    case PhysicalButtonAction_Upload:
        return ZPad_Base::BUTTON_R_SHOULDER;
    case PhysicalButtonAction_Download:
        return ZPad_Base::BUTTON_L_SHOULDER;
        
    default:
        return ZPad_Base::BUTTON_FACE_SOUTH;
    }
}

//------------------------------------------------------------------------------
const ContextIconsManager::ButtonIconMapping& ContextIconsManager::getButtonMappingForPadType(InputAdapter::PadType _padType)
{
    switch (_padType)
    {
    case InputAdapter::Pad_X360:
        return s_iconMapping_X360;
    case InputAdapter::Pad_GenericXBox:
        return s_iconMapping_XboxSeries;
    case InputAdapter::Pad_PS3:
        return s_iconMapping_PS3;
    case InputAdapter::Pad_PS4:
    case InputAdapter::Pad_PS5:
        return s_iconMapping_PS5;
    case InputAdapter::Pad_NX_Joycon:
        return s_iconMapping_Switch_Joycon;
    case InputAdapter::Pad_NX_Joycon_Dual:
    case InputAdapter::Pad_NX_Pro:
        return s_iconMapping_Switch_Pro;
    case InputAdapter::Pad_WiiSideWay:
        return s_iconMapping_Wii_Sideway;
    case InputAdapter::Pad_WiiNunchuk:
        return s_iconMapping_Wii_Nunchuk;
    case InputAdapter::Pad_WiiClassic:
        return s_iconMapping_Wii_Classic;
    case InputAdapter::Pad_Keyboard:
        return s_iconMapping_Keyboard;
    default:
        return s_iconMapping_Fallback;
    }
}

//------------------------------------------------------------------------------
String8 ContextIconsManager::formatIconTag(const char* _iconName)
{
    if (!_iconName || !_iconName[0])
        return String8("<N/A>");

    return String8("[icon:") + _iconName + "]";
}

//------------------------------------------------------------------------------
String8 ContextIconsManager::getIconNameForPhysicalButton(u32 _physicalButton, InputAdapter::PadType _padType, bbool _isInverted) const
{
    const ButtonIconMapping& mapping = getButtonMappingForPadType(_padType);
    const char* iconName = nullptr;
    if (_physicalButton == ZPad_Base::BUTTON_FACE_SOUTH)
        iconName = mapping.south;
    else if (_physicalButton == ZPad_Base::BUTTON_FACE_EAST)
        iconName = mapping.east;
    else if (_physicalButton == ZPad_Base::BUTTON_FACE_WEST)
        iconName = mapping.west;
    else if (_physicalButton == ZPad_Base::BUTTON_FACE_NORTH)
        iconName = mapping.north;
    else if (_physicalButton == ZPad_Base::BUTTON_R_SHOULDER)
        iconName = mapping.r_shoulder;
    else if (_physicalButton == ZPad_Base::BUTTON_L_SHOULDER)
        iconName = mapping.l_shoulder;
    
    return formatIconTag(iconName);
}

//------------------------------------------------------------------------------
const String8& ContextIconsManager::getIconStr(u32 _padType, EContextIconType _context)
{
    ITF_ASSERT(_padType<InputAdapter::PadType_Count);
    ITF_ASSERT(_context<ContextIconType_Count && _context!=ContextIconType_Invalid);

    if(_context>=ContextIconType_Count || _context==ContextIconType_Invalid) _context = ContextIconType_Select;
    if(_padType>=InputAdapter::PadType_Count) _padType = 0;

    if(!m_template)
        return String8::emptyString;

    EPhysicalButtonAction action = PhysicalButtonAction_Invalid;
    switch (_context)
    {
    case ContextIconType_Select:
        action = PhysicalButtonAction_Confirm;
        break;
    case ContextIconType_Back:
        action = PhysicalButtonAction_Back;
        break;
    case ContextIconType_Delete:
        action = PhysicalButtonAction_Delete;
        break;
    case ContextIconType_RayHome:
        action = PhysicalButtonAction_HomeMap;
        break;
    default:
        action = PhysicalButtonAction_Confirm;
        break;
    }
    bbool isInverted = SYSTEM_ADAPTER->isBackAndSelectButtonsInverted();
    u32 physicalButton = getPhysicalButtonForAction(action, isInverted);
    InputAdapter::PadType padType = static_cast<InputAdapter::PadType>(_padType);
    static String8 s_iconStrCache;
    s_iconStrCache = getIconNameForPhysicalButton(physicalButton, padType, isInverted);
    return s_iconStrCache;
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
        // Get the player in control (main player)
        u32 mainPlayer = GAMEMANAGER->getMainIndexPlayer();

        // Get controller type
        InputAdapter::PadType padType = INPUT_ADAPTER->getLastUsedPadType(mainPlayer);
        if (padType == InputAdapter::Pad_Invalid) return;
        EPhysicalButtonAction action = getPhysicalButtonAction(_icon);
        if (action == PhysicalButtonAction_Invalid) return;
        bbool isInverted = SYSTEM_ADAPTER->isBackAndSelectButtonsInverted();
        u32 physicalButton = getPhysicalButtonForAction(action, isInverted);
        String8 content = getIconNameForPhysicalButton(physicalButton, padType, isInverted);
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
