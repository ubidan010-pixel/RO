#pragma once

#include <engine/AdaptersInterfaces/OnlineAdapter/NewsService.h>
#include <engine/AdaptersInterfaces/OnlineAdapter/SessionService.h>

#if defined(ITF_SUPPORT_UBISERVICES)
#include <ubiservices/services/news/newsClient.h>
#include <mutex>

namespace ITF
{

class NewsService_Ubiservices : public NewsService
{
    public:
        NewsService_Ubiservices();
        virtual ~NewsService_Ubiservices();

        virtual void update() override;

        virtual OnlineError downloadNews(ITF_LANGUAGE _lang=ITF_LANGUAGE_ENGLISH) override;
        virtual NewsData cachedNews() override;
        
    private:
        US_NS::AsyncResult<US_NS::Vector<US_NS::NewsInfo>> m_newsResult;

        std::mutex m_newsLock;
        ITF_MAP<ITF_LANGUAGE, NewsData> m_cachedNews;
        f64 m_lastFetch = 0.0f;

        static const f64 m_NewsFetchInterval;
};

}

#endif // ITF_SUPPORT_UBISERVICES
