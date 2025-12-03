#ifndef _ITF_ACCOUNTADAPTER_H_
#define _ITF_ACCOUNTADAPTER_H_

#include "core/AdaptersInterfaces/Account.h"
#include "core/templateSingleton.h"

namespace ITF
{
    class TRCMessage_Base;

    class AccountAdapter: public TemplateSingleton<AccountAdapter>
    {
    public:
        AccountAdapter();
        virtual ~AccountAdapter() {}

        virtual void initialize() = 0;
        virtual void update();
        virtual void terminate() = 0;

        typedef ITF_MAP<UUID, Account::OnlineAccountInfo> OnlineAccountsInfo;
        enum EOnlineAccoutPermissions
        {
            EOnlineAccountPermissions_None                  = 0,
            EOnlineAccountPermissions_ViewOnlinePresence    = (1 << 0),
            EOnlineAccountPermissions_ViewGameHistory       = (1 << 1),
            EOnlineAccountPermissions_ViewGameUGC           = (1 << 2),

            // Those 2 values must stay at the end of the list
            EOnlineAccountPermissions_LastPlusOne,
            EOnlineAccountPermissions_All                   = ((EOnlineAccountPermissions_LastPlusOne - 1) << 1) - 1,
        };

        enum ETokenStatus
        {
            ETokenStatus_Preparing,
            ETokenStatus_Ready,
            ETokenStatus_Error,
            ETokenStatus_Count
        };

        inline ETokenStatus getTokenStatus() { return m_tokenStatus; }

        virtual u32 getMaxNbAccounts() const = 0;
        virtual const Account* getAccount(u32 _accountIndex) = 0;
        virtual const Account& getActiveAccount() = 0;
       
        void setActiveAccountLinkedToUplay(bool _isLinkedToUplay) { m_isLinkedToUplay = _isLinkedToUplay; }
        bool isActiveAccountLinkedToUplay() { return m_isLinkedToUplay; }
        void setActiveAccountLinkedToFacebook(bool _isLinkedToFacebook) { m_isLinkedToFacebook = _isLinkedToFacebook; }
        void setActiveAccountNetworkId(const UUID& _networkId) { m_networkId = _networkId; }
        void setActiveAccountNameOnUplay(const String8& _uplayPlatformName) {
            const_cast<Account&>(getActiveAccount()).setNameOnUplay(_uplayPlatformName);
            m_activeAccNameOnUplay = _uplayPlatformName;
        }

        virtual u32 getPlatformSpecificErrorCode() { return 0; };

        Account::OnlineAccountInfo getDefaultOnlineAccountInfo(const String8& _idOnPlatform);
        bool getActivePlayerName(String8& _outName) const;
        bool getPlayerNameOnUplay(String8& _outName) const;
        virtual bool isMainActiveAccountValid() const { return m_activeAccountInd != U32_INVALID; }

        virtual bool canRequestBlockedUsers() { return false; }
        virtual bool needToUpdateBlockedUsersList(bool /*_unsetFlagAfterCheck*/) { return false; }

    protected:
        virtual Account& accessActiveAccount() = 0;
        void resetActiveAccountProperties();
        void setTokenStatus(ETokenStatus _accountStatus);

        u32 m_activeAccountInd;
        bool m_isLinkedToUplay;
        bool m_isLinkedToFacebook;
        UUID m_networkId;
        String8 m_activeAccNameOnUplay;

    private:
        ETokenStatus m_tokenStatus;
    };

    #define ACCOUNT_ADAPTER ITF::AccountAdapter::getptr()

} // namespace ITF


#endif //_ITF_ACCOUNTMANAGER_H_
