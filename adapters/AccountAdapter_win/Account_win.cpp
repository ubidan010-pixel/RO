#include "precompiled_AccountAdapter_win.h"

#include "adapters/AccountAdapter_win/Account_win.h"
#include <core/error/ErrorHandler.h>

namespace ITF
{

    Account_win::Account_win()
        : Account()
        , m_email()
        , m_onlinePassword()
    {
    }


    void Account_win::dumpToLog() const
    {
        Account::dumpToLog();

        LOG("\t* Windows-specific properties:"
            "\n\t\t- Email: %s"
            "\n\t\t- Online Password: %s"
            , m_email.cStr()
            , m_onlinePassword.cStr()
            );
    }


} // namespace ITF
