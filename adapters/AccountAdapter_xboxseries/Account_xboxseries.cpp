#include "precompiled_AccountAdapter_xboxseries.h"

#include "adapters/AccountAdapter_xboxseries/Account_xboxseries.h"
#include "core/error/ErrorHandler.h"

#include <xsapi-c/types_c.h>
#include <xsapi-c/xbox_live_context_c.h>

namespace ITF
{
    Account_xboxseries::Account_xboxseries()
        : Account()
        , m_XboxLiveId(U64_INVALID)
        , m_userHandle(nullptr)
        , m_contextHandle(nullptr)
    {
        Account::setNativePlatformId<NativeAccountId_xboxseries>();
    }

    Account_xboxseries::~Account_xboxseries()
    {
        destroyXUserHandle();
        destroyContextHandle();
    }

    void Account_xboxseries::dumpToLog() const
    {
        Account::dumpToLog();

        LOG("\t* XboxSeries-specific properties:"
            "\n\t\t- Local ID: %d"
            "\n\t\t- XBox live ID (XUID): %u"
            , getNativePlatformId<NativeAccountId_xboxseries>()->m_userLocalId
            , m_XboxLiveId
        );
    }

    void Account_xboxseries::ensureCreateContextHandle()
    {
        if (!m_contextHandle)
        {
            HRESULT hr = XblContextCreateHandle(m_userHandle, &m_contextHandle);
            ITF_ASSERT_MSG(SUCCEEDED(hr), "[AccountAdapter] Unable to create a XboxLive Context Handle");
        }
    }

    void Account_xboxseries::destroyContextHandle()
    {
        if (m_contextHandle)
        {
            XblContextCloseHandle(m_contextHandle);
            m_contextHandle = nullptr;
        }
    }

    void Account_xboxseries::destroyXUserHandle()
    {
        if (m_userHandle)
        {
            XUserCloseHandle(m_userHandle);
            m_userHandle = nullptr;
        }
    }

} // namespace ITF

