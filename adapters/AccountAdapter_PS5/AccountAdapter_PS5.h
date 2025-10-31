#ifdef ITF_PS5

#pragma once

#include "adapters/AccountAdapter_PS5/Account_PS5.h"
#include "adapters/SystemAdapter_PS5/SystemAdapter_PS5.h"

#include "core/AdaptersInterfaces/AccountAdapter.h"

namespace ITF
{
    class AccountAdapter_PS5 : public AccountAdapter
    {
    public:
        AccountAdapter_PS5();
        virtual ~AccountAdapter_PS5() {}

        virtual void initialize() override;
        virtual void update() override;
        virtual void terminate() override;

        virtual u32 getMaxNbAccounts() const override { return ITF_ARRAY_SIZE(m_accounts); }
        virtual const Account_PS5* getAccount(u32 _accountIndex) override;
        virtual const Account_PS5& getActiveAccount() override;

        virtual u32 getPlatformSpecificErrorCode() override { return m_accounts[m_activeAccountInd].getPlatformSpecificErrorCode(); };

    private:
        virtual Account_PS5& accessActiveAccount() override;

        void handleUserEvents();
        void addAccount(SceUserServiceUserId _userId);
        void removeAccount(SceUserServiceUserId _userId);

        // Returns U32_INVALID if no account with this user ID is in the list
        u32 getAccountIndex(SceUserServiceUserId _userId) const;
        u32 getFreeAccountSlot() const;

        Account_PS5 m_accounts[SCE_USER_SERVICE_MAX_LOGIN_USERS];
};


#define ACCOUNT_ADAPTER_PS5 (static_cast<AccountAdapter_PS5*>(ACCOUNT_ADAPTER))


} // namespace ITF


#endif // ITF_PS5

