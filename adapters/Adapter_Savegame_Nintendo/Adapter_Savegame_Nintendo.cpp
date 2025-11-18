#include "precompiled_Adapter_Savegame_Nintendo.h"

#include "Adapter_Savegame_Nintendo.h"
#include "core/AdaptersInterfaces/ThreadManager.h"
#include "core/file/FileServer.h"

#include "core/utility.h"

// #include "adapters/TRCAdapter_Orbis/TRCAdapter_Orbis.h"
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#include "adapters/SystemAdapter_Nintendo/SystemAdapter_Nintendo.h"
#include "engine/AdaptersInterfaces/RewardAdapter.h"
#include "engine/localisation/LocalisationManager.h"
#include "core/crc.h"
#include "core/UnicodeTools.h"

#define ITF_NINTENDO_ACCOUNT_ALREADY_SELECTED_AT_STARTUP

namespace ITF
{

    static constexpr u32 MAX_SAVE_DATA_MEMORY_SIZE = 1 * 1024 * 1024;

    Adapter_Savegame_Nintendo::Adapter_Savegame_Nintendo() = default;

    Adapter_Savegame_Nintendo::~Adapter_Savegame_Nintendo() = default;

    void Adapter_Savegame_Nintendo::init()
    {
        m_initialUserUID = SYSTEM_ADAPTER_NINTENDO->getInitialUserId();

        setDeviceChosenFlags(btrue, btrue);

        // ???
        m_chooseDevice_MinFreeSize = MAX_SAVE_DATA_MEMORY_SIZE;

        m_operationThreadDone = bfalse;

        m_operationThread.create(
            [](void* _this) -> u32
            {
                return reinterpret_cast<Adapter_Savegame_Nintendo*>(_this)->operationThreadMethod();
            },
            this,
            ThreadSettings::m_settings[eThreadId_SaveManager]);
    }

    static String8 getSaveMountPoint()
    {
        static String8 saveMountPoint("save");
        return saveMountPoint;
    }

    static String8 getSaveFilePath()
    {
        // A more complex setup could separate save files by slots
        static String8 saveFilePath = getSaveMountPoint() + ":/AllSlots.sav";
        return saveFilePath; 
    }

    void Adapter_Savegame_Nintendo::term()
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

