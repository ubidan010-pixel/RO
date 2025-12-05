#include "precompiled_AccountAdapter_win.h"

#include "adapters/AccountAdapter_win/AccountAdapter_win.h"

#include "core/Config.h"
#include "core/String8.h"
#include "core/error/ErrorHandler.h"

#include "engine/singleton/Singletons.h"
#include "engine/networkservices/UPlayService.h"

#include <lmcons.h>     // for Win username max length

namespace ITF
{

    AccountAdapter_win::AccountAdapter_win()
        : AccountAdapter()
        , m_activeAccount()
        , m_uplayClientConnected(bfalse)
        , m_uplayClientOnline(btrue)
    {
    }

    void AccountAdapter_win::initialize()
    {
#ifdef ITF_UPLAYPC
        if (!UPLAYSERVICE || !CONFIG->m_enableUplay)
        {
            LOG("[AccountAdapter] Uplay is disabled; use uplay=1 runtime arg");
            return;
        }

        createActiveAccountUplay();
        m_uplayClientOnline = UPLAYSERVICE->isClientOnline();
#else
        createActiveAccount();
#endif
    }

    void AccountAdapter_win::terminate()
    {
    }

    void AccountAdapter_win::createActiveAccount()
    {
        char userName[UNLEN + 1];
        DWORD length = ITF_ARRAY_SIZE(userName);
        BOOL result = GetUserNameA(userName, &length);
        ITF_ASSERT(result != FALSE);
        m_activeAccount.setName(String8(userName));

        String8 email = CONFIG->m_onlineLogin;
        if (email.isEmpty())
        {
            email.setText(m_activeAccount.getName().cStr(), 15);
            email += "_ro4k@ubisoft.com";
        }
        m_activeAccount.setEmail(email);

        String8 onlinePassword = CONFIG->m_onlinePassword;
        if (onlinePassword.isEmpty())
        {
            onlinePassword = "#RO4kTestUAT";
        }
        m_activeAccount.setOnlinePassword(onlinePassword);

        m_activeAccount.setSignedOnPlatformNetwork(true);
        m_activeAccount.setPermissions(Account::EPermissions_All);
        m_activeAccount.setAgeGroup(Account::EAgeGroup_Adult);

        LOG("[AccountAdapter] The active account has been created:");
        m_activeAccount.dumpToLog();

        m_activeAccountInd = 0;
        m_uplayClientConnected = btrue;
        setTokenStatus(AccountAdapter::ETokenStatus_Ready);
    }

    void AccountAdapter_win::createActiveAccountUplay()
    {
        if (UPLAYSERVICE && CONFIG->m_enableUplay)
        {
            m_activeAccount.setName(UPLAYSERVICE->getUserName());
            m_activeAccount.setEmail(UPLAYSERVICE->getUserEmail());
        }
        m_activeAccount.setLinkedToUplay(true);
        m_activeAccount.setSignedOnPlatformNetwork(true);

        m_activeAccount.setPermissions(Account::EPermissions_All);
        m_activeAccount.setAgeGroup(Account::EAgeGroup_Adult);

        LOG("[AccountAdapter] The active account has been created:");
        m_activeAccount.dumpToLog();

        m_activeAccountInd = 0;
        updateAccountConnectionStatus();
    }

    void AccountAdapter_win::update()
    {
        updateAccountConnectionStatus();
        AccountAdapter::update();

        if (!UPLAYSERVICE || !CONFIG->m_enableUplay)
            return;

        // poll ~2 times; Uplay get name is retrieved shortly after
        if (m_activeAccount.getName().isEmpty())
        {
            String8 name = UPLAYSERVICE->getUserName();
            if (!name.isEmpty())
            {
                LOG("[AccountAdapter] obtained name from UPlay: %s", name.cStr());
                m_activeAccount.setName(name);
            }
        }
    }

    void AccountAdapter_win::updateAccountConnectionStatus()
    {
        if (!UPLAYSERVICE || !CONFIG->m_enableUplay)
            return;

        if (m_uplayClientConnected != UPLAYSERVICE->isClientConnected())
        {
            m_uplayClientConnected = ~m_uplayClientConnected;
            m_activeAccount.setSignedOnPlatformNetwork((m_uplayClientConnected && m_uplayClientOnline) ? true : false);
            setTokenStatus((m_uplayClientConnected && m_uplayClientOnline) ? AccountAdapter::ETokenStatus_Ready : AccountAdapter::ETokenStatus_Error);
        }
    }

} // namespace ITF
