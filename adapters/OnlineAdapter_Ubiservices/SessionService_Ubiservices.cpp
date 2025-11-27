#include "precompiled_OnlineAdapter_Ubiservices.h"

#if defined(ITF_SUPPORT_UBISERVICES)

#include "adapters/OnlineAdapter_Ubiservices/OnlineAdapter_Ubiservices.h"
#include "adapters/OnlineAdapter_Ubiservices/SessionService_Ubiservices.h"

#include "core/error/ErrorHandler.h"
#include "core/AdaptersInterfaces/Account.h"
#include "core/AdaptersInterfaces/AccountAdapter.h"
#include "core/AdaptersInterfaces/SystemAdapter.h"
#include "engine/singleton/Singletons.h"
#include "engine/events/EventManager.h"
#include "engine/networkservices/NetworkServices.h"
#include "engine/networkservices/UPlayService.h"

#include <ubiservices/facade.h>

#include <ubiservices/core/helpers/platformHelper/platformHelper.h>
#include <ubiservices/core/tasks/asyncResult.h>
#include <ubiservices/core/tasks/asyncResultError.h>

#include <ubiservices/services/authentication/authenticationClient.h>
#include <ubiservices/services/authentication/playerCredentials.h>
#include <ubiservices/services/authentication/sessionInfo.h>
#include <ubiservices/services/parameters/parametersClient.h>
#include <ubiservices/services/parameters/parameters_module.h>
#include <ubiservices/services/populations/populationClient.h>
#include <ubiservices/services/player_notifications/player_notifications_module.h>
#include <ubiservices/services/notification/playerNotificationClient.h>
#include <ubiservices/services/notification/playerNotificationCustom.h>
#include <ubiservices/services/ubisoftConnect/ubisoftConnectClient.h>

#if defined(ITF_WINDOWS)
#   include "adapters/AccountAdapter_win/AccountAdapter_win.h"
#elif defined(ITF_NX) || defined(ITF_OUNCE)
#   include "adapters/AccountAdapter_Nintendo/AccountAdapter_Nintendo.h"
#elif defined(ITF_PS5)
#   include "adapters/AccountAdapter_PS5/AccountAdapter_PS5.h"
#elif defined(ITF_XBOX_SERIES)
#   include "adapters/AccountAdapter_xboxseries/AccountAdapter_xboxseries.h"
#endif

using namespace ubiservices;

