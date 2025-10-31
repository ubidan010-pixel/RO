#include "precompiled_core.h"

#include "core/AdaptersInterfaces/AccountAdapter.h"
#include "core/error/ErrorHandler.h"

namespace ITF
{
    namespace {
        const ITF::String8 stateNames[ITF::AccountAdapter::ETokenStatus_Count] = {"ETokenStatus_Preparing", "ETokenStatus_Ready", "ETokenStatus_Error"}; 
    };

    AccountAdapter::AccountAdapter()
        : m_activeAccountInd(U32_INVALID)
        , m_isLinkedToUplay(false)
        , m_isLinkedToFacebook(false)
        , m_networkId()
        , m_tokenStatus(ETokenStatus_Preparing)
    {

    }

    void AccountAdapter::setTokenStatus(ETokenStatus _tokenStatus)
    {
        if (m_tokenStatus != _tokenStatus)
        {
            LOG("[AccountAdapter] Switching to state - %s", stateNames[_tokenStatus].cStr());
            m_tokenStatus = _tokenStatus;
        }
    }

    Account::OnlineAccountInfo AccountAdapter::getDefaultOnlineAccountInfo(const String8& _idOnPlatform)
    {
        Account::OnlineAccountInfo onlineAccountInfo;
        onlineAccountInfo.m_idOnPlatform = _idOnPlatform;
        onlineAccountInfo.m_permissions = EOnlineAccountPermissions_ViewOnlinePresence | EOnlineAccountPermissions_ViewGameHistory;
        if (getActiveAccount().checkPermissions(Account::EPermissions_UserCreatedContent) == Account::EPermissions_StatusAuthorized)
        {
            onlineAccountInfo.m_permissions |= EOnlineAccountPermissions_ViewGameUGC;
        }
        return onlineAccountInfo;
    }

	bool AccountAdapter::getActivePlayerName(String8& _outName) const
    {
        if (isMainActiveAccountValid())
        {
            _outName = ACCOUNT_ADAPTER->getActiveAccount().getName();
            return true;
        }
        else
        {
            return false;
        }
    }

    bool AccountAdapter::getPlayerNameOnUplay(String8& _outName) const
    {
        if (m_isLinkedToUplay)
        {
            _outName = ACCOUNT_ADAPTER->getActiveAccount().getNameOnUplay();
            return true;
        }
        else
        {
            return false;
        }
    }

    void AccountAdapter::update()
    {
        if (isMainActiveAccountValid())
        {
            Account& activeAccount = accessActiveAccount();
            activeAccount.setLinkedToUplay(m_isLinkedToUplay);
            activeAccount.setLinkedToFacebook(m_isLinkedToFacebook);
            activeAccount.setNetworkId(m_networkId);
            activeAccount.setNameOnUplay(m_activeAccNameOnUplay);
        }
        else
        {
            resetActiveAccountProperties();
        }
    }

    void AccountAdapter::resetActiveAccountProperties()
    {
        m_isLinkedToUplay = false;
        m_isLinkedToFacebook = false;
        m_networkId.makeNil();
        m_activeAccNameOnUplay = String8::emptyString;
    }


} // namespace ITF
