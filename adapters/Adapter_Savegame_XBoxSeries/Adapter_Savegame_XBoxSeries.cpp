#include "precompiled_Adapter_Savegame_XBoxSeries.h"

#include "adapters/Adapter_Savegame_XBoxSeries/Adapter_Savegame_XBoxSeries.h"
#include "adapters/SystemAdapter_xboxseries/SystemAdapter_xboxseries.h"

#include "core/utility.h"

#include <algorithm>

#include <XGameSaveWrappers.hpp>

namespace ITF
{
    static constexpr u32 MAX_SAVE_DATA_MEMORY_SIZE = 1 * 1024 * 1024;

    Adapter_Savegame_XBoxSeries::Adapter_Savegame_XBoxSeries() = default;
    Adapter_Savegame_XBoxSeries::~Adapter_Savegame_XBoxSeries() = default;


    void Adapter_Savegame_XBoxSeries::init()
    {
        setDeviceChosenFlags(btrue, btrue);

        // ???
        m_chooseDevice_MinFreeSize = MAX_SAVE_DATA_MEMORY_SIZE;

        m_operationThreadDone = bfalse;

        m_operationThread.create(
            methodToCallback<&Adapter_Savegame_XBoxSeries::operationThreadMethod>(),
            this,
            ThreadSettings::m_settings[eThreadId_SaveManager]);
    }

    void Adapter_Savegame_XBoxSeries::term()
    {
        {
            ScopeLockMutex lock(m_operationCondVar.getMutex());
            m_stopOperationThread = btrue;
        }

        m_operationCondVar.signal();

        {
            ScopeLockMutex lock(m_operationCondVar.getMutex());
            while (!m_operationThreadDone)
            {
                ScopeUnLockMutex unlock(m_operationCondVar.getMutex());
                sleep(1);
            }
        }

        m_provider.reset();
    }

    u32 Adapter_Savegame_XBoxSeries::operationThreadMethod()
    {
        initializeLibrary();

        ScopeLockMutex lock(m_operationCondVar.getMutex());

        m_initDone = true;

        while (!m_stopOperationThread)
        {
            if (m_operations.empty())
            {
                m_operationCondVar.wait();
            }
            else
            {
                Operation operation = std::move(m_operations.front());
                m_operations.erase(m_operations.begin());
                ScopeUnLockMutex unlock(m_operationCondVar.getMutex()); // the operation was moved, we are safe to unlock
                processOperation(std::move(operation));
            }
        }

        m_operationThreadDone = true;
        return 0;
    }

    static XUserHandle pullUserHandle()
    {
        XUserHandle handle = nullptr;
        do
        {
            handle = SYSTEM_ADAPTER_XBOX_SERIES->getInitialUserInfo().handle;
            if (handle == nullptr)
            {
                sleep(100);
            }
        } while (handle == nullptr);
        return handle;
    }

    void Adapter_Savegame_XBoxSeries::initializeLibrary()
    {
        m_provider = makeUnique<MemoryId::mId_SavegameManager, Microsoft::Xbox::Wrappers::GameSave::Provider>();
        XUserHandle handle = pullUserHandle();
        const char* serviceConfId = SYSTEM_ADAPTER_XBOX_SERIES->getServiceConfigurationId();
        m_provider->Initialize(handle, serviceConfId);

    }

    void Adapter_Savegame_XBoxSeries::addSaveOperation(SaveDataEntry _saveData)
    {
        ScopeLockMutex lock(m_operationCondVar.getMutex());
        m_operations.push_back(Operation{ Operation::Save, std::move(_saveData) });
        m_operationCondVar.signal();
    }

    void Adapter_Savegame_XBoxSeries::addLoadOperation()
    {
        ScopeLockMutex lock(m_operationCondVar.getMutex());
        Operation newOp;
        newOp.opType = Operation::Load;
        m_operations.push_back(std::move(newOp));
        m_operationCondVar.signal();
    }

    static u32 readBytesAsU32(const u8* _data)
    {
        return u32(_data[0]) + (u32(_data[1]) << 8) + (u32(_data[2]) << 16) + (u32(_data[3]) << 24);
    }

