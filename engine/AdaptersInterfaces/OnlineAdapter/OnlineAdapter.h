#ifndef _ITF_ONLINEADAPTER_H_
#define _ITF_ONLINEADAPTER_H_

#include "core/templateSingleton.h"
#include "core/Macros.h"
#include "core/container/extendedVector.h"
#include "core/String8.h"

namespace ITF
{
    class SessionService;
    class TrackingService;
    class NewsService;
    class CloudSaveService;

    class OnlineAdapter : public TemplateSingleton<OnlineAdapter>
    {
    public:
        OnlineAdapter()
            : m_sessionService(nullptr)
            , m_trackingService(nullptr)
            , m_newsService(nullptr)
            , m_cloudSaveService(nullptr)
            , m_offlineMode(bfalse)
        {
        }

        virtual ~OnlineAdapter() {}

        virtual void initialize() {}
        virtual void update() {}
        virtual void terminate() {}

        virtual void setOfflineMode(bbool _isOffline)
        {
            m_offlineMode = _isOffline;
        }

        virtual inline bbool getOfflineMode() { return m_offlineMode; }

        const ITF_VECTOR<String8> getParameterGroupNames()
        {
            ITF_VECTOR<String8> ret;
            ret.push_back("UbiConnect");
            ret.push_back("roGameConfigs");
            return ret;
        }

        SessionService* getSessionService()
        {
            ITF_ASSERT_CRASH(m_sessionService != nullptr, "Online service not implemented");
            return m_sessionService;
        }

        TrackingService* getTrackingService()
        {
            ITF_ASSERT_CRASH(m_trackingService != nullptr, "Online service not implemented");
            return m_trackingService;
        }

        NewsService* getNewsService()
        {
            ITF_ASSERT_CRASH(m_newsService != nullptr, "News service not implemented");
            return m_newsService;
        }

        CloudSaveService* getCloudSaveService()
        {
            ITF_ASSERT_CRASH(m_cloudSaveService != nullptr, "Cloud save service not implemented");
            return m_cloudSaveService;
        }

    protected:
        SessionService* m_sessionService;
        TrackingService* m_trackingService;
        NewsService* m_newsService;
        CloudSaveService* m_cloudSaveService;

        bbool m_offlineMode;
    };

#define ONLINE_ADAPTER ITF::OnlineAdapter::getptr()

} // namespace ITF

#endif // _ITF_ONLINEADAPTER_H_
