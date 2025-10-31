#ifdef ITF_WINDOWS

#ifndef _ITF_ACCOUNTADAPTER_WIN_H_
#define _ITF_ACCOUNTADAPTER_WIN_H_

#include "adapters/AccountAdapter_win/Account_win.h"
#include "core/AdaptersInterfaces/AccountAdapter.h"

namespace ITF
{

    class AccountAdapter_win : public AccountAdapter
    {
    public:
        AccountAdapter_win();
        ~AccountAdapter_win() override {}

        void initialize() override;
        void terminate() override;
        void update() override;

        u32 getMaxNbAccounts() const override { return 1; }
        const Account_win* getAccount(u32 _accountIndex) override { return (_accountIndex == 0 ? &m_activeAccount : nullptr); }

        const Account_win& getActiveAccount() override { return m_activeAccount; }

    protected:
        Account_win& accessActiveAccount() override { return m_activeAccount; }

    private:
        Account_win m_activeAccount;

        void createActiveAccount ();
        void createActiveAccountUplay ();
        void updateAccountConnectionStatus ();

        bbool m_uplayClientConnected;
        bbool m_uplayClientOnline;
    };


    #define ACCOUNT_ADAPTER_WIN (static_cast<AccountAdapter_win*>(ACCOUNT_ADAPTER))


} // namespace ITF


#endif // _ITF_ACCOUNTADAPTER_WIN_H_

#endif // ITF_WINDOWS
