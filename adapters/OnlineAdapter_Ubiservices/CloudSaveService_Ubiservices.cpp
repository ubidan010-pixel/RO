#include "precompiled_OnlineAdapter_Ubiservices.h"

#if defined(ITF_SUPPORT_UBISERVICES)

#include "adapters/OnlineAdapter_Ubiservices/CloudSaveService_Ubiservices.h"
#include "adapters/OnlineAdapter_Ubiservices/OnlineAdapter_Ubiservices.h"
#include "engine/AdaptersInterfaces/OnlineAdapter/SessionService.h"

#include "core/crc.h"
#include "core/error/ErrorHandler.h"

#include <ubiservices/core/JSON/json.h>
#include <ubiservices/core/JSON/jsonWriter.h>
#include <ubiservices/core/helpers/resultRange.h>
#include <ubiservices/core/tasks/asyncResult.h>
#include <ubiservices/core/tasks/asyncResultError.h>
#include <ubiservices/services/entity/entityCreation.h>
#include <ubiservices/services/entity/entity_module.h>
#include <ubiservices/services/parameters/parameters_module.h>

#include <cstdlib>
#include <cstring>

using namespace ubiservices;

namespace ITF
{

namespace
{
    static const u32 kCloudSaveSlotCount = 3;

    static const char* kCloudSaveTag = "rayman";
    static const char* kCloudSaveType = "rayman_playercard";

    static US_NS::String slotName(u32 _slotIndex)
    {
        return US_NS::String::formatText("slot_%u", _slotIndex);
    }

    static bbool isSlotIndexValid(u32 _slotIndex)
    {
        return _slotIndex < kCloudSaveSlotCount;
    }

    static bbool parseSlotIndexFromName(const US_NS::String& _name, u32& _outSlotIndex)
    {
        const char* name = _name.getUtf8();
        if (!name)
            return bfalse;

        static const char* prefix = "slot_";
        static const size_t prefixLen = 5;
        if (std::strncmp(name, prefix, prefixLen) != 0)
            return bfalse;

        i32 value = atoi(name + prefixLen);
        if (value < 0)
            return bfalse;

        u32 slotIndex = (u32)value;
        if (!isSlotIndexValid(slotIndex))
            return bfalse;

        _outSlotIndex = slotIndex;
        return btrue;
    }

    static CloudSaveSlotInfo parseInfoFromJsonData(const US_NS::String& _jsonData)
    {
        CloudSaveSlotInfo info;

        if (_jsonData.isEmpty())
            return info;

        US_NS::Json json(_jsonData);
        if (!json.isValid() || !json.isTypeObject())
            return info;

        US_NS::String displayName;
        if (json["displayName"].getValue(displayName))
        {
            info.m_displayName = displayName.getUtf8();
        }

        US_NS::uint64 timestamp = 0;
        if (json["timestamp"].getValue(timestamp))
        {
            info.m_timestamp = (u64)timestamp;
        }

        u32 dataSize = 0;
        if (json["dataSize"].getValue(dataSize))
        {
            info.m_dataSize = dataSize;
        }

        u32 crc32 = 0;
        if (json["crc32"].getValue(crc32))
        {
            info.m_crc32 = crc32;
        }

        return info;
    }

    static US_NS::String buildSlotJsonData(u32 _slotIndex, const String8& _displayName, u64 _timestamp, u32 _dataSize, u32 _crc32)
    {
        US_NS::JsonWriter writer;
        writer["version"] = (u32)1;
        writer["slot"] = (u32)_slotIndex;
        writer["displayName"] = US_NS::String(_displayName.cStr());
        writer["timestamp"] = static_cast<US_NS::uint64>(_timestamp);
        writer["dataSize"] = (u32)_dataSize;
        writer["crc32"] = (u32)_crc32;
        return writer.getJson().renderContent();
    }

    static US_NS::Vector<uint8> makeUsByteVector(const void* _data, u32 _dataSize)
    {
        US_NS::Vector<uint8> out;
        if (_dataSize == 0 || _data == nullptr)
            return out;

        const uint8* bytes = (const uint8*)_data;
        out.insert(out.end(), bytes, bytes + _dataSize);
        return out;
    }

