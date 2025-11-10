#include "precompiled_Adapter_Savegame_PS5.h"

#include "Adapter_Savegame_PS5.h"
#include "core/AdaptersInterfaces/ThreadManager.h"
#include "core/file/FileServer.h"

// #include "adapters/TRCAdapter_Orbis/TRCAdapter_Orbis.h"
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#include "adapters/SystemAdapter_PS5/SystemAdapter_PS5.h"
#include "engine/AdaptersInterfaces/RewardAdapter.h"
#include "engine/localisation/LocalisationManager.h"
#include "core/crc.h"
#include "core/UnicodeTools.h"

#include <sstream>
#include <save_data_dialog.h>
#include <libsysmodule.h>

namespace ITF
{

    static constexpr u32 MAX_SAVE_DATA_MEMORY_SIZE = 1*1024*1024;
    static constexpr u32 SAVE_DATA_MEMORY_SLOT_ID = 0;

    Adapter_Savegame_PS5::Adapter_Savegame_PS5() = default;

    Adapter_Savegame_PS5::~Adapter_Savegame_PS5() = default;
    
    void Adapter_Savegame_PS5::init()
    {
        initializeLibrary();

        setDeviceChosenFlags(btrue, btrue);

        // ???
        m_chooseDevice_MinFreeSize = MAX_SAVE_DATA_MEMORY_SIZE;

        m_operationThreadDone = bfalse;

        m_operationThread.create(
            [](void* _this) -> u32
            {
                return reinterpret_cast<Adapter_Savegame_PS5*>(_this)->operationThreadMethod();
            },
            this,
            ThreadSettings::m_settings[eThreadId_SaveManager]);
    }

    void Adapter_Savegame_PS5::term()
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

