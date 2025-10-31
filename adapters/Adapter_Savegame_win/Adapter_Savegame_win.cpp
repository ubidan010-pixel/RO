#include "precompiled_Adapter_Savegame_win.h"

#include "adapters/Adapter_Savegame_win/Adapter_Savegame_win.h"

#include "adapters/RewardAdapter_win/RewardAdapter_win.h"

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#include <io.h>
#include <shlobj.h>

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_

#define ZEROVARIABLE(x) ZeroMemory(&x, sizeof(x))
#define SF_CLOSEHANDLE(x) {if ((x)!=INVALID_HANDLE_VALUE) {CloseHandle(x); x = INVALID_HANDLE_VALUE;} }

namespace ITF
{

    Adapter_Savegame_win::Adapter_Savegame_win() :
        m_mainState(State_Idle),
        m_contentEnumeration_PlayerIndex(0),
        m_frameSinceLastStateChange(0),
        m_IOOperationThread(INVALID_HANDLE_VALUE),
        m_wasEnumerated(bfalse),
        m_choseDevice_PlayerIndex(0)
        {
        }

    void Adapter_Savegame_win::init()
    {
        //tests();
    }

    void Adapter_Savegame_win::tests()
    {
        /*
        startChooseDevice(0, 1048576,btrue);
        while (hasPendingOperation())
        {
            update();
            SYSTEM_ADAPTER->present();
        }
        SafeArray<u8> savetst;
        for (u32 i=0; i<512; i++)
            savetst.push_back((u8)i);

        
        startSave(0, "hello", String((u16*)L"mon display name"), &savetst[0], savetst.size(), bfalse);
        while (hasPendingOperation())
        {
            update();
            SYSTEM_ADAPTER->present();
        }

        LOG("---Found savegames:");
        u32 enumCount = getEnumeratedContentCount(0);
        for (u32 enumIndex=0; enumIndex<enumCount; enumIndex++)
        {
            String displayName;
            String8 baseName;
            bbool ok = getEnumeratedContent(0, enumIndex, baseName, displayName);
            ITF_ASSERT(ok);
            LOG("%ls (%s)", displayName.cStr(), baseName.cStr());
        }

        String displayName;
        ErrorCode code = startLoad(0, "hello");        
        ITF_ASSERT(code==Error_Ok);
        const ITF_VECTOR<u8> &loaded = getLastLoadedOrSavedData();
        ITF_ASSERT(savetst.size()==loaded.size());
        for (u32 i=0; i<savetst.size(); i++)
        {
            ITF_ASSERT(savetst[i]==loaded[i]);
        }
        */
    }

