#pragma once

#include "core/AdaptersInterfaces/Account.h"

#include <XUser.h>
#include <xsapi-c/pal.h>
#include <xsapi-c/types_c.h>

namespace ITF
{
    struct NativeAccountId_xboxseries : public NativeAccountId
    {
        explicit NativeAccountId_xboxseries(XUserLocalId _id = { U64_INVALID })
            : m_userLocalId(_id)
        {}

        XUserLocalId m_userLocalId;
    };

    class Account_xboxseries : public Account
    {
    public:
        Account_xboxseries();
        ~Account_xboxseries();

        void dumpToLog() const override;

        void setLocalUserId(XUserLocalId _id) { setNativePlatformId<NativeAccountId_xboxseries>(_id); }
        const XUserLocalId getLocalUserId() const { return getNativePlatformId<NativeAccountId_xboxseries>()->m_userLocalId; }

        void setXboxLiveId(u64 _XboxLiveID) { m_XboxLiveId = _XboxLiveID; }
        const u64 getXboxLiveId() const { return m_XboxLiveId; }

        void setXUserHandle(XUserHandle _userHandle) { m_userHandle = _userHandle; }
        void destroyXUserHandle();
        const XUserHandle& getXUserHandle() const { return m_userHandle; }

        void ensureCreateContextHandle();
        void destroyContextHandle();
        const XblContextHandle& getContextHandle() const { return m_contextHandle; }

        bbool hasXboxUser() const { return getLocalUserId().value != U64_INVALID; }

    private:
        u64 m_XboxLiveId;
        XUserHandle m_userHandle;
        XblContextHandle m_contextHandle;
    };

} // namespace ITF