        ITF_VERIFY_SCE_CALL(sceSaveDataTerminate());
    }

    u32 Adapter_Savegame_PS5::operationThreadMethod()
    {
        initializeLibrary();
        setupMemorySaveArea();

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

    void Adapter_Savegame_PS5::initializeLibrary()
    {
        ITF_VERIFY_SCE_CALL(sceSaveDataInitialize3(nullptr));
    }

    void Adapter_Savegame_PS5::setupMemorySaveArea()
    {
        SceSaveDataMemorySetup2 setupParam{};
        setupParam.option = SCE_SAVE_DATA_MEMORY_OPTION_NONE;
        setupParam.userId = SYSTEM_ADAPTER_PS5->getInitialUserId();
        setupParam.memorySize = MAX_SAVE_DATA_MEMORY_SIZE;
        setupParam.iconMemorySize = 0;
        setupParam.slotId = 0;

        SceSaveDataMemorySetupResult setupResult{};

        ITF_VERIFY_SCE_CALL(sceSaveDataSetupSaveDataMemory2(&setupParam, &setupResult));
    }


    void Adapter_Savegame_PS5::addSaveOperation(SaveDataEntry _saveData)
    {
        ScopeLockMutex lock(m_operationCondVar.getMutex());
        m_operations.push_back(Operation{ Operation::Save, std::move(_saveData) });
        m_operationCondVar.signal();
    }

    void Adapter_Savegame_PS5::addLoadOperation()
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

    void Adapter_Savegame_PS5::processLoadOperation(SaveDataEntry& dataLoaded)
    {
        SceSaveDataMemoryData memData{};
        SceSaveDataMemoryGet2 getParam{};
        getParam.userId = SYSTEM_ADAPTER_PS5->getInitialUserId();
        getParam.data = &memData;
        getParam.slotId = SAVE_DATA_MEMORY_SLOT_ID;

        u8 sizeIn4Bytes[4]{};
        memData.buf = sizeIn4Bytes;
        memData.bufSize = sizeof(sizeIn4Bytes);
        memData.offset = 0;
        ITF_VERIFY_SCE_CALL(sceSaveDataGetSaveDataMemory2(&getParam));

        u32 size = readBytesAsU32(sizeIn4Bytes);
        ITF_ASSERT(size < MAX_SAVE_DATA_MEMORY_SIZE);

        Vector<u8> loadedData;
        loadedData.resize(size);

        memData.buf = loadedData.data();
        memData.bufSize = loadedData.size();
        memData.offset = sizeof(sizeIn4Bytes);
        if (memData.bufSize > 0)
        {
            ITF_VERIFY_SCE_CALL(sceSaveDataGetSaveDataMemory2(&getParam));
        }

        dataLoaded.loadFromBuffer(loadedData);
    }

    void Adapter_Savegame_PS5::processSaveOperation(const SaveDataEntry& _dataToSave) // in thread
    {
        Vector<u8> saveData;
        _dataToSave.saveToBuffer(saveData);

        SceSaveDataMemoryData memData{};
        SceSaveDataMemorySet2 getParam{};
        getParam.userId = SYSTEM_ADAPTER_PS5->getInitialUserId();
        getParam.data = &memData;
        getParam.slotId = SAVE_DATA_MEMORY_SLOT_ID;

        u8 sizeIn4Bytes[4]{};
        writeU32ToBytes(saveData.size(), sizeIn4Bytes);
        memData.buf = sizeIn4Bytes;
        memData.bufSize = sizeof(sizeIn4Bytes);
        memData.offset = 0;
        ITF_VERIFY_SCE_CALL(sceSaveDataSetSaveDataMemory2(&getParam));

        memData.buf = saveData.data();
        memData.bufSize = saveData.size();
        memData.offset = sizeof(sizeIn4Bytes);
        ITF_VERIFY_SCE_CALL(sceSaveDataSetSaveDataMemory2(&getParam));
    }


    void Adapter_Savegame_PS5::processOperation(Operation operation) // in thread
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

    bbool Adapter_Savegame_PS5::hasPendingOperation()
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

    void Adapter_Savegame_PS5::SaveDataEntry::loadFromBuffer(const Vector<u8>& _buffer)
    {
        m_files.clear();
        auto it = _buffer.begin();
        while (it < _buffer.end())
        {
            SaveDataEntry::File & file = m_files.emplace_back();

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

    void Adapter_Savegame_PS5::SaveDataEntry::saveToBuffer(Vector<u8>& _buffer) const
    {
        _buffer.clear();
        for (const auto& file : m_files)
        {
            const u8 * fileNameAsU8 = reinterpret_cast<const u8*>(file.name.cStr());
            _buffer.insert(_buffer.end(), fileNameAsU8, fileNameAsU8 + file.name.getLen() + 1);
            fileNameAsU8 = reinterpret_cast<const u8*>(file.displayName.cStr());
            _buffer.insert(_buffer.end(), fileNameAsU8, fileNameAsU8 + file.displayName.getLen() + 1);
            u8 sizeIn4Bytes[4]{};
            writeU32ToBytes(file.data.size(), sizeIn4Bytes);
            _buffer.insert(_buffer.end(), sizeIn4Bytes, sizeIn4Bytes + sizeof(sizeIn4Bytes));
            _buffer.insert(_buffer.end(), file.data.begin(), file.data.end());
        }
    }


    Adapter_Savegame::ErrorCode Adapter_Savegame_PS5::startSavegameEnumerationUpdate(u32 _playerIndex)
    {
        {
            ScopeLockMutex lock(m_operationCondVar.getMutex());
            ITF_ASSERT(m_operations.empty());
        }

        addLoadOperation();

        return Error_Ok;
    }

    // Player index is ignored for the moment: progression is the one of the initial player
    bbool Adapter_Savegame_PS5::getEnumeratedContent(u32 /* _playerIndex */, u32 _index, String8& _baseName, String& _displayName, bbool& _emptySave)
    {
        ITF_ASSERT(!hasPendingOperation());

        SaveDataEntry & saveData = m_currentFullSaveData;

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

    u32 Adapter_Savegame_PS5::getEnumeratedContentCount(u32 /* _playerIndex */)
    {
        ITF_ASSERT(!hasPendingOperation());
        return m_currentFullSaveData.m_files.size();
    }

    Adapter_Savegame::ErrorCode Adapter_Savegame_PS5::startSave(u32 /* _playerIndex */, u32 _slotIndex, const String8& _baseName, const String& displayName, const void* _data, u32 _dataSize, bbool /*_warnIfOverwriteForNewGame*/)
    {
        ITF_ASSERT(!hasPendingOperation());

        // Copy save data to modify them
        SaveDataEntry saveData = m_currentFullSaveData;

        if (_slotIndex >= saveData.m_files.size())
        {
            saveData.m_files.resize(_slotIndex + 1);
        }
        SaveDataEntry::File & file = saveData.m_files[_slotIndex];
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

    Adapter_Savegame::ErrorCode Adapter_Savegame_PS5::startLoad(u32 /* _playerIndex */, u32 _slotIndex, const String8& baseName)
    {
        ITF_ASSERT(!hasPendingOperation());

        m_fileIdxToLoad = _slotIndex;
        m_fileToLoad = baseName;

        // addLoadOperation will perform load in a thread, then assign m_currentSavegame in main thread during the update.
        addLoadOperation();

        return Error_Ok;
    }

    void Adapter_Savegame_PS5::update()
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
