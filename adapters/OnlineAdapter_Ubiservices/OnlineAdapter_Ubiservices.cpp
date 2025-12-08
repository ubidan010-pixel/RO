#include "precompiled_OnlineAdapter_Ubiservices.h"

#include "adapters/OnlineAdapter_Ubiservices/OnlineAdapter_Ubiservices.h"
#include "adapters/OnlineAdapter_Ubiservices/TrackingService_Ubiservices.h"
#include "adapters/OnlineAdapter_Ubiservices/SessionService_Ubiservices.h"
#include "adapters/OnlineAdapter_Ubiservices/NewsService_Ubiservices.h"

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
#include "engine/AdaptersInterfaces/OnlineAdapter/BuildId.h"

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

#if ITF_PS5
#include <ubiservices/core/firstPartyInit/psApi/jsonMemAllocator.h>
#include <ubiservices/core/firstPartyInit/psApi/jsonInitializer.h>
namespace FirstPartyInit_BF
{
    // We use pointers here because we cant create these object statically without loading certain lib before.
    // SDK is dumb now; used to have an internal FirstPartyInit api call for this.
    std::unique_ptr<US_NS::JsonMemAllocator> s_allocator;
    std::unique_ptr<sce::Json::Initializer> s_jsonInitializer;
}

extern "C"
{
    unsigned int sceLibcHeapExtendedAlloc = 1;                           /* Switch to dynamic allocation */
    size_t sceLibcHeapSize = SCE_LIBC_HEAP_SIZE_EXTENDED_ALLOC_NO_LIMIT; /* no upper limit for heap area */
};
#endif

using namespace ubiservices;

