#ifndef _ITF_SYSTEMADAPTER_
#define _ITF_SYSTEMADAPTER_

#pragma once

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/vec2d.h"
#endif //_ITF_VEC2D_H_


namespace ITF
{


enum ITF_MESSAGEBOX
{
    ITF_MSG_OK              = 1,
    ITF_MSG_ICONERROR       = 2,
    ITF_MSG_ICONWARNING     = 4,
    ITF_MSG_YESNOCANCEL     = 8,
    ITF_MSG_ICONQUESTION    = 16,
    ITF_MSG_YESNO           = 32,
    ENUM_FORCE_SIZE_32_0
};

enum ITF_MESSAGEBOX_RESULT
{
    ITF_IDYES       = 0,
    ITF_IDNO        = 1,
    ITF_IDCANCEL    = 2,
    ENUM_FORCE_SIZE_32(1)
};

enum ITF_LANGUAGE
{
    ITF_LANGUAGE_ENGLISH=0,
    ITF_LANGUAGE_FRENCH,
    ITF_LANGUAGE_ITALIAN,
    ITF_LANGUAGE_GERMAN,
    ITF_LANGUAGE_SPANISH,
    ITF_LANGUAGE_PORTUGUESE,
    ITF_LANGUAGE_JAPANESE,
    ITF_LANGUAGE_TRADITIONALCHINESE,
    ITF_LANGUAGE_SIMPLIFIEDCHINESE,
    ITF_LANGUAGE_RUSSIAN,
    ITF_LANGUAGE_POLISH,
    ITF_LANGUAGE_KOREAN,
    ITF_LANGUAGE_DUTCH,
    ITF_LANGUAGE_CZECH,
    ITF_LANGUAGE_HUNGARIAN,
    ITF_LANGUAGE_UNKNOWN,
    ITF_LANGUAGE_COUNT,
    ENUM_FORCE_SIZE_32(2)
};

struct CursorIcon
{
    enum Type
    {
        Normal,
        Scale_X,
        Scale_Y,
        Scale_XY,
        Move,
        Rotate,
		Count,
		ENUM_FORCE_SIZE_32(3)
    };
};

enum ITF_TERRITORY
{
    ITF_TERRITORY_AMERICA=0,
    ITF_TERRITORY_EUROPE,
    ITF_TERRITORY_OTHER,
    ITF_TERRITORY_UNKNOWN
};

class SystemAdapter : public TemplateSingleton<SystemAdapter>
{
public:
    SystemAdapter():m_bAutoFocus(btrue),m_ClipBoardFormat(0), m_isInSystemPause(bfalse), m_isOSUIActive(bfalse), m_currentLanguage(ITF_LANGUAGE_ENGLISH), m_isFullTitle(bfalse), m_exitRequested(bfalse), m_isInSilentMode(false), m_processDumpOnCrash(false) {};
    virtual ~SystemAdapter()        {}
    virtual bbool   initialize      (   )    {return bfalse;};
    virtual bbool   openGraphics    (   int /*_width*/, int /*_height*/, int /*_alphaBits*/, int /*_depthBits*/, int /*_stencilBits*/, bbool /*_fullscreen */= bfalse, bbool /*_mouseCursor */= bfalse, const String& /*_name */= "", bbool _waitVBL=btrue )  {ITF_ASSERT(0);return bfalse;}
    virtual void    terminate       (   )    {}
    virtual void    present         (   )    {}
    virtual void    update          (   bbool _minimal  )                   {}
    virtual bbool   gameUpdateAllowed    (   )                                   { return btrue; }
    virtual bbool   getWindowSize   (   int& /*_width*/, int& /*_height*/)  { return bfalse; }
    virtual bbool   getWindowPos    (   int& _x, int& _y    )               { _x = 0; _y = 0; return bfalse; }

    virtual bbool   exitRequested   (   )const                              { return m_exitRequested; }
    virtual void    resetExitRequest(   )                                   { m_exitRequested = bfalse; }
    virtual void    requestExit     (   )                                   { m_exitRequested = btrue;}