    static void waitForAsyncResultProcessing(const char* _logPrefix, US_NS::AsyncResult<US_NS::Empty>& _result)
    {
        while (_result.isProcessing())
        {
            sleep(100);
        }
        if (_result.hasFailed())
        {
            LOG("[%s] async failed. code %d  | description: %s", _logPrefix, _result.getAsyncResultError().m_baseError.m_code,  _result.getAsyncResultError().m_baseError.m_description.getUtf8());
        }
    }

    template <typename TResult>
    static void waitForAsyncResultProcessing(const char* _logPrefix, US_NS::AsyncResult<TResult>& _result)
    {
        while (_result.isProcessing())
        {
            sleep(100);
        }

        if (_result.hasFailed())
        {
            LOG("[%s] async failed. code %d  | description: %s", _logPrefix, _result.getAsyncResultError().m_baseError.m_code,  _result.getAsyncResultError().m_baseError.m_description.getUtf8());

        }
    }

    static OnlineError mapUbiservicesAsyncError(const US_NS::AsyncResultError& _err)
    {
        return OnlineError(OnlineError::UbiServer, OnlineError::Profile, _err.m_baseError.m_code);
    }

    static OnlineError validateCloudSavePreconditions()
    {
        if (ONLINE_ADAPTER->getSessionService()->getSessionStatus() != SessionService::ESessionStatus_Valid)
        {
            LOG("[CloudSave] no valid session");
            return ONLINE_ADAPTER->getSessionService()->getSessionError();
        }

        auto session = ONLINE_ADAPTER_UBISERVICES->getSession();
        US_NS::ParametersModule& moduleParam = US_NS::ParametersModule::module(*session);
        const bool enabled = moduleParam.isUsFeatureSwitchEnabled(ubiservices::FeatureSwitchId(ubiservices::FeatureSwitchId::EntitiesProfile));
        if (!enabled)
        {
            LOG("[CloudSave] EntitiesProfile feature switch is OFF");
            return OnlineError(OnlineError::UbiServer, OnlineError::Maintenance, 0);
        }

        return OnlineError(OnlineError::Success, OnlineError::Profile);
    }

