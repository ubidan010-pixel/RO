#include "precompiled_AccountAdapter_PS5.h"

#include "adapters/AccountAdapter_PS5/AccountAdapter_PS5.h"
#include "adapters/InputAdapter_PS5/InputAdapter_PS5.h"

#include "engine/AdaptersInterfaces/TRCManager_Adapter.h"
#include "engine/events/EventManager.h"
#include "engine/events/Events.h"

#include <np.h>
#include <np/np_common.h>
#include <np/np_webapi2.h>
#include <user_service.h>


namespace ITF
{
    AccountAdapter_PS5::AccountAdapter_PS5()
        : AccountAdapter()
    {
    }

    void AccountAdapter_PS5::initialize()
    {
        handleUserEvents(); // Initialize the account list

        SceUserServiceUserId initialUserId = SCE_USER_SERVICE_USER_ID_INVALID;
        int32_t ret = sceUserServiceGetInitialUser(&initialUserId);
        ITF_ASSERT(ret == SCE_OK);
        ITF_UNUSED(ret);
        m_activeAccountInd = getAccountIndex(initialUserId);
        ITF_ASSERT_MSG(m_activeAccountInd != U32_INVALID, "Active user not found!");
    }

    void AccountAdapter_PS5::update()
    {
        handleUserEvents();
        sceNpCheckCallback();
        AccountAdapter::update();
    }

    void AccountAdapter_PS5::terminate()
    {
    }

    const Account_PS5* AccountAdapter_PS5::getAccount(u32 _accountIndex)
    {
        if (_accountIndex < ITF_ARRAY_SIZE(m_accounts))
        {
            const Account_PS5& account = m_accounts[_accountIndex];
            if (account.getSceUserId() != SCE_USER_SERVICE_USER_ID_INVALID)
            {
                return &account;
            }
        }

        return nullptr;
    }

    const Account_PS5& AccountAdapter_PS5::getActiveAccount()
    {
        return this->accessActiveAccount();
    }

    Account_PS5& AccountAdapter_PS5::accessActiveAccount()
    {
        ITF_ASSERT(m_activeAccountInd < ITF_ARRAY_SIZE(m_accounts));
        ITF_ASSERT(m_accounts[m_activeAccountInd].getSceUserId() != SCE_USER_SERVICE_USER_ID_INVALID);
        return m_accounts[m_activeAccountInd];
    }

    void AccountAdapter_PS5::handleUserEvents()
    {
        SceUserServiceEvent event;
        i32 sceResult;
        while ((sceResult = sceUserServiceGetEvent(&event)) == SCE_OK)
        {
            SceUserServiceEventType eventType = event.eventType;
            switch (eventType)
            {
            case SCE_USER_SERVICE_EVENT_TYPE_LOGIN:
                addAccount(event.userId);
                break;

            case SCE_USER_SERVICE_EVENT_TYPE_LOGOUT:
                removeAccount(event.userId);
                break;

            default:
                ITF_ASSERT_MSG(false, "Unknown SceUserServiceEvent event type (%s)", eventType);
                break;
            }
        }

        ITF_ASSERT(sceResult == SCE_USER_SERVICE_ERROR_NO_EVENT);
    }

    void AccountAdapter_PS5::addAccount(SceUserServiceUserId _userId)
    {
        u32 accountIndex = getAccountIndex(_userId);

        ITF_VERIFY_MSG(accountIndex == U32_INVALID, "Trying to add an account that is already in the list");
        if (accountIndex == U32_INVALID)
        {
            accountIndex = getFreeAccountSlot();
            ITF_VERIFY_MSG(accountIndex != U32_INVALID, "Trying to add an account while the account list is full");
            if (accountIndex != U32_INVALID)
            {
                Account_PS5& newAccount = m_accounts[accountIndex];

                newAccount.setSceUserId(_userId);

                char userName[SCE_USER_SERVICE_MAX_USER_NAME_LENGTH + 1];
                memset(userName, 0x0, SCE_USER_SERVICE_MAX_USER_NAME_LENGTH);
                int32_t sceResult = sceUserServiceGetUserName(_userId, userName, ITF_ARRAY_SIZE(userName));
                ITF_ASSERT(sceResult == SCE_OK);
                ITF_UNUSED(sceResult);
                newAccount.setName(userName);

                LOG("[AccountAdapter] Account %u has been added:", accountIndex);
                newAccount.dumpToLog();

                INPUT_ADAPTER_PS5->loginUser(accountIndex, _userId);
            }
        }
    }

    void AccountAdapter_PS5::removeAccount(SceUserServiceUserId _userId)
    {
        u32 accountIndex = getAccountIndex(_userId);
        ITF_VERIFY_MSG(accountIndex != U32_INVALID, "Trying to remove an account that is not in the list");

        if (accountIndex != U32_INVALID)
        {
            m_accounts[accountIndex] = Account_PS5();
            INPUT_ADAPTER_PS5->logoutUser(accountIndex, _userId);
        }
    }

    u32 AccountAdapter_PS5::getAccountIndex(SceUserServiceUserId _userId) const
    {
        ITF_ASSERT(_userId != SCE_USER_SERVICE_USER_ID_INVALID);
        for (u32 accountIndex = 0; accountIndex < ITF_ARRAY_SIZE(m_accounts); accountIndex++)
        {
            const Account_PS5& account = m_accounts[accountIndex];
            if (account.getSceUserId() == _userId)
            {
                return accountIndex;
            }
        }

        return U32_INVALID;
    }

    u32 AccountAdapter_PS5::getFreeAccountSlot() const
    {
        for (u32 accountIndex = 0; accountIndex < ITF_ARRAY_SIZE(m_accounts); accountIndex++)
        {
            const Account_PS5& account = m_accounts[accountIndex];
            if (account.getSceUserId() == SCE_USER_SERVICE_USER_ID_INVALID)
            {
                return accountIndex;
            }
        }

        return U32_INVALID;
    }
} // namespace ITF

