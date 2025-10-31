#pragma once

#include "core/AdaptersInterfaces/Account.h"

#include <np/np_common.h>


namespace ITF
{
    struct NativeAccountId_PS5 : public NativeAccountId
    {
        explicit NativeAccountId_PS5(SceUserServiceUserId _userId = SCE_USER_SERVICE_USER_ID_INVALID)
            : m_sceUserId(_userId)
        {}

        SceUserServiceUserId m_sceUserId;
    };

    class Account_PS5 : public Account
    {
    public:
        Account_PS5();
        virtual ~Account_PS5();

        virtual void dumpToLog() const override;

        SceUserServiceUserId getSceUserId() const
        {
            return getNativePlatformId<NativeAccountId_PS5>()->m_sceUserId;
        }

        void setSceUserId(const SceUserServiceUserId& _sceUserId)
        {
            setNativePlatformId<NativeAccountId_PS5>(_sceUserId);
        }

        const SceNpAccountId& getAccountId() const { return m_accountId; }
        void setAccountId(const SceNpAccountId& _accountId);

        const SceNpOnlineId& getOnlineId() const { return m_onlineId; }
        void setOnlineId(const SceNpOnlineId& _onlineId) { m_onlineId = _onlineId; }

        const String8& getNpCountryCode() const { return m_npCountryCode; }
        void setNpCountryCode(const String8& _npCountryCode) { m_npCountryCode = _npCountryCode; }

        const uint8_t getAge() const { return m_age; }
        void setAge(uint8_t _age) { m_age = _age; }

        void setPlatformSpecificErrorCode(u32 _errorCode) { m_firstPartyErrorCode = _errorCode; }
        u32 getPlatformSpecificErrorCode() const { return m_firstPartyErrorCode; };

    private:
        SceNpAccountId m_accountId;
        SceNpOnlineId m_onlineId;
        String8 m_npCountryCode;
        uint8_t m_age;
        u32 m_firstPartyErrorCode;
    };

} // namespace ITF

