#include "precompiled_engine.h"

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_PUGIXMLWRAP_H_
#include "core/XML/PugiXMLWrap.h"
#endif //_ITF_PUGIXMLWRAP_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_ALIASMANAGER_H_
#include "engine/aliasmanager/aliasmanager.h"
#endif //_ITF_ALIASMANAGER_H_

#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_H

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_ZINPUTMANAGER_H_
#include "engine/zinput/ZInputManager.h"
#endif //_ITF_ZINPUTMANAGER_H_

#ifndef _ITF_ZPAD_BASE_H_
#include "engine/zinput/ZPad_Base.h"
#endif //_ITF_ZPAD_BASE_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_


namespace ITF
{
    #define UITEXTCONFIG_PATH GETPATH_ALIAS("uitextconfig")

    struct ControllerIconDescriptor
    {
        ControllerIconSlot slot;
        const char* prefix;
    };

    static const ControllerIconDescriptor g_controllerIconDescriptors[] =
    {
        { IconSlot_Wii,        "WII_"    },
        { IconSlot_PS3,        "PS3_"    },
        { IconSlot_PS5,        "PS5_"    },
        { IconSlot_Vita,       "VITA_"   },
        { IconSlot_CTR,        "CTR_"    },
        { IconSlot_Switch,     "SWITCH_" },
        { IconSlot_Ounce,      "OUNCE_"  },
        { IconSlot_XboxSeries, "XBOX_"   },
        { IconSlot_X360,       "X360_"   },
        { IconSlot_Keyboard,   "KEYBOARD_" },
        { IconSlot_Default,    NULL      },
    };

    static ITF_INLINE u32 ControllerSlotToIndex(ControllerIconSlot _slot)
    {
        ITF_ASSERT(_slot >= 0 && _slot < IconSlot_Count);
        return static_cast<u32>(_slot);
    }

    static ControllerIconSlot ControllerSlotFromIconName(const String8& _name)
    {
        for (u32 i = 0; i < ITF_ARRAY_SIZE(g_controllerIconDescriptors); ++i)
        {
            const ControllerIconDescriptor& descriptor = g_controllerIconDescriptors[i];
            if (descriptor.prefix && _name.startsWith(descriptor.prefix))
            {
                return descriptor.slot;
            }
        }
        return IconSlot_Default;
    }

    static ControllerIconSlot ControllerSlotFromType(i32 _controllerType)
    {
        switch (_controllerType)
        {
        case CONTROLLER_WII:         return IconSlot_Wii;
        case CONTROLLER_PS3:         return IconSlot_PS3;
        case CONTROLLER_PS5:         return IconSlot_PS5;
        case CONTROLLER_VITA:        return IconSlot_Vita;
        case CONTROLLER_CTR:         return IconSlot_CTR;
        case CONTROLLER_SWITCH:      return IconSlot_Switch;
        case CONTROLLER_OUNCE:       return IconSlot_Ounce;
        case CONTROLLER_XBOX:        return IconSlot_XboxSeries;
        case CONTROLLER_X360:        return IconSlot_X360;
        case CONTROLLER_KEYBOARD:    return IconSlot_Keyboard;
        default:                     return IconSlot_Default;
        }
    }

    static EControllerType ControllerTypeFromSlot(ControllerIconSlot _slot)
    {
        switch (_slot)
        {
        case IconSlot_Wii:        return CONTROLLER_WII;
        case IconSlot_PS3:        return CONTROLLER_PS3;
        case IconSlot_PS5:        return CONTROLLER_PS5;
        case IconSlot_Vita:       return CONTROLLER_VITA;
        case IconSlot_CTR:        return CONTROLLER_CTR;
        case IconSlot_Switch:     return CONTROLLER_SWITCH;
        case IconSlot_Ounce:      return CONTROLLER_OUNCE;
        case IconSlot_XboxSeries: return CONTROLLER_XBOX;
        case IconSlot_X360:       return CONTROLLER_X360;
        case IconSlot_Keyboard:   return CONTROLLER_KEYBOARD;
        default:                  return CONTROLLER_DEFAULT;
        }
    }

    static ControllerIconSlot ControllerSlotFromPadType(InputAdapter::PadType _padType)
    {
        switch (_padType)
        {
        case InputAdapter::Pad_WiiSideWay:
        case InputAdapter::Pad_WiiNunchuk:
        case InputAdapter::Pad_WiiClassic:
            return IconSlot_Wii;
        case InputAdapter::Pad_PS3:
            return IconSlot_PS3;
        case InputAdapter::Pad_PS5:
        case InputAdapter::Pad_PS4:
            return IconSlot_PS5;
        case InputAdapter::Pad_Vita:
            return IconSlot_Vita;
        case InputAdapter::Pad_CTR:
            return IconSlot_CTR;
        case InputAdapter::Pad_NX_Joycon:
        case InputAdapter::Pad_NX_Joycon_Dual:
        case InputAdapter::Pad_NX_Pro:
            return IconSlot_Switch;
        case InputAdapter::Pad_Other:
            return IconSlot_Default;
        case InputAdapter::Pad_X360:
            return IconSlot_X360;
        case InputAdapter::Pad_GenericXBox:
            return IconSlot_XboxSeries;
        case InputAdapter::Pad_Keyboard:
            return IconSlot_Keyboard;
        default:
            break;
        }

        return IconSlot_Default;
    }

