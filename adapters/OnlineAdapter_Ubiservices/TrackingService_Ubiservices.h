#ifndef ITF_TRACKINGSERVICE_UBISERVICES_H
#define ITF_TRACKINGSERVICE_UBISERVICES_H

#if defined(ITF_SUPPORT_UBISERVICES) && defined(ITF_SUPPORT_ONLINETRACKING)
#include <ubiservices/ubiservices.h>
#include <ubiservices/facade.h>

#include <ealmem.h>
#include <eallog.h>

#include "core/Macros.h"
#include "engine/AdaptersInterfaces/OnlineAdapter/TrackingService.h"
#include <ubiservices/services/event/subtitlesActivation.h>

namespace ITF
{
    class TrackingService_Ubiservices : public TrackingService
    {
    public:
        TrackingService_Ubiservices();
        virtual ~TrackingService_Ubiservices();

        virtual bool init() override;
        virtual void term() override;
        virtual void update() override;

        // create a new tag, attributes need to be added to that tag
        virtual void startTag(const char* _name) override;
        virtual void updatePlayTime(u32 _sessionTime) override;
        // add as many attributes as needed for a tag
        virtual void appendAttribute(const char* _attributeName, const char* _value, const ATTRIBUTE_DATATYPE dataType) override;

        // fire-and-forget
        virtual void sendTag() override;
    private:
        ubiservices::TimeSeconds m_playTime;
        ubiservices::String m_currentTagName;
        std::unique_ptr<ubiservices::JsonWriter> m_jsonWriter;

        bool m_isStandardEvent;

        enum EVENT_TYPE
        {
            EVENT_TYPE_NONE = 0,
            GAME_LOCALIZATION,
            PLAYER_PROGRESSION,
            CONTEXT_START,
            CONTEXT_STOP,
            ACHIEVEMENT_UNLOCK
        };

        EVENT_TYPE m_standardEventtType;

        ubiservices::uint32 m_achievementId;
        ubiservices::String m_contextName;
        ubiservices::String m_contextType;
        ubiservices::String m_progressionValue;
        ubiservices::String m_progressionType;
        ubiservices::String m_gameLanguage;
        ubiservices::String m_subtitlesLanguage;
        ubiservices::SubtitlesActivation m_subtitleEnabled;
    };

} // namespace ITF

#endif // defined(ITF_SUPPORT_UBISERVICES) && defined(ITF_SUPPORT_ONLINETRACKING)

#endif // ITF_TRACKINGSERVICE_UBISERVICES_H
