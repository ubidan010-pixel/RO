#include "precompiled_OnlineAdapter_Ubiservices.h"

#include "adapters/OnlineAdapter_Ubiservices/OnlineAdapter_Ubiservices.h"
#include "adapters/OnlineAdapter_Ubiservices/TrackingService_Ubiservices.h"
#include "adapters/OnlineAdapter_Ubiservices/SessionService_Ubiservices.h"

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

        m_sessionService = newAlloc(mId_OnlineServices, SessionService_Ubiservices);
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
            m_sessionService->terminate();
            terminateUbiservices();
            m_initialized = false;
        }
    }

    bool OnlineAdapter_Ubiservices::convertITFtoUSLanguage(ITF_LANGUAGE _itfLang, String8& _lang, String8& _locale)
    {
        bool knownLang = true;

        switch (_itfLang)
        {
        case ITF_LANGUAGE_ENGLISH:
            _lang = "en";
            _locale = "US";
            break;

        case ITF_LANGUAGE_FRENCH:
            _lang = "fr";
            _locale = "FR";
            break;

        case ITF_LANGUAGE_ITALIAN:
            _lang = "it";
            _locale = "IT";
            break;

        case ITF_LANGUAGE_GERMAN:
            _lang = "de";
            _locale = "DE";
            break;

        case ITF_LANGUAGE_SPANISH:
            _lang = "es";
            _locale = "ES";
            break;

        case ITF_LANGUAGE_PORTUGUESE:
            _lang = "pt";
            _locale = "BR";
            break;

        case ITF_LANGUAGE_JAPANESE:
            _lang = "ja";
            _locale = "JP";
            break;

        case ITF_LANGUAGE_TRADITIONALCHINESE:
            _lang = "zh";
            _locale = "TW";
            break;

        case ITF_LANGUAGE_SIMPLIFIEDCHINESE:
            _lang = "zh";
            _locale = "CN";
            break;

        case ITF_LANGUAGE_RUSSIAN:
            _lang = "ru";
            _locale = "RU";
            break;

        case ITF_LANGUAGE_POLISH:
            _lang = "pl";
            _locale = "PL";
            break;

        case ITF_LANGUAGE_KOREAN:
            _lang = "ko";
            _locale = "KO";
            break;

        case ITF_LANGUAGE_DUTCH:
            _lang = "nl";
            _locale = "NL";
            break;

        case ITF_LANGUAGE_CZECH:
            _lang = "cs";
            _locale = "CZ";
            break;

        case ITF_LANGUAGE_HUNGARIAN:
            _lang = "hu";
            _locale = "HU";
            break;

        default:
            _lang = "en";
            _locale = "US";
            knownLang = false;
            break;
        };

        return knownLang;
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
        ITF::ITF_LANGUAGE itfLang = LOCALISATIONMANAGER->getCurrentLanguage();
        bool bOk = convertITFtoUSLanguage(itfLang, lang, locale);

        LOG("[Ubiservices] localize language %s / %s. itfknown: %d", lang.cStr(), locale.cStr(), bOk);
        US_NS::LocalizationHelper::setLocaleCode(US_NS::String::formatText("%s-%s", lang.cStr(), locale.cStr()));

        m_sessionService->initialize();

        m_initialized = true;
    }

    const char* OnlineAdapter_Ubiservices::getUSApplicationId()
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
        const US_NS::ApplicationId applicationId(getUSApplicationId());
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

#if defined(ITF_WINDOWS)
        US_NS::UplayPCPolicy uplayPCPolicy = US_NS::UplayPCPolicy::UseUplayPC;
#endif

#if defined(ITF_WINDOWS)
        const US_NS::GameConfigConsole gameConfigConsole(uplayPCPolicy);
#elif defined(ITF_PS5)
        const US_NS::String titleId("CUSA001234_00");
        const US_NS::String clientId("11111111-2222-3333-4444-555555555555");
        const US_NS::uint32 titleStoreServiceLabel = 0;
        const US_NS::String clientSecret("ClientSecret");
        const bool isSceNpCheckCallbackManagedBySdk = true;

        const US_NS::GameConfigConsole gameConfigConsole(titleId, clientId, titleStoreServiceLabel, clientSecret, isSceNpCheckCallbackManagedBySdk);
#elif defined(ITF_NX) || defined(ITF_OUNCE)
        // This is the primary store id given to the game in the first party publishing tool
        const US_NS::String applicationId("a11a1a1a-1111-11aa-111a-1a1111a11aaa");
        const US_NS::GameConfigConsole gameConfigConsole(applicationId);
#elif defined(ITF_SCARLETT)
        const US_NS::String productId("a11a1a1a-1111-11aa-111a-1a1111a11aaa"); // This is the primary store id given to the game in the first party publishing tool
        const US_NS::PartyMultiplayerPrivilegeModel model = US_NS::PartyMultiplayerPrivilegeModel::Standard; // Xbox title payment model used to check party multiplayer privileges.
        const US_NS::GameConfigConsole gameConfigConsole(productId, model);
#endif

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
    }

    US_NS::SharedPtr<US_NS::Session> OnlineAdapter_Ubiservices::getSession()
    {
        if (!m_sdk)
            return US_NS::SharedPtr<US_NS::Session>();

        if (!m_session)
            m_session = US_NS::Session::create(*m_sdk);

        return m_session;
    }
} // namespace ITF

#endif // ITF_SUPPORT_UBISERVICES