    static void writeU32ToBytes(u32 _src, u8(&_dst)[4])
    {
        _dst[0] = u8(_src & 0xFF);
        _dst[1] = u8((_src >> 8) & 0xFF);
        _dst[2] = u8((_src >> 16) & 0xFF);
        _dst[3] = u8((_src >> 24) & 0xFF);
    }

    const char SAVEGAME_CONTAINER_NAME[] = "SaveContainer";
    const char SAVEGAME_BLOB_NAME[] = "SaveBlob";

    void Adapter_Savegame_XBoxSeries::processLoadOperation(SaveDataEntry& dataLoaded)
    {
        auto blobData = m_provider->Load(SAVEGAME_CONTAINER_NAME, SAVEGAME_BLOB_NAME);
        dataLoaded.loadFromBuffer(blobData);
    }

    void Adapter_Savegame_XBoxSeries::processSaveOperation(const SaveDataEntry& _dataToSave) // in thread
    {
        SaveDataEntry::BlobData saveData;
        _dataToSave.saveToBuffer(saveData);
        m_provider->Save(SAVEGAME_CONTAINER_NAME, SAVEGAME_BLOB_NAME, saveData.size(), saveData.data());
    }


    void Adapter_Savegame_XBoxSeries::processOperation(Operation operation) // in thread
    {
        if (operation.opType == Operation::Load)
        {
            processLoadOperation(operation.saveData);
        }
        else if (operation.opType == Operation::Save)
        {
            processSaveOperation(operation.saveData);
        }

        ScopeLockMutex lock(m_doneLoadSaveMutex);
        m_doneLoadOrSave.push_back(operation.saveData);
    }

    bbool Adapter_Savegame_XBoxSeries::hasPendingOperation()
    {
        {
            ScopeLockMutex lock(m_operationCondVar.getMutex());
            if (!m_operationThreadDone && (!m_initDone || !m_operations.empty()))
            {
                return btrue;
            }
        }

        {
            ScopeLockMutex lock(m_doneLoadSaveMutex);
            return !m_doneLoadOrSave.empty();
        }
    }

    void Adapter_Savegame_XBoxSeries::SaveDataEntry::loadFromBuffer(const Adapter_Savegame_XBoxSeries::SaveDataEntry::BlobData& _buffer)
    {
        m_files.clear();
        auto it = _buffer.begin();
        while (it < _buffer.end())
        {
            SaveDataEntry::File& file = m_files.emplace_back();

            file.name = String8(reinterpret_cast<const char*>(&*it));
            ITF_ASSERT(it + file.name.getLen() + 1 < _buffer.end());
            it += file.name.getLen() + 1;

            file.displayName = String8(reinterpret_cast<const char*>(&*it));
            ITF_ASSERT(it + file.displayName.getLen() + 1 < _buffer.end());
            it += file.displayName.getLen() + 1;

            // encoded size as 4 u8 (allow wrong alignment)
            ITF_ASSERT(it + sizeof(u32) <= _buffer.end());
            u32 size = readBytesAsU32(&*it);
            it += sizeof(u32);
            ITF_ASSERT(it + size <= _buffer.end());
            file.data.assign(it, it + size);
            it += size;
        }
    }

    void Adapter_Savegame_XBoxSeries::SaveDataEntry::saveToBuffer(Adapter_Savegame_XBoxSeries::SaveDataEntry::BlobData& _buffer) const
    {
        _buffer.clear();
        for (const auto& file : m_files)
        {
            const u8* fileNameAsU8 = reinterpret_cast<const u8*>(file.name.cStr());
            _buffer.insert(_buffer.end(), fileNameAsU8, fileNameAsU8 + file.name.getLen() + 1);
            fileNameAsU8 = reinterpret_cast<const u8*>(file.displayName.cStr());
            _buffer.insert(_buffer.end(), fileNameAsU8, fileNameAsU8 + file.displayName.getLen() + 1);
            u8 sizeIn4Bytes[4]{};
            writeU32ToBytes(u32(file.data.size()), sizeIn4Bytes);
            _buffer.insert(_buffer.end(), sizeIn4Bytes, sizeIn4Bytes + sizeof(sizeIn4Bytes));
            _buffer.insert(_buffer.end(), file.data.begin(), file.data.end());
        }
    }


