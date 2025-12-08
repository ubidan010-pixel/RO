#ifndef _ITF_SESSIONSERVICE_UBISERVICES_H
#define _ITF_SESSIONSERVICE_UBISERVICES_H

#include "engine/AdaptersInterfaces/OnlineAdapter/SessionService.h"

#if defined(ITF_SUPPORT_UBISERVICES)
#include <ubiservices/services/authentication/authenticationClient.h>
#include <ubiservices/services/authentication/authenticationNotification.h>
#include <ubiservices/services/notification/playerNotificationCustom.h>
#include <ubiservices/services/notification/playerNotificationUbiServices.h>
#include <ubiservices/services/populations/populationInfo.h>
#include <ubiservices/services/profile/profileInfo.h>

#include "engine/AdaptersInterfaces/OnlineAdapter/OnlineError.h"
#include "engine/AdaptersInterfaces/OnlineAdapter/PopulationInfo.h"
#include "engine/events/IEventListener.h"
#include "core/UUID.h"

#include <future>

namespace ubiservices
{
    class PlayerCredentials;
}

namespace ITF
{
    typedef ITF_MAP<ITF::StringID, ITF::String8> FieldsMap;
    typedef ITF_MAP<StringID, FieldsMap> ParametersMap;
    typedef ITF_MAP<StringID, ParametersMap> ParameterGroupsMap;

    class SessionService_Ubiservices : public SessionService, public IEventListener
    {
    public:
        SessionService_Ubiservices();
        virtual ~SessionService_Ubiservices();

        virtual void update() override;

        virtual ESessionStatus getSessionStatus() const override;
        virtual OnlineError getSessionError() override { ITF_ASSERT(getSessionStatus() == ESessionStatus_Error); return m_sessionError; }

        const ITF_VECTOR<PopulationInfo>& getPopulations() const override { return m_populationsInfos; }
        bool isInPopulation(const String8& _subject, const String8& _popName) const override;

        virtual OnlineError launchConnect(const String8& _deepLink = "", std::list<std::pair<String8, String8> > _params = {}) override;
        virtual OnlineError createSession() override;
        virtual void createSessionAsync() override;
        virtual bool hasUserAccountLinked() override;

        bool isClubFeatureSwitchEnabled() const override;
        bool isNewsFeatureSwitchEnabled() const override;

        virtual bool getConstantValue(String8 _parameterGroupName, StringID _parameterName, StringID _fieldName, f32 &_fieldValue) const override;
        virtual bool getConstantValue(String8 _parameterGroupName, StringID _parameterName, StringID _fieldName, u32 &_fieldValue) const override;
        virtual bool getConstantValue(String8 _parameterGroupName, StringID _parameterName, StringID _fieldName, i32 &_fieldValue) const override;
        virtual bool getConstantValue(String8 _parameterGroupName, StringID _parameterName, StringID _fieldName, String8 &_fieldValue) const override;
        virtual bool getConstantValue(String8 _parameterGroupName, StringID _parameterName, StringID _fieldName, bool &_fieldValue) const override;
        virtual bool getConstantValue(String8 _parameterGroupName, StringID _parameterName, StringID _fieldName, ITF_VECTOR<String8> &_fieldValue) const override;
        virtual bool getConstantValue(String8 _parameterGroupName, StringID _parameterName, StringID _fieldName, ITF_VECTOR<u32> &_fieldValue) const override;
        virtual bool getConstantValue(String8 _parameterGroupName, StringID _parameterName, StringID _fieldName, ITF_VECTOR<i32> &_fieldValue) const override;
        virtual bool getConstantValue(String8 _parameterGroupName, StringID _parameterName, StringID _fieldName, ITF_VECTOR<bool> &_fieldValue) const override;
        virtual bool getConstantValue(String8 _parameterGroupName, StringID _parameterName, StringID _fieldName, ITF_VECTOR<StringID> &_fieldValue) const override;

        virtual void requestRefresh() override;
        virtual void forceRequestRefresh() override;

        virtual void initialize() override;
        virtual void terminate() override;

        virtual void onEvent(Event* _event) override;
        virtual String8 getListenerName() const { return "SessionService_Ubiservices";}

        virtual bool isTransitioningToForeground() const override { return m_isTransitioningToForeground; }

