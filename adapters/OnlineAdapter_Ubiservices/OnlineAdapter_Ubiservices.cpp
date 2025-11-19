#include "precompiled_OnlineAdapter_Ubiservices.h"

#include "adapters/OnlineAdapter_Ubiservices/OnlineAdapter_Ubiservices.h"
#include "adapters/OnlineAdapter_Ubiservices/TrackingService_Ubiservices.h"

#ifdef ITF_SUPPORT_UBISERVICES

#include "core/AdaptersInterfaces/SystemAdapter.h"
#include "core/versioning.h"
#include "core/Config.h"
#include "core/itfstring.h"
#include "core/memory/memory.h"
#include "core/error/ErrorHandler.h"

#include "engine/localisation/LocalisationManager.h"
#include "engine/singleton/Singletons.h"
#include "engine/networkservices/UPlayService.h"

#include <ubiservices/ubiservicesSdk.h>
#include <ubiservices/facade.h>
#include <ubiservices/ubiservices.h>
#include <ubiservices/version.h>
#include <ubiservices/session.h>
#include <ubiservices/localizationHelper.h>
#include <ubiservices/loggingHelper.h>

#include <ubiservices/core/types/string.h>
#include <ubiservices/core/types/containers.h>
#include <ubiservices/core/log/log.h>
#include <ubiservices/core/log/logDevice.h>
#include <ubiservices/core/configs/threadingConfig.h>
#include <ubiservices/core/configs/systemConfig.h>
#include <ubiservices/core/configs/telemetryConfig.h>
#include <ubiservices/core/configs/gameConfig.h>
#include <ubiservices/core/configs/gameConfigEvent.h>

#include <ubiservices/services/parameters/parametersClient.h>
#include <ubiservices/services/parameters/parametersGroupInfo.h>
#include <ubiservices/services/event/types/eventTypeInfo.h>
#include <ubiservices/services/authentication/sessionInfo.h>
#include <ubiservices/services/notification/playerNotificationClient.h>
#include <ubiservices/services/notification/playerNotificationUbiServices.h>

using namespace ubiservices;

namespace ITF
{
    String8 OnlineAdapter_Ubiservices::generateBuildId()
    {
        String8 buildId;

        String8 onlineBuildId = CONFIG->m_onlineBuildId;
        if (!onlineBuildId.isEmpty())
        {
            buildId = onlineBuildId;
        }
        else
        {
            auto platformName = SYSTEM_ADAPTER->getPlatformName();

            buildId.setTextFormat("RO4K_%s_%d%d%d",
                platformName,
                Versioning::getEngineVersion(),
                Versioning::gameVer(),
                Versioning::bundleVer()
            );
        }

        buildId.toUpper();
        return buildId;
    }

    OnlineAdapter_Ubiservices::OnlineAdapter_Ubiservices()
        : m_initialized(false)
    {
        EalLogDllPopulateInterface(m_ealLogInterface);
        EalMemDllPopulateInterface(m_ealMemInterface);

        US_NS::initializeSdk(&m_ealLogInterface, &m_ealMemInterface);

#if defined(ITF_SUPPORT_ONLINETRACKING)
        m_trackingService = newAlloc(mId_OnlineServices, TrackingService_Ubiservices);
#endif
    }

    OnlineAdapter_Ubiservices::~OnlineAdapter_Ubiservices()
    {
#if defined(ITF_SUPPORT_ONLINETRACKING)
        SF_DEL(m_trackingService);
#endif
    }

    void OnlineAdapter_Ubiservices::initialize()
    {
        initializeUbiservices();

#if defined(ITF_SUPPORT_ONLINETRACKING)
        m_trackingService->init();
#endif
    }

    void OnlineAdapter_Ubiservices::update()
    {
        if (!m_initialized)
        {
            return;
        }

#if defined(ITF_SUPPORT_ONLINETRACKING)
        m_trackingService->update();
#endif
    }

    void OnlineAdapter_Ubiservices::terminate()
    {
        if (m_initialized)
        {
#if defined(ITF_SUPPORT_ONLINETRACKING)
            m_trackingService->term();
#endif
            terminateUbiservices();
            m_initialized = false;
        }
    }

    void OnlineAdapter_Ubiservices::initializeUbiservices()
    {
        String8 buildId = generateBuildId();

        US_NS::LoggingHelper::enableMultiLines(true);
        US_NS::LoggingHelper::enableVerbose(true);

        m_sdk = US_NS::UbiservicesSdk::create();

        configureUbiservices(buildId.cStr());

        // Set the locale so Uplay actions are localized nicely
        String8 lang = "en";
        String8 locale = "US";
        //LOCALISATIONMANAGER->getCurrentLocaleString(lang, locale);
        US_NS::LocalizationHelper::setLocaleCode(US_NS::String::formatText("%s-%s", lang.cStr(), locale.cStr()));

        createSession();

        m_initialized = true;
    }

    const char* OnlineAdapter_Ubiservices::GetUSApplicationId()
    {
#ifdef ITF_WINDOWS
        return "1e243814-1c99-4b7d-a2dd-e67c84712812";
#elif ITF_PS5
        return "4b29c383-6e72-48cf-b5bd-f0e8742b5b59";
#elif ITF_NX
        return "681b13a3-ec1a-4ebe-86da-ccf274cb1f59";
#elif ITF_OUNCE
        return "aac7c0a4-ea46-4561-b1c9-84c3bd4c3cfd";
#elif ITF_XBOX_SERIES
        return "12009240-ba52-40f5-aa4f-97b6841060a6";
#endif
    }