namespace ITF
{
    // Creates a generic-looking build ID that doesn't increment automatically
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
        m_newsService = newAlloc(mId_OnlineServices, NewsService_Ubiservices);
    }

    OnlineAdapter_Ubiservices::~OnlineAdapter_Ubiservices()
    {
#if defined(ITF_SUPPORT_ONLINETRACKING)
        SF_DEL(m_trackingService);
#endif
        SF_DEL(m_newsService);
        SF_DEL(m_sessionService);
    }

    void OnlineAdapter_Ubiservices::initialize()
    {
        initFirstParty();
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
        m_sessionService->update();
        m_newsService->update();
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

        uninitFirstParty();
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

    void OnlineAdapter_Ubiservices::initFirstParty()
    {
#if defined(ITF_PS5)
        US_NS::JsonInitializer usJsonInitializer;
        FirstPartyInit_BF::s_jsonInitializer.reset(new sce::Json::Initializer);
        FirstPartyInit_BF::s_allocator.reset(new US_NS::JsonMemAllocator);
        int returnCode = usJsonInitializer.initialize(*FirstPartyInit_BF::s_jsonInitializer, *FirstPartyInit_BF::s_allocator);
        if (returnCode < 0)
        {
            LOG("[OnlineAdapter] sce::Json::Initializer initialize() error %d.\n", returnCode);
        }
#endif
        LOG("[OnlineAdapter] initFirstParty OK");
    }

    void OnlineAdapter_Ubiservices::uninitFirstParty()
    {
#if defined(ITF_PS5)
        if (FirstPartyInit_BF::s_jsonInitializer.get() != nullptr)
        {
            US_NS::JsonInitializer usJsonInitializer;
            usJsonInitializer.uninitialize(*FirstPartyInit_BF::s_jsonInitializer);
        }

        // Must force release before uninitialize the Json module in uninitializeModules
        FirstPartyInit_BF::s_jsonInitializer.release();
        FirstPartyInit_BF::s_allocator.release();
#endif

        LOG("[OnlineAdapter] uninitFirstParty OK");
    }

    void OnlineAdapter_Ubiservices::initializeUbiservices()
    {
        // use Jenkins build string
        String8 buildId = APPLICATION_BUILD_ID;

        // If we're not on a Jenkins build, send the fallback string
        if (buildId.strstr("#UC") != NULL)
            buildId = generateBuildId();

        US_NS::LoggingHelper::enableMultiLines(true);
        US_NS::LoggingHelper::enableVerbose(true);

        m_sdk = US_NS::UbiservicesSdk::create();

        configureUbiservices(buildId.cStr());

        // Set the locale so Uplay actions are localized nicely
        String8 lang = "en";
        String8 locale = "US";
        ITF::ITF_LANGUAGE itfLang = LOCALISATIONMANAGER->getCurrentLanguage();
        bool bOk = convertITFtoUSLanguage(itfLang, lang, locale);

        LOG("[OnlineAdapter] localize language %s / %s. itfknown: %d", lang.cStr(), locale.cStr(), bOk);
        US_NS::LocalizationHelper::setLocaleCode(US_NS::String::formatText("%s-%s", lang.cStr(), locale.cStr()));

        m_sessionService->initialize();

        m_initialized = true;
    }

    const char* OnlineAdapter_Ubiservices::getApplicationId()
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

    const char* OnlineAdapter_Ubiservices::getSku()
    {
#ifdef ITF_WINDOWS
        return "PC_WW";
#elif ITF_PS5
        return "PS5_WW";
#elif ITF_NX
        return "NX_WW";
#elif ITF_OUNCE
        return "OUNCE_WW";
#elif ITF_XBOX_SERIES
        return "XS_WW";
#endif
    }

    void OnlineAdapter_Ubiservices::configureUbiservices(const ubiservices::String& _buildId)
    {
        const US_NS::ApplicationId applicationId(getApplicationId());
        US_NS::String applicationBuildId(_buildId);
        US_NS::OnlineAccessContext onlineAccessContext = US_NS::OnlineAccessContext::Standard;
        US_NS::ProfilePolicy profilePolicy = US_NS::ProfilePolicy::UseUplayProfile;

        const US_NS::String gameVersion(SYSTEM_ADAPTER->isTrialMode() ? "Trial": "Full");
        const US_NS::String sku(getSku());
        const US_NS::Vector<US_NS::EventTypeInfo> eventTypesForSaveGame;
        const US_NS::StringJson gameStartEventCustomData = "{}";
        const US_NS::String secretKey;
        const US_NS::uint32 saveGameMaxSize = 16 * 1024;
        const US_NS::StringJson initialEventsDefinitionsJson = "{}";
        bool isExtendedEventPlayerSessionEnabled = false;

        US_NS::GameConfigEvent gameConfigEvent = US_NS::GameConfigEvent(
                gameVersion
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
        const US_NS::String titleId("PPSA34569_00");
        const US_NS::GameConfigConsole gameConfigConsole(titleId);
#elif defined(ITF_NX)
        // NintendoApplicationId
        const US_NS::String theApplicationId("0x0100ac702659e000");
        const US_NS::GameConfigConsole gameConfigConsole(theApplicationId);
#elif defined(ITF_OUNCE)
        const US_NS::String theApplicationId("0x04005b00265a0000");
        const US_NS::GameConfigConsole gameConfigConsole(theApplicationId);

#elif defined(ITF_XBOX_SERIES)
        // https://confluence.ubisoft.com/display/onlineservices/UbiServices+Client+SDK+-+Configuring+Xbox+Products+2025.x
        const US_NS::String productId("6562f122"); // XDK uses legacy id; GDK uses the 8 char Xbox Title ID.
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
            LOG("[OnlineAdapter] configure OK!");
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

    void OnlineAdapter_Ubiservices::setOfflineMode(bbool _offline)
    {
        SessionService_Ubiservices* pService = (SessionService_Ubiservices*)m_sessionService;
        if (_offline == btrue)
        {
            LOG("[OnlineAdapter] going offline...");
            // order is important; deleteSession internally sets a different state
            pService->deleteSession();
            pService->m_sessionStatus = SessionService_Ubiservices::EInternalStatus_Offline;
        }
        else
        {
            LOG("[OnlineAdapter] start creating a session...");
            pService->createSessionAsync();
        }
    }
} // namespace ITF

#endif // ITF_SUPPORT_UBISERVICES
