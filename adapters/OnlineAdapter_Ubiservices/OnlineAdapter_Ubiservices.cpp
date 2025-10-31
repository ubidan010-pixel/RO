#include "precompiled_OnlineAdapter_Ubiservices.h"

#include "adapters/OnlineAdapter_Ubiservices/OnlineAdapter_Ubiservices.h"

#include "core/AdaptersInterfaces/SystemAdapter.h"
#include "core/versioning.h"
#include "core/Config.h"
#include "core/itfstring.h"
#include "core/memory/memory.h"

#include "engine/localisation/LocalisationManager.h"
#include "engine/singleton/Singletons.h"

#include <ubiservices/facade.h>
#include <ubiservices/ubiservices.h>
#include <ubiservices/session.h>
#include <ubiservices/services/parameters/parametersClient.h>
#include <ubiservices/services/parameters/parametersGroupInfo.h>
#include <ubiservices/services/authentication/sessionInfo.h>
#include <ubiservices/localizationHelper.h>
#include <ubiservices/loggingHelper.h>
#include <ubiservices/core/types/string.h>
#include <ubiservices/core/types/containers.h>
#include <ubiservices/core/log/log.h>
#include <ubiservices/core/log/logDevice.h>
#include <ubiservices/core/configs/threadingConfig.h>
#include <ubiservices/core/configs/gameConfig.h>

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
        , m_usFacade(nullptr)
    {
        US_NS::initializeSdk(); // init EAL, must be first
    }

    OnlineAdapter_Ubiservices::~OnlineAdapter_Ubiservices()
    {
        while (US_NS::uninitializeSdk() != US_NS::UninitializeResult::Complete)
        {
            sleep(100);
        }
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
        configureUbiservices(buildId.cStr());

        US_NS::LoggingHelper::enableMultiLines(true);
        US_NS::LoggingHelper::enableVerbose(true);

        // Set the locale so Uplay actions are localized nicely
        String8 lang = "en";
        String8 locale = "US";
        //LOCALISATIONMANAGER->getCurrentLocaleString(lang, locale);
        US_NS::LocalizationHelper::setLocaleCode(US_NS::String::formatText("%s-%s", lang.cStr(), locale.cStr()));

        m_usFacade = US_NEW(ubiservices::Facade);
    }

    void OnlineAdapter_Ubiservices::configureUbiservices(const ubiservices::String& _buildId)
    {
        // Initializes UbiServices. This must be done before any other call to UbiServices.
        const US_NS::ApplicationId applicationId("4be81211-c3b6-427b-ab0a-5e2264da4529");
        const US_NS::String applicationBuildId(_buildId);
        const US_NS::SpaceId spaceId("d1f6588a-19b5-421b-beae-5d95372756c6");

        const US_NS::GameConfig gameConfig(applicationId, applicationBuildId);

        US_NS::configureSdk(gameConfig);
    }

    void OnlineAdapter_Ubiservices::terminateUbiservices()
    {
        US_DELETE(m_usFacade);
    }

    void OnlineAdapter_Ubiservices::recreateFacade()
    {
        if (m_usFacade)
        {
            US_DELETE(m_usFacade);
        }

        m_usFacade = US_NEW(ubiservices::Facade);
    }

} // namespace ITF