    virtual void    setSize           (int /*_w*/, int /*_h */) {};
    virtual void    setPositionAndSize(int /*_x*/, int /*_y */,int /*_w*/,int /*_h*/) {};

    virtual void    addTextToTitle(const String& _extraInfo) {};

    // Unit is seconds
    virtual void    timerStart      (   )    {}
    virtual void    timerStop       (   )    {}
    virtual f64     getTime         (   ) const {return 0.0f;}
    virtual f64     getElapsedTime  (   ) { return 0.0f; }
    virtual f32     getfPs          (   ) { return 0.0f; }
    virtual bbool   isAllowResize() const { return btrue; }

    virtual void    setWindowCloseCb(void* /*_func*/) { }
    virtual void    saveWindowPos(const String& /*_filepathAbsolute*/) { }
    virtual void*   registerWindow(u32 /*_screenWidth*/,u32 /*_screenHeight*/, const String& /*_szName*/,const String& /*_szFilename*/) { return NULL;}

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// returns btrue if trial mode
    virtual bbool isTrialMode() const {return bfalse;}

    // returns btrue if it's a hdd boot game
    virtual bbool isHDDMode() const {return bfalse;}

    virtual bbool isBackAndSelectButtonsInverted() const { return bfalse; }

    ///////////////////////////////////////////////////////////////////////////////////////////
    virtual ITF_TERRITORY getSystemTerritory()  const { return ITF_TERRITORY_UNKNOWN; }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Get system language.
    /// Given a list of the languages supported by the game, return the best choice.
    //  Platform specific policy on language choice can be applied here.
    /// @param _supportedLanguages array of supported languages, or NULL if all languages are supported
    /// @param _supportedCount item count in _supportedLanguages
    /// @param _defaultLanguage if the language is not supported, return this one
    virtual ITF_LANGUAGE getSystemLanguage(const ITF_LANGUAGE * _supportedLanguages, u32 _supportedCount, ITF_LANGUAGE _defaultLanguage) const {return ITF_LANGUAGE_ENGLISH;}

    static void  getLanguageToStr( const ITF_LANGUAGE _language, String& _strLanguage );
    static i32   getLanguageCount () { return (i32)ITF_LANGUAGE_COUNT; }

    virtual String  userName()const = 0;
    virtual String  dateTime() const = 0;
    virtual void    getTime(Time & /*_time*/) const {ITF_ASSERT(0);};
    virtual f64     getEllaspedFromStart (  ) {ITF_ASSERT(0); return 0.;}

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// On some system, there is a system pause. For ex, on console, the system pause is when system UI covers the screen.
    bbool                   isInSystemPause() const;
    bbool                   isInSystemUnPause() const;
    void                    setSystemPauseFlag(bbool _isSystemPause);
    void                    setSystemUnPauseFlag(bbool _isSystemUnPause);

    void                    setOSUIActiveState(bbool _isActive);
    bbool                   isOSUIActive() const;

    virtual bbool           isFullScreenMode() {return btrue;}

    virtual void            queryPerformanceCounter(LARGE_INTEGER *counter) const = 0;
    virtual void            queryPerformanceFrequency(LARGE_INTEGER *TicksPerSecond) const = 0;

    void                    setAutofocus(bbool _bautofocus) {m_bAutoFocus = _bautofocus;}

    //must be move in the win32 adapter
    void*                   getMainWindow()                 {return m_hwnd;}
    void                    setMainWindow(void* ptr)        {m_hwnd =ptr;}

    virtual void            registerClipBoardFormat()       {};
    u32                     getClipboardFormat()            {return m_ClipBoardFormat;}

    static const String&    getModuleName();
    virtual const  char*    getPlatformName() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// toggleShowTitleBar
    virtual void            toggleShowTitleBar() {}
    virtual void            toggleFullScreen()   {}

    virtual void setCursorIcon(CursorIcon::Type /*_icon*/)const {}