    static OnlineError findCloudSaveEntityForSlot(u32 _slotIndex, US_NS::EntityProfile& _outEntity)
    {
        auto session = ONLINE_ADAPTER_UBISERVICES->getSession();
        US_NS::EntityModule& modEntity = US_NS::EntityModule::module(*session);

        US_NS::EntityModule::SearchFilter filter;
        filter.m_type = US_NS::String(kCloudSaveType);
        filter.m_name = slotName(_slotIndex);
        filter.m_tags.push_back(US_NS::String(kCloudSaveTag));

        US_NS::AsyncResult<US_NS::Vector<US_NS::EntityProfile>> result =
            modEntity.searchEntitiesProfile(filter, US_NS::Vector<US_NS::ProfileId>(), session->getWorkingSpaceId(), US_NS::ResultRange(0, 10));

        waitForAsyncResultProcessing("CloudSave.Find", result);

        if (result.hasFailed())
        {
            return mapUbiservicesAsyncError(result.getAsyncResultError());
        }

        if (result.isCanceled())
        {
            return OnlineError(OnlineError::Cancelled, OnlineError::Profile, 0);
        }

        if (!result.hasSucceeded())
        {
            ITF_ASSERT_MSG(false, "Async result logic issue!");
            return OnlineError(OnlineError::Invalid, OnlineError::Profile, 0);
        }

        const US_NS::Vector<US_NS::EntityProfile>& entities = result.getResult();
        if (entities.size() == 0)
        {
            return OnlineError(OnlineError::Success, OnlineError::Profile, 0);
        }

        if (entities.size() > 1)
        {
            LOG("[CloudSave] multiple entities found for slot %u (count %d), using first", _slotIndex, entities.size());
        }

        _outEntity = entities[0];
        return OnlineError(OnlineError::Success, OnlineError::Profile, 0);
    }

} // namespace

CloudSaveService_Ubiservices::CloudSaveService_Ubiservices()
    : m_mutex()
    , m_slots()
    , m_hasRefreshed(bfalse)
{
    std::scoped_lock lock(m_mutex);
    resetCacheLocked();
}

CloudSaveService_Ubiservices::~CloudSaveService_Ubiservices()
{
}

void CloudSaveService_Ubiservices::update()
{
}

void CloudSaveService_Ubiservices::resetCacheLocked()
{
    m_slots.resize(kCloudSaveSlotCount);
    for (u32 i = 0; i < kCloudSaveSlotCount; ++i)
    {
        m_slots[i] = CloudSaveSlotInfo();
    }
    m_hasRefreshed = bfalse;
}

OnlineError CloudSaveService_Ubiservices::refreshCloudSaves()
{
    OnlineError pre = validateCloudSavePreconditions();
    if (pre.getType() != OnlineError::Success)
    {
        std::scoped_lock lock(m_mutex);
        resetCacheLocked();
        return pre;
    }

    auto session = ONLINE_ADAPTER_UBISERVICES->getSession();
    US_NS::EntityModule& modEntity = US_NS::EntityModule::module(*session);

    US_NS::EntityModule::SearchFilter filter;
    filter.m_type = US_NS::String(kCloudSaveType);
    filter.m_tags.push_back(US_NS::String(kCloudSaveTag));

    US_NS::AsyncResult<US_NS::Vector<US_NS::EntityProfile>> result =
        modEntity.searchEntitiesProfile(filter, US_NS::Vector<US_NS::ProfileId>(), session->getWorkingSpaceId(), US_NS::ResultRange(0, 20));

    waitForAsyncResultProcessing("CloudSave.Refresh", result);

    if (result.hasFailed())
    {
        std::scoped_lock lock(m_mutex);
        resetCacheLocked();
        return mapUbiservicesAsyncError(result.getAsyncResultError());
    }

    if (result.isCanceled())
    {
        std::scoped_lock lock(m_mutex);
        resetCacheLocked();
        return OnlineError(OnlineError::Cancelled, OnlineError::Profile, 0);
    }

    if (!result.hasSucceeded())
    {
        ITF_ASSERT_MSG(false, "Async result logic issue!");
        std::scoped_lock lock(m_mutex);
        resetCacheLocked();
        return OnlineError(OnlineError::Invalid, OnlineError::Profile, 0);
    }

    SafeArray<CloudSaveSlotInfo> slotInfos;
    slotInfos.resize(kCloudSaveSlotCount);

    for (u32 i = 0; i < kCloudSaveSlotCount; ++i)
    {
        slotInfos[i] = CloudSaveSlotInfo();
    }

    const US_NS::Vector<US_NS::EntityProfile>& entities = result.getResult();
    for (const auto& entity : entities)
    {
        u32 slotIndex = U32_INVALID;
        if (!parseSlotIndexFromName(entity.getName(), slotIndex))
        {
            continue;
        }

        CloudSaveSlotInfo info = parseInfoFromJsonData(entity.getJsonData());
        info.m_isAvailable = btrue;
        slotInfos[slotIndex] = info;
    }

    {
        std::scoped_lock lock(m_mutex);
        m_slots = slotInfos;
        m_hasRefreshed = btrue;
    }

    return OnlineError(OnlineError::Success, OnlineError::Profile);
}

bbool CloudSaveService_Ubiservices::getSlotInfo(u32 _slotIndex, CloudSaveSlotInfo& _outInfo)
{
    std::scoped_lock lock(m_mutex);
    if (!m_hasRefreshed)
        return bfalse;

    if (!isSlotIndexValid(_slotIndex))
        return bfalse;

    _outInfo = m_slots[_slotIndex];
    return btrue;
}

OnlineError CloudSaveService_Ubiservices::uploadSlot(
    u32 _slotIndex,
    const void* _data,
    u32 _dataSize,
    const String8& _displayName,
    u64 _timestamp)
{
    if (!isSlotIndexValid(_slotIndex))
    {
        return OnlineError(OnlineError::Invalid, OnlineError::Profile, 0);
    }

    OnlineError pre = validateCloudSavePreconditions();
    if (pre.getType() != OnlineError::Success)
        return pre;

    const u32 crc32 = (_data && _dataSize) ? CRC::compute32((const unsigned char*)_data, (int)_dataSize) : 0;
    const US_NS::String jsonData = buildSlotJsonData(_slotIndex, _displayName, _timestamp, _dataSize, crc32);

    auto session = ONLINE_ADAPTER_UBISERVICES->getSession();
    US_NS::EntityModule& modEntity = US_NS::EntityModule::module(*session);

    US_NS::EntityProfile entity;
    OnlineError findErr = findCloudSaveEntityForSlot(_slotIndex, entity);
    if (findErr.getType() != OnlineError::Success)
        return findErr;

    const US_NS::Vector<uint8> usData = makeUsByteVector(_data, _dataSize);
    const US_NS::Vector tags = {US_NS::String(kCloudSaveTag)};

    if (!entity.getEntityId().isValid())
    {
        US_NS::EntityCreation creation(US_NS::String(kCloudSaveType), slotName(_slotIndex), jsonData, tags, session->getWorkingSpaceId());

        US_NS::AsyncResult<US_NS::EntityProfile> createResult =
            modEntity.createEntityProfile(creation, US_NS::ExtendedStorageProvider::S3, usData, US_NS::String("bin"));

        waitForAsyncResultProcessing("CloudSave.Create", createResult);

        if (createResult.hasFailed())
        {
            return mapUbiservicesAsyncError(createResult.getAsyncResultError());
        }
        if (createResult.isCanceled())
        {
            return OnlineError(OnlineError::Cancelled, OnlineError::Profile, 0);
        }
        if (!createResult.hasSucceeded())
        {
            ITF_ASSERT_MSG(false, "Async result logic issue!");
            return OnlineError(OnlineError::Invalid, OnlineError::Profile, 0);
        }

        entity = createResult.getResult();
    }
    else
    {
        if (entity.getExtendedStorageProvider() == US_NS::ExtendedStorageProvider::None)
        {
            US_NS::AsyncResult<US_NS::EntityProfile> extResult =
                modEntity.createExtendedStorage(entity, US_NS::ExtendedStorageProvider::S3, usData, US_NS::String("bin"));

            waitForAsyncResultProcessing("CloudSave.CreateExt", extResult);

            if (extResult.hasFailed())
            {
                return mapUbiservicesAsyncError(extResult.getAsyncResultError());
            }
            if (extResult.isCanceled())
            {
                return OnlineError(OnlineError::Cancelled, OnlineError::Profile, 0);
            }
            if (!extResult.hasSucceeded())
            {
                ITF_ASSERT_MSG(false, "Async result logic issue!");
                return OnlineError(OnlineError::Invalid, OnlineError::Profile, 0);
            }

            entity = extResult.getResult();
        }
        else
        {
            US_NS::AsyncResult<US_NS::Empty> uploadResult = modEntity.updateExtendedStorage(entity, usData);
            waitForAsyncResultProcessing("CloudSave.UploadExt", uploadResult);

            if (uploadResult.hasFailed())
            {
                return mapUbiservicesAsyncError(uploadResult.getAsyncResultError());
            }
            if (uploadResult.isCanceled())
            {
                return OnlineError(OnlineError::Cancelled, OnlineError::Profile, 0);
            }
            if (!uploadResult.hasSucceeded())
            {
                ITF_ASSERT_MSG(false, "Async result logic issue!");
                return OnlineError(OnlineError::Invalid, OnlineError::Profile, 0);
            }
        }

        US_NS::EntityProfile toUpdate = entity;
        toUpdate.setTags(tags);
        toUpdate.setJsonData(jsonData);

        US_NS::AsyncResult<US_NS::EntityProfile> updateResult = modEntity.updateEntityProfile(toUpdate);
        waitForAsyncResultProcessing("CloudSave.UpdateMeta", updateResult);

        if (updateResult.hasFailed())
        {
            US_NS::AsyncResult<US_NS::EntityProfile> reqResult = modEntity.requestEntityProfile(entity.getEntityId());
            waitForAsyncResultProcessing("CloudSave.ReqMeta", reqResult);

            if (reqResult.hasFailed())
                return mapUbiservicesAsyncError(reqResult.getAsyncResultError());

            if (!reqResult.hasSucceeded())
                return OnlineError(OnlineError::Invalid, OnlineError::Profile, 0);

            toUpdate = reqResult.getResult();
            toUpdate.setTags(tags);
            toUpdate.setJsonData(jsonData);

            updateResult = modEntity.updateEntityProfile(toUpdate);
            waitForAsyncResultProcessing("CloudSave.UpdateMeta2", updateResult);

            if (updateResult.hasFailed())
                return mapUbiservicesAsyncError(updateResult.getAsyncResultError());
        }

        if (updateResult.isCanceled())
            return OnlineError(OnlineError::Cancelled, OnlineError::Profile, 0);

        if (updateResult.hasSucceeded())
            entity = updateResult.getResult();
    }

    CloudSaveSlotInfo slotInfo;
    slotInfo.m_isAvailable = btrue;
    slotInfo.m_displayName = _displayName;
    slotInfo.m_timestamp = _timestamp;
    slotInfo.m_dataSize = _dataSize;
    slotInfo.m_crc32 = crc32;

    {
        std::scoped_lock lock(m_mutex);
        if (m_slots.size() != kCloudSaveSlotCount)
        {
            m_slots.resize(kCloudSaveSlotCount);
        }
        m_slots[_slotIndex] = slotInfo;
        m_hasRefreshed = btrue;
    }

    return OnlineError(OnlineError::Success, OnlineError::Profile);
}

OnlineError CloudSaveService_Ubiservices::downloadSlot(
    u32 _slotIndex,
    ITF_VECTOR<u8>& _outData,
    CloudSaveSlotInfo& _outInfo)
{
    _outData.clear();
    _outInfo = CloudSaveSlotInfo();

    if (!isSlotIndexValid(_slotIndex))
    {
        return OnlineError(OnlineError::Invalid, OnlineError::Profile, 0);
    }

    OnlineError pre = validateCloudSavePreconditions();
    if (pre.getType() != OnlineError::Success)
        return pre;

    US_NS::EntityProfile entity;
    OnlineError findErr = findCloudSaveEntityForSlot(_slotIndex, entity);
    if (findErr.getType() != OnlineError::Success)
        return findErr;

    if (!entity.getEntityId().isValid())
    {
        return OnlineError(OnlineError::UbiServer, OnlineError::Profile, 404);
    }

    auto session = ONLINE_ADAPTER_UBISERVICES->getSession();
    US_NS::EntityModule& modEntity = US_NS::EntityModule::module(*session);

    US_NS::AsyncResult<US_NS::Vector<uint8>> downloadResult = modEntity.requestExtendedStorage(entity);
    waitForAsyncResultProcessing("CloudSave.DownloadExt", downloadResult);

    if (downloadResult.hasFailed())
    {
        return mapUbiservicesAsyncError(downloadResult.getAsyncResultError());
    }
    if (downloadResult.isCanceled())
    {
        return OnlineError(OnlineError::Cancelled, OnlineError::Profile, 0);
    }
    if (!downloadResult.hasSucceeded())
    {
        ITF_ASSERT_MSG(false, "Async result logic issue!");
        return OnlineError(OnlineError::Invalid, OnlineError::Profile, 0);
    }

    const US_NS::Vector<uint8>& usData = downloadResult.getResult();
    _outData.resize(usData.size());
    if (!usData.empty())
    {
        std::memcpy(&_outData[0], &usData[0], usData.size());
    }

    CloudSaveSlotInfo slotInfo = parseInfoFromJsonData(entity.getJsonData());
    slotInfo.m_isAvailable = btrue;

    if (_outData.size() > 0)
    {
        const u32 crc32 = CRC::compute32((const unsigned char*)&_outData[0], (int)_outData.size());
        if (slotInfo.m_crc32 != 0 && slotInfo.m_crc32 != crc32)
        {
            LOG("[CloudSave] CRC mismatch for slot %u (meta %u, dl %u)", _slotIndex, slotInfo.m_crc32, crc32);
        }
        slotInfo.m_dataSize = (u32)_outData.size();
        slotInfo.m_crc32 = crc32;
    }

    _outInfo = slotInfo;

    {
        std::scoped_lock lock(m_mutex);
        if (m_slots.size() != kCloudSaveSlotCount)
        {
            m_slots.resize(kCloudSaveSlotCount);
        }
        m_slots[_slotIndex] = slotInfo;
        m_hasRefreshed = btrue;
    }

    return OnlineError(OnlineError::Success, OnlineError::Profile);
}

} // namespace ITF

#endif // ITF_SUPPORT_UBISERVICES