    void OnlineAdapter_Ubiservices::configureUbiservices(const ubiservices::String& _buildId)
    {
        const US_NS::ApplicationId applicationId(GetUSApplicationId());
        US_NS::String applicationBuildId(_buildId);
        US_NS::OnlineAccessContext onlineAccessContext = US_NS::OnlineAccessContext::Standard;
        US_NS::ProfilePolicy profilePolicy = US_NS::ProfilePolicy::UseUplayProfile;

        const US_NS::String sku("Full");
        const US_NS::Vector<US_NS::EventTypeInfo> eventTypesForSaveGame;
        const US_NS::StringJson gameStartEventCustomData = "{}";
        const US_NS::String secretKey;
        const US_NS::uint32 saveGameMaxSize = 16 * 1024;
        const US_NS::StringJson initialEventsDefinitionsJson = "{}";
        bool isExtendedEventPlayerSessionEnabled = false;

        US_NS::GameConfigEvent gameConfigEvent = US_NS::GameConfigEvent(
                US_SDK_VERSION_CONST_CHAR
                , sku
                , eventTypesForSaveGame
                , gameStartEventCustomData
                , secretKey
                , saveGameMaxSize
                , initialEventsDefinitionsJson
                , isExtendedEventPlayerSessionEnabled
        );

        US_NS::UplayPCPolicy uplayPCPolicy = US_NS::UplayPCPolicy::UseUplayPC;
        const US_NS::GameConfigConsole gameConfigConsole(uplayPCPolicy);

        const US_NS::GameConfig gameConfig(applicationId
            , applicationBuildId
            , onlineAccessContext
            , gameConfigEvent
            , gameConfigConsole
            , profilePolicy);

        US_NS::ConfigureResult res = m_sdk->configure(gameConfig);
        if (res == ConfigureResult::Success)
        {
            LOG("[Ubiservices] configure OK!");
        }
    }

    void OnlineAdapter_Ubiservices::terminateUbiservices()
    {
        if (m_session)
        {
            closeSession();
        }
    }

    void OnlineAdapter_Ubiservices::createSession()
    {
        // Get/Create the UbiServices session instance (assuming there is only one player).
        // This object must remain alive as long as the player is connected.
        m_session = US_NS::Session::create(*m_sdk);

        // Create a NotificationUbiServices Handler to receive UbiServices notifications
        US_NS::ListenerHandler<PlayerNotificationUbiServices> handler = US_NS::PlayerNotificationsModule::module(*m_session).createNotificationListenerUbiServices();

        // Fill up the player credentials. This step is platform dependent.
        // This object can be deleted once the create session method returns.
        US_NS::AsyncResult<US_NS::Empty> openResult;

        const US_NS::Vector<US_NS::String> parametersGroupApplication;
        const US_NS::Vector<US_NS::String> parametersGroupSpace;

        if (!CONFIG->m_onlineLogin.isEmpty() && !CONFIG->m_onlinePassword.isEmpty())
        {
            const US_NS::PlayerCredentials playerCredentials = US_NS::PlayerCredentials(CONFIG->m_onlineLogin.cStr(), CONFIG->m_onlinePassword.cStr());
            openResult = m_session->open(playerCredentials, parametersGroupApplication, parametersGroupSpace);
        }
#if ITF_SUPPORT_UPLAY
        else
        {
            const US_NS::PlayerCredentials playerCredentials = US_NS::PlayerCredentials(UPLAYSERVICE->getToken().cStr(), CredentialsType::UplayPC);
            openResult = m_session->open(playerCredentials, parametersGroupApplication, parametersGroupSpace);
        }
#endif

        // Simulate a pooling loop waiting for the process to complete.
        while (openResult.isProcessing())
        {
            // (Game loop)
        }

        if (openResult.hasFailed())
        {
            LOG("[Ubiservices] error creating session");
        }

        // Wait for notification confirming if the websocket is connected or not
        while (openResult.hasSucceeded() && !handler.isNotificationAvailable())
        {
            sleep(10);
        }

        const US_NS::PlayerNotificationUbiServices notification = handler.popNotification();
        if (notification.m_type == US_NS::PlayerNotificationUbiServicesType::NotificationThrottled)
        {
            // Handle throttling..
            LOG("[Ubiservices] Websocket creation has been throttled, deleting session!");

            // Set a default wait time before retry. Time in seconds       
            uint32 waitTimeBeforeRetry = 30;

            // Check if time to wait before retry has been provided
            if (!notification.m_content.isEmpty())
            {
                waitTimeBeforeRetry = US_NS::String::convertToInt(notification.m_content);
                LOG("[Ubiservices] Retry websocket in %d", waitTimeBeforeRetry);
            }

            // Start waiting for at least waitTimeBeforeRetry      
            // Delete Session
            AsyncResult<Empty> closeResult = m_session->close();
            // Wait for session deletion
            closeResult.wait();
            // Wait time finished. Try creating session again
        }
        else if (notification.m_type == US_NS::PlayerNotificationUbiServicesType::NotificationConnectionFailed)
        {
            // Handle connection failed..
            LOG("[Ubiservices] Websocket creation has failed, deleting session!");
            AsyncResult<Empty> closeResult = m_session->close();
        }
        else if (notification.m_type == US_NS::PlayerNotificationUbiServicesType::NotificationConnect)
        {
            // You are connected and all is good!
            LOG("[Ubiservices] Session connected");
        }
    }

    void OnlineAdapter_Ubiservices::closeSession()
    {
        if (!m_session)
            return;

        LOG("[Ubiservices] Closing session");

        // Now closing, delete player session
        AsyncResult<Empty> closeResult = m_session->close();
        // Wait for the delete session completion. Wait is a blocking call.
        closeResult.wait();
        if (!closeResult.hasSucceeded())
        {
            // Error
            LOG("[Ubiservices] Error closing session");
        }
    }

} // namespace ITF

#endif // ITF_SUPPORT_UBISERVICES