	// for ITF_MESSAGEBOX_RESULT
	// please keep in mind, this is used for "TOOLS", it's not meant has a default choise for user
	// so when giving this value to a message box, please keep in mind, that we want the "worst case" scenario for tools to detect errors
    virtual i32 messageBox( const String& /*_title*/, const String& /*_msg*/, u32 /*_type*/,ITF_MESSAGEBOX_RESULT /*silentResult*/ ) = 0;

    virtual void *allocatePhysicalMemory(u32 /*_size*/, u32 /*_alignment*/) {ITF_ASSERT(0);return NULL;};
    virtual void freePhysicalMemory(void * /*_pointer*/) {};

    static void findInExplorer(const String& _fileFullpath);
    static void launchFromExplorer(const String& _fileFullpath);

    void            updateMousePos(i32 _x, i32 _y)  { m_mousePos.m_x = f32(_x); m_mousePos.m_y = f32(_y);  }
    const Vec2d&    getMousePos()const              { return m_mousePos; }

	void			setSilentMode(bbool isSilent)   { m_isInSilentMode = isSilent; }
	bbool			IsSilent()						{ return m_isInSilentMode; }

    virtual void    launchExe(const char* /*_exeName*/, const char* /*_params*/ = NULL, bbool /*_avoidDoublons*/ = bfalse, bbool /*_focus*/ = btrue) {};

    void            SetProcessDumpOnCrash(bbool On)  { m_processDumpOnCrash = On;   }
    bbool           bProcessDumpOnCrash()            { return m_processDumpOnCrash; }

#ifndef ITF_FINAL
    bbool           hasDebugInput() const            { return !m_debugInput.isEmpty(); }
    const char*     getDebugInput() const            { return m_debugInput.cStr(); }
    void            setDebugInput(const char* _val)  { m_debugInput = _val; }
#endif

    ///returns a filtered language, given an array of supported languages
    static ITF_LANGUAGE filterSupportedLanguages(ITF_LANGUAGE _language, const ITF_LANGUAGE *_supportedLanguages, u32 _supportedCount, ITF_LANGUAGE _defaultLanguage);

    virtual void hideStartupSplashScreen() {}

protected :

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief  Gets the module name (sample : "Techno.exe")
    /// @return The module name.
    virtual String  recordModuleName() {return "";};

    void*           m_hwnd = nullptr;
    static String   m_szModuleName; //allow to get a module name with out system adapter allocated

    u32             m_ClipBoardFormat;
    bbool           m_bAutoFocus;
    bbool           m_isInSystemPause;
    bbool           m_isInSystemUnPause;
    bbool           m_isOSUIActive;
    ITF_LANGUAGE    m_currentLanguage;
    bbool           m_isFullTitle;
    bbool           m_exitRequested;
    Vec2d           m_mousePos;
	bbool			m_isInSilentMode;
    bbool           m_processDumpOnCrash;

#ifndef ITF_FINAL
    String8         m_debugInput;
#endif
};


#ifndef ITF_FINAL
    enum TraceTypes
    {
        TraceType_Invalid = 0,
        TraceType_Malloc,
        TraceType_Free,
        TraceType_Count,
        ENUM_FORCE_SIZE_32(TraceTypes)
    };

    typedef struct MemAllocTrace
    {
        MemAllocTrace() : m_traceType((u32)TraceType_Malloc) {};

        u32  m_traceType;
        u32 m_amount;
        u32 m_address;
    } MemAllocTrace;

    typedef struct MemFreeTrace
    {
        MemFreeTrace() : m_traceType((u32)TraceType_Free) {};

        u32  m_traceType;
        u32 m_address;
    } MemFreeTrace;
#endif // ITF_FINAL

#define SYSTEM_ADAPTER SystemAdapter::getptr()

class FullScreenDialog_ProtectScope
{
public:
    FullScreenDialog_ProtectScope();
    ~FullScreenDialog_ProtectScope();

private:
    bbool m_restoreFullScreen;
};

} // namespace ITF



#endif //_ITF_SYSTEMADAPTER_
