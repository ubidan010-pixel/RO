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


namespace ITF
{
    #define UITEXTCONFIG_PATH GETPATH_ALIAS("uitextconfig")

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
    , m_buttonMap()
    , m_gpePath()
    , m_gpeMap()
    {
        m_currentColor = Color::black().getAsU32();
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

        // copy button icons info
        const ITF_VECTOR<String8>& buttonNames = m_template->getButtonNames();
        i32 buttonNamesCount = i32(buttonNames.size());
        for (i32 i=0; i<buttonNamesCount; ++i)
        {
            m_buttonMap[buttonNames[i]] = i;
        }

        // copy GPE icons info
        m_gpePath = m_template->getGpePath();
        const ITF_VECTOR<String8>& gpeNames = m_template->getGpeNames();
        i32 gpeNamesCount = i32(gpeNames.size());
        for (i32 i=0; i<gpeNamesCount; ++i)
        {
            m_gpeMap[gpeNames[i]] = i;
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

            // add pad buttons icons
            m_buttonPath = GAMEMANAGER->getIconsBtnPath();
            m_buttonTextureId = ((ResourceGroup*)(m_iconsGroup.getResource()))->addResource(Resource::ResourceType_Texture, m_buttonPath);

            // add GPE icons
            m_gpePath = GAMEMANAGER->getIconsGpePath();
            m_gpeTextureId = ((ResourceGroup*)(m_iconsGroup.getResource()))->addResource(Resource::ResourceType_Texture, m_gpePath);

            Path skipIconsPath = GAMEMANAGER->getIconsSkipPath();
            m_skipIconsTextureId = ((ResourceGroup*)(m_iconsGroup.getResource()))->addResource(Resource::ResourceType_Texture, skipIconsPath);

            m_menuLogosPath = GAMEMANAGER->getMenuLogosPath();
            m_menuLogosTextureId = ((ResourceGroup*)(m_iconsGroup.getResource()))->addResource(Resource::ResourceType_Texture, m_menuLogosPath);

            // done
            RESOURCE_MANAGER->addUserToResourcePhysicalData(m_iconsGroup);
        }
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
    bbool UITextManager::getIconInfo(const String8& _tag, bbool& _isButton, i32& _index) const
    {
        // search in buttons map
        IconMap::const_iterator it = m_buttonMap.find(_tag);
        if (it != m_buttonMap.end())
        {
            _isButton = btrue;
            _index = it->second;
            return btrue;
        }

        // search in GPEs map
        it = m_gpeMap.find(_tag);
        if (it != m_gpeMap.end())
        {
            _isButton = bfalse;
            _index = it->second;
            return btrue;
        }

        // not found, try a platform specific fallback
        it = m_buttonMap.find(getIconFallback(_tag));
        if (it != m_buttonMap.end())
        {
            _isButton = btrue;
            _index = it->second;
            return btrue;
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
        return (Texture*)m_buttonTextureId.getResource();
    }

    //////////////////////////////////////////////////////////////////////////
    Texture *UITextManager::getGpeTexture()
    {
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
        SERIALIZE_CONTAINER("buttonNames", m_buttonNames);
        SERIALIZE_MEMBER("gpePath", m_gpePath);
        SERIALIZE_CONTAINER("gpeNames", m_gpeNames);
    END_SERIALIZATION()

}
