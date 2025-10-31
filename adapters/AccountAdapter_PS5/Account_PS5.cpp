#include "precompiled_AccountAdapter_PS5.h"

#include "adapters/AccountAdapter_PS5/Account_PS5.h"
#include "core/error/ErrorHandler.h"

#include <sstream>
#include <np.h>


namespace ITF
{
    Account_PS5::Account_PS5()
        : Account()
        , m_accountId(SCE_NP_INVALID_ACCOUNT_ID)
        , m_onlineId()
        , m_npCountryCode()
        , m_age(0)
        , m_firstPartyErrorCode(0)
    {
        memset(&m_onlineId, 0x0, sizeof(m_onlineId));
        Account::setNativePlatformId<NativeAccountId_PS5>(SCE_USER_SERVICE_USER_ID_INVALID);
    }

    Account_PS5::~Account_PS5()
    {
    }

    void Account_PS5::dumpToLog() const
    {
        Account::dumpToLog();

        LOG("\t* PS4-specific properties:"
            "\n\t\t- SCE USER ID: 0x%08X"
            "\n\t\t- ONLINE ID: %s"
            "\n\t\t- Country code: %s"
            , getNativePlatformId<NativeAccountId_PS5>()->m_sceUserId
            , m_onlineId.data
            , m_npCountryCode.cStr()
        );
    }

    void Account_PS5::setAccountId(const SceNpAccountId& _accountId)
    {
        m_accountId = _accountId;
        m_platformId.ui64toa(_accountId);
    }
} // namespace ITF