namespace ITF
{
const f64 SessionService_Ubiservices::ms_sessionRetryInterval = 10.0f; // seconds
const f64 SessionService_Ubiservices::ms_requestRefreshInterval = 30.0f; // seconds

#if defined(ITF_NX)
const bool SessionService_Ubiservices::ms_enableAutomaticRetry(false);
#else
const bool SessionService_Ubiservices::ms_enableAutomaticRetry(true);
#endif

SessionService_Ubiservices::SessionService_Ubiservices()
    : m_initialized(false)
    , m_sessionError()
    , m_usFormatPopulationInfo()
    , m_populationsInfos()
    , m_authenticationNotificationHandler()
	, m_customNotificationHandler()
    , m_nextRequestRefreshTime(0.0f)
    , m_nextSessionRetryTime(0.0f)
    , m_sessionStatus(EInternalStatus_Idle)
    , m_isTransitioningToForeground(false)
    , m_retryInstantlyAfterError(true)
    , m_ubiservicesSessionUpdated(false)
{
}

SessionService_Ubiservices::~SessionService_Ubiservices()
{
    
}

void SessionService_Ubiservices::initialize()
{
    //EVENTMANAGER->registerEvent(EventMainUserChanged_CRC,this);
    //EVENTMANAGER->registerEvent(EventResumeFromSuspendedState_CRC, this);
    //EVENTMANAGER->registerEvent(EventEnterSuspendedState_CRC, this);
    //EVENTMANAGER->registerEvent(EventTitleFocusChanged_CRC, this);

    auto session = ONLINE_ADAPTER_UBISERVICES->getSession();
    US_NS::AuthenticationModule& moduleAuth = US_NS::AuthenticationModule::module(*session);
    US_NS::PlayerNotificationsModule& modulePNOT = US_NS::PlayerNotificationsModule::module(*session);

    m_authenticationNotificationHandler = moduleAuth.createNotificationListener();
	m_customNotificationHandler = modulePNOT.createNotificationListenerCustom();
    m_initialized = true;
}

void SessionService_Ubiservices::terminate()
{
    m_authenticationNotificationHandler = ubiservices::NotificationSource<ubiservices::AuthenticationNotification>::ListenerHandler();
	m_customNotificationHandler = ubiservices::NotificationSource<ubiservices::PlayerNotificationCustom>::ListenerHandler();

    //EVENTMANAGER->unregisterEvent(EventMainUserChanged_CRC,this);
    //EVENTMANAGER->unregisterEvent(EventResumeFromSuspendedState_CRC, this);
    //EVENTMANAGER->unregisterEvent(EventEnterSuspendedState_CRC, this);
    //EVENTMANAGER->unregisterEvent(EventTitleFocusChanged_CRC, this);
    m_initialized = false;
}

void SessionService_Ubiservices::onEvent(Event* _event)
{
}

String8 SessionService_Ubiservices::getUbiservicesEnvName() const
{
    US_NS::AuthenticationModule& moduleAuth = US_NS::AuthenticationModule::module(*ONLINE_ADAPTER_UBISERVICES->getSession());
    if (!m_initialized || !moduleAuth.hasValidSessionInfo())
    {
        return String8("N/A");
    }
    else
    {
        return moduleAuth.getSessionInfo().getEnvironmentName().getUtf8();
    }
}

void SessionService_Ubiservices::setError(const OnlineError &error)
{
    m_sessionStatus = EInternalStatus_Error;
    m_sessionError = error;
    m_nextSessionRetryTime = SYSTEM_ADAPTER->getTime() + ms_sessionRetryInterval;
}

void SessionService_Ubiservices::update()
{
    ITF_ASSERT_CRASH(m_initialized, "error: update is called before initialization is complete");

    if (m_isTransitioningToForeground)
    {
        // while transitioning we don't update anything
        return;
    }

    checkUbiservicesNotifications();

    switch (m_sessionStatus)
    {
    case EInternalStatus_Preparing:
        updatePreparing();
        break;
    case EInternalStatus_Creating:
        updateCreating();
        break;
    case EInternalStatus_Deleting:
        updateDeleting();
        break;
    case EInternalStatus_Ready:
        updateReady();
        break;
    case EInternalStatus_Error:
        updateError();
        break;
    default:
        break;
    }   
}

void SessionService_Ubiservices::checkUbiservicesNotifications()
{
    while (m_ubiservicesNotificationHandler.isNotificationAvailable())
    {
        ubiservices::PlayerNotificationUbiServices notif = m_ubiservicesNotificationHandler.popNotification();

        if (notif.m_type == PlayerNotificationUbiServicesType::NotificationConnect)
        {
        }
        else if (notif.m_type == PlayerNotificationUbiServicesType::NotificationDisconnect)
        {
        }
        else if (notif.m_type == PlayerNotificationUbiServicesType::NotificationMaintenancePending)
        {
        }
        else if (notif.m_type == PlayerNotificationUbiServicesType::NotificationThrottled)
        {
        }
        else if (notif.m_type == PlayerNotificationUbiServicesType::NotificationConnectionFailed)
        {
        }
        else if (notif.m_type == PlayerNotificationUbiServicesType::NotificationMaintenanceStart)
        {
        }
        else if (notif.m_type == PlayerNotificationUbiServicesType::NotificationMaintenanceEnd)
        {
        }
    }

    while (m_authenticationNotificationHandler.isNotificationAvailable())
    {
        ubiservices::AuthenticationNotification notif = m_authenticationNotificationHandler.popNotification();
        if (notif.m_type == ubiservices::AuthenticationNotificationType::SessionInfoChanged)
        {
            m_ubiservicesSessionUpdated = true;
        }
    }

	while (m_customNotificationHandler.isNotificationAvailable())
	{
		ubiservices::PlayerNotificationCustom notif = m_customNotificationHandler.popNotification();
		if (notif.m_type == ubiservices::String("UBICONNECT_VISUAL_NOTIFICATION"))
		{
			ubiservices::Json notifContent(notif.m_content);
			if (notifContent.isValid())
			{
                LOG("[SessionService][checkUbiservicesNotifications] VNOT received %s", notif.m_content.getUtf8());
			}
		}
		else
		{
			LOG("[SessionService][checkUbiservicesNotifications] Unknown custom notification -- type: %s content: %s", notif.m_type.getUtf8(), notif.m_content.getUtf8());
		}
	}
}

void SessionService_Ubiservices::updatePreparing()
{
    // Check pre-requisites: Start with checking the network
    if (NETWORKSERVICES->getNetworkStatus() == NetworkServices::ENetworkStatus_Error)
    {
        OnlineError error(OnlineError::Network, NETWORKSERVICES->getPlatformSpecificErrorCode());
        setError(error);
        return;
    } 
    else if (NETWORKSERVICES->getNetworkStatus() == NetworkServices::ENetworkStatus_Preparing)
    {
        return;
    }

    // Check pre-requisites: Check the account
    if (ACCOUNT_ADAPTER->getTokenStatus() == AccountAdapter::ETokenStatus_Error)
    {
        if (ACCOUNT_ADAPTER->getPlatformSpecificErrorCode())
        {
            OnlineError error(OnlineError::FirstParty, ACCOUNT_ADAPTER->getPlatformSpecificErrorCode());
            setError(error);
        }
        else
        {
            OnlineError error(OnlineError::UbiServer);
            setError(error);
        }
        return;
    } 
    else if (ACCOUNT_ADAPTER->getTokenStatus() == AccountAdapter::ETokenStatus_Preparing)
    {
        return;
    }

    startCreatingSession();
}

void SessionService_Ubiservices::updateCreating()
{
    ITF_ASSERT(m_sessionStatus == EInternalStatus_Creating);

    if (m_createSessionResult.isProcessing())
    {
        return;
    }
    else if (m_createSessionResult.hasSucceeded())
    {
        onCreateSessionSuccess();
    }
    else if (m_createSessionResult.hasFailed())
    {
        const US_NS::AsyncResultError& resultCreateError = m_createSessionResult.getAsyncResultError();
        u32 code = resultCreateError.m_baseError.m_code;

       LOG("[SessionService] error creating: %d / %s", code, resultCreateError.m_baseError.m_description.getUtf8());
        // TODO handle Switch m_specificNative 1st Party

        OnlineError err(OnlineError::ErrorType::UbiServer, code);
        onCreateSessionFailure(err);
    }
}

void SessionService_Ubiservices::updateDeleting()
{
    ITF_ASSERT(m_sessionStatus == EInternalStatus_Deleting);
    if (m_closeSessionResult.isProcessing())
    {
        return;
    }
    else if (m_closeSessionResult.hasSucceeded() || m_closeSessionResult.isCanceled())
    {
        onDeleteSessionSuccess();
    }
    else if (m_closeSessionResult.hasFailed())
    {
        const US_NS::AsyncResultError& resultCloseError = m_closeSessionResult.getAsyncResultError();
        u32 code = resultCloseError.m_baseError.m_code;
        LOG("[SessionService] error deleting: %d / %s", code, resultCloseError.m_baseError.m_description.getUtf8());

        // TODO handle Switch m_specificNative 1st Party
        
        OnlineError err(OnlineError::ErrorType::UbiServer, code);
        onDeleteSessionFailure(err);
    }
}

void SessionService_Ubiservices::updateReady()
{
    if (m_ubiservicesSessionUpdated)
    {
        m_ubiservicesSessionUpdated = false;
        refreshSessionInfo();
        return;
    }

    m_retryInstantlyAfterError = true;
    if (NETWORKSERVICES->getNetworkStatus() != NetworkServices::ENetworkStatus_Ready)
    {
        if (NETWORKSERVICES->getNetworkStatus() == NetworkServices::ENetworkStatus_Error)
        {
            OnlineError error(OnlineError::Network, NETWORKSERVICES->getPlatformSpecificErrorCode());
            setError(error);
        }
        else
        {
            m_sessionStatus = EInternalStatus_Preparing;
        }
    }

    //  This 'SHOULD' take care of PSN sign outs and X1 profile switching
    if (ACCOUNT_ADAPTER->getTokenStatus() != AccountAdapter::ETokenStatus_Ready)
    {
        if (ACCOUNT_ADAPTER->getTokenStatus() == AccountAdapter::ETokenStatus_Error)
        {
            OnlineError error(OnlineError::FirstParty, ACCOUNT_ADAPTER->getPlatformSpecificErrorCode());
            setError(error);
        }
        else
        {
            m_sessionStatus = EInternalStatus_Preparing;
        }
    }

    US_NS::AuthenticationModule& moduleAuth = US_NS::AuthenticationModule::module(*m_session);
    if(!moduleAuth.getSessionInfo().hasValidSession())
    {
        m_sessionStatus = EInternalStatus_Preparing;
    }
}

void SessionService_Ubiservices::updateError()
{
    bool automaticRetry = ms_enableAutomaticRetry && SYSTEM_ADAPTER->getTime() >= m_nextSessionRetryTime;
    if (automaticRetry)
    {
        m_sessionStatus = EInternalStatus_Preparing;
    }
    else if (m_retryInstantlyAfterError)
    {
        if ((NETWORKSERVICES->getNetworkStatus() == NetworkServices::ENetworkStatus_Ready || NETWORKSERVICES->getNetworkStatus() == NetworkServices::ENetworkStatus_Preparing)
            && m_sessionError.getType() == OnlineError::Network)
        {
            m_sessionStatus = EInternalStatus_Preparing;
            m_retryInstantlyAfterError = false;
        }
        else if ((ACCOUNT_ADAPTER->getTokenStatus() == AccountAdapter::ETokenStatus_Ready || ACCOUNT_ADAPTER->getTokenStatus() == AccountAdapter::ETokenStatus_Preparing)
            && m_sessionError.getType() == OnlineError::FirstParty)
        {
            m_sessionStatus = EInternalStatus_Preparing;
            m_retryInstantlyAfterError = false;
        }
    }
}

bool SessionService_Ubiservices::areUbiServicesCredentialsAvailable()
{
#if defined(ITF_WINDOWS)
    return true;

#elif defined(ITF_NX) || defined(ITF_OUNCE)
    return ACCOUNT_ADAPTER_NINTENDO->getActiveAccount().hasValidNsaId();

#elif defined(ITF_PS5)
    const SceUserServiceUserId activeUserId = ACCOUNT_ADAPTER_PS5->getActiveAccount().getSceUserId();
    return activeUserId != SCE_USER_SERVICE_USER_ID_INVALID;

#elif defined(ITF_SCARLETT)
    const XblContextHandle& activeAccountContextHandle = ACCOUNT_ADAPTER_SCARLETT->getActiveAccount().getContextHandle();
    return activeAccountContextHandle != nullptr;
#endif

    return false;
}

ubiservices::PlayerCredentials SessionService_Ubiservices::getUbiServicesCredentials()
{
    ITF_ASSERT(areUbiServicesCredentialsAvailable());

#if defined(ITF_WINDOWS)
#if defined(ITF_UPLAYPC)
       String8 token = UPLAYSERVICE->getToken();
       LOG("[Uplay Ticket] %s", token.cStr());
       return ubiservices::PlayerCredentials(token.cStr(), ubiservices::CredentialsType::UplayPC);
#else
       const Account_win& activeAccount = ACCOUNT_ADAPTER_WIN->getActiveAccount();
       return ubiservices::PlayerCredentials(activeAccount.getEmail().cStr(), activeAccount.getOnlinePassword().cStr());
#endif

#elif defined(ITF_NX) || defined(ITF_OUNCE)
    const Account_Nintendo& activeAccount = ACCOUNT_ADAPTER_NINTENDO->getActiveAccount();
    return ubiservices::PlayerCredentials(activeAccount.getHandle());

#elif defined(ITF_PS5)
    SceUserServiceUserId activeUserId = ACCOUNT_ADAPTER_PS5->getActiveAccount().getSceUserId();
    return ubiservices::PlayerCredentials(activeUserId);

#elif defined(ITF_XBOX_SERIES)
    const XblContextHandle& activeAccountContextHandle = ACCOUNT_ADAPTER_XBOXSERIES->getActiveAccount().getContextHandle();
    ITF_ASSERT_MSG(activeAccountContextHandle != nullptr, "SessionService_Ubiservices::getUbiServicesCredentials() - Account Context Handle is null");
    return ubiservices::PlayerCredentials(activeAccountContextHandle);
#endif
}

UUID SessionService_Ubiservices::convertProfileIdFromUbiservices(const ubiservices::ProfileId& _usId)
{
    UUID profileId;

    if (_usId.isValid())
    {
        const ubiservices::String& usIdString = static_cast<const ubiservices::String&>(_usId);
        profileId.fromString(usIdString.getUtf8());
    }

    return profileId;
}

ubiservices::ProfileId SessionService_Ubiservices::convertProfileIdToUbiservices(const UUID& _profileId)
{
    ubiservices::String usIdString = _profileId.toString().cStr();
    return ubiservices::ProfileId(usIdString);
}


//-------------- Session Creation --------------
void SessionService_Ubiservices::startCreatingSession()
{
    ITF_ASSERT_MSG(NETWORKSERVICES->getNetworkStatus() == NetworkServices::ENetworkStatus_Ready, "SessionService_Ubiservices::startCreatingSession() - NetworkAdapter is not ready.");
    ITF_ASSERT_MSG(ACCOUNT_ADAPTER->getTokenStatus() == AccountAdapter::ETokenStatus_Ready, "SessionService_Ubiservices::startCreatingSession() - AccountAdapter is not ready.");

    m_sessionError = OnlineError();
    m_sessionStatus = EInternalStatus_Creating;

    // skip the network request if possible
    US_NS::AuthenticationModule& moduleAuth = US_NS::AuthenticationModule::module(*m_session);
    const ubiservices::SessionInfo sessionInfo = moduleAuth.getSessionInfo();
    if (sessionInfo.hasValidSession())
    {
        // but first, check if the profile id known by the account adapter is the same as the one in the ubiservices session info, when switching accounts
        if (ACCOUNT_ADAPTER->getActiveAccount().getNetworkId() != convertProfileIdFromUbiservices(sessionInfo.getProfileId()))
        {
            deleteSession();
        }
        else
        {
            onCreateSessionSuccess();
        }
        return;
    }

    US_NS::Vector<US_NS::String> parametersGroupApplication;
    US_NS::Vector<US_NS::String> parametersGroupSpace;

    ITF_VECTOR<String8> parameterGroupNamesTitle = ONLINE_ADAPTER->getParameterGroupNames();
    for (auto itGroupsToFetch = parameterGroupNamesTitle.begin(); itGroupsToFetch != parameterGroupNamesTitle.end(); ++itGroupsToFetch)
    {
        parametersGroupSpace.push_back(ubiservices::String(itGroupsToFetch->cStr()));
    }

    m_createSessionResult = m_session->open(getUbiServicesCredentials(), parametersGroupApplication, parametersGroupSpace);
}

void SessionService_Ubiservices::onCreateSessionSuccess()
{
    ITF_ASSERT(m_sessionStatus == EInternalStatus_Creating);
    refreshSessionInfo();
}

void SessionService_Ubiservices::onCreateSessionFailure(const OnlineError& _error)
{
    if (_error == OnlineError::Cancelled)
    {
        deleteSession();
        return;
    }

    ITF_ASSERT(m_sessionStatus == EInternalStatus_Creating);
    setError(_error);

#if defined(ITF_NX) || defined(ITF_OUNCE)
    // On NX, we don't want to retry instantly after a first party error.
    // It would cause Ubiservices to ask for the nintendo token immediately.
    // Which is not compliant with Nintendo requirements.
    if (_error.getType() == OnlineError::FirstParty) m_retryInstantlyAfterError = false;
#endif
}

//-------------- Fetch Ubiservices Populations and Parameters --------------

// Helper functions
void logUsParameters(const ubiservices::Map<ubiservices::String, ubiservices::ParametersGroupInfo> _usMap)
{
    ITF::String8 stringToLog;
    ubiservices::Map<ubiservices::String, ubiservices::ParametersGroupInfo>::const_iterator itGroup;
    for (itGroup = _usMap.begin(); itGroup != _usMap.end(); ++itGroup)
    {
        stringToLog += '\n';
        stringToLog += itGroup->second.m_fields.renderContent(ubiservices::Json::RenderType::Formatted).getUtf8();
    }

    LOG("Ubiservices: Fetching parameters success %s", stringToLog.cStr());
}

void convertParametersToMap(const ubiservices::Map<ubiservices::String, ubiservices::ParametersGroupInfo> _usMap, ParameterGroupsMap& _itfMap)
{
    ubiservices::Map<ubiservices::String, ubiservices::ParametersGroupInfo>::const_iterator itGroup;
    for (itGroup = _usMap.begin(); itGroup != _usMap.end(); ++itGroup)
    {
        ParametersMap pmap;
        ubiservices::Vector<ubiservices::Json> paramsJson = itGroup->second.m_fields.getItems();
        ubiservices::Vector<ubiservices::Json>::const_iterator itParam;
        for (itParam = paramsJson.begin(); itParam != paramsJson.end(); ++itParam)
        {
            ITF_ASSERT_MSG(itParam->getType() == ubiservices::JsonValueType::Object, "Ubiservices parameter '%s' in group '%s' must be of type Json Object", itParam->getKeyFast(), itGroup->first.getUtf8());
            ubiservices::Vector<ubiservices::Json> fieldsJson = itParam->getItems();
            ubiservices::Vector<ubiservices::Json>::const_iterator itField;
            for (itField = fieldsJson.begin(); itField != fieldsJson.end(); ++itField)
            {
                String8& constantValue = _itfMap[StringID(itGroup->first.getUtf8())][StringID(itParam->getKeyFast())][StringID(itField->getKeyFast())];
                if (itField->isTypeBoolean())
                {
                    constantValue.setTextFormat("%u", itField->getValueBoolean());
                }
                else if (itField->isTypeNumber())
                {
                    constantValue.setTextFormat("%f", itField->getValueDouble());
                }
                else if (itField->isTypeArray())
                {
                    constantValue.setTextFormat("%s", itField->renderContent().getUtf8());
                }
                else
                {
                    constantValue = itField->getValueStringFast();
                }
            }

        }
    }
}

void SessionService_Ubiservices::onFetchPopulationsSuccess(const ubiservices::Vector<ubiservices::PopulationInfo>& _populationInfo)
{
    ITF_ASSERT(m_sessionStatus == EInternalStatus_Creating);
    LOG("Ubiservices: Fetching populations success");

    m_populationsInfos.clear();
    m_usFormatPopulationInfo = _populationInfo;
    ubiservices::Vector<ubiservices::PopulationInfo>::const_iterator it;
    for (it = _populationInfo.begin(); it != _populationInfo.end(); ++it)
    {
        PopulationInfo pop;
        pop.m_name = it->m_name.getUtf8();
        pop.m_subject = it->m_subject.getUtf8();
        pop.m_object = it->m_object.renderContent().getUtf8();
        pop.m_spaceId = ubiservices::String(it->m_spaceId).getUtf8();
        m_populationsInfos.push_back(pop);
    }

    // Setup US parameters
    ITF_VECTOR<String8> parameterGroupNamesTitle = ONLINE_ADAPTER->getParameterGroupNames();
    ubiservices::Vector<ubiservices::String> parameterGroupNames;
    ITF_VECTOR<String8>::const_iterator itGroupsToFetch;
    for (itGroupsToFetch = parameterGroupNamesTitle.begin(); itGroupsToFetch != parameterGroupNamesTitle.end(); ++itGroupsToFetch)
    {
        parameterGroupNames.push_back(ubiservices::String(itGroupsToFetch->cStr()));
    }

    US_NS::ParametersModule& moduleParam = US_NS::ParametersModule::module(*m_session);
    logUsParameters(moduleParam.getParametersGroupsSpace(parameterGroupNames));
    convertParametersToMap(moduleParam.getParametersGroupsSpace(parameterGroupNames), m_usParameterGroups);
}

void SessionService_Ubiservices::onFetchPopulationsFailure(const OnlineError& _error)
{
    ITF_ASSERT(m_sessionStatus == EInternalStatus_Creating);
    LOG("Fetching populations failed with error: \"%s\"", _error.toString().cStr());
    setError(_error);
}

//-------------- Session Deletion --------------
void SessionService_Ubiservices::deleteSession()
{
    ITF_ASSERT(m_sessionStatus == EInternalStatus_Ready ||
        m_sessionStatus == EInternalStatus_Creating);

    m_sessionStatus = EInternalStatus_Deleting;
    m_closeSessionResult = m_session->close();
}

void SessionService_Ubiservices::onDeleteSessionSuccess()
{
    ITF_ASSERT(m_sessionStatus == EInternalStatus_Deleting);
    m_sessionStatus = EInternalStatus_Preparing;
}

void SessionService_Ubiservices::onDeleteSessionFailure(const OnlineError& _error)
{
    ITF_ASSERT(m_sessionStatus == EInternalStatus_Deleting);
    setError(_error);
}

//-------------- External session status --------------
SessionService_Ubiservices::ESessionStatus SessionService_Ubiservices::getSessionStatus() const
{
    if (m_isTransitioningToForeground)
    {
        return SessionService::ESessionStatus_Preparing;
    }

    switch(m_sessionStatus)
    {
        case EInternalStatus_Error:
            return SessionService::ESessionStatus_Error;
        case EInternalStatus_Deleting:
        case EInternalStatus_Preparing:
        case EInternalStatus_Creating:
            return SessionService::ESessionStatus_Preparing;
        case EInternalStatus_Ready:
            return SessionService::ESessionStatus_Valid;
        default:
            return SessionService::ESessionStatus_Idle;
    }
}

bool SessionService_Ubiservices::isInPopulation(const String8& _subject, const String8& _popName) const
{
    for (const PopulationInfo &populationInfo : m_populationsInfos)
    {
        if (populationInfo.m_subject == _subject)
        {
            return populationInfo.m_name == _popName;
        }
    }
    return false;
}

bool SessionService_Ubiservices::isClubFeatureSwitchEnabled() const
{
    ITF_ASSERT_MSG(isSessionValid(), "isClubFeatureSwitchEnabled should not be called if online session is not valid");
    if (!isSessionValid()) { return false; }

    US_NS::ParametersModule& moduleParam = US_NS::ParametersModule::module(*m_session);

    return moduleParam.isUsFeatureSwitchEnabled(ubiservices::FeatureSwitchId(ubiservices::FeatureSwitchId::ClubService));
}

bool SessionService_Ubiservices::isNewsFeatureSwitchEnabled() const
{
    ITF_ASSERT_MSG(isSessionValid(), "isNewsFeatureSwitchEnabled should not be called if online session is not valid");
    if (!isSessionValid()) { return false; }

    US_NS::ParametersModule& moduleParam = US_NS::ParametersModule::module(*m_session);

    return moduleParam.isUsFeatureSwitchEnabled(ubiservices::FeatureSwitchId(ubiservices::FeatureSwitchId::News));
}

bool SessionService_Ubiservices::getConstantValue(String8 _parameterGroupName, StringID _parameterName, StringID _fieldName, f32 &_fieldValue) const
{
    String8 stringValue;
    if (!getConstantValueAsString(_parameterGroupName, _parameterName, _fieldName, stringValue))
        return false;

    _fieldValue = stringValue.atof32();
    return true;
}

bool SessionService_Ubiservices::getConstantValue(String8 _parameterGroupName, StringID _parameterName, StringID _fieldName, u32 &_fieldValue) const
{
    String8 stringValue;
    if (!getConstantValueAsString(_parameterGroupName, _parameterName, _fieldName, stringValue))
        return false;

    ITF::f32 floatValue = stringValue.atof32();
    _fieldValue = (ITF::u32)floatValue;
    return true;
}

bool SessionService_Ubiservices::getConstantValue(String8 _parameterGroupName, StringID _parameterName, StringID _fieldName, i32 &_fieldValue) const
{
    String8 stringValue;
    if (!getConstantValueAsString(_parameterGroupName, _parameterName, _fieldName, stringValue))
        return false;

    ITF::f32 floatValue = stringValue.atof32();
    _fieldValue = (ITF::i32)floatValue;
    return true;
}

bool SessionService_Ubiservices::getConstantValue(String8 _parameterGroupName, StringID _parameterName, StringID _fieldName, String8 &_fieldValue) const
{
    if (!getConstantValueAsString(_parameterGroupName, _parameterName, _fieldName, _fieldValue))
        return false;
    return true;
}

bool SessionService_Ubiservices::getConstantValue(String8 _parameterGroupName, StringID _parameterName, StringID _fieldName, bool &_fieldValue) const
{
    String8 stringValue;
    if (!getConstantValueAsString(_parameterGroupName, _parameterName, _fieldName, stringValue))
        return false;

    _fieldValue = (stringValue.atoi32() != 0);
    return true;
}

bool SessionService_Ubiservices::getConstantValue(String8 _parameterGroupName, StringID _parameterName, StringID _fieldName, ITF_VECTOR<String8> &_fieldValue) const
{
    String8 stringValue;
    if (!getConstantValueAsString(_parameterGroupName, _parameterName, _fieldName, stringValue)) return false;
    
    ubiservices::String usStringValue(stringValue.cStr());
    ubiservices::Json usJsonValue(usStringValue);
    if (!usJsonValue.isValid()) return false;

    _fieldValue.clear();
    ubiservices::Vector<ubiservices::Json> jsonArray = usJsonValue.getValueArray();
    ubiservices::Vector<ubiservices::Json>::const_iterator it;
    for (it = jsonArray.begin(); it != jsonArray.end(); ++it)
    {
        _fieldValue.push_back(it->getValueStringFast());
    }

    return true;
}

bool SessionService_Ubiservices::getConstantValue(String8 _parameterGroupName, StringID _parameterName, StringID _fieldName, ITF_VECTOR<StringID> &_fieldValue) const
{
    String8 stringValue;
    if (!getConstantValueAsString(_parameterGroupName, _parameterName, _fieldName, stringValue)) return false;

    ubiservices::String usStringValue(stringValue.cStr());
    ubiservices::Json usJsonValue(usStringValue);
    if (!usJsonValue.isValid()) return false;

    _fieldValue.clear();
    ubiservices::Vector<ubiservices::Json> jsonArray = usJsonValue.getValueArray();
    ubiservices::Vector<ubiservices::Json>::const_iterator it;
    for (it = jsonArray.begin(); it != jsonArray.end(); ++it)
    {
        if (it->isTypeString())
        {
            _fieldValue.push_back(StringID(it->getValueStringFast()));
        }
        else
        {
            _fieldValue.push_back(StringID(it->getValueUnsignedInteger()));
        }
    }

    return true;
}

bool SessionService_Ubiservices::getConstantValue(String8 _parameterGroupName, StringID _parameterName, StringID _fieldName, ITF_VECTOR<u32> &_fieldValue) const
{
    String8 stringValue;
    if (!getConstantValueAsString(_parameterGroupName, _parameterName, _fieldName, stringValue)) return false;

    ubiservices::String usStringValue(stringValue.cStr());
    ubiservices::Json usJsonValue(usStringValue);
    if (!usJsonValue.isValid()) return false;

    _fieldValue.clear();
    ubiservices::Vector<ubiservices::Json> jsonArray = usJsonValue.getValueArray();
    ubiservices::Vector<ubiservices::Json>::const_iterator it;
    for (it = jsonArray.begin(); it != jsonArray.end(); ++it)
    {
        _fieldValue.push_back(it->getValueUnsignedInteger());
    }

    return true;
}

bool SessionService_Ubiservices::getConstantValue(String8 _parameterGroupName, StringID _parameterName, StringID _fieldName, ITF_VECTOR<i32> &_fieldValue) const
{
    String8 stringValue;
    if (!getConstantValueAsString(_parameterGroupName, _parameterName, _fieldName, stringValue)) return false;

    ubiservices::String usStringValue(stringValue.cStr());
    ubiservices::Json usJsonValue(usStringValue);
    if (!usJsonValue.isValid()) return false;

    _fieldValue.clear();
    ubiservices::Vector<ubiservices::Json> jsonArray = usJsonValue.getValueArray();
    ubiservices::Vector<ubiservices::Json>::const_iterator it;
    for (it = jsonArray.begin(); it != jsonArray.end(); ++it)
    {
        _fieldValue.push_back(it->getValueInteger());
    }

    return true;
}

bool SessionService_Ubiservices::getConstantValue(String8 _parameterGroupName, StringID _parameterName, StringID _fieldName, ITF_VECTOR<bool> &_fieldValue) const
{
    String8 stringValue;
    if (!getConstantValueAsString(_parameterGroupName, _parameterName, _fieldName, stringValue)) return false;

    ubiservices::String usStringValue(stringValue.cStr());
    ubiservices::Json usJsonValue(usStringValue);
    if (!usJsonValue.isValid()) return false;

    _fieldValue.clear();
    ubiservices::Vector<ubiservices::Json> jsonArray = usJsonValue.getValueArray();
    ubiservices::Vector<ubiservices::Json>::const_iterator it;
    for (it = jsonArray.begin(); it != jsonArray.end(); ++it)
    {
        _fieldValue.push_back(it->getValueBoolean());
    }

    return true;
}

bool SessionService_Ubiservices::getConstantValueAsString(String8 _parameterGroupName, StringID _parameterName, StringID _fieldName, String8& _stringValue) const
{
    ParameterGroupsMap::const_iterator parameterGroup = m_usParameterGroups.find(StringID(_parameterGroupName));
    if (parameterGroup == m_usParameterGroups.end())
        return false;

    ParametersMap::const_iterator parameter = parameterGroup->second.find(_parameterName);
    if (parameter == parameterGroup->second.end())
        return false;

    FieldsMap::const_iterator field = parameter->second.find(_fieldName);
    if (field == parameter->second.end())
        return false;

    _stringValue = field->second;
    return true;
}

void SessionService_Ubiservices::requestRefresh()
{
    if (m_sessionStatus == EInternalStatus_Error && SYSTEM_ADAPTER->getTime() >= m_nextRequestRefreshTime)
    {
        m_nextRequestRefreshTime = SYSTEM_ADAPTER->getTime() + ms_requestRefreshInterval;
        NETWORKSERVICES->requestRefresh();
        m_sessionStatus = EInternalStatus_Preparing;
    }
}

void SessionService_Ubiservices::forceRequestRefresh()
{
    if (m_sessionStatus == EInternalStatus_Error)
    {
        // Update the timer anyway so that a requestRefresh just after the force is bypassed (count as a RequestRefresh).
        m_nextRequestRefreshTime = SYSTEM_ADAPTER->getTime() + ms_requestRefreshInterval;
        NETWORKSERVICES->requestRefresh();
        m_sessionStatus = EInternalStatus_Preparing;
    }
}

void SessionService_Ubiservices::refreshSessionInfo()
{
    US_NS::AuthenticationModule& moduleAuth = US_NS::AuthenticationModule::module(*m_session);
    const ubiservices::SessionInfo sessionInfo = moduleAuth.getSessionInfo();
    const bool hasValidSession = sessionInfo.hasValidSession();
    if (hasValidSession)
    {
        LOG("Ubiservices session info:\n"
            "   - Environment: %s\n"
            "   - Ticket     : %s\n"
            "   - Expired?   : %s",
            sessionInfo.getEnvironmentName().getUtf8(),
            sessionInfo.getTicket().getUtf8(),
            sessionInfo.isExpired() ? "true" : "false");
        
        ACCOUNT_ADAPTER->setActiveAccountNetworkId(convertProfileIdFromUbiservices(sessionInfo.getProfileId()));
        ACCOUNT_ADAPTER->setActiveAccountLinkedToUplay(sessionInfo.hasUserAccountLinked());

        LOG("The UbiServices properties has been refreshed on account:");
        ACCOUNT_ADAPTER->getActiveAccount().dumpToLog();

        if (sessionInfo.hasUserAccountLinked())
        {
            // Fetch Uplay name
        }
        else
        {
            ACCOUNT_ADAPTER->setActiveAccountNameOnUplay(String8::emptyString);
        }
    }
}

void  SessionService_Ubiservices::transitionToSuspended ()
{
#if !defined(ITF_PS5) && !defined(ITF_XBOX_SERIES)
    ubiservices::PlatformHelper::changeState( ubiservices::PlatformState::Suspended );
#endif
}

void  SessionService_Ubiservices::transitionToForeground ()
{
    m_isTransitioningToForeground = true;
}

void SessionService_Ubiservices::onFetchUplayNameSuccess(const String8& _nameOnUplay)
{
    ACCOUNT_ADAPTER->setActiveAccountNameOnUplay(_nameOnUplay);
}

void SessionService_Ubiservices::onFetchUplayNameFailure(const OnlineError& /*_error*/)
{
    ACCOUNT_ADAPTER->setActiveAccountNameOnUplay(String8::emptyString);
}

bool SessionService_Ubiservices::launchConnect(const String8& _deepLink,
    std::list<std::pair<String8, String8> > _params)
{
    if (!m_initialized || !m_session)
        return false;

    US_NS::AuthenticationModule& modAuth = US_NS::AuthenticationModule::module(*m_session);
    if (!modAuth.hasValidSessionInfo())
        return false;

    // Win uses UPC_ * api / UplayService overlay directly
#if !defined(ITF_WINDOWS)
    US_NS::String microAppId(_deepLink.cStr());
    US_NS::Vector<std::pair<US_NS::String, US_NS::String> > microAppParameters;

    for (auto& param : _params)
    {
        US_NS::String first(param.first.cStr());
        US_NS::String second(param.second.cStr());
        microAppParameters.push_back(std::pair<US_NS::String, US_NS::String>(first, second));
    }

    US_NS::UbisoftConnectModule& modConnect = US_NS::UbisoftConnectModule::module(*m_session);

    auto result = modConnect.launchUbisoftConnect(microAppId, microAppParameters);
    while (result.isProcessing())
    {
        sleep(100);
    }

    if (result.hasFailed())
    {
        setError(OnlineError());
    }

    return result.hasSucceeded();
#endif

    return false;
}

} // namespace ITF

#endif // ITF_SUPPORT_UBISERVICES