    Adapter_Savegame::ErrorCode Adapter_Savegame_XBoxSeries::startSavegameEnumerationUpdate(u32 _playerIndex)
    {
        {
            ScopeLockMutex lock(m_operationCondVar.getMutex());
            ITF_ASSERT(m_operations.empty());
        }

        addLoadOperation();

        return Error_Ok;
    }

    // Player index is ignored for the moment: progression is the one of the initial player
    bbool Adapter_Savegame_XBoxSeries::getEnumeratedContent(u32 /* _playerIndex */, u32 _index, String8& _baseName, String& _displayName, bbool& _emptySave)
    {
        ITF_ASSERT(!hasPendingOperation());

        SaveDataEntry& saveData = m_currentFullSaveData;

        if (_index >= saveData.m_files.size())
        {
            _emptySave = btrue;
            return bfalse;
        }

        _baseName = saveData.m_files[_index].name;
        _displayName = String(saveData.m_files[_index].displayName.cStr());
        _emptySave = saveData.m_files[_index].data.empty();

        return btrue;
    }

    u32 Adapter_Savegame_XBoxSeries::getEnumeratedContentCount(u32 /* _playerIndex */)
    {
        ITF_ASSERT(!hasPendingOperation());
        return m_currentFullSaveData.m_files.size();
    }

    Adapter_Savegame::ErrorCode Adapter_Savegame_XBoxSeries::startSave(u32 /* _playerIndex */, u32 _slotIndex, const String8& _baseName, const String& displayName, const void* _data, u32 _dataSize, bbool /*_warnIfOverwriteForNewGame*/)
    {
        ITF_ASSERT(!hasPendingOperation());

        // Copy save data to modify them
        SaveDataEntry saveData = m_currentFullSaveData;

        if (_slotIndex >= saveData.m_files.size())
        {
            saveData.m_files.resize(_slotIndex + 1);
        }
        SaveDataEntry::File& file = saveData.m_files[_slotIndex];
        file.name = _baseName;
        file.displayName = String8(UTF16ToUTF8(displayName.cStr()).get());
        const u8* srcData = reinterpret_cast<const u8*>(_data);
        const u8* srcDataEnd = srcData + _dataSize;
        file.data.assign(srcData, srcDataEnd);

        // m_currentSavegame (returned by getLastLoadedOrSavedData) will be updated in the main thread, after the operation.
        // We set here the fileIdxToLoad
        m_fileIdxToLoad = _slotIndex;
        m_fileToLoad = _baseName;

        addSaveOperation(std::move(saveData));

        return Error_Ok;
    }

    Adapter_Savegame::ErrorCode Adapter_Savegame_XBoxSeries::startLoad(u32 /* _playerIndex */, u32 _slotIndex, const String8& baseName)
    {
        ITF_ASSERT(!hasPendingOperation());

        m_fileIdxToLoad = _slotIndex;
        m_fileToLoad = baseName;

        // addLoadOperation will perform load in a thread, then assign m_currentSavegame in main thread during the update.
        addLoadOperation();

        return Error_Ok;
    }

    void Adapter_Savegame_XBoxSeries::update()
    {
        bool savedUpdated = false;

        {
            ScopeLockMutex lock(m_doneLoadSaveMutex);
            if (!m_doneLoadOrSave.empty())
            {
                m_currentFullSaveData = std::move(m_doneLoadOrSave.back());
                savedUpdated = true;
                m_doneLoadOrSave.clear();
            }
        }

        if (savedUpdated && m_fileIdxToLoad != U32_INVALID) // m_fileIdxToLoad == U32_INVALID means that it's just a loading from enumeration.
        {
            ITF_ASSERT_MSG(m_fileIdxToLoad < m_currentFullSaveData.m_files.size(), "Invalid save file index requested: %d", m_fileIdxToLoad);
            if (m_fileIdxToLoad < m_currentFullSaveData.m_files.size())
            {
                ITF_ASSERT(m_currentFullSaveData.m_files[m_fileIdxToLoad].name == m_fileToLoad);
                SaveDataEntry::BlobData& blobSave = m_currentFullSaveData.m_files[m_fileIdxToLoad].data;
                m_currentSavegame.assign(blobSave.begin(), blobSave.end());
            }
        }
    }


}
