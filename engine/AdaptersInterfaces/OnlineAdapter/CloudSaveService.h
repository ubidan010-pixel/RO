#pragma once

#include <engine/AdaptersInterfaces/OnlineAdapter/OnlineError.h>
#include <core/String8.h>
#include <core/container/extendedVector.h>

namespace ITF
{

struct CloudSaveSlotInfo
{
    bbool m_isAvailable = bfalse;
    String8 m_displayName;
    u64 m_timestamp = 0;
    u32 m_dataSize = 0;
    u32 m_crc32 = 0;
};

class CloudSaveService
{
public:
    virtual ~CloudSaveService() {}

    virtual void update() = 0;
    
    virtual OnlineError refreshCloudSaves() = 0;
    
    virtual bbool getSlotInfo(u32 _slotIndex, CloudSaveSlotInfo& _outInfo) = 0;
    
    virtual OnlineError uploadSlot(
        u32 _slotIndex,
        const void* _data,
        u32 _dataSize,
        const String8& _displayName,
        u64 _timestamp) = 0;
    
        virtual OnlineError downloadSlot(
        u32 _slotIndex,
        ITF_VECTOR<u8>& _outData,
        CloudSaveSlotInfo& _outInfo) = 0;
};

} // namespace ITF

