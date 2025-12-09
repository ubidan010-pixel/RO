#ifndef _ITF_UITEXTMANAGER_H_
#define _ITF_UITEXTMANAGER_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_EVENTLISTENER_H_
#include "engine/events/IEventListener.h"
#endif //_ITF_EVENTLISTENER_H_

#ifndef _ITF_FONT_H_
#include "engine/display/font.h"
#endif //_ITF_FONT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

namespace ITF
{
    class XMLFile;
    class UITextManager_Template;

    ///////////////////////////////////////////////////////////////////////////////////////////
    class UITextManager final : public IEventListener
    {
    public:

        // NB[LaurentCou]: this is *sensible* stuff; handle with care
        enum
        {
                                      // ranks 10 to 1 in menus (*reverse order*)
            menusDepthRank      = 10, // rank 0 in menus
            fadeScreenDepthRank = 11, // screen fade, stuff to render behind go *below*
            scoreTextDepthRank  = 12, // rank -2 in menus
            scoreAnimDepthRank  = 13, // rank -3 in menus
            textsDepthRank      = 14, // rank -4 in menus
            animDepthRank       = 15, // rank -5 in menus
                                      // post effects like the paper effect in WM
        };

        UITextManager();
        ~UITextManager();


        void                        init                (       );
        void                        onEvent             (Event* _event);
        void                        update              (       );

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Add a variable to the table of variables
        /// @param _varFriendly friendly of the variable
        /// @param _varValue value of the variable
        void            addVariable (const StringID &_varFriendly, const String &_varValue);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Remove a variable of the table of variables
        /// @param _varFriendly friendly of the variable
        void            removeVariable (const StringID &_varFriendly);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Update the variable in the table of variables
        /// @param _varFriendly friendly of the variable
        /// @param _newValue new value of the variable
        void            updateVariable (const StringID &_varFriendly, const String &_newValue);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Get the value of the variable
        /// @param _varFriendly friendly of the variable
        const String&   getVariableValue (const StringID &_varFriendly) const;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Fill m_textPaths with all the paths of the texts
        /// @param _path path
        void            fillTextPaths           (const String& _path);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Call the parser on the files
        void            loadTextFiles           (       );

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Fade out or fade in the text
        /// @param _deltaTime
        /// @param _fadeDuration duration of the fade in/out
        /// @param _isFadeOut true if the text fade out, false if it fade in
        void            textFade                (f32 _deltaTime, f32 _fadeDuration, bbool _isFadeOut);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Add a text to the text tab
        /// @param _friendly friendly of the text
        /// @param _text text
        void            addText (StringID _friendly, const String8 &_text);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Return the highest size used with this text
        /// @param _textInfo text tab
        f32             getHighestSize (const ITF_VECTOR<TextInfo>& _textInfo);

        StringID        getTextLanguage () const { return m_textLanguage; }
        bbool           getTextByFriendly (StringID _friendly, TextInfo& _textInfo, u32 &_indexStart);
        bbool           isReady () const { return m_isReady; }
        void            setCurrentColor (u32 _color) { m_currentColor = _color; }
        void            setCurrentSize (f32 _size) { m_currentSize = _size; }
        f32             getCurrentSize () const { return m_currentSize; }
        void            setDefaultSize (f32 _size) { m_defaultSize = _size; }
        f32             getDefaultSize () const { return m_defaultSize; }
        f32             getTextBoxScale () const { return m_textBoxScale; }
        bbool           needsScale () const { return m_needsScale; }
        i32             getMenusDepthRank() const { return menusDepthRank; }
        i32             getFadeScreenDepthRank() const {return fadeScreenDepthRank;}
        i32             getScoreAnimDepthRank() const {return scoreAnimDepthRank;}
        i32             getScoreTextDepthRank() const {return scoreTextDepthRank;}
        i32             getTextsDepthRank () const { return textsDepthRank; }
        i32             getAnimDepthRank() const { return animDepthRank; }
        f32             getIconSize() const;
        f32             getIconYOffset() const;
        f32             getIconXOffset() const;
        f32             getMoveIconScale() const;
        f32             getMoveIconYOffset() const;

        bbool           getIconInfo(const String8& _tag, bbool& _isButton, i32& _index, bbool& _isGpeExtra) const;
        bbool           getSkipIconInfo(const String8& _tag, i32& _index) const;
        bbool           getMenuLogoInfo(const String8& _tag, i32& _index) const;
        Texture*        getButtonTexture();
        Texture*        getButtonTexture(const String8& _iconName) const;
        Texture*        getButtonTextureByType(i32 _controllerType) const;
        EControllerType getControllerTypeFromIcon(const String8& _iconName) const;
        Texture*        getGpeTexture(bbool _useExtra = bfalse);
        Texture*        getSkipIconsTexture();
        Texture*        getMenuLogosTexture();
        String8         GetIconForAction(u32 _playerIndex, u32 _action);
        String8         GetIconFromActionTag(const String8& _tagContent);
        bbool           GetMoveIconCluster(u32 _playerIndex, MoveIconCluster& _outCluster);
        static bbool    IsMoveClusterAction(const String8& _tagContent);

        ITF_INLINE f32  getMenuLogoScale() const { return 0.28f; }

    protected:

        void            loadIcons();
        void            unloadIcons();
        Texture*        getControllerTexture(ControllerIconSlot slot) const;

        static String8  getIconFallback(const String8& _iconName); // a fallback system to translate some icon names between platforms

        UITextManager_Template* m_template;

        Path            m_configPath;

