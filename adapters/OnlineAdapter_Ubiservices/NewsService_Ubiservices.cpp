#include "precompiled_OnlineAdapter_Ubiservices.h"

#if defined(ITF_SUPPORT_UBISERVICES)
#include "adapters/OnlineAdapter_Ubiservices/OnlineAdapter_Ubiservices.h"
#include "engine/AdaptersInterfaces/OnlineAdapter/SessionService.h"
#include "adapters/OnlineAdapter_Ubiservices/NewsService_UbiServices.h"

#include "core/error/ErrorHandler.h"    // for LOG()
#include "core/memory/memoryId.h"
#include <engine/singleton/Singletons.h>
#include <engine/localisation/LocalisationManager.h>

#include <ubiservices/facade.h>
#include <ubiservices/core/tasks/asyncResult.h>
#include <ubiservices/core/tasks/asyncResultError.h>
#include <ubiservices/services/news/news_module.h>
#include <ubiservices/localizationHelper.h>

namespace ITF
{

    NewsService_Ubiservices::NewsService_Ubiservices()
        : NewsService()
        , m_newsResult()
        , m_cachedNews()
        , m_newsLock()
    {
    }

    NewsService_Ubiservices::~NewsService_Ubiservices()
    {
        if (m_newsResult.isProcessing())
            m_newsResult.cancel();
    }

    OnlineError NewsService_Ubiservices::downloadNews()
    {
        if (ONLINE_ADAPTER->getSessionService()->getSessionStatus() != SessionService::ESessionStatus_Valid)
        {
            LOG("[News] no valid session");
            return ONLINE_ADAPTER->getSessionService()->getSessionError();
        }

        if (m_newsResult.isProcessing())
        {
            LOG("[News] downloadNews already processing");
            return OnlineError(OnlineError::UbiServer, OnlineError::News, 0);
        }

        // Set the locale so Uplay actions are localized nicely
        String8 lang = "en";
        String8 locale = "US";
        ITF::ITF_LANGUAGE itfLang = LOCALISATIONMANAGER->getCurrentLanguage();
        bool bOk = ONLINE_ADAPTER_UBISERVICES->convertITFtoUSLanguage(itfLang, lang, locale);

        LOG("[Ubiservices] localize language %s / %s. itfknown: %d", lang.cStr(), locale.cStr(), bOk);
        US_NS::LocalizationHelper::setLocaleCode(US_NS::String::formatText("%s-%s", lang.cStr(), locale.cStr()));

        auto session = ONLINE_ADAPTER_UBISERVICES->getSession();
        US_NS::NewsModule& modNews = US_NS::NewsModule::module(*session);

        US_NS::SpaceId crossRetailSpace("ddd8818b-f7e0-497e-9901-0e76bbdba154");
        US_NS::String usLocale = US_NS::String::formatText("%s-%s", lang.cStr(), locale.cStr());
        m_newsResult = modNews.requestSpaceNews(crossRetailSpace, US_NS::CountryName::NotSpecified, usLocale);
        while (m_newsResult.isProcessing())
        {
            sleep(100);
        }

        if (m_newsResult.hasFailed())
        {
            i32 code = m_newsResult.getAsyncResultError().m_baseError.m_code;
            LOG("[News] failed. code %d", code);
            return OnlineError(OnlineError::UbiServer, OnlineError::News, code);
        }
        else if (m_newsResult.isCanceled())
        {
            LOG("[News] cancelled.");
            return OnlineError(OnlineError::UbiServer, OnlineError::News, 1);
        }
        else if (m_newsResult.hasSucceeded())
        {
            const US_NS::Vector<US_NS::NewsInfo> &result = m_newsResult.getResult();
            LOG("[News] success. got %d news", result.size());

            std::scoped_lock lock(m_newsLock);

            m_cachedNews.clear();

            for (auto& news : result)
            {
                NewsInfo info;
                info.m_id = news.m_id.getUtf8();
                info.m_type = news.m_type.getUtf8();
                info.m_title = news.m_title.getUtf8();
                info.m_body = news.m_body.getUtf8();
                info.m_mediaUrl = news.m_mediaUrl.getUtf8();
                info.m_placement = news.m_placement.getUtf8();
                info.m_priority = news.m_priority;

                m_cachedNews.push_back(info);
            }

            return OnlineError(OnlineError::Success, OnlineError::News);
        }

        ITF_ASSERT_MSG(false, "Async result logic issue!");
        return OnlineError(OnlineError::Invalid, OnlineError::News, 0);
    }

    void NewsService_Ubiservices::update()
    {

    }

    NewsData NewsService_Ubiservices::cachedNews()
    {
        return m_cachedNews;
    }

} // namespace ITF

#endif // ITF_SUPPORT_UBISERVICES
