#include "precompiled_AccountAdapter_Nintendo.h"

#include "adapters/AccountAdapter_Nintendo/AccountAdapter_Nintendo.h"
#include "core/error/ErrorHandler.h"
#include "engine/AdaptersInterfaces/TRCManager_Adapter.h"
#include "engine/events/EventManager.h"
#include "engine/events/Events.h"

namespace ITF
{
    AccountAdapter_Nintendo::AccountAdapter_Nintendo()
        : AccountAdapter()
        , m_activeAccount()
        , m_platformErrorCode(0)
        , m_accountRefreshNeeded(false)
        , m_openPreselectedAccount(false)      // set true if SystemAdapter is not opening it first
    {
    }

    AccountAdapter_Nintendo::~AccountAdapter_Nintendo()
    {
        SYSTEM_ADAPTER_NINTENDO->unregisterNotificationListener(this);
    }

    void AccountAdapter_Nintendo::initialize()
    {
        nn::account::Initialize();
        m_activeAccountInd = 0;

        SYSTEM_ADAPTER_NINTENDO->registerNotificationListener(this);

        if (m_openPreselectedAccount)
        {
            nn::account::UserHandle userHandle;

            bool userIsOpened = TryOpenPreselectedUser(&userHandle);
            ITF_VERIFY(userIsOpened);

            if (userIsOpened)
            {
                nn::account::Uid accountUid;
                nn::Result result = nn::account::GetUserId(&accountUid, userHandle);
                ITF_VERIFY(result.IsSuccess());

                if (result.IsSuccess())
                {
                    LOG("[AccountAdapter] Using preselected user \"%s\" for the active account", Account_Nintendo::uidToString(accountUid).cStr());
                    switchActiveAccount(accountUid, &userHandle);
                }
            }
        }
        else // when sysadapter already opened it, TryOpenPreselectedUser would crash, despite "noexcept" :/
        {
            nn::account::Uid accountUid = SYSTEM_ADAPTER_NINTENDO->getInitialUserId();
            nn::account::UserHandle userHandle = SYSTEM_ADAPTER_NINTENDO->getInitialUserHandle();
            nn::Result result = nn::account::GetUserId(&accountUid, userHandle);
            ITF_VERIFY(result.IsSuccess());

            if (result.IsSuccess())
            {
                LOG("[AccountAdapter] Using preselected user \"%s\" for the active account", Account_Nintendo::uidToString(accountUid).cStr());
                switchActiveAccount(accountUid, &userHandle);
            }
        }
    }

    void AccountAdapter_Nintendo::update()
    {
        AccountAdapter::update();

        if (m_accountRefreshNeeded)
        {
            m_accountRefreshNeeded = false;

            if (isMainActiveAccountValid())
            {
                refreshActiveAccount();
            }
        }
    }

    void AccountAdapter_Nintendo::terminate()
    {
    }

    void AccountAdapter_Nintendo::switchActiveAccount(const nn::account::Uid& _accountUid, const nn::account::UserHandle* _userHandle)
    {
        if (_accountUid != m_activeAccount.getUid())
        {
            closeActiveAccount();

            m_activeAccount.setUid(_accountUid);

            // actual Age check requires game title to be configured to request that permission on each start / it's intrusive
            m_activeAccount.setAgeGroup(Account::EAgeGroup_Adult);

            if (_userHandle != nullptr)
            {
                m_activeAccount.setHandle(*_userHandle);
            }
            else
            {
                openActiveAccount();
            }

            refreshActiveAccount();

            //EventMainUserChanged evt;
            //EVENTMANAGER->broadcastEvent(&evt);
            resetActiveAccountProperties();
        }
    }

    void AccountAdapter_Nintendo::openActiveAccount()
    {
        if (m_activeAccount.hasValidUid())
        {
            nn::account::UserHandle activeHandle;
            nn::Result nnResult = nn::account::OpenUser(&activeHandle, m_activeAccount.getUid());
            ITF_VERIFY(nnResult.IsSuccess());

            if (nnResult.IsSuccess())
            {
                m_activeAccount.setHandle(activeHandle);
                LOG("[AccountAdapter] Active account \"%s\" has been opened", m_activeAccount.getUidAsString().cStr());
            }
            else
            {
                LOG("[AccountAdapter] Can't open active account \"%s\"", m_activeAccount.getUidAsString().cStr());
            }
        }
        else
        {
            LOG("[AccountAdapter] No valid account to open");
        }
    }

    void AccountAdapter_Nintendo::closeActiveAccount()
    {
        if (m_activeAccount.hasValidUid())
        {
            nn::account::CloseUser(m_activeAccount.getHandle());
            LOG("[AccountAdapter] Active account \"%s\" has been closed", m_activeAccount.getName().cStr());
            setTokenStatus(ETokenStatus_Preparing);
            m_platformErrorCode = 0;
            m_activeAccount.setSignedOnPlatformNetwork(false);
            m_activeAccount.invalidateNsaId();
            m_activeAccount.invalidateHandle();
        }
        else
        {
            LOG("[AccountAdapter] No valid account to close");
        }
    }

    void AccountAdapter_Nintendo::onSystemNotification(const nn::oe::Message& _message)
    {
        if (_message == nn::oe::MessageFocusStateChanged)
        {
            nn::oe::FocusState state = nn::oe::GetCurrentFocusState();
            if (state == nn::oe::FocusState_InFocus)
            {
                m_accountRefreshNeeded = true;
            }
        }
    }

    void AccountAdapter_Nintendo::refreshActiveAccount()
    {
        nn::account::Nickname userNickname;
        nn::Result result = nn::account::GetNickname(&userNickname, m_activeAccount.getUid());
        ITF_VERIFY_MSG(result.IsSuccess(), "nn::account::GetNickname() returned 0x%08X", result.GetInnerValueForDebug());

        if (result.IsSuccess())
        {
            const char* accountName = userNickname.name;
            if (m_activeAccount.getName() != accountName)
            {
                m_activeAccount.setName(accountName);
            }
        }

        if (m_activeAccount.hasValidHandle())
        {
            nn::account::NetworkServiceAccountId nsaId;
            result = nn::account::GetNetworkServiceAccountId(&nsaId, m_activeAccount.getHandle());
            if (result.IsSuccess())
            {
                // If the NSA ID has changed (could it happen?)
                if (nsaId != m_activeAccount.getNsaId())
                {
                    m_activeAccount.setNsaId(nsaId);
                    m_activeAccount.setSignedOnPlatformNetwork(true);

                    m_platformErrorCode = 0;
                    setTokenStatus(ETokenStatus_Ready);
                }
                else
                {
                    ITF_ASSERT(m_activeAccount.isSignedOnPlatformNetwork());
                }

                LOG("[AccountAdapter] Active account has NSA ID: \"%s\"", m_activeAccount.getPlatformId().cStr());
            }
            else
            {
                m_activeAccount.invalidateNsaId();
                m_activeAccount.setSignedOnPlatformNetwork(false);

                m_platformErrorCode = result.GetInnerValueForDebug();
                setTokenStatus(ETokenStatus_Error);
                LOG("[AccountAdapter] Active account has no NSA ID");
            }
        }

        LOG("[AccountAdapter] The active account has been refreshed:");
        m_activeAccount.dumpToLog();
    }

} // namespace ITF
