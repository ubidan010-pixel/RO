#include "precompiled_AccountAdapter_xboxseries.h"

#include "adapters/AccountAdapter_xboxseries/AccountAdapter_xboxseries.h"
#include "engine/AdaptersInterfaces/TRCManager_Adapter.h"
#include "engine/events/EventManager.h"
#include "engine/events/Events.h"

#include "core/UnicodeTools.h"

#include <XUser.h>

namespace
{
    void onUserChangedEvent(void* context, XUserLocalId userLocalId, XUserChangeEvent event)
    {
        ITF::AccountAdapter_xboxseries* pAccountAdapterXS = static_cast<ITF::AccountAdapter_xboxseries*>(context);

        if (userLocalId.value == pAccountAdapterXS->getActiveAccount().getLocalUserId().value)
        {
            switch (event)
            {
            case XUserChangeEvent::SigningOut:
            {
                pAccountAdapterXS->onSignedOut();
                break;
            }
            case XUserChangeEvent::SignedInAgain:
            case XUserChangeEvent::SignedOut:
            case XUserChangeEvent::Gamertag:
            case XUserChangeEvent::GamerPicture:
            case XUserChangeEvent::Privileges:
            {
                break;
            }
            default:
            {
                ITF_ASSERT_MSG(false, "[AccountAdapter] onUserChangedEvent : Unknown XUserChangeEvent %u", event);
            }
            }
        }
    }
}

namespace ITF
{
    AccountAdapter_xboxseries::AccountAdapter_xboxseries()
        : AccountAdapter()
        , m_mainUserChanged(bfalse)
        , m_taskQueue(nullptr)
    {
        m_activeAccountInd = U32_INVALID;

        m_registrationToken.token = 0;
    }

    AccountAdapter_xboxseries::~AccountAdapter_xboxseries()
    {
    }

    void AccountAdapter_xboxseries::initialize()
    {
        HRESULT hr = XTaskQueueCreate(XTaskQueueDispatchMode::ThreadPool, XTaskQueueDispatchMode::Manual, &m_taskQueue);
        if (FAILED(hr))
        {
            ITF_ASSERT_MSG(false, "[AccountAdapter] XTaskQueueCreate failed with error %ld", hr);
            setTokenStatus(AccountAdapter::ETokenStatus_Error);
            return;
        }

        hr = XUserGetMaxUsers(&m_maxNbAccounts);
        if (FAILED(hr))
        {
            ITF_ASSERT_MSG(false, "[AccountAdapter] XUserGetMaxUsers failed with error %ld", hr);
            setTokenStatus(AccountAdapter::ETokenStatus_Error);
            return;
        }

        hr = XUserRegisterForChangeEvent(m_taskQueue, this, onUserChangedEvent, &m_registrationToken);
        if (FAILED(hr))
        {
            ITF_ASSERT_MSG(false, "[AccountAdapter] XUserRegisterForChangeEvent failed with error %ld", hr);
            setTokenStatus(AccountAdapter::ETokenStatus_Error);
            return;
        }

        this->signinMainUser(XUserAddOptions::AddDefaultUserSilently);
    }

    void AccountAdapter_xboxseries::update()
    {
        // Manually poll events
        while (XTaskQueueDispatch(m_taskQueue, XTaskQueuePort::Completion, 0)) // 0 ms timeout (we don't wait)
        {
        }

        if (m_mainUserChanged)
        {
            m_mainUserChanged = bfalse;
            resetActiveAccountProperties();

            //EventMainUserChanged evt;
            //EVENTMANAGER->broadcastEvent(&evt);
        }

        AccountAdapter::update();
    }

    void AccountAdapter_xboxseries::terminate()
    {
        if (m_taskQueue)
        {
            XTaskQueueTerminate(m_taskQueue, false, nullptr, nullptr);
            XTaskQueueCloseHandle(m_taskQueue);
        }

        XUserUnregisterForChangeEvent(m_registrationToken, false);
    }

