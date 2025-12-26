#ifndef _ITF_CONTEXTICONSMANAGER_H_
#define _ITF_CONTEXTICONSMANAGER_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifndef _ITF_GAMEPLAYTYPES_H_
#include "gameplay/GameplayTypes.h"
#endif //_ITF_GAMEPLAYTYPES_H_

#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_H


namespace ITF {

class ContextIconsManager_Template;
class UIComponent;
class TextureGraphicComponent2D;
enum EPhysicalButtonAction
{
    PhysicalButtonAction_Confirm = 0,  // Usually mapped to BUTTON_FACE_SOUTH
    PhysicalButtonAction_Back,         // Usually mapped to BUTTON_FACE_EAST
    PhysicalButtonAction_Delete,       // Usually mapped to BUTTON_FACE_WEST
    PhysicalButtonAction_HomeMap,      // Usually mapped to BUTTON_FACE_WEST
    PhysicalButtonAction_Upload,
    PhysicalButtonAction_Download,
    PhysicalButtonAction_ResetControlToDefault, // Mapped to BUTTON_FACE_NORTH
    PhysicalButtonAction_Count,
    PhysicalButtonAction_Invalid = -1
};

//------------------------------------------------------------------------------
class ContextIconsManager
{
public:
    ContextIconsManager();
    ~ContextIconsManager();

    void init();
    void update();
    bbool isVisible() const;
    void show(
        EContextIcon _left,
        EContextIcon _right,
        EContextIcon _topLeft = ContextIcon_Invalid,
        EContextIcon _topRight = ContextIcon_Invalid
        );
    void hide();
    void setSkipProgressPercent(f32 _percent);
    void highlightText(EContextIcon _contextIcon, bbool _isSelected);
    void changeLeftIcon(EContextIcon _icon);
    void changeRightIcon(EContextIcon _icon);
    void changeTopLeftIcon(EContextIcon _icon);
    void changeTopRightIcon(EContextIcon _icon);
    EContextIconType getType(EContextIcon _icon) const;

    const String8& getIconStr(u32 _padType, EContextIconType _context);

    ITF_INLINE void setRuntimeDepthRank(u32 _val) { m_useRuntimeDepthRank = btrue; m_runtimeDepthRank = _val; }
    ITF_INLINE void resetRuntimeDepthRank() { m_useRuntimeDepthRank = bfalse; m_runtimeDepthRank = 0; }

private:
    struct IconDataHolder
    {
        UIComponent* iconUI;
        UIComponent* textUI;
    };

    struct ButtonIconMapping
    {
        const char* south;
        const char* east;
        const char* west;
        const char* north;
        const char* r_shoulder;
        const char* l_shoulder;
    };

    Map<EContextIcon,IconDataHolder> m_iconData;
    void setupMenu();
    void setupIcon(EContextIcon _icon, UIComponent* _iconUI, UIComponent* _textUI);

    EPhysicalButtonAction getPhysicalButtonAction(EContextIcon _icon) const;
    u32 getPhysicalButtonForAction(EPhysicalButtonAction _action, bbool _isInverted) const;
    String8 getIconNameForPhysicalButton(u32 _physicalButton, InputAdapter::PadType _padType, bbool _isInverted) const;
    static const ButtonIconMapping& getButtonMappingForPadType(InputAdapter::PadType _padType);
    static String8 formatIconTag(const char* _iconName);
    static const EPhysicalButtonAction s_iconToActionMap[ContextIcon_Count];
    static const EContextIconType s_iconsTypes[ContextIcon_Count];
    static const ButtonIconMapping s_iconMapping_X360;
    static const ButtonIconMapping s_iconMapping_XboxSeries;
    static const ButtonIconMapping s_iconMapping_PS3;
    static const ButtonIconMapping s_iconMapping_PS5;
    static const ButtonIconMapping s_iconMapping_Switch_Joycon;
    static const ButtonIconMapping s_iconMapping_Switch_Pro;
    static const ButtonIconMapping s_iconMapping_Wii_Sideway;
    static const ButtonIconMapping s_iconMapping_Wii_Nunchuk;
    static const ButtonIconMapping s_iconMapping_Wii_Classic;
    static const ButtonIconMapping s_iconMapping_Keyboard;
    static const ButtonIconMapping s_iconMapping_Fallback;

    ContextIconsManager_Template* m_template;
    Path m_configPath;

    EContextIcon m_left;
    EContextIcon m_right;
    EContextIcon m_topLeft;
    EContextIcon m_topRight;

    bbool m_useRuntimeDepthRank;
    i32 m_runtimeDepthRank;
    UIComponent* iconBgSkipCine;
    UIComponent* iconProcessSkipCine;
};

//------------------------------------------------------------------------------
class ContextIconsManager_Template : public TemplateObj
{
    DECLARE_OBJECT_CHILD_RTTI(ContextIconsManager_Template, TemplateObj,543381722);
    DECLARE_SERIALIZE()

public:
    ContextIconsManager_Template();
    ContextIconsManager_Template(const Path& _path);
    virtual ~ContextIconsManager_Template();

    ITF_INLINE const StringID& getMenuId() const { return m_menuId; }
    ITF_INLINE const ITF_VECTOR<LocalisationId>& getLineIds() const { return m_lineIds; }
    ITF_INLINE const ITF_VECTOR<ITF_VECTOR<String8> >& getButtonNames() const { return m_buttonNames; }

private:
    StringID m_menuId;
    ITF_VECTOR<LocalisationId> m_lineIds;
    ITF_VECTOR<ITF_VECTOR<String8> > m_buttonNames;
};

} // namespace ITF

#endif //_ITF_CONTEXTICONSMANAGER_H_
