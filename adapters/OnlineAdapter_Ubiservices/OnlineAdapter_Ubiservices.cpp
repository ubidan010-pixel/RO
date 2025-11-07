#include "precompiled_OnlineAdapter_Ubiservices.h"

#include "adapters/OnlineAdapter_Ubiservices/OnlineAdapter_Ubiservices.h"

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
#include <ubiservices/session.h>
#include <ubiservices/localizationHelper.h>
#include <ubiservices/loggingHelper.h>

#include <ubiservices/core/types/string.h>
#include <ubiservices/core/types/containers.h>
#include <ubiservices/core/log/log.h>
#include <ubiservices/core/log/logDevice.h>
#include <ubiservices/core/configs/threadingConfig.h>
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
        , m_gameConfig(nullptr)
        , m_sysConfig(nullptr)
    {
        US_NS::initializeSdk();
        bool isMemLibinit = EalMemLibInit();
        bool isLogLibinit = EalLogLibInit();
    }

    OnlineAdapter_Ubiservices::~OnlineAdapter_Ubiservices()
    {
    }

    void OnlineAdapter_Ubiservices::initialize()
    {
        initializeUbiservices();
    }

    void OnlineAdapter_Ubiservices::update()
    {
        if (!m_initialized)
        {
            return;
        }
    }

    void OnlineAdapter_Ubiservices::terminate()
    {
        if (m_initialized)
        {
            terminateUbiservices();
            m_initialized = false;
        }
    }

    void OnlineAdapter_Ubiservices::initializeUbiservices()
    {
        String8 buildId = generateBuildId();
        m_sdk = US_NS::UbiservicesSdk::create();

        configureUbiservices(buildId.cStr());

        // Set the locale so Uplay actions are localized nicely
        String8 lang = "en";
        String8 locale = "US";
        //LOCALISATIONMANAGER->getCurrentLocaleString(lang, locale);
        US_NS::LocalizationHelper::setLocaleCode(US_NS::String::formatText("%s-%s", lang.cStr(), locale.cStr()));

        //createSession();

        m_initialized = true;
    }

    void OnlineAdapter_Ubiservices::configureUbiservices(const ubiservices::String& _buildId)
    {
        const US_NS::ApplicationId applicationId("4be81211-c3b6-427b-ab0a-5e2264da4529");
        const US_NS::String applicationBuildId(_buildId);
        US_NS::OnlineAccessContext onlineAccessContext = US_NS::OnlineAccessContext::Standard;
        US_NS::ProfilePolicy profilePolicy = US_NS::ProfilePolicy::UseUplayProfile;

        US_NS::UplayPCPolicy uplayPCPolicy = US_NS::UplayPCPolicy::UseUplayPC;
        const US_NS::GameConfigConsole gameConfigConsole(uplayPCPolicy);

        m_gameConfig = new US_NS::GameConfig(applicationId
            , applicationBuildId
            , onlineAccessContext
            , US_NS::GameConfigEvent("Full", "WW", US_NS::Vector<US_NS::EventTypeInfo>())
        );

        m_sysConfig = new US_NS::SystemConfig();

        US_NS::ConfigureResult res = m_sdk->configure(*m_gameConfig, *m_sysConfig);
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

        m_sdk.release();

        LOG("[Ubiservices] terminateUbiservices start");
        while (US_NS::uninitializeSdk() != US_NS::UninitializeResult::Complete)
        {
            sleep(100);
        }

        LOG("[Ubiservices] terminateUbiservices completed");
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