    void AccountAdapter_xboxseries::selectAccountThroughSystemUI(const bbool _silently /* = bfalse */)
    {
        signinMainUser(_silently ? XUserAddOptions::AddDefaultUserAllowingUI : XUserAddOptions::None);
    }

    void AccountAdapter_xboxseries::signinMainUser(XUserAddOptions _options = XUserAddOptions::None)
    {
        setTokenStatus(AccountAdapter::ETokenStatus_Preparing);

        if (!m_taskQueue)
        {
            LOG("[AccountAdapter] Missing internal tasks queue m_taskQueue");
            ITF_ASSERT_MSG(false, "[AccountAdapter] Missing internal tasks queue m_taskQueue");
            setTokenStatus(AccountAdapter::ETokenStatus_Error);
            return;
        }

        // Setup async block and function
        XAsyncBlock* asyncBlock = newAlloc(mId_System, XAsyncBlock);
        if (!asyncBlock)
        {
            LOG("[AccountAdapter] Unable to alloc XAsyncBlock");
            ITF_ASSERT_MSG(false, "[AccountAdapter] Unable to alloc XAsyncBlock");
            setTokenStatus(AccountAdapter::ETokenStatus_Error);
            return;
        }

        asyncBlock->queue = m_taskQueue;
        asyncBlock->context = this;
        asyncBlock->callback = [](XAsyncBlock* asyncBlock)
            {
                AccountAdapter_xboxseries* pThis = static_cast<AccountAdapter_xboxseries*>(asyncBlock->context);

                XUserHandle newUser = nullptr;
                HRESULT result = XUserAddResult(asyncBlock, &newUser);
                if (SUCCEEDED(result))
                {
                    pThis->onSigninMainUserSuccess(newUser);
                }
                else if (result == E_ABORT)
                {
                    pThis->setTokenStatus(AccountAdapter::ETokenStatus_Ready);
                }
                else
                {
                    pThis->onSigninMainUserFailed();
                }

                ITF_ASSERT(pThis->getTokenStatus() != AccountAdapter::ETokenStatus_Preparing);

                ITF_DELETE(asyncBlock);
            };

        // Request to silently sign in the default user.
        HRESULT hr = XUserAddAsync(_options, asyncBlock);
        if (FAILED(hr))
        {
            LOG("[AccountAdapter] Unable to add user with XUserAddAsync");
            ITF_ASSERT_MSG(false, "[AccountAdapter] Unable to add user with XUserAddAsync");
            setTokenStatus(AccountAdapter::ETokenStatus_Error);
            ITF_DELETE(asyncBlock);
        }
    }