    static const char* GetKeyboardIconNameFromControl(u32 control)
    {
        switch (control)
        {
        case ZPad_Base::STICK_L_UP:
        case ZPad_Base::DPAD_UP:
            return "KEYBOARD_ARROW_UP";
        case ZPad_Base::STICK_L_DOWN:
        case ZPad_Base::DPAD_DOWN:
            return "KEYBOARD_ARROW_DOWN";
        case ZPad_Base::STICK_L_LEFT:
        case ZPad_Base::DPAD_LEFT:
            return "KEYBOARD_ARROW_LEFT";
        case ZPad_Base::STICK_L_RIGHT:
        case ZPad_Base::DPAD_RIGHT:
            return "KEYBOARD_ARROW_RIGHT";
        case ZPad_Base::TRIGGER_LEFT:
            return "KEYBOARD_CTRL";
        case ZPad_Base::TRIGGER_RIGHT:
            return "KEYBOARD_SHIFT";
        case ZPad_Base::BUTTON_FACE_SOUTH:
            return "KEYBOARD_SPACE";
        case ZPad_Base::BUTTON_FACE_EAST:
            return "KEYBOARD_BACKSPACE";
        case ZPad_Base::BUTTON_FACE_WEST:
            return "KEYBOARD_S";
        case ZPad_Base::BUTTON_FACE_NORTH:
            return "KEYBOARD_W";
        case ZPad_Base::BUTTON_L_SHOULDER:
            return "KEYBOARD_Q";
        case ZPad_Base::BUTTON_R_SHOULDER:
            return "KEYBOARD_E";
        case ZPad_Base::BUTTON_L_THUMB:
            return "KEYBOARD_A";
        case ZPad_Base::BUTTON_R_THUMB:
            return "KEYBOARD_D";
        case ZPad_Base::BUTTON_SELECT:
            return "KEYBOARD_ENTER";
        case ZPad_Base::BUTTON_START:
            return "KEYBOARD_ESCAPE";
        default:
            break;
        }

        return "";
    }


#if defined(ITF_WINDOWS)
    static String8 GetIconNameForKeyboardKey(i32 rawKey)
    {
        switch (rawKey)
        {
        case KEY_UP:    return String8("KEYBOARD_ARROW_UP");
        case KEY_DOWN:  return String8("KEYBOARD_ARROW_DOWN");
        case KEY_LEFT:  return String8("KEYBOARD_ARROW_LEFT");
        case KEY_RIGHT: return String8("KEYBOARD_ARROW_RIGHT");
        case KEY_SPACE: return String8("KEYBOARD_SPACE");
        case KEY_ESC:   return String8("KEYBOARD_ESCAPE");
        case KEY_ENTER: return String8("KEYBOARD_ENTER");
        case KEY_TAB:   return String8("KEYBOARD_TAB");
        case KEY_LSHIFT:
        case KEY_RSHIFT: return String8("KEYBOARD_SHIFT");
        case KEY_LCTRL:
        case KEY_RCTRL:  return String8("KEYBOARD_CTRL");
        case KEY_LALT:
        case KEY_RALT:   return String8("KEYBOARD_ALT");
        case KEY_BACKSPACE: return String8("KEYBOARD_BACKSPACE");
        case KEY_DEL:    return String8("KEYBOARD_DELETE");
        default:
            break;
        }

        if (rawKey >= 'a' && rawKey <= 'z')
        {
            char keyName[16];
            snprintf(keyName, sizeof(keyName), "KEYBOARD_%c", (char)(rawKey - 'a' + 'A'));
            return String8(keyName);
        }

        if (rawKey >= '0' && rawKey <= '9')
        {
            char keyName[16];
            snprintf(keyName, sizeof(keyName), "KEYBOARD_%c", (char)rawKey);
            return String8(keyName);
        }
        return String8("KEYBOARD_UNKNOWN");
    }

    static i32 GetDefaultKeyForAction(u32 action)
    {
        switch (action)
        {
        case ZInputManager::Action_Up:    return KEY_UP;
        case ZInputManager::Action_Down:  return KEY_DOWN;
        case ZInputManager::Action_Left:  return KEY_LEFT;
        case ZInputManager::Action_Right: return KEY_RIGHT;
        case ZInputManager::Action_Jump:  return KEY_SPACE;
        case ZInputManager::Action_Hit:   return 's';
        case ZInputManager::Action_Run:   return KEY_LSHIFT;
        case ZInputManager::Action_Back:  return KEY_BACKSPACE;
        default:
            break;
        }
        return -1;
    }
#endif // ITF_WINDOWS