        // icons
        ResourceID      m_iconsGroup;
        ResourceID      m_controllerButtonTextureIds[IconSlot_Count];
        ResourceID      m_gpeTextureId;
        ResourceID      m_gpeExtraTextureId;
        ResourceID      m_skipIconsTextureId;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Convert an ITF_Language to a StringID
        /// @param _language ITF_Language language to convert
        StringID                    ITFLanguageToStringID (ITF_LANGUAGE _language);

        StringID                    m_textLanguage;

        typedef ITF_MAP<StringID,String>   TextVariable;
        TextVariable                m_variables;

        ITF_VECTOR<TextInfo>        m_textTabWithVarTags; // Var tags haven't been parsed
        ITF_VECTOR<String>          m_textPaths;
        u32                         m_currentColor;
        f32                         m_currentSize;
        f32                         m_defaultSize;
        bbool                       m_isReady;
        u32                         m_screenHeight;
        f32                         m_textBoxScale;
        bbool                       m_needsScale;

        f32                         m_iconSize;
        f32                         m_iconYOffset;
        f32                         m_iconXOffset;
        typedef ITF_MAP<String8, i32> IconMap;
        Path                        m_buttonPath;
        IconMap                     m_controllerButtonMaps[IconSlot_Count];
        Path                        m_gpePath;
        IconMap                     m_gpeMap;
        Path                        m_gpeExtraPath;
        IconMap                     m_gpeExtraMap;
        IconMap                     m_skipIconsMap;
        Path                        m_menuLogosPath;
        IconMap                     m_menuLogosMap;
        ResourceID                  m_menuLogosTextureId;
    };


    ///////////////////////////////////////////////////////////////////////////////////////////
    class UITextManager_Template : public TemplateObj
    {
        DECLARE_OBJECT_CHILD_RTTI(UITextManager_Template, TemplateObj,2855701572);
        DECLARE_SERIALIZE()

    public:
        UITextManager_Template()
            : Super()
            , m_iconSize(32.0f)
            , m_iconYOffset(0.0f)
            , m_iconXOffset(0.f)
            , m_moveIconScale(0.7f)
            , m_moveIconYOffset(0.0f)
            , m_gpePath()
            , m_gpeNames()
            , m_gpeExtraPath()
            , m_gpeExtraNames()
            {}
        UITextManager_Template(const Path& _path)
            : Super(_path)
            , m_iconSize(32.0f)
            , m_iconYOffset(0.0f)
            , m_iconXOffset(0.f)
            , m_moveIconScale(0.7f)
            , m_moveIconYOffset(0.0f)
            , m_gpePath()
            , m_gpeNames()
            , m_gpeExtraPath()
            , m_gpeExtraNames()
            {}

        ITF_INLINE f32 getIconSize() const { return m_iconSize; }
        ITF_INLINE f32 getIconYOffset() const { return m_iconYOffset; }
        ITF_INLINE f32 getIconXOffset() const { return m_iconXOffset; }
        ITF_INLINE f32 getMoveIconScale() const { return m_moveIconScale; }
        ITF_INLINE f32 getMoveIconYOffset() const { return m_moveIconYOffset; }
        ITF_INLINE const ITF_VECTOR<String8>& getButtonNamesForSlot(ControllerIconSlot slot) const
        {
            switch(slot)
            {
            case IconSlot_Wii: return m_buttonNamesWII;
            case IconSlot_PS3: return m_buttonNamesPS3;
            case IconSlot_PS5: return m_buttonNamesPS5;
            case IconSlot_Switch: return m_buttonNamesNX;
            case IconSlot_Ounce: return m_buttonNamesOunce;
            case IconSlot_X360: return m_buttonNamesX360;
            case IconSlot_Vita: return m_buttonNamesVita;
            case IconSlot_CTR: return m_buttonNamesCTR;
            case IconSlot_XboxSeries: return m_buttonNamesXboxSeries;
            case IconSlot_Keyboard: return m_buttonNamesKeyboard;
            default: return m_buttonNamesX360;
            }
        }
        ITF_INLINE const Path& getGpePath() const { return m_gpePath; }
        ITF_INLINE const ITF_VECTOR<String8>& getGpeNames() const { return m_gpeNames; }
        ITF_INLINE const Path& getGpeExtraPath() const { return m_gpeExtraPath; }
        ITF_INLINE const ITF_VECTOR<String8>& getGpeExtraNames() const { return m_gpeExtraNames; }

    private:
        f32 m_iconSize;
        f32 m_iconYOffset;
        f32 m_iconXOffset;
        f32 m_moveIconScale;
        f32 m_moveIconYOffset;
        Path m_buttonPath;
        ITF_VECTOR<String8> m_buttonNamesWII;
        ITF_VECTOR<String8> m_buttonNamesPS3;
        ITF_VECTOR<String8> m_buttonNamesPS5;
        ITF_VECTOR<String8> m_buttonNamesNX;
        ITF_VECTOR<String8> m_buttonNamesOunce;
        ITF_VECTOR<String8> m_buttonNamesX360;
        ITF_VECTOR<String8> m_buttonNamesVita;
        ITF_VECTOR<String8> m_buttonNamesCTR;
        ITF_VECTOR<String8> m_buttonNamesXboxSeries;
        ITF_VECTOR<String8> m_buttonNamesKeyboard;
        Path m_gpePath;
        ITF_VECTOR<String8> m_gpeNames;
        Path m_gpeExtraPath;
        ITF_VECTOR<String8> m_gpeExtraNames;
    };

}
#endif // _ITF_UITEXTMANAGER_H_
