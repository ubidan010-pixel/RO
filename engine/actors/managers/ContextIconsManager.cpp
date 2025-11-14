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

namespace ITF {

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
        return bfalse;
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

        InputAdapter::PadType padType = INPUT_ADAPTER->getDebugInputPadType(mainPlayer);
        if (padType == InputAdapter::Pad_Invalid) return;
        String8 content = m_template->getButtonNames()[padType][iconType];
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
