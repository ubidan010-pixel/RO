#ifndef _ITF_ACCOUNT_WIN_H_
#define _ITF_ACCOUNT_WIN_H_

#include "core/AdaptersInterfaces/Account.h"


namespace ITF
{
    class Account_win: public Account
    {
    public:
        Account_win();
        ~Account_win() override {}

        void dumpToLog() const override;

        const String8& getEmail() const { return m_email; }
        void setEmail(const String8& _email) { m_email = _email; }

        const String8& getOnlinePassword() const { return m_onlinePassword; }
        void setOnlinePassword(const String8& _onlinePassword) { m_onlinePassword = _onlinePassword; }

    protected:
        String8 m_email;
        String8 m_onlinePassword;
    };


} // namespace ITF


#endif //_ITF_ACCOUNT_WIN_H_