    ///////////////////////////////////////////////////////////////////////////////////////////
    UITextManager::UITextManager ()
    : m_template(NULL)
    , m_textLanguage (StringID::Invalid)
    , m_isReady(bfalse)
    , m_currentSize(17.f)
    , m_defaultSize(17.f)
    , m_textBoxScale (1.f)
    , m_screenHeight(0)
    , m_needsScale(bfalse)
    , m_iconSize(32.0f)
    , m_iconYOffset(0.0f)
    , m_iconXOffset(0.f)
    , m_buttonPath()
    , m_gpePath()
    , m_gpeMap()
    , m_gpeExtraPath()
    , m_gpeExtraMap()
    {
        m_currentColor = Color::black().getAsU32();
        for (u32 i = 0; i < IconSlot_Count; ++i)
        {
            m_controllerButtonTextureIds[i].invalidateResourceId();
            m_controllerButtonMaps[i].clear();
        }
        m_gpeTextureId.invalidateResourceId();
        m_gpeExtraTextureId.invalidateResourceId();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    UITextManager::~UITextManager ()
    {
        unloadIcons();

        if (EVENTMANAGER)
        {
            EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventSingletonConfigChanged,32858252), this);
            EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventLanguageChanged,1029567175), this);
        }
        if (TEMPLATEDATABASE && !m_configPath.isEmpty())
        {
            TEMPLATEDATABASE->removeTemplateClient(m_configPath.getStringID(), this);
        }
    }

    void UITextManager::init()
    {
        ITF_ASSERT(EVENTMANAGER);
        if (EVENTMANAGER)
        {
            EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventSingletonConfigChanged,32858252),this);
            EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventLanguageChanged,1029567175),this);
        }

        // load config file
        m_configPath = UITEXTCONFIG_PATH;
        TEMPLATEDATABASE->addTemplateClient(m_configPath.getStringID(), this);
        const UITextManager_Template* config = TEMPLATEDATABASE->getTemplate<UITextManager_Template>(this, m_configPath);
        if (!config)
        {
            ITF_FATAL_ERROR("Couldn't load UI text config: %s", m_configPath.getString8().cStr());
        }

        // store template
        m_template = const_cast<UITextManager_Template*>(config);

        const ControllerIconSlot platformSlots[] = {
            IconSlot_Wii, IconSlot_PS3, IconSlot_PS5, IconSlot_Switch, IconSlot_Ounce,
            IconSlot_X360, IconSlot_Vita, IconSlot_CTR, IconSlot_XboxSeries, IconSlot_Keyboard
        };

        for (u32 slotIdx = 0; slotIdx < ITF_ARRAY_SIZE(platformSlots); ++slotIdx)
        {
            ControllerIconSlot slot = platformSlots[slotIdx];
            const ITF_VECTOR<String8>& buttonNames = m_template->getButtonNamesForSlot(slot);

            if (buttonNames.empty())
                continue;

            u32 slotIndex = ControllerSlotToIndex(slot);

            for (i32 i = 0; i < (i32)buttonNames.size(); ++i)
            {
                m_controllerButtonMaps[slotIndex][buttonNames[i]] = i;
            }
        }

        // copy GPE icons info
        m_gpePath = m_template->getGpePath();
        const ITF_VECTOR<String8>& gpeNames = m_template->getGpeNames();
        i32 gpeNamesCount = i32(gpeNames.size());
        for (i32 i=0; i<gpeNamesCount; ++i)
        {
            m_gpeMap[gpeNames[i]] = i;
        }

        m_gpeExtraPath = m_template->getGpeExtraPath();
        const ITF_VECTOR<String8>& gpeExtraNames = m_template->getGpeExtraNames();
        i32 gpeExtraNamesCount = i32(gpeExtraNames.size());
        for (i32 i=0; i<gpeExtraNamesCount; ++i)
        {
            m_gpeExtraMap[gpeExtraNames[i]] = i;
        }

        m_skipIconsMap["SKIP_BG"] = 0;
        for (i32 i = 1; i <= 100; ++i)
        {
            String8 name;
            name.setTextFormat("SKIP_PROGRESS_%d", i);
            m_skipIconsMap[name] = i;
        }

        m_menuLogosMap["UBICONNECT"] = 0;

        loadIcons();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UITextManager::update ()
    {
#ifndef ITF_WII
        if (m_textLanguage == StringID::Invalid)
        {
            m_textLanguage = ITFLanguageToStringID(LOCALISATIONMANAGER->getCurrentLanguage());
            if (m_textLanguage != StringID::Invalid)
                loadTextFiles();

        }

        if (m_screenHeight != GFX_ADAPTER->getScreenHeight())
        {
            m_screenHeight = GFX_ADAPTER->getScreenHeight();
            //m_textBoxScale = (f32)m_screenHeight / 600.0f;
            m_textBoxScale = 1.0f;  // don't know what this hard-coded 600 does, removed for FPP
            m_needsScale = btrue;
        }
        else
        {
            if (m_needsScale)
                m_needsScale = bfalse;
        }
#endif
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UITextManager::onEvent(Event* _event)
    {
        if (EventLanguageChanged* languageChanged = _event->DynamicCast<EventLanguageChanged>(ITF_GET_STRINGID_CRC(EventLanguageChanged,1029567175)))
        {
            m_textLanguage = ITFLanguageToStringID(languageChanged->getNewLanguage());
            m_textTabWithVarTags.clear();
            loadTextFiles();
            return;
        }
#ifdef ITF_SUPPORT_HOTRELOAD
        if (EventSingletonConfigChanged* configChanged = _event->DynamicCast<EventSingletonConfigChanged>(ITF_GET_STRINGID_CRC(EventSingletonConfigChanged,32858252)))
        {
            // reload camera shake config file
            String relativeFile = configChanged->getAbsoluteFilename();
            FilePath::transformToRelative(relativeFile);
            Path relativeFilePath(relativeFile);
            const Path configFile(UITEXTCONFIG_PATH);
            if (relativeFilePath == configFile)
            {
                const UITextManager_Template* config = TEMPLATEDATABASE->reloadTemplate<UITextManager_Template>(configFile);
                if (!config)
                {
                    ITF_FATAL_ERROR("Couldn't reload load UI text config: %s", configFile.getString8().cStr());
                }
                m_template = const_cast<UITextManager_Template*>(config); // store template
            }
        }
#endif //ITF_SUPPORT_HOTRELOAD
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    StringID UITextManager::ITFLanguageToStringID (ITF_LANGUAGE _language)
    {
        switch (_language)
        {
        case ITF_LANGUAGE_ENGLISH:
            return StringID("english");
        case ITF_LANGUAGE_FRENCH:
            return StringID("french");
        case ITF_LANGUAGE_JAPANESE:
            return StringID("japanese");
        case ITF_LANGUAGE_GERMAN:
            return StringID("german");
        case ITF_LANGUAGE_SPANISH:
            return StringID("spanish");
        case ITF_LANGUAGE_ITALIAN:
            return StringID("italian");
        case ITF_LANGUAGE_KOREAN:
            return StringID("korean");
        case ITF_LANGUAGE_TRADITIONALCHINESE:
            return StringID("traditionalchinese");
        case ITF_LANGUAGE_PORTUGUESE:
            return StringID("portuguese");
        case ITF_LANGUAGE_SIMPLIFIEDCHINESE:
            return StringID("simplifiedchinese");
        case ITF_LANGUAGE_POLISH:
            return StringID("polish");
        case ITF_LANGUAGE_RUSSIAN:
            return StringID("russian");
        case ITF_LANGUAGE_DUTCH:
            return StringID("dutch");
        case ITF_LANGUAGE_CZECH:
            return StringID("czech");
        case ITF_LANGUAGE_HUNGARIAN:
            return StringID("hungarian");
        default:
            return StringID("english");
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UITextManager::addVariable (const StringID &_varFriendly, const String& _varValue)
    {
        m_variables[_varFriendly] = _varValue;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UITextManager::removeVariable (const StringID &_varFriendly)
    {
        TextVariable::iterator it = m_variables.find(_varFriendly);
        if (it != m_variables.end())
           m_variables.erase(it);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UITextManager::updateVariable (const StringID &_varFriendly, const String &_newValue)
    {
        TextVariable::iterator it = m_variables.find(_varFriendly);
        if (it != m_variables.end())
            (*it).second = _newValue;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    const String& UITextManager::getVariableValue (const StringID &_varFriendly) const
    {
        TextVariable::const_iterator it = m_variables.find(_varFriendly);
        if (it != m_variables.end())
            return (*it).second;
        else return String::emptyString;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UITextManager::fillTextPaths (const String& _path)
    {
#ifdef ITF_SUPPORT_XML
        PugiXMLWrap XML;

        if (!XML.openFile(_path, bfalse, PugiXMLWrap::RaiseError, bfalse))
            return;

        if (XML.enterElement("root"))
        {
            if (XML.enterElement("TextFile"))
            {
                do
                {
                    m_textPaths.push_back(XML.readAttribute("path"));
                } while (XML.nextSameElement());
            }
        }
#endif //ITF_SUPPORT_XML
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UITextManager::loadTextFiles()
    {

#ifdef ITF_SUPPORT_XML
        for (ITF_VECTOR<String>::iterator iter = m_textPaths.begin(); iter!=m_textPaths.end(); iter++)
            XMLAll::readTextDB((*iter));
#endif //ITF_SUPPORT_XML
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UITextManager::textFade (f32 _deltaTime, f32 _fadeDuration, bbool _isFadeOut)
    {
        // Modify the alpha value of each block
        for (ITF_VECTOR<TextInfo>::iterator iter = m_textTabWithVarTags.begin(); iter!=m_textTabWithVarTags.end(); iter++)
        {
            const f32 fadeValue = _deltaTime / _fadeDuration;
            const u32 initialColor = (*iter).m_color;
            f32 alpha = Color::getAlphaFromU32(initialColor);

            if (_isFadeOut)
                alpha -= fadeValue;
            else
                alpha += fadeValue;

            alpha = Clamp(alpha, 0.f, 1.f);
            (*iter).m_color = ITFCOLOR_SET_A(initialColor, alpha);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UITextManager::addText (StringID _friendly, const String8 &_text)
    {
        TextInfo textInfo (_text, m_currentColor, m_currentSize, _friendly, LocalisationId::Invalid);
        m_textTabWithVarTags.push_back(textInfo);

        if (!m_isReady)
            m_isReady = btrue; // There is at least one text
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool UITextManager::getTextByFriendly (StringID _friendly, TextInfo& _textInfo, u32 &_indexStart)
    {
        u32 tabSize = m_textTabWithVarTags.size();
        for (u32 iter = _indexStart; iter < tabSize; iter++)
        {
            TextInfo textInfo = m_textTabWithVarTags[iter];
            if (textInfo.m_friendly == _friendly)
            {
                _textInfo = textInfo;
                _indexStart = iter + 1;
                return btrue;
            }
        }
        return bfalse; // No more textInfo with this friendly
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    f32 UITextManager::getIconSize() const
    {
        ITF_ASSERT(m_template);
        f32 iconSize = m_template ? m_template->getIconSize() : 32.0f;
        return iconSize;
    }

    f32 UITextManager::getIconYOffset() const
    {
        ITF_ASSERT(m_template);
        f32 iconYOffset = m_template ? m_template->getIconYOffset() : 0.0f;
        return iconYOffset;
    }

    f32 UITextManager::getIconXOffset() const
    {
        ITF_ASSERT(m_template);
        f32 iconXOffset = m_template ? m_template->getIconXOffset() : 0.0f;
        return iconXOffset;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    f32 UITextManager::getHighestSize (const ITF_VECTOR<TextInfo>& _textInfo)
    {
        f32 highestSize = 0.f;
        for (ITF_VECTOR<TextInfo>::const_iterator iter = _textInfo.begin(); iter!=_textInfo.end(); iter++)
        {
            f32 currentSize = (*iter).m_size;
            if (highestSize < currentSize)
                highestSize = currentSize;
        }
        return highestSize;
    }

    /// Buttons:
    void UITextManager::loadIcons()
    {
        if (!m_iconsGroup.isValidResourceId())
        {
            // create resource group
            m_iconsGroup = RESOURCE_MANAGER->newResourceIDFromFile(Resource::ResourceType_ResourceGroup, Path::EmptyPath);

            ResourceGroup* resourceGroup = (ResourceGroup*)(m_iconsGroup.getResource());

            // add pad buttons icons
            m_buttonPath = GAMEMANAGER->getIconsBtnPath();

            for (u32 i = 0; i < ITF_ARRAY_SIZE(g_controllerIconDescriptors); ++i)
            {
                const ControllerIconDescriptor& descriptor = g_controllerIconDescriptors[i];
                const Path& iconPath = GAMEMANAGER->getIconsBtnPath(descriptor.slot);
                if (!iconPath.isEmpty())
                {
                    ResourceID& resourceId = m_controllerButtonTextureIds[ControllerSlotToIndex(descriptor.slot)];
                    resourceId = resourceGroup->addResource(Resource::ResourceType_Texture, iconPath);

                    if (descriptor.slot == IconSlot_Default)
                    {
                        m_buttonPath = iconPath;
                    }
                }
            }

            // add GPE icons
            m_gpePath = GAMEMANAGER->getIconsGpePath();
            m_gpeTextureId = resourceGroup->addResource(Resource::ResourceType_Texture, m_gpePath);

            Path extraGpePath = GAMEMANAGER->getIconsGpeExtraPath();
            if (!extraGpePath.isEmpty())
            {
                m_gpeExtraPath = extraGpePath;
                m_gpeExtraTextureId = resourceGroup->addResource(Resource::ResourceType_Texture, extraGpePath);
            }

            Path skipIconsPath = GAMEMANAGER->getIconsSkipPath();
            m_skipIconsTextureId = resourceGroup->addResource(Resource::ResourceType_Texture, skipIconsPath);

            m_menuLogosPath = GAMEMANAGER->getMenuLogosPath();
            m_menuLogosTextureId = resourceGroup->addResource(Resource::ResourceType_Texture, m_menuLogosPath);

            // done
            RESOURCE_MANAGER->addUserToResourcePhysicalData(m_iconsGroup);
        }
    }

    Texture* UITextManager::getControllerTexture(ControllerIconSlot _slot) const
    {
        const ResourceID& resourceId = m_controllerButtonTextureIds[ControllerSlotToIndex(_slot)];
        return (Texture*)resourceId.getResource();
    }

    void UITextManager::unloadIcons()
    {
        if (m_iconsGroup.isValidResourceId())
        {
            RESOURCE_MANAGER->removeUserToResourcePhysicalData(m_iconsGroup);
            RESOURCE_MANAGER->removeUserToResourceLogicalData(m_iconsGroup);
            m_iconsGroup.invalidateResourceId();
        }
    }

    //////////////////////////////////////////////////////////////////////////
    bbool UITextManager::getIconInfo(const String8& _tag, bbool& _isButton, i32& _index, bbool& _isGpeExtra) const
    {
        IconMap::const_iterator it;
        _isGpeExtra = bfalse;

        ControllerIconSlot slot = ControllerSlotFromIconName(_tag);
        u32 slotIndex = ControllerSlotToIndex(slot);
        it = m_controllerButtonMaps[slotIndex].find(_tag);
        if (it != m_controllerButtonMaps[slotIndex].end())
        {
            _isButton = btrue;
            _index = it->second;
            return btrue;
        }

        if (slot != IconSlot_Default)
        {
            it = m_controllerButtonMaps[ControllerSlotToIndex(IconSlot_Default)].find(_tag);
            if (it != m_controllerButtonMaps[ControllerSlotToIndex(IconSlot_Default)].end())
            {
                _isButton = btrue;
                _index = it->second;
                return btrue;
            }
        }

        // search in GPEs map
        it = m_gpeMap.find(_tag);
        if (it != m_gpeMap.end())
        {
            _isButton = bfalse;
            _index = it->second;
            return btrue;
        }

        it = m_gpeExtraMap.find(_tag);
        if (it != m_gpeExtraMap.end())
        {
            _isButton = bfalse;
            _isGpeExtra = btrue;
            _index = it->second;
            return btrue;
        }

        // not found, try a platform specific fallback
        String8 fallbackName = getIconFallback(_tag);
        if (fallbackName != _tag)
        {
            ControllerIconSlot fallbackSlot = ControllerSlotFromIconName(fallbackName);
            u32 fallbackIndex = ControllerSlotToIndex(fallbackSlot);
            it = m_controllerButtonMaps[fallbackIndex].find(fallbackName);
            if (it != m_controllerButtonMaps[fallbackIndex].end())
            {
                _isButton = btrue;
                _index = it->second;
                return btrue;
            }

            if (fallbackSlot != IconSlot_Default)
            {
                it = m_controllerButtonMaps[ControllerSlotToIndex(IconSlot_Default)].find(fallbackName);
                if (it != m_controllerButtonMaps[ControllerSlotToIndex(IconSlot_Default)].end())
                {
                    _isButton = btrue;
                    _index = it->second;
                    return btrue;
                }
            }
        }

        // not found, warn
        ITF_WARNING_CATEGORY(L10n, NULL, bfalse, "Unknown icon: %s", _tag.cStr());
        _isButton = bfalse;
        _index = 0;
        return bfalse;
    }

    String8 UITextManager::getIconFallback(const String8& _iconName)
    {
    #if defined(ITF_PS5)
        if (_iconName.startsWith("PS3_"))
        {
            if (_iconName == "PS3_BUTTON_SELECT")
                return "PS5_BUTTON_OPTIONS";

            if (_iconName == "PS3_STICK")
                return "PS5_STICK_L";

            return String8("PS5_") + _iconName.substr(strlen("PS3_"));
        }
    #elif defined(ITF_XBOX_SERIES)
        if (_iconName.startsWith("X360_"))
        {
            return String8("XBOX_") + _iconName.substr(u32(strlen("X360_")));
        }
    #elif defined(ITF_NX) || defined(ITF_OUNCE)
        static auto createMap = []()
        {
            auto* pIconMap = new Map<String8, String8>();
            auto& iconMap = *pIconMap;
            // Wii buttons mapping for horizontal joycon mode
            iconMap["WII_BUTTON_1"] = "SWITCH_JOYCON_DPAD_DOWN";
            iconMap["WII_BUTTON_2"] = "SWITCH_JOYCON_DPAD_RIGHT";
            iconMap["WII_BUTTON_B"] = "SWITCH_BUTTON_SR";
            iconMap["WII_DPAD"] = "SWITCH_STICK";
            iconMap["WII_BUTTON_A"] = "SWITCH_PLUS_MINUS";

            // Xbox 360 buttons for Dual joycon and Handheld mode
            iconMap["X360_BUTTON_A"] = "SWITCH_BUTTON_B";
            iconMap["X360_BUTTON_X"] = "SWITCH_BUTTON_Y";
            iconMap["X360_BUTTON_RT"] = "SWITCH_BUTTON_ZR";
            iconMap["X360_STICK"] = "SWITCH_STICK_L";
            iconMap["X360_BUTTON_BACK"] = "SWITCH_BUTTON_MINUS";

            // PS3 buttons for Pro mode
            iconMap["PS3_BUTTON_CROSS"] = "SWITCH_BUTTON_B";
            iconMap["PS3_BUTTON_SQUARE"] = "SWITCH_BUTTON_Y";
            iconMap["PS3_BUTTON_R2"] = "SWITCH_BUTTON_ZR";
            iconMap["PS3_STICK"] = "SWITCH_STICK_L";
            iconMap["PS3_BUTTON_SELECT"] = "SWITCH_BUTTON_MINUS";

            return pIconMap;
        };

        static std::unique_ptr<Map<String8, String8>> pIconNamesMap{ createMap() };
        auto it = pIconNamesMap->find(_iconName);
        if (it != pIconNamesMap->end())
        {
            return it->second;
        }
    #elif defined(ITF_WINDOWS)
        if (_iconName.startsWith("X360_"))
        {
            return String8("XBOX_") + _iconName.substr(u32(strlen("X360_")));
        }
    #endif
        return _iconName;
    }

    //////////////////////////////////////////////////////////////////////////
    Texture *UITextManager::getButtonTexture()
    {
        Texture* texture = getControllerTexture(IconSlot_Default);
        if (!texture)
        {
            texture = getControllerTexture(IconSlot_XboxSeries);
        }
        return texture;
    }

    //////////////////////////////////////////////////////////////////////////
    Texture *UITextManager::getButtonTexture(const String8& _iconName) const
    {
        ControllerIconSlot slot = ControllerSlotFromIconName(_iconName);
        Texture* texture = getControllerTexture(slot);
        if (!texture && slot != IconSlot_Default)
        {
            texture = getControllerTexture(IconSlot_Default);
        }
        return texture;
    }

    //////////////////////////////////////////////////////////////////////////
    Texture *UITextManager::getButtonTextureByType(i32 _controllerType) const
    {
        ControllerIconSlot slot = ControllerSlotFromType(_controllerType);
        Texture* texture = getControllerTexture(slot);
        if (!texture)
        {
            texture = getControllerTexture(IconSlot_Default);
        }
        return texture;
    }

    //////////////////////////////////////////////////////////////////////////
    EControllerType UITextManager::getControllerTypeFromIcon(const String8& _iconName) const
    {
        ControllerIconSlot slot = ControllerSlotFromIconName(_iconName);
        return ControllerTypeFromSlot(slot);
    }

    //////////////////////////////////////////////////////////////////////////
    Texture *UITextManager::getGpeTexture(bbool _useExtra)
    {
        if (_useExtra)
        {
            return (Texture*)m_gpeExtraTextureId.getResource();
        }
        return (Texture*)m_gpeTextureId.getResource();
    }

    //////////////////////////////////////////////////////////////////////////
    Texture *UITextManager::getSkipIconsTexture()
    {
        return (Texture*)m_skipIconsTextureId.getResource();
    }

    //////////////////////////////////////////////////////////////////////////
    bbool UITextManager::getSkipIconInfo(const String8& _tag, i32& _index) const
    {
        IconMap::const_iterator it = m_skipIconsMap.find(_tag);
        if (it != m_skipIconsMap.end())
        {
            _index = it->second;
            return btrue;
        }
        return bfalse;
    }

    //////////////////////////////////////////////////////////////////////////
    bbool UITextManager::getMenuLogoInfo(const String8& _tag, i32& _index) const
    {
        IconMap::const_iterator it = m_menuLogosMap.find(_tag);
        if (it != m_menuLogosMap.end())
        {
            _index = it->second;
            return btrue;
        }
        return bfalse;
    }

    //////////////////////////////////////////////////////////////////////////
    Texture *UITextManager::getMenuLogosTexture()
    {
        return (Texture*)m_menuLogosTextureId.getResource();
    }

    //////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UITextManager_Template)
    BEGIN_SERIALIZATION(UITextManager_Template)
        SERIALIZE_MEMBER("iconSize", m_iconSize);
        SERIALIZE_MEMBER("iconYOffset", m_iconYOffset);
        SERIALIZE_MEMBER("iconXOffset", m_iconXOffset);
        SERIALIZE_CONTAINER("buttonNamesWII", m_buttonNamesWII);
        SERIALIZE_CONTAINER("buttonNamesPS3", m_buttonNamesPS3);
        SERIALIZE_CONTAINER("buttonNamesPS5", m_buttonNamesPS5);
        SERIALIZE_CONTAINER("buttonNamesNX", m_buttonNamesNX);
        SERIALIZE_CONTAINER("buttonNamesOunce", m_buttonNamesOunce);
        SERIALIZE_CONTAINER("buttonNamesX360", m_buttonNamesX360);
        SERIALIZE_CONTAINER("buttonNamesVita", m_buttonNamesVita);
        SERIALIZE_CONTAINER("buttonNamesCTR", m_buttonNamesCTR);
        SERIALIZE_CONTAINER("buttonNamesXboxSeries", m_buttonNamesXboxSeries);
        SERIALIZE_CONTAINER("buttonNamesKeyboard", m_buttonNamesKeyboard);
        SERIALIZE_MEMBER("gpePath", m_gpePath);
        SERIALIZE_CONTAINER("gpeNames", m_gpeNames);
        SERIALIZE_MEMBER("gpeExtraPath", m_gpeExtraPath);
        SERIALIZE_CONTAINER("gpeExtraNames", m_gpeExtraNames);
    END_SERIALIZATION()

    ////////////////////////////////////////////////////////////////////////////
    static const char* GetIconNameFromControl(u32 control, ControllerIconSlot slot)
    {
        switch (slot)
        {
        case IconSlot_PS5:
        case IconSlot_PS3:
            {
                bbool isPS5 = (slot == IconSlot_PS5);
                switch (control)
                {
                case ZPad_Base::BUTTON_FACE_SOUTH: return isPS5 ? "PS5_BUTTON_CROSS" : "PS3_BUTTON_CROSS";
                case ZPad_Base::BUTTON_FACE_EAST:  return isPS5 ? "PS5_BUTTON_CIRCLE" : "PS3_BUTTON_CIRCLE";
                case ZPad_Base::BUTTON_FACE_WEST:  return isPS5 ? "PS5_BUTTON_SQUARE" : "PS3_BUTTON_SQUARE";
                case ZPad_Base::BUTTON_FACE_NORTH: return isPS5 ? "PS5_BUTTON_TRIANGLE" : "PS3_BUTTON_TRIANGLE";
                case ZPad_Base::STICK_LX:
                case ZPad_Base::STICK_LY:          return isPS5 ? "PS5_STICK_L" : "PS3_STICK";
                case ZPad_Base::STICK_L_UP:        return isPS5 ? "PS5_STICK_L_UP" : "PS3_STICK_L_UP";
                case ZPad_Base::STICK_L_DOWN:      return isPS5 ? "PS5_STICK_L_DOWN" : "PS3_STICK_L_DOWN";
                case ZPad_Base::STICK_L_LEFT:      return isPS5 ? "PS5_STICK_L_LEFT" : "PS3_STICK_L_LEFT";
                case ZPad_Base::STICK_L_RIGHT:     return isPS5 ? "PS5_STICK_L_RIGHT" : "PS3_STICK_L_RIGHT";
                case ZPad_Base::STICK_RX:
                case ZPad_Base::STICK_RY:          return isPS5 ? "PS5_STICK_R" : "PS3_STICK_TILT_R";
                case ZPad_Base::STICK_R_UP:        return isPS5 ? "PS5_STICK_R_UP" : "PS3_STICK_R_UP";
                case ZPad_Base::STICK_R_DOWN:      return isPS5 ? "PS5_STICK_R_DOWN" : "PS3_STICK_R_DOWN";
                case ZPad_Base::STICK_R_LEFT:      return isPS5 ? "PS5_STICK_R_LEFT" : "PS3_STICK_R_LEFT";
                case ZPad_Base::STICK_R_RIGHT:     return isPS5 ? "PS5_STICK_R_RIGHT" : "PS3_STICK_R_RIGHT";
                case ZPad_Base::DPAD_UP:           return isPS5 ? "PS5_DPAD_UP" : "PS3_DPAD";
                case ZPad_Base::DPAD_DOWN:         return isPS5 ? "PS5_DPAD_DOWN" : "PS3_DPAD";
                case ZPad_Base::DPAD_LEFT:         return isPS5 ? "PS5_DPAD_LEFT" : "PS3_DPAD";
                case ZPad_Base::DPAD_RIGHT:        return isPS5 ? "PS5_DPAD_RIGHT" : "PS3_DPAD";
                case ZPad_Base::BUTTON_L_THUMB:    return isPS5 ? "PS5_BUTTON_L3" : "PS3_STICK";
                case ZPad_Base::BUTTON_R_THUMB:    return isPS5 ? "PS5_BUTTON_R3" : "PS3_STICK_TILT_R";
                case ZPad_Base::BUTTON_L_SHOULDER: return isPS5 ? "PS5_BUTTON_L1" : "PS3_BUTTON_L1";
                case ZPad_Base::BUTTON_R_SHOULDER: return isPS5 ? "PS5_BUTTON_R1" : "PS3_BUTTON_R1";
                case ZPad_Base::TRIGGER_LEFT:      return isPS5 ? "PS5_BUTTON_L2" : "PS3_BUTTON_L2";
                case ZPad_Base::TRIGGER_RIGHT:     return isPS5 ? "PS5_BUTTON_R2" : "PS3_BUTTON_R2";
                case ZPad_Base::BUTTON_SELECT:     return isPS5 ? "PS5_TOUCHPAD" : "PS3_BUTTON_SELECT";
                case ZPad_Base::BUTTON_START:      return isPS5 ? "PS5_BUTTON_OPTIONS" : "PS3_BUTTON_START";
                }
            }
            break;

        case IconSlot_X360:
        case IconSlot_XboxSeries:
            {
                bbool isSeries = (slot == IconSlot_XboxSeries);
                switch (control)
                {
                case ZPad_Base::BUTTON_FACE_SOUTH: return isSeries ? "XBOX_BUTTON_A" : "X360_BUTTON_A";
                case ZPad_Base::BUTTON_FACE_EAST:  return isSeries ? "XBOX_BUTTON_B" : "X360_BUTTON_B";
                case ZPad_Base::BUTTON_FACE_WEST:  return isSeries ? "XBOX_BUTTON_X" : "X360_BUTTON_X";
                case ZPad_Base::BUTTON_FACE_NORTH: return isSeries ? "XBOX_BUTTON_Y" : "X360_BUTTON_Y";
                case ZPad_Base::STICK_LX:
                case ZPad_Base::STICK_LY:
                case ZPad_Base::BUTTON_L_THUMB:    return isSeries ? "XBOX_STICK_L" : "X360_STICK_L";
                case ZPad_Base::STICK_L_UP:        return isSeries ? "XBOX_STICK_L_UP" : "X360_STICK_L_UP";
                case ZPad_Base::STICK_L_DOWN:      return isSeries ? "XBOX_STICK_L_DOWN" : "X360_STICK_L_DOWN";
                case ZPad_Base::STICK_L_LEFT:      return isSeries ? "XBOX_STICK_L_LEFT" : "X360_STICK_L_LEFT";
                case ZPad_Base::STICK_L_RIGHT:     return isSeries ? "XBOX_STICK_L_RIGHT" : "X360_STICK_L_RIGHT";
                case ZPad_Base::STICK_RX:
                case ZPad_Base::STICK_RY:
                case ZPad_Base::BUTTON_R_THUMB:    return isSeries ? "XBOX_STICK_R" : "X360_STICK_R";
                case ZPad_Base::STICK_R_UP:        return isSeries ? "XBOX_STICK_R_UP" : "X360_STICK_R_UP";
                case ZPad_Base::STICK_R_DOWN:      return isSeries ? "XBOX_STICK_R_DOWN" : "X360_STICK_R_DOWN";
                case ZPad_Base::STICK_R_LEFT:      return isSeries ? "XBOX_STICK_R_LEFT" : "X360_STICK_R_LEFT";
                case ZPad_Base::STICK_R_RIGHT:     return isSeries ? "XBOX_STICK_R_RIGHT" : "X360_STICK_R_RIGHT";
                case ZPad_Base::DPAD_UP:           return isSeries ? "XBOX_DPAD_UP" : "X360_DPAD_UP";
                case ZPad_Base::DPAD_DOWN:         return isSeries ? "XBOX_DPAD_DOWN" : "X360_DPAD_DOWN";
                case ZPad_Base::DPAD_LEFT:         return isSeries ? "XBOX_DPAD_LEFT" : "X360_DPAD_LEFT";
                case ZPad_Base::DPAD_RIGHT:        return isSeries ? "XBOX_DPAD_RIGHT" : "X360_DPAD_RIGHT";
                case ZPad_Base::BUTTON_L_SHOULDER: return isSeries ? "XBOX_BUTTON_LB" : "X360_BUTTON_LB";
                case ZPad_Base::BUTTON_R_SHOULDER: return isSeries ? "XBOX_BUTTON_RB" : "X360_BUTTON_RB";
                case ZPad_Base::TRIGGER_LEFT:      return isSeries ? "XBOX_BUTTON_LT" : "X360_BUTTON_LT";
                case ZPad_Base::TRIGGER_RIGHT:     return isSeries ? "XBOX_BUTTON_RT" : "X360_BUTTON_RT";
                case ZPad_Base::BUTTON_SELECT:     return isSeries ? "XBOX_BUTTON_BACK" : "X360_BUTTON_BACK";
                case ZPad_Base::BUTTON_START:      return isSeries ? "XBOX_BUTTON_START" : "X360_BUTTON_START";
                }
            }
            break;
        case IconSlot_Switch:
        case IconSlot_Ounce:
            {
                switch (control)
                {
                case ZPad_Base::BUTTON_FACE_SOUTH: return "SWITCH_BUTTON_B";
                case ZPad_Base::BUTTON_FACE_EAST:  return "SWITCH_BUTTON_A";
                case ZPad_Base::BUTTON_FACE_WEST:  return "SWITCH_BUTTON_Y";
                case ZPad_Base::BUTTON_FACE_NORTH: return "SWITCH_BUTTON_X";
                case ZPad_Base::STICK_LX:
                case ZPad_Base::STICK_LY:          return "SWITCH_STICK_L";
                case ZPad_Base::STICK_L_UP:        return "SWITCH_STICK_L_UP";
                case ZPad_Base::STICK_L_DOWN:      return "SWITCH_STICK_L_DOWN";
                case ZPad_Base::STICK_L_LEFT:      return "SWITCH_STICK_L_LEFT";
                case ZPad_Base::STICK_L_RIGHT:     return "SWITCH_STICK_L_RIGHT";
                case ZPad_Base::STICK_RX:
                case ZPad_Base::STICK_RY:          return "SWITCH_STICK_R";
                case ZPad_Base::STICK_R_UP:        return "SWITCH_STICK_R_UP";
                case ZPad_Base::STICK_R_DOWN:      return "SWITCH_STICK_R_DOWN";
                case ZPad_Base::STICK_R_LEFT:      return "SWITCH_STICK_R_LEFT";
                case ZPad_Base::STICK_R_RIGHT:     return "SWITCH_STICK_R_RIGHT";
                case ZPad_Base::BUTTON_L_THUMB:    return "SWITCH_BUTTON_LSTICK";
                case ZPad_Base::BUTTON_R_THUMB:    return "SWITCH_BUTTON_RSTICK";
                case ZPad_Base::DPAD_UP:           return "SWITCH_PRO_DPAD_UP";
                case ZPad_Base::DPAD_DOWN:         return "SWITCH_PRO_DPAD_DOWN";
                case ZPad_Base::DPAD_LEFT:         return "SWITCH_PRO_DPAD_LEFT";
                case ZPad_Base::DPAD_RIGHT:        return "SWITCH_PRO_DPAD_RIGHT";
                case ZPad_Base::BUTTON_L_SHOULDER: return "SWITCH_BUTTON_L";
                case ZPad_Base::BUTTON_R_SHOULDER: return "SWITCH_BUTTON_R";
                case ZPad_Base::TRIGGER_LEFT:      return "SWITCH_BUTTON_ZL";
                case ZPad_Base::TRIGGER_RIGHT:     return "SWITCH_BUTTON_ZR";
                case ZPad_Base::BUTTON_SELECT:     return "SWITCH_BUTTON_MINUS";
                case ZPad_Base::BUTTON_START:      return "SWITCH_BUTTON_PLUS";
                }
            }
            break;

        case IconSlot_Keyboard:
            return GetKeyboardIconNameFromControl(control);

        default:
            break;
        }
        return "";
    }

    String8 UITextManager::GetIconForAction(u32 _playerIndex, u32 _action)
    {
        if (!GAMEMANAGER || !GAMEMANAGER->getInputManager())
            return String8::emptyString;
        if (!INPUT_ADAPTER)
        {
            return String8::emptyString;
        }

        InputAdapter::PadType padType = INPUT_ADAPTER->getLastUsedPadType(_playerIndex);
        if (padType == InputAdapter::Pad_Invalid)
        {
            padType = INPUT_ADAPTER->getDebugInputPadType(_playerIndex);
        }

#if defined(ITF_WINDOWS)
        bool useKeyboardIcons;
        PCControlMode pcMode = INPUT_ADAPTER->GetPCControlMode();
        switch (pcMode)
        {
        case PCControlMode_Keyboard:
            useKeyboardIcons = true;
            break;
        case PCControlMode_Controller:
            useKeyboardIcons = false;
            break;
        case PCControlMode_Hybrid:
        default:
            useKeyboardIcons = (INPUT_ADAPTER->getLastUsedInputDevice(_playerIndex) == InputDevice_Keyboard);
            break;
        }

        if (useKeyboardIcons && INPUT_ADAPTER->IsKeyboardMouseEnabled())
        {
            i32 keyCode = GAMEMANAGER->getInputManager()->GetKeyboardKeyFromAction(_playerIndex, (ZInputManager::EGameAction)_action);
            if (keyCode >= 0)
            {
                return GetIconNameForKeyboardKey(keyCode);
            }
            keyCode = GetDefaultKeyForAction(_action);
            if (keyCode >= 0)
            {
                return GetIconNameForKeyboardKey(keyCode);
            }
        }
#endif

        u32 physicalControl = GAMEMANAGER->getInputManager()->GetPhysicalFromAction(_playerIndex, (ZInputManager::EGameAction)_action);
        if (physicalControl == U32_INVALID)
            return String8::emptyString;

        ControllerIconSlot slot = ControllerSlotFromPadType(padType);
        if (slot == IconSlot_Default)
        {
            slot = IconSlot_X360;
        }
        const char* iconName = GetIconNameFromControl(physicalControl, slot);
        if ((!iconName || iconName[0] == '\0') && slot != IconSlot_X360)
        {
            iconName = GetIconNameFromControl(physicalControl, IconSlot_X360);
        }
        if (iconName && iconName[0] != '\0')
        {
            return String8(iconName);
        }

        return String8::emptyString;
    }

    String8 UITextManager::GetIconFromActionTag(const String8& _tagContent)
    {
        const char* rawStr = _tagContent.cStr();
        const char* dashPtr = strchr(rawStr, '-');
        i32 dashPos = dashPtr ? (i32)(dashPtr - rawStr) : (i32)strlen(rawStr);

        char actionBuf[64];
        if (dashPos < 64) {
            strncpy(actionBuf, rawStr, dashPos);
            actionBuf[dashPos] = 0;
        } else {
            return String8::emptyString;
        }
        String8 actionStr(actionBuf);

        u32 playerIndex = 0;
        if (dashPtr && dashPtr[1] != '\0')
        {
            playerIndex = (u32)atoi(dashPtr + 1);
        }
        if (INPUT_ADAPTER)
        {
             if (playerIndex >= JOY_MAX_COUNT) return String8::emptyString;
        }

        u32 action = ZInputManager::Action_Count;
        if (actionStr == "Action_Up") action = ZInputManager::Action_Up;
        else if (actionStr == "Action_Down") action = ZInputManager::Action_Down;
        else if (actionStr == "Action_Left") action = ZInputManager::Action_Left;
        else if (actionStr == "Action_Right") action = ZInputManager::Action_Right;
        else if (actionStr == "Action_Run") action = ZInputManager::Action_Run;
        else if (actionStr == "Action_Jump") action = ZInputManager::Action_Jump;
        else if (actionStr == "Action_Hit") action = ZInputManager::Action_Hit;
        else if (actionStr == "Action_Back") action = ZInputManager::Action_Back;

        if (action == ZInputManager::Action_Count) return String8::emptyString;

        return GetIconForAction(playerIndex, action);
    }

}
