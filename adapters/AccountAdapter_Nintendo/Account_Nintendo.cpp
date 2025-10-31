#include "precompiled_AccountAdapter_Nintendo.h"

#include "core/error/ErrorHandler.h"
#include "adapters/AccountAdapter_Nintendo/Account_Nintendo.h"
#include "adapters/AccountAdapter_Nintendo/AccountAdapter_Nintendo.h"


namespace ITF
{
    Account_Nintendo::Account_Nintendo() : Account()
    {
        Account::setNativePlatformId<NativeAccountId_Nintendo>();
        invalidateNsaId();
    }

    void Account_Nintendo::dumpToLog() const
    {
        Account::dumpToLog();

        LOG("\t* Nintendo-specific properties:"
            "\n\t\t- UID: %s"
            "\n\t\t- NSA ID: %s"
            , getUidAsString().cStr()
            , m_platformId.cStr()
            );
    }

    Account::EPermissions_Status Account_Nintendo::checkPermissions(u32 _permissions) const
    {
        EPermissions_Status status = Account::checkPermissions(_permissions);
        return status;
    }

    bool Account_Nintendo::hasValidUid() const
    {
        return (getNativePlatformId<NativeAccountId_Nintendo>()->m_uid != nn::account::InvalidUid);
    }

    nn::account::Uid Account_Nintendo::getUid() const
    {
        return getNativePlatformId<NativeAccountId_Nintendo>()->m_uid;
    }

    void Account_Nintendo::setUid(nn::account::Uid _uid)
    {
        getNativePlatformId<NativeAccountId_Nintendo>()->m_uid = _uid;
    }

    String8 Account_Nintendo::uidToString(const nn::account::Uid& _uid)
    {
        String8 result;
        result.setTextFormat("%08x_%08x_%08x_%08x", (u32)(_uid._data[0] >> 32), (u32)(_uid._data[0]), (u32)(_uid._data[1] >> 32), (u32)(_uid._data[1]));
        return result;
    }

    String8 Account_Nintendo::getUidAsString() const
    {
        return uidToString(getNativePlatformId<NativeAccountId_Nintendo>()->m_uid);
    }

    bool Account_Nintendo::hasValidHandle() const
    {
        const nn::account::UserHandle InvalidHandle = InvalidUserHandle_Nintendo();
        auto ptr = getNativePlatformId<NativeAccountId_Nintendo>();
        return memcmp(&ptr->m_handle, &InvalidHandle, sizeof(ptr->m_handle)) != 0;
    }

    nn::account::UserHandle Account_Nintendo::getHandle() const
    {
        return getNativePlatformId<NativeAccountId_Nintendo>()->m_handle;
    }

    void Account_Nintendo::setHandle(nn::account::UserHandle _handle)
    {
        getNativePlatformId<NativeAccountId_Nintendo>()->m_handle = _handle;
    }

    void Account_Nintendo::invalidateHandle()
    {
        getNativePlatformId<NativeAccountId_Nintendo>()->m_handle = InvalidUserHandle_Nintendo();
    }

    bool Account_Nintendo::hasValidNsaId() const
    {
        return (getNativePlatformId<NativeAccountId_Nintendo>()->m_nsaId.id != 0x00ull);
    }

    void Account_Nintendo::setNsaId(nn::account::NetworkServiceAccountId _id)
    {
        getNativePlatformId<NativeAccountId_Nintendo>()->m_nsaId = _id;
        m_platformId.ui64toa(getNativePlatformId<NativeAccountId_Nintendo>()->m_nsaId.id);
    }

    nn::account::NetworkServiceAccountId Account_Nintendo::getNsaId() const
    {
        return getNativePlatformId<NativeAccountId_Nintendo>()->m_nsaId;
    }

    void Account_Nintendo::invalidateNsaId()
    {
        getNativePlatformId<NativeAccountId_Nintendo>()->m_nsaId.id = 0;
        m_platformId = "";
    }

} // namespace ITF
