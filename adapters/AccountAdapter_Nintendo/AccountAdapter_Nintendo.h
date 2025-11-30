#ifdef ITF_NINTENDO

#ifndef _ITF_ACCOUNTADAPTER_NINTENDO_H_
#define _ITF_ACCOUNTADAPTER_NINTENDO_H_

#include "adapters/AccountAdapter_Nintendo/Account_Nintendo.h"
#include "adapters/SystemAdapter_Nintendo/SystemAdapter_Nintendo.h"
#include "core/AdaptersInterfaces/AccountAdapter.h"

namespace ITF
{

    class AccountAdapter_Nintendo : public AccountAdapter, public ISystemNotificationListener_Nintendo
    {
    public:
        AccountAdapter_Nintendo();
        virtual ~AccountAdapter_Nintendo();

        virtual void initialize() override;
        virtual void update() override;
        virtual void terminate() override;

        virtual u32 getMaxNbAccounts() const override { return 1; }
        virtual const Account_Nintendo* getAccount(u32 _accountIndex) override { return (_accountIndex == 0 ? &m_activeAccount : nullptr); }
        virtual const Account_Nintendo& getActiveAccount() override { ITF_ASSERT(m_activeAccountInd == 0); return m_activeAccount; }
        virtual bool isMainActiveAccountValid() const override { return m_activeAccount.hasValidUid(); }

        virtual u32 getPlatformSpecificErrorCode() override { ITF_ASSERT_CRASH(getTokenStatus() == ETokenStatus_Error, "AccountAdapter has no error"); return m_platformErrorCode; };

        void openPreselectedAccount(bool _doOpen) { m_openPreselectedAccount = _doOpen; }

    protected:
        virtual Account_Nintendo& accessActiveAccount() override { ITF_ASSERT(m_activeAccountInd == 0); return m_activeAccount; }

    private:
        Account_Nintendo m_activeAccount;
        u32 m_platformErrorCode;
        bool m_accountRefreshNeeded;
        bool m_openPreselectedAccount; 

        void switchActiveAccount(const nn::account::Uid& _accountUid, const nn::account::UserHandle* _userHandle);
        void openActiveAccount();
        void closeActiveAccount();

        void onSystemNotification(const nn::oe::Message& _message) override;
        void refreshActiveAccount();
    };

    #define ACCOUNT_ADAPTER_NINTENDO (static_cast<AccountAdapter_Nintendo*>(ACCOUNT_ADAPTER))


} // namespace ITF


#endif // _ITF_ACCOUNTADAPTER_NINTENDO_H_

#endif // ITF_NINTENDO
