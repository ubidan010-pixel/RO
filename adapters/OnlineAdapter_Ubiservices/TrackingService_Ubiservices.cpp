#include "precompiled_OnlineAdapter_Ubiservices.h"

#if defined(ITF_SUPPORT_UBISERVICES) && defined(ITF_SUPPORT_ONLINETRACKING)
#include "adapters/OnlineAdapter_Ubiservices/OnlineAdapter_Ubiservices.h"
#include "adapters/OnlineAdapter_Ubiservices/TrackingService_Ubiservices.h"

#include "core/error/ErrorHandler.h"

#include <ubiservices/services/event/eventClient.h>
#include <ubiservices/services/event/types/eventInfoContextStart.h>
#include <ubiservices/services/event/types/eventInfoContextStop.h>
#include <ubiservices/services/event/types/eventInfoCustom.h>
#include <ubiservices/services/event/types/eventInfoGameLocalization.h>
#include <ubiservices/services/event/types/eventInfoPlayerProgression.h>
#include <ubiservices/services/event/types/eventInfoPlayerAchievement.h>
#include <ubiservices/services/event/types/eventInfoService.h>
#include <ubiservices/services/event/types/types.h>

using namespace ubiservices;

namespace ITF
{
    TrackingService_Ubiservices::TrackingService_Ubiservices()
        :TrackingService()
        , m_currentTagName()
        , m_jsonWriter()
        , m_isStandardEvent(false)
        , m_standardEventtType(EVENT_TYPE_NONE)
        , m_achievementId(0)
        , m_contextName()
        , m_contextType()
        , m_progressionValue()
        , m_progressionType()
        , m_gameLanguage()
        , m_subtitlesLanguage()
        , m_subtitleEnabled(ubiservices::SubtitlesActivation::Enabled)
    {

    }

    TrackingService_Ubiservices::~TrackingService_Ubiservices()
    {

    }

    bool TrackingService_Ubiservices::init()
    {
        return true;
    }

    void TrackingService_Ubiservices::term() {}
    void TrackingService_Ubiservices::update() {}

    void TrackingService_Ubiservices::startTag(const char* _name)
    {
        m_currentTagName = _name;
        m_jsonWriter.reset(US_NEW(ubiservices::JsonWriter));  // Reset the JSON writer
    }

    void TrackingService_Ubiservices::updatePlayTime(u32 _sessionTime)
    {
        if (m_playTime == _sessionTime)
            return;

        m_playTime = _sessionTime;

        auto session = ONLINE_ADAPTER_UBISERVICES->getSession();
        if (!session)
            return;

        PlayerEventModule& module = PlayerEventModule::module(*session);
        module.updatePlayTimeClock(m_playTime);
    }
    
    void TrackingService_Ubiservices::appendAttribute(const char* _attributeName, const char* _value, const ATTRIBUTE_DATATYPE dataType)
    { }

    
    void TrackingService_Ubiservices::sendTag()
    {
        auto session = ONLINE_ADAPTER_UBISERVICES->getSession();
        if (!session)
            return;

        PlayerEventModule& module = PlayerEventModule::module(*session);

        ubiservices::PushEventResult pushResult;
        ubiservices::Json eventAttributes = m_jsonWriter->getJson();

        if (m_isStandardEvent)
        {
            std::unique_ptr<ubiservices::EventInfoBase> event(nullptr);

            switch (m_standardEventtType)
            {
            case GAME_LOCALIZATION:
                event.reset(US_NEW(ubiservices::EventInfoGameLocalization, "", m_gameLanguage, m_subtitleEnabled, m_subtitlesLanguage, eventAttributes.renderContent()));
                break;
            case PLAYER_PROGRESSION:
                event.reset(US_NEW(ubiservices::EventInfoPlayerProgression, m_progressionType, m_progressionValue, eventAttributes.renderContent()));
                break;
            case CONTEXT_START:
                event.reset(US_NEW(ubiservices::EventInfoContextStart, m_contextType, m_contextName, eventAttributes.renderContent()));
                break;
            case CONTEXT_STOP:
                event.reset(US_NEW(ubiservices::EventInfoContextStop, m_contextType, m_contextName, eventAttributes.renderContent()));
                break;
            case ACHIEVEMENT_UNLOCK:
                event.reset(US_NEW(ubiservices::EventInfoPlayerAchievement, m_achievementId, eventAttributes.renderContent()));
                break;
            default:
                break;
            }

            if (event.get())
            {
                pushResult = module.pushEvent(*event);
            }
            else
            {
                LOG("[TrackingService] Unsupported standard event (%d)", m_standardEventtType);
                pushResult = ubiservices::PushEventResult::ENUM_END;
            }
        }
        else
        {
            ubiservices::EventInfoCustom event(m_currentTagName, eventAttributes.renderContent());
            pushResult = module.pushEvent(event);
        }

        if (pushResult != ubiservices::PushEventResult::Success)
        {
            LOG("[Tracking] Custom event %s could not be queued", m_currentTagName.getUtf8());
        }
        else
        {
            LOG("[Tracking] Custom event % s was queued", m_currentTagName.getUtf8());
        }


        // reset string accumulators
        m_isStandardEvent = false;
        m_standardEventtType = EVENT_TYPE_NONE;
        m_currentTagName = ubiservices::String();
    }
} // namespace ITF

#endif //defined(ITF_SUPPORT_UBISERVICES) && defined(ITF_SUPPORT_ONLINETRACKING)