    void Adapter_Savegame_win::term()
    {
        ITF_ASSERT(m_mainState == State_Idle);
        SF_CLOSEHANDLE(m_IOOperationThread);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_win::handle_MustChooseDevice()
    {
        setMainState(ChooseDevice_ShowingSelector);
        setDeviceChosenFlags(bfalse,bfalse);
    }
    i32 Adapter_Savegame_win::findBasenameAmongEnumerated(const String8 &_baseName)
    {
        i32 foundIndex = -1;
        for (u32 i=0; i<m_filteredDeviceContent.size(); i++)
        {
            if (strcmp(m_filteredDeviceContent[i].szFileName, _baseName.cStr())==0)
            {
                foundIndex=(i32)i;
                break;
            }
        }
        return foundIndex;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Adapter_Savegame_win::getEnumeratedContent(u32 /*_playerIndex*/, u32 _index, String8 &_baseName, String &_displayName, bbool &_emptySave)
    {
        ITF_ASSERT(!hasPendingOperation());

        i32 rightIndex = findBasenameAmongEnumerated(_baseName);
        if(rightIndex>=0)
        {
            _index = rightIndex;

            _baseName = m_filteredDeviceContent[_index].szFileName;
            _displayName = (u16*)m_filteredDeviceContent[_index].sHeader.SaveName;
            _emptySave = bfalse;
        } else
        {
            _emptySave = btrue;
        }
        return btrue;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    Adapter_Savegame::ErrorCode Adapter_Savegame_win::startSavegameEnumerationUpdate(u32 _playerIndex)
    {
        bbool couldStartEnumeration = startContentEnumeration(_playerIndex);
        return couldStartEnumeration?Error_Ok:Error_CouldNotEnumerate;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Adapter_Savegame_win::startContentEnumeration(u32 _player)
    {
        //Start enumeration

        ITF_ASSERT(m_mainState == State_Idle);

        m_contentEnumeration_PlayerIndex = _player;

        m_filteredDeviceContent.clear();

        setMainState(EnumeratingContent);
        return btrue;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_win::handle_JustClosedDeviceSelectorGUI()
    {
        setMainState(State_Idle);

        bbool couldStartEnumeration = startContentEnumeration(m_choseDevice_PlayerIndex);
        LOG("--- Adapter_Savegame_win::handle_JustClosedDeviceSelectorGUI");
        setDeviceChosenFlags(btrue, !couldStartEnumeration);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    Adapter_Savegame::ErrorCode Adapter_Savegame_win::startChooseDevice(u32 _playerIndex, u32 _minFreeSize, bbool _isSilentWhenChosenNone)
    {
        bbool incorrectCall = (m_mainState!=State_Idle) ;
        ITF_ASSERT(!incorrectCall);

        m_lastLoadSaveError = Error_Ok;
        CreateDirectoryA(getSavegamePath("").cStr(), NULL);
        
        m_choseDevice_PlayerIndex = _playerIndex;
        setMainState(ChooseDevice_PostedShowSelector);
        m_filteredDeviceContent.clear();
        m_chooseDevice_MinFreeSize = _minFreeSize;
        setDeviceChosenFlags(bfalse,bfalse);

        return Error_Ok;

    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_win::handle_AsynchronousContentEnumeration()
    {
        String8 searchPath = getSavegamePath("*");
        _finddata_t findData;

        if (m_frameSinceLastStateChange!=20)
            return;

        intptr_t handle = _findfirst(searchPath.cStr(), &findData);
        if (handle!=-1)
        {
            do 
            {
                if (findData.attrib & _A_ARCH)
                {
                    // Read header
                    Header header;
                    String8 fullPath = getSavegamePath(findData.name);
                    if(readHeader(fullPath, &header))
                    {
                        Content cnt;
                        strcpy(cnt.szFileName, findData.name);                        
                        ITF_Memcpy(&cnt.sHeader, &header, getHeaderSize());
                        m_filteredDeviceContent.push_back(cnt);
                    }                    
                }
            } while (_findnext(handle, &findData)==0);
        }
        
        m_wasEnumerated = btrue;
        setMainState(State_Idle);
    }

    u32 Adapter_Savegame_win::getHeaderSize() const
    {
        static const u32 HEADER_SIZE = sizeof(Header);
        return HEADER_SIZE;
    }

    bbool Adapter_Savegame_win::readHeader(String8& filePath, Header* pHeader)
    {
        FILE *f = fopen(filePath.cStr(), "rb");
        if(f)
        {
            const u32 HEADER_SIZE = getHeaderSize();
            fseek(f, 0, SEEK_END);
            u32 size = (u32)ftell(f);
            
            if(size<HEADER_SIZE)
            {
                // corrupted file ?
                fclose(f);
                return bfalse;
            }

            fseek(f, 0, SEEK_SET);
            u32 count = u32(fread(pHeader, 1, HEADER_SIZE, f));
            fclose(f);

            if (count!=HEADER_SIZE)
            {
                ITF_ASSERT(0);
                return bfalse;
            }
            return btrue;
        }
        return bfalse;
    }

    Adapter_Savegame::ErrorCode Adapter_Savegame_win::startLoad(u32 _playerIndex, u32 _slotIndex, const String8 &_baseName)
    {
        ITF_ASSERT(m_wasEnumerated);
        ITF_ASSERT(!hasPendingOperation());

        //Search for content among enumerated
        if(findBasenameAmongEnumerated(_baseName)<0)
            return Error_SavegameNotFound;

        String8 searchPath = getSavegamePath(_baseName);
        FILE *f = fopen(searchPath.cStr(), "rb");
        if(f)
        {
            m_loadSaveBaseName = _baseName;
            const u32 HeaderSize    = getHeaderSize();
            const u32 RewardsSize   = REWARD_ADAPTER_WIN->getRewardsDataSize();

            fseek(f, 0, SEEK_END);
            const u32 GameDataSize = (u32)ftell(f);
            if(GameDataSize < HeaderSize + RewardsSize)
            {
                // Save corrupted
                fclose(f);
                return Error_IncorrectSavegameFormat;
            }

            u32 size = GameDataSize - HeaderSize - RewardsSize;
            
            // Skip header
            fseek(f, 0, SEEK_SET);
            Header sHeader;
            if ( fread(&sHeader, 1, HeaderSize, f)!=HeaderSize)
            {
                fclose(f);
                return Error_IncorrectSavegameFormat;
            }

            if(sHeader.CodeCRC != m_codeCRC)
            {
                fclose(f);
                return Error_IncorrectSavegameFormat;
            }

            // Read save data
            m_currentSavegame.resize(size);            
            if ( fread(&m_currentSavegame[0], 1, size, f)!=size)
            {
                fclose(f);
                return Error_IncorrectSavegameFormat;
            }

            // Read Rewards            
            u8* rewardsData = (u8*)Memory::malloc(RewardsSize);
            u32 rewardsSizeReaden = u32(fread(rewardsData, 1, RewardsSize, f));
            bbool res = REWARD_ADAPTER_WIN->setRewardsData(rewardsData, rewardsSizeReaden);
            Memory::free(rewardsData);
            fclose(f);

            if(!res) return Error_IncorrectSavegameFormat;
        }
        else
        {
            m_loadSaveBaseName.clear();
            return Error_LoadSave_IOError;
        }

        setMainState(Load_WaitingBeforeIdle);
        return Error_Ok;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    Adapter_Savegame::ErrorCode Adapter_Savegame_win::startSave(u32 _playerIndex, u32 _slotIndex, const String8 &baseName, const String &displayName, const void *_data, u32 _dataSize, bbool)
    {
        ITF_ASSERT_MSG(_dataSize<=m_chooseDevice_MinFreeSize, "Savegame size is greater than maximum size from device selector - ignore if you bypassed the selector");
        ITF_ASSERT(!isSaving());

        if(!_data)
        {
            DeleteFileA( getSavegamePath(baseName).cStr() );
            startSavegameEnumerationUpdate(_playerIndex);
            return Error_Ok;
        }

        m_lastLoadSaveError = Error_Ok;
        m_isSaving = btrue;
        m_loadSaveBaseName = baseName;
        m_displayedName = displayName;
        setMainState(Save_CreatingContent);
        m_currentSavegame.resize(_dataSize);
        memcpy(&m_currentSavegame[0], _data, _dataSize);
        m_wasEnumerated = bfalse;
        return Error_Ok;
    }

    
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_win::handle_Save_CreateContent()
    {
        if (m_frameSinceLastStateChange!=10)
        {
            setMainState(Save_WritingFile);
            m_IOOperationThread = CreateThread(NULL, 65536, saveThreadRoutine, this,  0, NULL);
        }

    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_win::handle_Load_WaitingBeforeIdle()
    {
        if (m_frameSinceLastStateChange!=20)
            return;
        setMainState(State_Idle);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    String8 Adapter_Savegame_win::getSavegamePath(const String8 &_filename)
    {
        char buffer[MAX_PATH];
        strcpy(buffer, "c:\\");
        PIDLIST_ABSOLUTE idList=NULL;
        SHGetFolderLocation(NULL, CSIDL_MYDOCUMENTS, NULL, 0, &idList);
        ITF_ASSERT(idList!=NULL);
        SHGetPathFromIDListA(idList, buffer);
        String8 r = buffer;
        char lastChar;
        if (r.getLen()>0)
        {
            lastChar = r.cStr()[r.getLen()-1];
            if (lastChar!='/' && lastChar!='\\')
                r += '\\';
        }
        r+="ITF savegames";

        if (_filename.getLen())
        {
            r = r+"\\"+_filename;
        }

        ILFree(idList);
        return r;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    DWORD Adapter_Savegame_win::saveThreadRoutine(void *_param)
    {
        Adapter_Savegame_win *caller = (Adapter_Savegame_win*)_param;
        
        HANDLE loadSaveFileHandle;
        CreateDirectoryA(getSavegamePath("").cStr(), NULL);
        loadSaveFileHandle = CreateFileA( getSavegamePath(caller->m_loadSaveBaseName).cStr(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

        if (loadSaveFileHandle==INVALID_HANDLE_VALUE)
        {
            caller->m_lastLoadSaveError = Error_LoadSave_IOError;
            return (DWORD)bfalse;
        }
        else
        {
            DWORD bytesWritten=0, headerBytesWritten=0, rewardsBytesWritten=0;
            Header header;
            wcscpy_s(header.SaveName, MAX_PATH, (const wchar_t *)caller->m_displayedName.cStr());
            header.CodeCRC = caller->m_codeCRC;
            header.DataCRC = caller->m_dataCRC;

            u32 RewardsSize = REWARD_ADAPTER_WIN->getRewardsDataSize();
            u8* DataRewards = (u8*)Memory::malloc(RewardsSize);
            REWARD_ADAPTER_WIN->getRewardsData(DataRewards);

            if(WriteFile(loadSaveFileHandle, &header, sizeof(Header), &headerBytesWritten, NULL)
                 && 
               WriteFile(loadSaveFileHandle, &caller->m_currentSavegame[0], caller->m_currentSavegame.size(), &bytesWritten, NULL)
                 && 
               WriteFile(loadSaveFileHandle, DataRewards, RewardsSize, &rewardsBytesWritten, NULL)
               )
            {
                if ( caller->m_currentSavegame.size()!=bytesWritten )
                {
                    caller->m_lastLoadSaveError = Error_LoadSave_IOError;
                    SF_CLOSEHANDLE(loadSaveFileHandle);
                    if(DataRewards) Memory::free(DataRewards);
                    return (DWORD)bfalse;
                }
                else
                {
                    caller->m_lastLoadSaveError = Error_Ok;
                    CloseHandle(loadSaveFileHandle);
                    if(DataRewards) Memory::free(DataRewards);
                    return (DWORD)btrue;
                }
            }            
            
            caller->m_lastLoadSaveError = Error_LoadSave_IOError;
            CloseHandle(loadSaveFileHandle);
            if(DataRewards) Memory::free(DataRewards);
            return (DWORD)bfalse;            
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_win::handle_WritingSaveFile()
    {
        if (WaitForSingleObject(m_IOOperationThread, 0)==WAIT_OBJECT_0)
        {
            SF_CLOSEHANDLE( m_IOOperationThread );
            setMainState(State_Idle);
            m_isSaving = bfalse;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_win::setMainState(MainState _newState)
    {
        m_mainState = _newState;
        m_frameSinceLastStateChange = 0;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_win::update()
    {        

        m_frameSinceLastStateChange++;

        //If there's a posted Choose Device request, try to display the device selector
        switch(m_mainState)
        {
        case ChooseDevice_PostedShowSelector:
            handle_MustChooseDevice();
            break;
        case ChooseDevice_ShowingSelector:
            if (m_frameSinceLastStateChange==10)
                handle_JustClosedDeviceSelectorGUI();
            break;
        case EnumeratingContent:
            handle_AsynchronousContentEnumeration();
            break;

        case Save_CreatingContent:
            handle_Save_CreateContent();
            break;
        case Save_WritingFile:
            handle_WritingSaveFile();
            break;
        case Load_WaitingBeforeIdle:
            handle_Load_WaitingBeforeIdle();
            break;

        }

        //
    }

    bbool Adapter_Savegame_win::checkDataCRCForCurrentSave( u32 _dataCRC )
    {
        i32 contentIndex = findBasenameAmongEnumerated(m_loadSaveBaseName);
        if(contentIndex>=0)
        {
            return (_dataCRC == m_filteredDeviceContent[contentIndex].sHeader.DataCRC);
        }
        return bfalse;
    }

}
