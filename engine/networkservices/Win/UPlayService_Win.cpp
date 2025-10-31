#include "precompiled_engine.h"

#ifndef _ITF_UPLAYSERVICE_WIN_H_
#include "engine/NetworkServices/Win/UPlayService_Win.h"
#endif //_ITF_UPLAYSERVICE_WIN_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#if defined(ITF_SUPPORT_UPLAY) && defined(ITF_WINDOWS)
namespace ITF
{
    UPlayService_Win::UPlayService_Win()
        : m_Context(NULL)
        , m_isInitialized(bfalse)
    {
    }

    UPlayService_Win::~UPlayService_Win()
    {
    }

    u32 UPlayService_Win::update()
    {
        int result = 1;
        if (!m_isInitialized)
            return result;

        result = UPC_Update(m_Context);
        if (result != UPC_Result_Ok)
        {
            LOG("[UPlay] UPC_Update unexpected result:%d", result);
        }

        return result;
    }

    int UPlayService_Win::initialize(int _uplayId)
    {
        int result = UPC_Init(UPC_VERSION, _uplayId);

        if (result != UPC_InitResult_Ok)
        {
            LOG("[UPlay] UPC_Init result: %d", result);
            return result;
        }

        UPC_ContextSettings optSettings{
            UPC_ContextSubsystem_Achievement |
            UPC_ContextSubsystem_Product |
            UPC_ContextSubsystem_Storage |
            UPC_ContextSubsystem_Overlay |
            UPC_ContextSubsystem_Friend |
            UPC_ContextSubsystem_Store
        };

#ifdef ITF_UPLAYPC
        // PC-only (no Steam). Needed to use install language api
        optSettings.subsystems |= UPC_ContextSubsystem_Install;
#endif

        m_Context = UPC_ContextCreate(UPC_VERSION, &optSettings);
        if (m_Context == NULL)
        {
            LOG("[UPlay] UPC_ContextCreate failed");
            return UPC_InitResult_Failed + 100;
        }

#ifdef ITF_UPLAYPC  // PC specific api, won't work with STEAM platform
        const char* langCodeUtf8 = NULL;
        result = UPC_InstallLanguageGet_Extended(m_Context, &langCodeUtf8);
        if (result != UPC_Result_Ok || langCodeUtf8 == NULL)
        {
            LOG("[UPlay] UPC_InstallLanguageGet_Extended error: %d", result);
            return UPC_InitResult_Failed + 200;
        }

        m_language.setText(const_cast<char*>(langCodeUtf8));
        LOG("[UPlay] Language is: % s", m_language.cStr());
#endif //ITF_UPLAYPC

        LOG("[UPlay] initialized OK");

        m_isInitialized = btrue;

        return UPC_InitResult_Ok;
    }

    void UPlayService_Win::terminate()
    {
        if (m_isInitialized)
        {
            if (m_Context != NULL)
            {
                UPC_ContextFree(m_Context);
                m_Context = NULL;
            }

            UPC_Uninit();
            m_isInitialized = bfalse;
        }

        LOG("[UPlay] terminated");
    }

    i32 UPlayService_Win::showOverlay()
    {
        int ret = m_isInitialized ?
            UPC_OverlayShow(m_Context, UPC_OverlaySection_Home, nullptr, nullptr) :
            UPC_Result_InternalError;

        return ret;
    }

} // namespace ITF

#endif //ITF_SUPPORT_UPLAY && ITF_WINDOWS