        if (m_saveMounted)
        {
            nn::fs::Unmount(getSaveMountPoint().cStr());
            m_saveMounted = false;
        }
    }

    void Adapter_Savegame_Nintendo::setupMemorySaveArea()
    {
    #if !defined(ITF_NINTENDO_ACCOUNT_ALREADY_SELECTED_AT_STARTUP)
        nn::Result ensureSaveResult = nn::fs::EnsureSaveData(m_initialUserUID); // not strictly needed as the application is supposely set to
        if (ensureSaveResult.IsSuccess())
    #endif
        {
            nn::Result result = nn::fs::MountSaveData(getSaveMountPoint().cStr(), m_initialUserUID);
            if (result.IsSuccess())
            {
                ScopeLockMutex lock(m_operationCondVar.getMutex());
                m_saveMounted = true;
                return;
            }
        }
        ITF_ASSERT_MSG(false, "Failed to mount %s", getSaveMountPoint().cStr());
        m_saveMounted = false;
    }


    u32 Adapter_Savegame_Nintendo::operationThreadMethod()
    {
        setupMemorySaveArea();

        if (!m_saveMounted)
            return 1;

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


    void Adapter_Savegame_Nintendo::addSaveOperation(SaveDataEntry _saveData)
    {
        ScopeLockMutex lock(m_operationCondVar.getMutex());
        m_operations.push_back(Operation{ Operation::Save, std::move(_saveData) });
        m_operationCondVar.signal();
    }

    void Adapter_Savegame_Nintendo::addLoadOperation()
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

    void Adapter_Savegame_Nintendo::processLoadOperation(SaveDataEntry& dataLoaded)
    {
        LOG("[SAVE_ADAPTER] processLoadOperation() - Starting load from: %s", getSaveFilePath().cStr());
        nn::fs::FileHandle fileHandle{};
        nn::Result result = nn::fs::OpenFile(&fileHandle, getSaveFilePath().cStr(), nn::fs::OpenMode_Read);

        ITF_ASSERT_MSG(result.IsSuccess() || nn::fs::ResultPathNotFound::Includes(result), "[SAVE ADAPTER] Cannot open %s", getSaveFilePath().cStr());

        if (result.IsSuccess())
        {
            LOG("[SAVE_ADAPTER] processLoadOperation() - File opened successfully");
            auto fileCloser = finally([&]()
            {
                LOG("[SAVE_ADAPTER] processLoadOperation() - Closing file");
                nn::fs::CloseFile(fileHandle);
            });

            u8 sizeIn4Bytes[4]{};

            size_t readSize = 0;
            LOG("[SAVE_ADAPTER] processLoadOperation() - Reading size header");
            result = nn::fs::ReadFile(&readSize, fileHandle, 0, &sizeIn4Bytes, sizeof(sizeIn4Bytes));

            ITF_ASSERT_MSG(result.IsSuccess(), "[SAVE ADAPTER] Size read failed");
            LOG("[SAVE_ADAPTER] processLoadOperation() - Size header read: %zu bytes", readSize);

            u32 size = readBytesAsU32(sizeIn4Bytes);
            ITF_ASSERT(size < MAX_SAVE_DATA_MEMORY_SIZE);
            LOG("[SAVE_ADAPTER] processLoadOperation() - Data size: %u bytes", size);

            Vector<u8> loadedData;
            loadedData.resize(size);

            if (loadedData.size() > 0)
            {
                LOG("[SAVE_ADAPTER] processLoadOperation() - Reading content data");
                result = nn::fs::ReadFile(&readSize, fileHandle, sizeof(sizeIn4Bytes), loadedData.data(), loadedData.size());
                ITF_ASSERT_MSG(result.IsSuccess(), "[SAVE ADAPTER] Content read failed");
                LOG("[SAVE_ADAPTER] processLoadOperation() - Content read: %zu bytes", readSize);
            }

            LOG("[SAVE_ADAPTER] processLoadOperation() - Loading data from buffer");
            dataLoaded.loadFromBuffer(loadedData);
            LOG("[SAVE_ADAPTER] processLoadOperation() - Load completed successfully");
        }
        else
        {
            LOG("[SAVE_ADAPTER] processLoadOperation() - File not found, creating empty entry");
            // If the file does not exist, we create an empty entry
            dataLoaded.loadFromBuffer(Vector<u8>{});
        }
    }

    void Adapter_Savegame_Nintendo::processSaveOperation(const SaveDataEntry& _dataToSave) // in thread
    {
        Vector<u8> saveData;
        _dataToSave.saveToBuffer(saveData);
        LOG("[SAVE_ADAPTER] processSaveOperation() - Starting save to: %s, size: %zu bytes", getSaveFilePath().cStr(), saveData.size());

        nn::fs::FileHandle fileHandle{};
        nn::Result result = nn::fs::CreateFile(getSaveFilePath().cStr(), 0); // at first call, ensure to create the file.
        ITF_ASSERT_MSG(result.IsSuccess() || nn::fs::ResultPathAlreadyExists::Includes(result), "[SAVE ADAPTER] Cannot create %s", getSaveFilePath().cStr());
        LOG("[SAVE_ADAPTER] processSaveOperation() - CreateFile completed");
        result = nn::fs::OpenFile(&fileHandle, getSaveFilePath().cStr(), nn::fs::OpenMode_Write);

        ITF_ASSERT_MSG(result.IsSuccess(), "[SAVE ADAPTER] Cannot open %s", getSaveFilePath().cStr());

        if (!result.IsSuccess())
        {
            LOG("[SAVE_ADAPTER] processSaveOperation() - Failed to open file");
            return;
        }

        LOG("[SAVE_ADAPTER] processSaveOperation() - File opened successfully");
        {
            auto fileCloser = finally([&]()
            {
                LOG("[SAVE_ADAPTER] processSaveOperation() - Closing file");
                nn::fs::CloseFile(fileHandle);
            });

            u8 sizeIn4Bytes[4]{};
            writeU32ToBytes(saveData.size(), sizeIn4Bytes);

            // Change the file size first so the write will succeed
            LOG("[SAVE_ADAPTER] processSaveOperation() - Setting file size to %zu bytes", sizeof(sizeIn4Bytes) + saveData.size());
            result = nn::fs::SetFileSize(fileHandle, sizeof(sizeIn4Bytes) + saveData.size());

            ITF_ASSERT_MSG(result.IsSuccess(), "[SAVE ADAPTER] Cannot resize %s", getSaveFilePath().cStr());
            if (!result.IsSuccess())
            {
                LOG("[SAVE_ADAPTER] processSaveOperation() - SetFileSize failed");
                return;
            }
            LOG("[SAVE_ADAPTER] processSaveOperation() - SetFileSize succeeded");

            LOG("[SAVE_ADAPTER] processSaveOperation() - Writing size header");
            result = nn::fs::WriteFile(fileHandle, 0, sizeIn4Bytes, sizeof(sizeIn4Bytes), nn::fs::WriteOption{});

            ITF_ASSERT_MSG(result.IsSuccess(), "[SAVE ADAPTER] Cannot write size header to %s", getSaveFilePath().cStr());
            if (!result.IsSuccess())
            {
                LOG("[SAVE_ADAPTER] processSaveOperation() - Write size header failed");
                return;
            }
            LOG("[SAVE_ADAPTER] processSaveOperation() - Size header written");

            LOG("[SAVE_ADAPTER] processSaveOperation() - Writing content data (%zu bytes)", saveData.size());
            result = nn::fs::WriteFile(fileHandle, sizeof(sizeIn4Bytes), saveData.data(), saveData.size(), nn::fs::WriteOption::MakeValue(nn::fs::WriteOptionFlag_Flush));
            ITF_ASSERT_MSG(result.IsSuccess(), "[SAVE ADAPTER] Cannot write content of %s", getSaveFilePath().cStr());
            if (!result.IsSuccess())
            {
                LOG("[SAVE_ADAPTER] processSaveOperation() - Write content failed");
                return;
            }
            LOG("[SAVE_ADAPTER] processSaveOperation() - Save completed successfully");
        }

        LOG("[SAVE_ADAPTER] processSaveOperation() - Committing save data");
        result = nn::fs::Commit(getSaveMountPoint().cStr());
        ITF_ASSERT_MSG(result.IsSuccess(), "[SAVE ADAPTER] Failed to commit mount point %s", getSaveMountPoint().cStr());
        if (result.IsSuccess())
        {
            LOG("[SAVE_ADAPTER] processSaveOperation() - Commit succeeded");
        }
        else
        {
            LOG("[SAVE_ADAPTER] processSaveOperation() - Commit failed");
        }
    }


    void Adapter_Savegame_Nintendo::processOperation(Operation operation) // in thread
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

    bbool Adapter_Savegame_Nintendo::hasPendingOperation()
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

    void Adapter_Savegame_Nintendo::SaveDataEntry::loadFromBuffer(const Vector<u8>& _buffer)
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

    void Adapter_Savegame_Nintendo::SaveDataEntry::saveToBuffer(Vector<u8>& _buffer) const
    {
        _buffer.clear();
        for (const auto& file : m_files)
        {
            const u8* fileNameAsU8 = reinterpret_cast<const u8*>(file.name.cStr());
            _buffer.insert(_buffer.end(), fileNameAsU8, fileNameAsU8 + file.name.getLen() + 1);
            fileNameAsU8 = reinterpret_cast<const u8*>(file.displayName.cStr());
            _buffer.insert(_buffer.end(), fileNameAsU8, fileNameAsU8 + file.displayName.getLen() + 1);
            u8 sizeIn4Bytes[4]{};
            writeU32ToBytes(file.data.size(), sizeIn4Bytes);
            _buffer.insert(_buffer.end(), sizeIn4Bytes, sizeIn4Bytes + sizeof(sizeIn4Bytes));
            _buffer.insert(_buffer.end(), file.data.begin(), file.data.end());
        }
    }

    Adapter_Savegame::ErrorCode Adapter_Savegame_Nintendo::startSavegameEnumerationUpdate(u32 _playerIndex)
    {
        {
            ScopeLockMutex lock(m_operationCondVar.getMutex());
            ITF_ASSERT(m_operations.empty());
        }

        addLoadOperation();

        return Error_Ok;
    }

    // Player index is ignored for the moment: progression is the one of the initial player
    bbool Adapter_Savegame_Nintendo::getEnumeratedContent(u32 /* _playerIndex */, u32 _index, String8& _baseName, String& _displayName, bbool& _emptySave)
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

    u32 Adapter_Savegame_Nintendo::getEnumeratedContentCount(u32 /* _playerIndex */)
    {
        ITF_ASSERT(!hasPendingOperation());
        return m_currentFullSaveData.m_files.size();
    }

    Adapter_Savegame::ErrorCode Adapter_Savegame_Nintendo::startSave(u32 /* _playerIndex */, u32 _slotIndex, const String8& _baseName, const String& displayName, const void* _data, u32 _dataSize, bbool /*_warnIfOverwriteForNewGame*/)
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

    Adapter_Savegame::ErrorCode Adapter_Savegame_Nintendo::startLoad(u32 /* _playerIndex */, u32 _slotIndex, const String8& baseName)
    {
        ITF_ASSERT(!hasPendingOperation());

        m_fileIdxToLoad = _slotIndex;
        m_fileToLoad = baseName;

        // addLoadOperation will perform load in a thread, then assign m_currentSavegame in main thread during the update.
        addLoadOperation();

        return Error_Ok;
    }

    void Adapter_Savegame_Nintendo::update()
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
                m_currentSavegame = m_currentFullSaveData.m_files[m_fileIdxToLoad].data;
            }
        }
    }
}
