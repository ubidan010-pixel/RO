#ifndef _ITF_ACCOUNT_NINTENDO_H_
#define _ITF_ACCOUNT_NINTENDO_H_

#include "core/AdaptersInterfaces/Account.h"

#include <nn/account.h>


namespace ITF
{
    
    constexpr const nn::account::UserHandle InvalidUserHandle_Nintendo()
    {
        return nn::account::UserHandle{ {0,0},0 };
    }

    struct NativeAccountId_Nintendo : public NativeAccountId
    {
        explicit NativeAccountId_Nintendo(nn::account::Uid uid = nn::account::InvalidUid,
                                    nn::account::UserHandle handle = InvalidUserHandle_Nintendo(),
                                    nn::account::NetworkServiceAccountId nsaId = { 0 })
            : m_uid(uid)
            , m_handle(handle)
            , m_nsaId(nsaId)
        {}

        nn::account::Uid m_uid;
        nn::account::UserHandle m_handle;
        nn::account::NetworkServiceAccountId m_nsaId;
    };

    class Account_Nintendo: public Account
    {
    public:
        Account_Nintendo();

        virtual void dumpToLog() const override;

        EPermissions_Status checkPermissions(u32 _permissions) const override;

        bool hasValidUid() const;
        nn::account::Uid getUid() const;
        static String8 uidToString(const nn::account::Uid& _uid);
        String8 getUidAsString() const;
        void setUid(nn::account::Uid _uid);

        bool hasValidHandle() const;
        void invalidateHandle();
        nn::account::UserHandle getHandle() const;
        void setHandle(nn::account::UserHandle _handle);

        void setNsaId(nn::account::NetworkServiceAccountId _id);
        nn::account::NetworkServiceAccountId getNsaId() const;
        bool hasValidNsaId() const;
        void invalidateNsaId();
    };
} // namespace ITF


#endif //_ITF_ACCOUNT_NINTENDO_H_
