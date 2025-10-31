#include "precompiled_core.h"

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#if defined(ITF_WINDOWS)
#include <shellapi.h>
#endif

namespace ITF
{

    String  SystemAdapter::m_szModuleName ="";
    static String  szNotFound = "";

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool   SystemAdapter::isInSystemPause() const 
    {
        return m_isInSystemPause;
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool   SystemAdapter::isInSystemUnPause() const 
    {
        return m_isInSystemUnPause;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void    SystemAdapter::setSystemPauseFlag(bbool _isSystemPause) 
    {
        m_isInSystemPause = _isSystemPause;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void    SystemAdapter::setSystemUnPauseFlag(bbool _isSystemUnPause) 
    {
        m_isInSystemUnPause = _isSystemUnPause;
    }

    void    SystemAdapter::setOSUIActiveState(bbool _isActive)
    {
        m_isOSUIActive = _isActive;
    }

    bbool   SystemAdapter::isOSUIActive() const
    {
        return m_isOSUIActive;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    const String& SystemAdapter::getModuleName()
    {
    #ifdef ITF_WINDOWS
        if (m_szModuleName == "")
        {
            wchar_t strExePath [MAX_PATH];
            const DWORD uSize = GetModuleFileName(NULL, strExePath, MAX_PATH);

            if(uSize == 0)
            {
                ITF_ASSERT(0); // Could not retrieve module name
                m_szModuleName = szNotFound;
            }
            else
                m_szModuleName.setText((u16*)strExePath);
        }

        return m_szModuleName;
    #else

        return szNotFound;
    #endif //ITF_WINDOWS
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    ITF_LANGUAGE SystemAdapter::filterSupportedLanguages(ITF_LANGUAGE _language, const ITF_LANGUAGE *_supportedLanguages, u32 _supportedCount, ITF_LANGUAGE _defaultLanguage)
    {
        if (!_supportedLanguages)
            return _language;
        const ITF_LANGUAGE *languageEnd = _supportedLanguages+_supportedCount;
        for (; _supportedLanguages!=languageEnd; _supportedLanguages++)
            if (*_supportedLanguages == _language)
                return _language;
        return _defaultLanguage;
    }

    void SystemAdapter::findInExplorer(const String& _fileFullpath)
    {
#if defined(ITF_WINDOWS)
        String normFullPath = _fileFullpath;
        normFullPath.replace('/', '\\');

        String exe = "Explorer";
        String command = "/select,"; // http://support.microsoft.com/kb/152457
        command += normFullPath;

        ShellExecute(0, NULL, (LPCWSTR)exe.cStr(), (LPCWSTR)command.cStr(), NULL, SW_SHOW);
#endif // ITF_WINDOWS
    }

    void SystemAdapter::launchFromExplorer( const String& _fileFullpath )
    {
#if defined(ITF_WINDOWS)
        String normFullPath = _fileFullpath;
        normFullPath.replace('/', '\\');

        const int ires = int(uPtr(ShellExecute(0, NULL, (LPCWSTR)normFullPath.cStr(), NULL, NULL, SW_NORMAL)));
        if(ires == SE_ERR_NOASSOC)
        {
            ITF_ERROR("Can't open the file : %ls because there is no association for this extension", normFullPath.cStr());
        }
#endif // ITF_WINDOWS
    }

    FullScreenDialog_ProtectScope::FullScreenDialog_ProtectScope()
        : m_restoreFullScreen(bfalse)
    {
#ifdef ITF_WINDOWS
        if(SYSTEM_ADAPTER->isFullScreenMode())
        {
            m_restoreFullScreen = btrue;

            SYSTEM_ADAPTER->toggleFullScreen();

            HWND wnd = (HWND)SYSTEM_ADAPTER->getMainWindow();
            ::ShowWindow(wnd, SW_HIDE);
            ::UpdateWindow(wnd);

        }
#else
        ITF_UNUSED(m_restoreFullScreen);
#endif // ITF_WINDOWS
    }

    FullScreenDialog_ProtectScope::~FullScreenDialog_ProtectScope()
    {
#ifdef ITF_WINDOWS
        if(m_restoreFullScreen)
        {
            SYSTEM_ADAPTER->toggleFullScreen();

            HWND wnd = (HWND)SYSTEM_ADAPTER->getMainWindow();
            ::ShowWindow(wnd, SW_SHOW);
            ::UpdateWindow(wnd);
        }
#endif // ITF_WINDOWS
    }

    void  SystemAdapter::getLanguageToStr( const ITF_LANGUAGE _language, String& _languageStr )
    {
        switch (_language)
        {

        case ITF_LANGUAGE_ENGLISH:              _languageStr = "ITF_LANGUAGE_ENGLISH";              break;
        case ITF_LANGUAGE_FRENCH:               _languageStr = "ITF_LANGUAGE_FRENCH";               break;
        case ITF_LANGUAGE_JAPANESE:             _languageStr = "ITF_LANGUAGE_JAPANESE";             break;
        case ITF_LANGUAGE_GERMAN:               _languageStr = "ITF_LANGUAGE_GERMAN";               break;
        case ITF_LANGUAGE_SPANISH:              _languageStr = "ITF_LANGUAGE_SPANISH";              break;
        case ITF_LANGUAGE_ITALIAN:              _languageStr = "ITF_LANGUAGE_ITALIAN";              break;
        case ITF_LANGUAGE_KOREAN:               _languageStr = "ITF_LANGUAGE_KOREAN";               break;
        case ITF_LANGUAGE_POLISH:               _languageStr = "ITF_LANGUAGE_POLISH";               break;
        case ITF_LANGUAGE_RUSSIAN:              _languageStr = "ITF_LANGUAGE_RUSSIAN";              break;
        case ITF_LANGUAGE_TRADITIONALCHINESE:   _languageStr = "ITF_LANGUAGE_TRADITIONALCHINESE";   break;
        case ITF_LANGUAGE_PORTUGUESE:           _languageStr = "ITF_LANGUAGE_PORTUGUESE";           break;
        case ITF_LANGUAGE_SIMPLIFIEDCHINESE:    _languageStr = "ITF_LANGUAGE_SIMPLIFIEDCHINESE";    break;
        case ITF_LANGUAGE_DUTCH:                _languageStr = "ITF_LANGUAGE_DUTCH";                break;
        case ITF_LANGUAGE_CZECH:                _languageStr = "ITF_LANGUAGE_CZECH";                break;
        case ITF_LANGUAGE_HUNGARIAN:            _languageStr = "ITF_LANGUAGE_HUNGARIAN";            break;

        case ITF_LANGUAGE_UNKNOWN:
        default:
                                                _languageStr = "ITF_LANGUAGE_UNKNOWN";              break;
        }
    }
} // namespace ITF