    void AccountAdapter_xboxseries::onSigninMainUserSuccess(XUserHandle _user)
    {
        // In practice, we only have one account for now (so main user is always at id=0)
        m_activeAccountInd = 0;
        Account_xboxseries &account = m_accounts[m_activeAccountInd];

        // Destroy the previous account handle (e.g., XUserHandle)
        account.destroyXUserHandle();
        account.destroyContextHandle();

        account.setXUserHandle(_user);
        account.ensureCreateContextHandle();

        XUserLocalId localId = { U64_INVALID };
        XUserGetLocalId(_user, &localId);
        account.setLocalUserId(localId);

        uint64_t XboxLiveID = U64_INVALID;
        XUserGetId(_user, &XboxLiveID);
        account.setXboxLiveId(XboxLiveID);

        XUserState state;
        XUserGetState(_user, &state);
        bool isSignedIn = state == XUserState::SignedIn;
        account.setSignedOnPlatformNetwork(isSignedIn);

        bool isGuest = false;
        XUserGetIsGuest(_user, &isGuest);
        account.setIsGuestAccount(isGuest);
        ITF_ASSERT_MSG(isGuest == false, "[AccountAdapter] The main account is not allowed to be a guest account");

        char gamertag[16];
        size_t gamertag_size = 0;
        XUserGetGamertag(_user, XUserGamertagComponent::Classic, 16, gamertag, &gamertag_size);
        account.setName(gamertag);

        XUserAgeGroup ageGroup = XUserAgeGroup::Unknown;
        XUserGetAgeGroup(_user, &ageGroup);
        switch (ageGroup)
        {
        case XUserAgeGroup::Adult:
            account.setAgeGroup(Account::EAgeGroup_Adult);
            break;

        case XUserAgeGroup::Teen:
            account.setAgeGroup(Account::EAgeGroup_Teenager);
            break;

        case XUserAgeGroup::Child:
            account.setAgeGroup(Account::EAgeGroup_Child);
            break;

        default:
            LOG("[AccountAdapter] Invalid XUserAgeGroup %d", ageGroup);
            ITF_ASSERT_MSG(false, "[AccountAdapter] Invalid XUserAgeGroup %d", ageGroup);
            account.setAgeGroup(Account::EAgeGroup_Adult);
            break;
        }

        account.setPermissions(Account::EPermissions_All);
        if (account.getAgeGroup() == Account::EAgeGroup_Child)
        {
            account.removePermissions(Account::EPermissions_UplayCreation);
        }

        HRESULT hr;
        XUserPrivilegeDenyReason denyReason = XUserPrivilegeDenyReason::None;
        bool hasPrivilege = false;

        hasPrivilege = false;
        hr = XUserCheckPrivilege(_user, XUserPrivilegeOptions::AllUsers, XUserPrivilege::Communications, &hasPrivilege, &denyReason);
        if (!hasPrivilege || hr != S_OK)
        {
            account.removePermissions(Account::EPermissions_OnlineInteraction);
        }

        hasPrivilege = false;
        hr = XUserCheckPrivilege(_user, XUserPrivilegeOptions::AllUsers, XUserPrivilege::UserGeneratedContent, &hasPrivilege, &denyReason);
        if (!hasPrivilege || hr != S_OK)
        {
            account.removePermissions(Account::EPermissions_UserCreatedContent);
            account.removePermissions(Account::EPermissions_MultimediaSharing);
        }

        hasPrivilege = false;
        hr = XUserCheckPrivilege(_user, XUserPrivilegeOptions::AllUsers, XUserPrivilege::SocialNetworkSharing, &hasPrivilege, &denyReason);
        if (!hasPrivilege || hr != S_OK)
        {
            account.removePermissions(Account::EPermissions_SocialNetworkSharing);
        }

        hasPrivilege = false;
        hr = XUserCheckPrivilege(_user, XUserPrivilegeOptions::AllUsers, XUserPrivilege::CrossPlay, &hasPrivilege, &denyReason);
        if (!hasPrivilege || hr != S_OK)
        {
            account.removePermissions(Account::EPermissions_CrossPlatformGameplay);
        }

        account.dumpToLog();

        setTokenStatus(AccountAdapter::ETokenStatus_Ready);

        setMainUserChangedEvent();
    }

    void AccountAdapter_xboxseries::onSigninMainUserFailed()
    {
        m_activeAccountInd = U32_INVALID;
        setTokenStatus(AccountAdapter::ETokenStatus_Error);
    }

    void AccountAdapter_xboxseries::onSignedOut()
    {
        m_activeAccountInd = U32_INVALID;
        // In the current implementation, only one user is signin the game: on signout, the AccountAdapter is in error.
        setTokenStatus(AccountAdapter::ETokenStatus_Error);
        setMainUserChangedEvent();
    }

    void AccountAdapter_xboxseries::onSuspend()
    {
        if (isMainActiveAccountValid())
        {
            accessActiveAccount().destroyContextHandle();
        }
    }

    void AccountAdapter_xboxseries::onResume()
    {
        if (isMainActiveAccountValid())
        {
            accessActiveAccount().ensureCreateContextHandle();
        }
    }

} // namespace ITF

