#ifdef ITF_XBOX_SERIES

#pragma once

#include "core/AdaptersInterfaces/AccountAdapter.h"
#include "core/container/extendedMap.h"

#include "adapters/AccountAdapter_xboxseries/Account_xboxseries.h"

namespace ITF
{
    class AccountAdapter_xboxseries : public AccountAdapter
    {
    public:
        AccountAdapter_xboxseries();
        ~AccountAdapter_xboxseries() override;

        void initialize() override;
        void update() override;
        void terminate() override;

        u32 getMaxNbAccounts() const override { ITF_ASSERT_MSG(m_maxNbAccounts > 0, "[AccountAdapter] Variable is not initialized"); return m_maxNbAccounts; }
        const Account_xboxseries* getAccount(u32 _accountIndex) override { return &m_accounts[_accountIndex]; }
        const Account_xboxseries& getActiveAccount() override { return m_accounts[m_activeAccountInd]; }

        void selectAccountThroughSystemUI(bbool _silently = bfalse);

        void onSignedOut();
        void setMainUserChangedEvent() { m_mainUserChanged = btrue; }

        void onSuspend();
        void onResume();

    private:
        Account_xboxseries& accessActiveAccount() override { return m_accounts[m_activeAccountInd]; }

        void signinMainUser(XUserAddOptions _options);
        void onSigninMainUserSuccess(XUserHandle _user);
        void onSigninMainUserFailed();

        ITF_MAP<u32, Account_xboxseries> m_accounts;
        uint32_t m_maxNbAccounts;
        XTaskQueueHandle m_taskQueue;
        XTaskQueueRegistrationToken m_registrationToken;

        bbool m_mainUserChanged;
    };

#define ACCOUNT_ADAPTER_XBOXSERIES (static_cast<ITF::AccountAdapter_xboxseries*>(ACCOUNT_ADAPTER))

} // namespace ITF

#endif // ITF_SCARLETT
