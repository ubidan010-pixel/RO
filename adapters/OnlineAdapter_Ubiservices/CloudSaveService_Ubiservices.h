#pragma once

#include <engine/AdaptersInterfaces/OnlineAdapter/CloudSaveService.h>

#if defined(ITF_SUPPORT_UBISERVICES)

#include <core/container/SafeArray.h>
#include <mutex>

namespace ITF
{

class CloudSaveService_Ubiservices : public CloudSaveService
{
public:
    CloudSaveService_Ubiservices();
    virtual ~CloudSaveService_Ubiservices();

    virtual void update() override;

    virtual OnlineError refreshCloudSaves() override;
    virtual bbool getSlotInfo(u32 _slotIndex, CloudSaveSlotInfo& _outInfo) override;

    virtual OnlineError uploadSlot(
        u32 _slotIndex,
        const void* _data,
        u32 _dataSize,
        const String8& _displayName,
        u64 _timestamp) override;

    virtual OnlineError downloadSlot(
        u32 _slotIndex,
        ITF_VECTOR<u8>& _outData,
        CloudSaveSlotInfo& _outInfo) override;

private:
    void resetCacheLocked();

    mutable std::mutex m_mutex;
    SafeArray<CloudSaveSlotInfo> m_slots;
    bbool m_hasRefreshed;
};

} // namespace ITF

#endif // ITF_SUPPORT_UBISERVICES

