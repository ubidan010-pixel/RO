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

        virtual OnlineError downloadNews() override;
        virtual NewsData cachedNews() override;
        
    private:
        US_NS::AsyncResult<US_NS::Vector<US_NS::NewsInfo>> m_newsResult;

        std::mutex m_newsLock;
        NewsData m_cachedNews;
};

}

#endif // ITF_SUPPORT_UBISERVICES
