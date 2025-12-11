#pragma once

#include <engine/AdaptersInterfaces/OnlineAdapter/OnlineError.h>
#include <core/container/extendedVector.h>
#include <core/String8.h>

namespace ITF
{

struct NewsInfo
{
    String8 m_id;
    String8 m_type;
    String8 m_title;
    String8 m_body;
    String8 m_mediaUrl;
    String8 m_placement;
    u32     m_priority;
};

typedef ITF_VECTOR<NewsInfo> NewsData;

class NewsService
{
    public:
        virtual ~NewsService() {}
        virtual void update() = 0;
        virtual OnlineError downloadNews(ITF_LANGUAGE _lang = ITF_LANGUAGE_ENGLISH) = 0;
        virtual NewsData cachedNews() = 0;
};

} // namespace ITF
