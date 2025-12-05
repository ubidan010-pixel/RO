#include "precompiled_engine.h"

#ifndef _ITF_UPLAYSERVICE_WIN_H_
#include "engine/NetworkServices/Win/UPlayService_Win.h"
#endif //_ITF_UPLAYSERVICE_WIN_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_ACCOUNTADAPTER_H_
#include "core/AdaptersInterfaces/AccountAdapter.h"
#endif

#include "engine/AdaptersInterfaces/TRCManager_Adapter.h"

#if defined(ITF_SUPPORT_UPLAY) && defined(ITF_WINDOWS)
namespace ITF
{
    UPlayService_Win::UPlayService_Win()
        : m_Context(NULL)
        , m_isInitialized(bfalse)
        , m_isOverlayActive(bfalse)
        , m_user(NULL)
        , m_userEmail(NULL)
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

            // stop updates
            m_isInitialized = false;
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
        LOG("[UPlay] Language is: %s", m_language.cStr());
#endif //ITF_UPLAYPC

        result = UPC_EventRegisterHandler(m_Context, UPC_Event_OverlayShown, showOverlayCallback, this);
        result |= UPC_EventRegisterHandler(m_Context, UPC_Event_OverlayHidden, showOverlayCallback, this);

        if (result != UPC_Result_Ok)
        {
            LOG("[UPlay] UPC_EventRegisterHandler failed: %d", result);
            return UPC_InitResult_Failed + 300;
        }

        // async for future getUserName()
        result = UPC_UserGet(m_Context, nullptr, &m_user, onUserGet, this);
        // Don't you wish your result was UPC_Result_Ok like other APIs and not 65536 =^.^=
        if (result <= 0)
        {
            LOG("[UPlay] UPC_UserGet failed: %d", result);
            return UPC_InitResult_Failed + 400;
        }

        LOG("[UPlay] initialized OK");

        m_isInitialized = btrue;

        return UPC_InitResult_Ok;
    }

    void UPlayService_Win::onUserGet(UPC_int32 aResult, void* aData)
    {
        // no op, but UPC_UserGet would fail without cb.
    }

    void UPlayService_Win::showOverlayCallback(UPC_Event* inEvent, void* inData)
    {
        UPlayService_Win* pThis = (UPlayService_Win*)inData;
        if (!inEvent || !pThis)
            return;

        if (inEvent->type == UPC_Event_OverlayShown)
        {
            pThis->m_isOverlayActive = true;
        }
        else if (inEvent->type == UPC_Event_OverlayHidden)
        {
            pThis->m_isOverlayActive = false;
        }
    }

    void UPlayService_Win::terminate()
    {
        if (m_isInitialized)
        {
            UPC_EventUnregisterHandler(m_Context, UPC_Event_OverlayShown);
            UPC_EventUnregisterHandler(m_Context, UPC_Event_OverlayHidden);

            if (m_user != NULL)
                UPC_UserFree(m_Context, m_user);

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

    String8 UPlayService_Win::getToken()
    {
        char* token = nullptr;
        String8 result;

        if (!m_isInitialized)
        {
            LOG("[UPlay] getToken not init");
            return result;
        }

        int ret = UPC_TicketGet_Extended(m_Context, const_cast<const char**>(&token));
        LOG("[UPlay] ticket get: %d, ticket: %s", ret, token);

        if (ret == UPC_Result_Ok)
        {
            result.setText(token);
        }

        return result;
    }

    bbool UPlayService_Win::isClientConnected()
    {
        return m_isInitialized == btrue && (m_Context != NULL);
    }

    bbool UPlayService_Win::isClientOnline()
    {
        UPC_bool isOffline = UPC_TRUE;
        if (!m_isInitialized || m_Context == NULL)
            return btrue;

        int ret = UPC_IsInOfflineMode(m_Context, &isOffline);
        if (ret != UPC_Result_Ok)
        {
            LOG("[UPlay] UPC_IsInOfflineMode failed: %d", ret);
            return btrue;
        }

        return isOffline == UPC_TRUE;
    }

    String8 UPlayService_Win::getUserName()
    {
        return (m_user != NULL) ? String8(m_user->nameUtf8) : String8::emptyString;
    }

    String8 UPlayService_Win::getUserEmail()
    {
        if (!m_userEmail)
        {
            int ret = UPC_EmailGet_Extended(m_Context, &m_userEmail);
            if (ret == UPC_Result_Ok && m_userEmail != NULL)
            {
                return String8(m_userEmail);
            }
            else
            {
                LOG("[UPlay] UPC_EmailGet_Extended failed with %d", ret);
                return String8::emptyString;
            }
        }

        return String8(m_userEmail);
    }

} // namespace ITF

#endif //ITF_SUPPORT_UPLAY && ITF_WINDOWS