        String8 getUbiservicesEnvName() const;

        static UUID convertProfileIdFromUbiservices(const ubiservices::ProfileId& _usId);
        static ubiservices::ProfileId convertProfileIdToUbiservices(const UUID& _profileId);

    private:
        virtual bool areUbiServicesCredentialsAvailable();
        virtual ubiservices::PlayerCredentials getUbiServicesCredentials();

        void onCreateSessionSuccess();
        void onCreateSessionFailure(const OnlineError& _error);

        void onGetProfileSuccess();
        void onGetProfileFailure(const OnlineError& _error);

        void refreshSessionInfo();

        void onFetchPopulationsSuccess(const ubiservices::Vector<ubiservices::PopulationInfo>& _populationInfo);
        void onFetchPopulationsFailure(const OnlineError& _error);

        void onDeleteSessionSuccess();
        void onDeleteSessionFailure(const OnlineError& _error);

        void transitionToSuspended();
        void transitionToForeground();
        void onTransitionToForegroundSuccess() { m_isTransitioningToForeground = false; };
        void onTransitionToForegroundFailed(const OnlineError& /*_success*/) { m_isTransitioningToForeground = false; };

        void onFetchUplayNameSuccess(const String8& _nameOnUplay);
        void onFetchUplayNameFailure(const OnlineError& _error);

        bool m_initialized;
        US_NS::SharedPtr<US_NS::Session> m_session;
        OnlineError m_sessionError;
        US_NS::AsyncResult<US_NS::Empty> m_createSessionResult;
        US_NS::AsyncResult<US_NS::Empty> m_closeSessionResult;

        typedef US_NS::Map<US_NS::ProfileId, US_NS::ProfileInfo> ProfileCache;

        US_NS::AsyncResult<ProfileCache> m_getProfileResult;
        ProfileCache m_cachedProfiles;

        void checkUbiservicesNotifications();

        US_NS::Vector<US_NS::PopulationInfo> m_usFormatPopulationInfo;
        ITF_VECTOR<ITF::PopulationInfo> m_populationsInfos;

        ParameterGroupsMap m_usParameterGroups;
        bool getConstantValueAsString(String8 _parameterGroupName, StringID _parameterName, StringID _fieldName, String8& _stringValue) const;

        US_NS::ListenerHandler<US_NS::PlayerNotificationUbiServices>   m_ubiservicesNotificationHandler;
        US_NS::ListenerHandler<US_NS::AuthenticationNotification>      m_authenticationNotificationHandler;
        US_NS::ListenerHandler<US_NS::PlayerNotificationCustom>        m_customNotificationHandler;

        enum EInternalStatus
        {
            EInternalStatus_Idle,           // waiting for some app logic to create a session
            EInternalStatus_Preparing,      // checking permissions, before actual open
            EInternalStatus_Creating,       // just the bare session opening
            EInternalStatus_Profile,        // fetch Uplay profile info for name, etc.
            EInternalStatus_Deleting,       // closing the session
            EInternalStatus_Ready,          // session is running; we can use any API
            EInternalStatus_Error,          // temporary failure, will try to recover if ms_enableAutomaticRetry 
            EInternalStatus_Offline,        // set by user input / will ignore ms_enableAutomaticRetry
            EInternalStatus_Count
        };

        void updatePreparing();
        void updateCreating();
        void updateDeleting();
        void updateReady();
        void updateError();
        void updateProfile();


        void setError(const OnlineError &error);

        void startCreatingSession();
        void startGetProfile();
        void deleteSession();
        
        static const bool ms_enableAutomaticRetry;
        f64 m_nextRequestRefreshTime;
        static const f64 ms_requestRefreshInterval;
        f64 m_nextSessionRetryTime;
        static const f64 ms_sessionRetryInterval;
        EInternalStatus m_sessionStatus;

        // by transitioning to foreground we hold the session status update until it is finished
        bool m_isTransitioningToForeground;

        bool m_retryInstantlyAfterError;

        bool m_ubiservicesSessionUpdated;

        friend class OnlineAdapter_Ubiservices;
    };


} // namespace ITF


#endif  // ITF_SUPPORT_UBISERVICES
#endif // _ITF_SESSIONSERVICE_UBISERVICES_H
