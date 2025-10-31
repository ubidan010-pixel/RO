#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_GAMEOPTIONPERSISTENCE_H_
#include "Ray_GameOptionPersistence.h"
#endif

#ifndef _ITF_RAY_GAMEOPTIONSERIALIZER_H_
#include "Ray_GameOptionSerializer.h"
#endif

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif

#define PERSISTENCE_LOG(format, ...) LOG("[GameOptionPersistence] - " format, ##__VA_ARGS__)

namespace ITF
{
static const char* DEFAULT_SAVE_BASENAME = "settings";
static const u32 DEFAULT_SAVE_SLOT = 3;
Ray_GameOptionPersistence::Ray_GameOptionPersistence(Ray_GameOptionManager* optionManager)
    : m_optionManager(optionManager)
    , m_state(State_Idle)
    , m_lastResult(Result_None)
    , m_currentPlayerIndex(0)
    , m_currentCallback(NULL)
    , m_saveBaseName(DEFAULT_SAVE_BASENAME)
    , m_saveSlotIndex(DEFAULT_SAVE_SLOT)
{
    ITF_ASSERT_MSG(m_optionManager, "Option manager cannot be NULL");
}

Ray_GameOptionPersistence::~Ray_GameOptionPersistence()
{
}

bbool Ray_GameOptionPersistence::isPlatformSupported()
{
#if defined(ITF_WINDOWS) || defined(ITF_PS5) || defined(ITF_NINTENDO)
    return btrue;
#else
    return bfalse;
#endif
}

void Ray_GameOptionPersistence::init()
{
    PERSISTENCE_LOG("Initialized (basename: '%s', slot: %u)", m_saveBaseName.cStr(), m_saveSlotIndex);
    
    if (!isPlatformSupported())
    {
        PERSISTENCE_LOG("Warning: GameOptions save/load not supported on this platform");
        return;
    }
    
    if (SAVEGAME_ADAPTER)
    {
        SAVEGAME_ADAPTER->prepareInit(64 * 1024);
    }
}

bbool Ray_GameOptionPersistence::startSaveOptions(u32 playerIndex, CompletionCallback_t callback)
{
    if (!isPlatformSupported())
    {
        PERSISTENCE_LOG("Cannot save: platform not supported");
        if (callback)
            callback(Result_SaveFailed);
        return bfalse;
    }

    if (m_state != State_Idle)
    {
        PERSISTENCE_LOG("Cannot save: operation already in progress (state: %d)", m_state);
        return bfalse;
    }

    if (!m_optionManager)
    {
        PERSISTENCE_LOG("Cannot save: option manager is NULL");
        return bfalse;
    }

    if (!SAVEGAME_ADAPTER)
    {
        PERSISTENCE_LOG("Cannot save: SAVEGAME_ADAPTER is NULL");
        return bfalse;
    }

    Vector<u8> buffer;
    m_optionManager->serializeToBuffer(buffer);

    if (buffer.empty())
    {
        PERSISTENCE_LOG("Cannot save: serialization produced empty buffer");
        return bfalse;
    }

    PERSISTENCE_LOG("Starting save (player: %u, slot: %u, size: %u bytes)", playerIndex, m_saveSlotIndex, buffer.size());
    String displayName = "Game Settings";

    SAVEGAME_ADAPTER->setCodeCRC(Ray_GameOptionSerializer::computeFormatSignatureCRC());

    Adapter_Savegame::ErrorCode err = SAVEGAME_ADAPTER->startSave(
        playerIndex,
        m_saveSlotIndex,
        m_saveBaseName,
        displayName,
        buffer.data(),
        buffer.size(),
        bfalse
    );

    if (err != Adapter_Savegame::Error_Ok)
    {
        PERSISTENCE_LOG("Failed to start save: error code %d", err);
        setResult(Result_SaveFailed);
        if (callback)
            callback(Result_SaveFailed);
        return bfalse;
    }
    setState(State_Saving);
    m_currentPlayerIndex = playerIndex;
    m_currentCallback = callback;

    return btrue;
}

bbool Ray_GameOptionPersistence::startLoadOptions(u32 playerIndex, CompletionCallback_t callback)
{
    if (!isPlatformSupported())
    {
        PERSISTENCE_LOG("Cannot load: platform not supported");
        if (callback)
            callback(Result_LoadNotFound);
        return bfalse;
    }

    if (m_state != State_Idle)
    {
        PERSISTENCE_LOG("Cannot load: operation already in progress (state: %d)", m_state);
        return bfalse;
    }

    if (!m_optionManager)
    {
        PERSISTENCE_LOG("Cannot load: option manager is NULL");
        return bfalse;
    }

    if (!SAVEGAME_ADAPTER)
    {
        PERSISTENCE_LOG("Cannot load: SAVEGAME_ADAPTER is NULL");
        return bfalse;
    }

    PERSISTENCE_LOG("Starting load (player: %u, slot: %u). Ensuring enumeration first...", playerIndex, m_saveSlotIndex);

    u32 crc = Ray_GameOptionSerializer::computeFormatSignatureCRC();
    PERSISTENCE_LOG("Setting code CRC to 0x%08x for load", crc);
    SAVEGAME_ADAPTER->setCodeCRC(crc);

    Adapter_Savegame::ErrorCode errEnum = SAVEGAME_ADAPTER->startSavegameEnumerationUpdate(playerIndex);
    if (errEnum != Adapter_Savegame::Error_Ok)
    {
        PERSISTENCE_LOG("Failed to start enumeration: error code %d", errEnum);
        setResult(Result_LoadFailed);
        if (callback)
            callback(Result_LoadFailed);
        return bfalse;
    }
    setState(State_Enumerating);
    m_currentPlayerIndex = playerIndex;
    m_currentCallback = callback;
    return btrue;
}

void Ray_GameOptionPersistence::update()
{
    if (m_state == State_Idle)
        return;

    if (!SAVEGAME_ADAPTER)
        return;
    if (SAVEGAME_ADAPTER->hasPendingOperation())
        return;
    Adapter_Savegame::ErrorCode err = SAVEGAME_ADAPTER->getLastAsyncErrorCode();

    if (m_state == State_Enumerating)
    {
        PERSISTENCE_LOG("Enumeration complete. Starting actual load for '%s' (slot %u)...", m_saveBaseName.cStr(), m_saveSlotIndex);
        u32 enumeratedIndex = U32_INVALID;
        u32 count = SAVEGAME_ADAPTER->getEnumeratedContentCount(m_currentPlayerIndex);
        PERSISTENCE_LOG("Enumerated content count: %u", count);
        for (u32 i = 0; i < count; ++i)
        {
            String8 baseName = m_saveBaseName;
            String displayName;
            bbool emptySave = bfalse;
            if (SAVEGAME_ADAPTER->getEnumeratedContent(m_currentPlayerIndex, i, baseName, displayName, emptySave))
            {
                if (baseName == m_saveBaseName)
                {
                    enumeratedIndex = i;
                    break;
                }
            }
        }
        if (enumeratedIndex == U32_INVALID)
        {
            PERSISTENCE_LOG("Save '%s' not found in enumeration; treating as not found.", m_saveBaseName.cStr());
            setResult(Result_LoadNotFound);
            fireCallback(Result_LoadNotFound);
            setState(State_Idle);
            return;
        }

        Adapter_Savegame::ErrorCode errLoad = SAVEGAME_ADAPTER->startLoad(
            m_currentPlayerIndex,
            enumeratedIndex,
            m_saveBaseName);

        if (errLoad == Adapter_Savegame::Error_SavegameNotFound)
        {
            PERSISTENCE_LOG("Save file not found (first time playing?)");
            setResult(Result_LoadNotFound);
            fireCallback(Result_LoadNotFound);
            setState(State_Idle);
            return;
        }
        if (errLoad != Adapter_Savegame::Error_Ok)
        {
            PERSISTENCE_LOG("Failed to start load: error code %d", errLoad);
            setResult(Result_LoadFailed);
            fireCallback(Result_LoadFailed);
            setState(State_Idle);
            return;
        }
        setState(State_Loading);
        return;
    }

    if (m_state == State_Saving)
    {
        if (err == Adapter_Savegame::Error_Ok)
        {
            PERSISTENCE_LOG("Save completed successfully");
            m_optionManager->clearAllDirtyFlags();

            setResult(Result_SaveSuccess);
            fireCallback(Result_SaveSuccess);
        }
        else
        {
            PERSISTENCE_LOG("Save failed: error code %d", err);
            setResult(Result_SaveFailed);
            fireCallback(Result_SaveFailed);
        }
        setState(State_Idle);
    }
    else if (m_state == State_Loading)
    {
        if (err == Adapter_Savegame::Error_Ok)
        {
            const Vector<u8>& buffer = SAVEGAME_ADAPTER->getLastLoadedOrSavedData();

            if (buffer.empty())
            {
                PERSISTENCE_LOG("Load failed: empty buffer");
                setResult(Result_LoadFailed);
                fireCallback(Result_LoadFailed);
            }
            else
            {
                bbool success = m_optionManager->deserializeFromBuffer(buffer);

                if (success)
                {
                    PERSISTENCE_LOG("Load completed successfully (%u bytes)", buffer.size());
                    setResult(Result_LoadSuccess);
                    fireCallback(Result_LoadSuccess);
                }
                else
                {
                    PERSISTENCE_LOG("Load failed: deserialization error");
                    setResult(Result_LoadFailed);
                    fireCallback(Result_LoadFailed);
                }
            }
        }
        else if (err == Adapter_Savegame::Error_SavegameNotFound)
        {
            PERSISTENCE_LOG("Load failed: save file not found");
            setResult(Result_LoadNotFound);
            fireCallback(Result_LoadNotFound);
        }
        else if (err == Adapter_Savegame::Error_IncorrectSavegameFormat)
        {
            PERSISTENCE_LOG("Load failed: incorrect savegame format");
            setResult(Result_LoadFailed);
            fireCallback(Result_LoadFailed);
        }
        else
        {
            PERSISTENCE_LOG("Load failed: error code %d", err);
            setResult(Result_LoadFailed);
            fireCallback(Result_LoadFailed);
        }
        setState(State_Idle);
    }
}

void Ray_GameOptionPersistence::setState(State newState)
{
    if (m_state != newState)
    {
        m_state = newState;
    }
}

void Ray_GameOptionPersistence::setResult(Result result)
{
    m_lastResult = result;
}

void Ray_GameOptionPersistence::fireCallback(Result result)
{
    if (m_currentCallback)
    {
        CompletionCallback_t callback = m_currentCallback;
        m_currentCallback = NULL;
        callback(result);
    }
}

} // namespace ITF
