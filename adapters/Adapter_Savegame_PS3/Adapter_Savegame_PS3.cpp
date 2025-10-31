#include "precompiled_Adapter_Savegame_PS3.h"

#ifndef _ITF_ADAPTER_SAVEGAME_PS3_H__
#include "adapters/Adapter_Savegame_PS3/Adapter_Savegame_PS3.h"
#endif //_ITF_ADAPTER_SAVEGAME_PS3_H__

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_REWARDADAPTER_H_
#include "engine/AdaptersInterfaces/RewardAdapter.h"
#endif //_ITF_REWARDADAPTER_H_

#ifndef _ITF_THREADMANAGER_H_
#include "core/AdaptersInterfaces/ThreadManager.h"
#endif //_ITF_THREADMANAGER_H_

#ifndef _ITF_FILE_H_
#include "core/file/File.h"
#endif //_ITF_FILE_H_

#ifndef _ITF_TRCADAPTER_PS3_H_
#include "adapters/TRCAdapter_PS3/TRCAdapter_PS3.h"
#endif //_ITF_TRCADAPTER_PS3_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_SYSTEMADAPTER_PS3_H_
#include "adapters/SystemAdapter_PS3/SystemAdapter_PS3.h"
#endif //_ITF_SYSTEMADAPTER_PS3_H_

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_

namespace ITF
{
    static const u8	Savegame_SecureId[CELL_SAVEDATA_SECUREFILEID_SIZE] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10 };

#define SAVEGAME_NAME                   "SYS-DATA"
#define SAVEGAME_DIR                    "PS3_DATA"
#define MAX_SLOTS                       3
#define LOCALISATION_ID_SAVE_NAME       4127
#define LOCALISATION_ID_GAME_NAME_HDD   4586

    Adapter_Savegame_PS3::Adapter_Savegame_PS3() :
        m_mainState(State_Idle),
        m_contentEnumeration_PlayerIndex(0),
        m_frameSinceLastStateChange(0),
        m_IOOperationThread(0),
        m_wasEnumerated(bfalse),
        m_choseDevice_PlayerIndex(0),
        m_thread(NULL),
        m_currentLoadOperation(LOAD_OPERATION_IDLE),
        m_currentSaveOperation(SAVE_OPERATION_IDLE),
        fileBuffer(NULL),
        m_FilesToSave(3),
        m_FilesToLoad(1),
        m_threadRunning(bfalse),
        m_firstLoading(btrue),
        m_currentSlot(0),
        m_readerUpdated(bfalse),
        m_isNotOwnerOfTheSave(bfalse),
        m_isCreatingBootFiles(bfalse),
        m_fileIsCorrupted(bfalse),
        m_hasToExit(bfalse)
        {
        }

    void Adapter_Savegame_PS3::init()
    {
		setDeviceChosenFlags(btrue, btrue);
 
        ITF_MemSet(SizeFilesSystem, 0, sizeof(SizeFilesSystem));
        initSystemPath();
    }

    void Adapter_Savegame_PS3::tests()
    {
    }

    void Adapter_Savegame_PS3::term()
    {
        if (m_threadRunning)
        {
            m_hasToExit = btrue;

            while ( m_threadRunning )
            {
                sleep( 1 );
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_PS3::handle_MustChooseDevice()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Adapter_Savegame_PS3::getEnumeratedContent(u32 /*_playerIndex*/, u32 _index, String8 &_baseName, String &_displayName, bbool &_emptySave)
    {
        ITF_ASSERT(!hasPendingOperation());
        if (_index>=m_filteredDeviceContent.size())
        {
            ITF_ASSERT_MSG(bfalse, "Index is out of range");
            return bfalse;
        }
        
        _baseName.setTextFormat("Savegame_%d", m_filteredDeviceContent[_index].slot);
        _displayName = m_filteredDeviceContent[_index].displayedName;
        _emptySave = m_filteredDeviceContent[_index].emptySave;
        return btrue;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_PS3::setCurrentOperationCompleteCallback(OperationCompleteCallback_t _callback)
    {
        LOG("check setCurrentOperationCompleteCallback\n");
        m_currentOperationCompleteCallback = _callback;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Adapter_Savegame_PS3::startContentEnumeration(u32 _player)
    {
        return btrue;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_PS3::handle_JustClosedDeviceSelectorGUI()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    Adapter_Savegame::ErrorCode Adapter_Savegame_PS3::startChooseDevice(u32 _playerIndex, u32 _minFreeSize, bbool _isSilentWhenChosenNone)
    {
        return Error_Ok;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_PS3::handle_AsynchronousContentEnumeration()
    {
    }

    Adapter_Savegame::ErrorCode Adapter_Savegame_PS3::startLoad(u32 _playerIndex, u32 _slotIndex, const String8 &_baseName)
    {
        if(!m_thread)
        {
            if(m_codeCRC && m_filteredDeviceContent[_slotIndex].codeCRC != m_codeCRC)
            {
                return Error_LoadSave_IOError;
            }
            
            //m_loadSaveBaseName.setTextFormat("%s/%s/%s", SYS_APP_HOME, "PS3_DATA", _baseName.cStr());
            m_loadSaveBaseName = SAVEGAME_NAME;
            m_currentSlot = _slotIndex;
            m_thread = THREADMANAGER->createThread(loadThreadRoutine, this, ThreadSettings::m_settings[eThreadId_SaveManager]);
            m_threadRunning = btrue;
            setMainState(Load_WaitingBeforeIdle);

            TRC_ADAPTER->disableRESETandPOWER();
        }        
        return Error_Ok;
	}
	
	Adapter_Savegame::ErrorCode Adapter_Savegame_PS3::startSavegameEnumerationUpdate(u32 _player)
	{
/*        if(!m_thread && !m_readerUpdated)
        {
            m_currentSavegame.resize(m_chooseDevice_MinFreeSize*MAX_SLOTS);
            m_currentSlot = 0;

            m_thread = THREADMANAGER->createThread(loadHeaderThreadRoutine, this, ThreadSettings::m_settings[eThreadId_SaveManager]);
            m_threadRunning = btrue;
            setMainState(HeaderLoad_WaitingBeforeIdle);
            TRC_ADAPTER->disableRESETandPOWER();
        }   */     
        return Error_Ok;
	}

    ///////////////////////////////////////////////////////////////////////////////////////////
    Adapter_Savegame::ErrorCode Adapter_Savegame_PS3::startSave(u32 _playerIndex, u32 _slotIndex, const String8 &baseName, const String &displayName, const void *_data, u32 _dataSize, bbool _warnIfOverwrite)
    {
        if(!m_thread && !m_isNotOwnerOfTheSave)
        {        
            ITF_ASSERT(getHeaderSize()+_dataSize<m_chooseDevice_MinFreeSize);
            m_currentSavegame.resize(_dataSize+getHeaderSize());
            
            m_filteredDeviceContent[_slotIndex].emptySave = (_dataSize == 0);

            ITF_Memcpy(m_filteredDeviceContent[_slotIndex].displayedName, displayName.cStr(), displayName.getLen()*sizeof(u16));
            m_filteredDeviceContent[_slotIndex].displayedName[displayName.getLen()] = 0;
            m_filteredDeviceContent[_slotIndex].fileSize = _dataSize;
            m_filteredDeviceContent[_slotIndex].codeCRC = m_codeCRC;
            m_filteredDeviceContent[_slotIndex].dataCRC = m_dataCRC;
            m_filteredDeviceContent[_slotIndex].emptySave = (_dataSize==0);

            ITF_Memcpy(&m_currentSavegame[0], &m_filteredDeviceContent[_slotIndex], getHeaderSize());
            if(_data)
            {
                ITF_Memcpy(&m_currentSavegame[0]+getHeaderSize(), _data, _dataSize);            
                m_isSaving = btrue;
            }            
            
            m_loadSaveBaseName = SAVEGAME_NAME;
            m_currentSlot = _slotIndex;

            m_displayedName = displayName;
            m_thread = THREADMANAGER->createThread(saveThreadRoutine, this, ThreadSettings::m_settings[eThreadId_SaveManager]);
            m_threadRunning = btrue;
            setMainState(Save_WritingFile);

            TRC_ADAPTER->disableRESETandPOWER();
            m_readerUpdated = bfalse;
            
        }        
        return Error_Ok;
    }

    
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_PS3::handle_Save_CreateContent()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_PS3::handle_HeaderLoad_WaitingBeforeIdle()
    {
        if(!m_threadRunning)
        {
            m_thread = NULL;
            readHeader();
            TRC_ADAPTER->enableRESETandPOWER();
            setMainState(State_Idle);
        }  
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_PS3::handle_Load_WaitingBeforeIdle()
    {
        if(!m_threadRunning)
        {
            m_thread = NULL;

            TRC_ADAPTER->enableRESETandPOWER();
            setMainState(State_Idle);
        }    
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    String8 Adapter_Savegame_PS3::getSavegamePath(const String8 &_filename)
    {
        static String8 savePath (SYSTEM_ADAPTER_PS3->getTitleID());// Code of Lead SKU for DIRNAME
        if(_filename.isEmpty())
            return savePath;
        
        return savePath + "//" + _filename;
    }

    void Adapter_Savegame_PS3::loadHeaderStatCallback(CellSaveDataCBResult *_cbResult, CellSaveDataStatGet *_get, CellSaveDataStatSet *_set)
    {
        bool isNew = _get->isNewData ;
        if(isNew)
        {
            // Data not found
            _cbResult->result = CELL_SAVEDATA_CBRESULT_ERR_NODATA;
            return;
        }

        _set->reCreateMode = CELL_SAVEDATA_RECREATE_NO_NOBROKEN;	// Do not handle save data as corrupt data.
        _set->setParam = NULL;		 								// PARAM.SFO won't be updated
        _set->indicator = NULL;


        // Trophies check 
        u32 bind = _get->bind;

        if( bind & CELL_SAVEDATA_BINDSTAT_ERR_OWNER != 0 )
        {                        
            Adapter_Savegame_PS3* pSaveGame = (Adapter_Savegame_PS3*) _cbResult->userdata;
            //pSaveGame->DoNotUseSaveSystem(btrue);
            pSaveGame->m_isNotOwnerOfTheSave = btrue;
        }

        _cbResult->result = CELL_SAVEDATA_CBRESULT_OK_NEXT;

        Adapter_Savegame_PS3* pSaveGame = (Adapter_Savegame_PS3*) _cbResult->userdata;
        pSaveGame->m_currentLoadOperation = LOAD_OPERATION_STEP_GAMEFILE;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_PS3::loadDataStatCallback(CellSaveDataCBResult *_cbResult, CellSaveDataStatGet *_get, CellSaveDataStatSet *_set)
    {
        bool isNew = _get->isNewData ; 

        if(isNew)    
        {
            // Data not found
            _cbResult->result = CELL_SAVEDATA_CBRESULT_ERR_NODATA;
            return;
        } 

        //if ( _get->fileListNum < _get->fileNum ) {
        //    // If number of file overflow, the save data should be judged as broken data 
        //    // Because you may not find file needed in obtained list 
        //    _cbResult->result = CELL_SAVEDATA_CBRESULT_ERR_BROKEN;
        //    return;
        //}

        _set->reCreateMode = CELL_SAVEDATA_RECREATE_NO_NOBROKEN;	// Do not handle save data as corrupt data.
        _set->setParam = NULL;		 								// PARAM.SFO won't be updated
        _set->indicator = NULL;
        
        _cbResult->result = CELL_SAVEDATA_CBRESULT_OK_NEXT;
       
        Adapter_Savegame_PS3* pSaveGame = (Adapter_Savegame_PS3*) _cbResult->userdata;
        pSaveGame->m_currentLoadOperation = LOAD_OPERATION_STEP_GAMEFILE;

        // Check for integrity
        for (u32 i = 0; i < _get->fileListNum; i++)
        {
            if(strcmp(SAVEGAME_NAME, _get->fileList[i].fileName) == 0)
            {
                pSaveGame->SizeFilesSystem[FILE_GAMEFILE] = _get->fileList[i].st_size;
                break;
            }            
        }
    }

    #define SIZEKB(x)	((x+1023)/1024)

    ///////////////////////////////////////////////////////////////////////////////////////////
    u32 Adapter_Savegame_PS3::getTotalNeededSizeKB()
    {
        u32 sizeKB = SIZEKB(m_chooseDevice_MinFreeSize * MAX_SLOTS); // GAMEFILE
        hwFile f ; 
        for (u32 i = 1; i < FILE_COUNT; i++)
        {
            if (f.open(PathFilesSystem[i], ITF_FILE_ATTR_READ))
            {
                sizeKB += SIZEKB(f.getLength());
                f.close();  
            }
        }    

        return sizeKB;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_PS3::readHeader()
    {
        m_filteredDeviceContent.clear();
        // Read header        
        for(u32 i = 0; i < MAX_SLOTS; i ++)
        {
            Header h ; 
            ITF_Memcpy(&h, &m_currentSavegame[0] + (i*m_chooseDevice_MinFreeSize), getHeaderSize());
            h.slot = i;

            m_filteredDeviceContent.push_back(h);
        }
        m_readerUpdated = btrue;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_PS3::saveDataStatCallback(CellSaveDataCBResult *_cbResult, CellSaveDataStatGet *_get, CellSaveDataStatSet *_set)
    {
        bool isNew = _get->isNewData ; 
        Adapter_Savegame_PS3* pSaveGame = (Adapter_Savegame_PS3*) _cbResult->userdata;
        _set->reCreateMode  = CELL_SAVEDATA_RECREATE_NO_NOBROKEN;
        _set->indicator     = NULL;
        
        //
        //u32 bind = _get->bind;
        
        if(isNew)    
        {
            ITF_MemSet(&pSaveGame->m_dataSystemFileParam, 0, sizeof(pSaveGame->m_dataSystemFileParam));
            char* saveGameStr = (char*)pSaveGame->m_saveName.convertStringClasstoUtf8Str();            
            String8 Detail ("Rayman");// we need something here .. 

            strncpy(pSaveGame->m_dataSystemFileParam.title,    SYSTEM_ADAPTER_PS3->getGameTitle(), CELL_SAVEDATA_SYSP_TITLE_SIZE);
            strncpy(pSaveGame->m_dataSystemFileParam.subTitle, saveGameStr, CELL_SAVEDATA_SYSP_SUBTITLE_SIZE);
            strncpy(pSaveGame->m_dataSystemFileParam.detail,   Detail.cStr(), CELL_SAVEDATA_SYSP_DETAIL_SIZE);
                            
            SF_DEL_ARRAY(saveGameStr);
            pSaveGame->m_dataSystemFileParam.attribute      = pSaveGame->m_saveCanBeDuplicated ? CELL_SAVEDATA_ATTR_NORMAL : CELL_SAVEDATA_ATTR_NODUPLICATE;

            ITF_MemSet(pSaveGame->m_dataSystemFileParam.reserved, 0, 256);
            ITF_MemSet(pSaveGame->m_dataSystemFileParam.reserved2, 0, 4);
            ITF_MemSet(pSaveGame->m_dataSystemFileParam.listParam, 0, CELL_SAVEDATA_SYSP_LPARAM_SIZE);
            
            _set->setParam = &pSaveGame->m_dataSystemFileParam;
            pSaveGame->m_currentSaveOperation = SAVE_OPERATION_STEP_ICON0;
            
            // Check for size 
            u32 filesKB  = pSaveGame->getTotalNeededSizeKB();
            u32 NeededKB = filesKB + _get->sysSizeKB;
            LOG("[SPACEDISK] Save system needs %d KB (%d (files) + %d (system))", NeededKB, filesKB, _get->sysSizeKB);

            if(!TRC_ADAPTER->checkSpaceDisk(TRCManagerAdapter::Sav_CheckSpaceBoot, NeededKB))
            {
                pSaveGame->disableSaveSystem();
                pSaveGame->m_currentSaveOperation = SAVE_OPERATION_STEP_FAILED;
                _cbResult->result           = CELL_SAVEDATA_CBRESULT_OK_LAST;
                return;
            }


        } else
        {
            _set->setParam      = &_get->getParam;
            
            ITF_MemSet(_set->setParam->reserved, 0, 256);
            ITF_MemSet(_set->setParam->reserved2, 0, 4);            
            _set->setParam->attribute      = pSaveGame->m_saveCanBeDuplicated ? CELL_SAVEDATA_ATTR_NORMAL : CELL_SAVEDATA_ATTR_NODUPLICATE;
            
            // Check for size 
            u32 NeededKB = pSaveGame->getTotalNeededSizeKB();
            i32 NewSizeNeededKB = NeededKB - _get->sizeKB;
            NewSizeNeededKB = Max (0, NewSizeNeededKB);
            

            if( pSaveGame->m_firstLoading )
            {
                pSaveGame->m_firstLoading = bfalse;

                // We only want the data from param.sfo ...
                if(_get->fileNum == pSaveGame->m_FilesToSave)
                {
                    /*Content content ; 
                    strncpy(content.szDisplayName, _set->setParam->detail, CELL_SAVEDATA_SYSP_DETAIL_SIZE);
                    pSaveGame->m_filteredDeviceContent.push_back(content);*/
                }                

                // ... and check the size
                if(!TRC_ADAPTER->checkSpaceDisk(TRCManagerAdapter::Sav_CheckSpaceBoot, NewSizeNeededKB))
                {
                    pSaveGame->disableSaveSystem();
                    pSaveGame->m_currentSaveOperation = SAVE_OPERATION_STEP_FAILED;
                    _cbResult->result           = CELL_SAVEDATA_CBRESULT_OK_LAST;
                    return;
                }

                pSaveGame->m_currentSaveOperation = SAVE_OPERATION_STEP_END;
            } else
            {

                pSaveGame->m_currentSaveOperation = SAVE_OPERATION_STEP_GAMEFILE;
            }            
        }

        _cbResult->result = CELL_SAVEDATA_CBRESULT_OK_NEXT;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_PS3::saveDataFileCallback(CellSaveDataCBResult *_cbResult, CellSaveDataFileGet *_get, CellSaveDataFileSet *_set)
    {
        Adapter_Savegame_PS3* pSaveGame = (Adapter_Savegame_PS3*) _cbResult->userdata;
        
        // if the buffer is already allocated, we have to free it
        if (pSaveGame->fileBuffer)
        {
            Memory::free (pSaveGame->fileBuffer);
            pSaveGame->fileBuffer = NULL;
        }

        // Default result, could be overwritten later
        _cbResult->result = CELL_SAVEDATA_CBRESULT_OK_NEXT;

   
        switch(pSaveGame->m_currentSaveOperation)
        { 
        case SAVE_OPERATION_STEP_ICON0:          
            _set->fileOperation = CELL_SAVEDATA_FILEOP_WRITE;
            _set->fileBuf = pSaveGame->DataFilesSystem[FILE_ICON0].pData;
            _set->fileBufSize = pSaveGame->DataFilesSystem[FILE_ICON0].dataSize;
            _set->fileSize = pSaveGame->DataFilesSystem[FILE_ICON0].dataSize;
            _set->fileType = CELL_SAVEDATA_FILETYPE_CONTENT_ICON0;
            _set->reserved = NULL;

            //pSaveGame->m_currentSaveOperation = SAVE_OPERATION_STEP_PIC1;         
            
            if( pSaveGame->m_firstLoading )
                pSaveGame->m_firstLoading = bfalse;
            pSaveGame->m_currentSaveOperation = SAVE_OPERATION_STEP_GAMEFILE;
        break;
        case SAVE_OPERATION_STEP_ICON1:break;
        case SAVE_OPERATION_STEP_PIC1:break;
            //_set->fileOperation = CELL_SAVEDATA_FILEOP_WRITE;
            //_set->fileBuf = pSaveGame->DataFilesSystem[FILE_PIC1].pData;
            //_set->fileBufSize = pSaveGame->DataFilesSystem[FILE_PIC1].dataSize;
            //_set->fileSize = pSaveGame->DataFilesSystem[FILE_PIC1].dataSize;
            //_set->fileType = CELL_SAVEDATA_FILETYPE_CONTENT_PIC1;
            //_set->reserved = NULL;       

            //if( pSaveGame->m_firstLoading )                
            //    pSaveGame->m_firstLoading = bfalse;
            //pSaveGame->m_currentSaveOperation = SAVE_OPERATION_STEP_GAMEFILE;
        break;
        case SAVE_OPERATION_STEP_GAMEFILE:
            _set->reserved      = NULL;            
            _set->fileOperation = CELL_SAVEDATA_FILEOP_WRITE_NOTRUNC;
            _set->fileOffset    = pSaveGame->m_chooseDevice_MinFreeSize*pSaveGame->m_currentSlot;
            _set->fileSize      = pSaveGame->m_currentSavegame.size();
            _set->fileBufSize   = pSaveGame->m_currentSavegame.size();
            _set->fileBuf       = &pSaveGame->m_currentSavegame[0];
            _set->fileType      = CELL_SAVEDATA_FILETYPE_SECUREFILE;
            ITF_Memcpy(_set->secureFileId, Savegame_SecureId, sizeof(Savegame_SecureId));

            _set->fileName      = (char*)pSaveGame->m_loadSaveBaseName.cStr();

            pSaveGame->m_currentSaveOperation = SAVE_OPERATION_STEP_END;
            break;
        case SAVE_OPERATION_STEP_FAILED:
        case SAVE_OPERATION_STEP_END:
        default:

            _cbResult->result = CELL_SAVEDATA_CBRESULT_OK_LAST;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_PS3::loadHeaderFileCallback(CellSaveDataCBResult *_cbResult, CellSaveDataFileGet *_get, CellSaveDataFileSet *_set)
    {
        Adapter_Savegame_PS3* pSaveGame = (Adapter_Savegame_PS3*) _cbResult->userdata;

        _cbResult->result = CELL_SAVEDATA_CBRESULT_OK_NEXT;


        switch(pSaveGame->m_currentLoadOperation)
        {        
        case LOAD_OPERATION_STEP_GAMEFILE:   
 
            _set->fileOperation = CELL_SAVEDATA_FILEOP_READ;            
            _set->fileType      = CELL_SAVEDATA_FILETYPE_SECUREFILE;
            ITF_Memcpy(_set->secureFileId, Savegame_SecureId, sizeof(Savegame_SecureId));

            _set->reserved      = NULL;      
            _set->fileOffset    = 0;
            _set->fileSize      = pSaveGame->m_currentSavegame.size();
            _set->fileName      = (char*)pSaveGame->m_loadSaveBaseName.cStr();

            _set->fileBufSize   = pSaveGame->m_currentSavegame.size();
            _set->fileBuf       = &pSaveGame->m_currentSavegame[0];

            pSaveGame->m_currentLoadOperation = LOAD_OPERATION_STEP_END;
            break;
        case LOAD_OPERATION_STEP_END:
        case LOAD_OPERATION_STEP_FAILED:
            _cbResult->result = CELL_SAVEDATA_CBRESULT_OK_LAST;
            break;
        case LOAD_OPERATION_IDLE: 
        default:
            break;
        }

    }

    void Adapter_Savegame_PS3::loadDataFileCallback(CellSaveDataCBResult *_cbResult, CellSaveDataFileGet *_get, CellSaveDataFileSet *_set)
    {
        Adapter_Savegame_PS3* pSaveGame = (Adapter_Savegame_PS3*) _cbResult->userdata;

        _cbResult->result = CELL_SAVEDATA_CBRESULT_OK_NEXT;

        switch(pSaveGame->m_currentLoadOperation)
        {        
        case LOAD_OPERATION_STEP_GAMEFILE:
            pSaveGame->m_currentSavegame.resize( pSaveGame->SizeFilesSystem[FILE_GAMEFILE] - getHeaderSize() );
            ITF_MemSet(&pSaveGame->m_currentSavegame[0], 0, pSaveGame->SizeFilesSystem[FILE_GAMEFILE]);

            _set->fileOperation = CELL_SAVEDATA_FILEOP_READ;            
            _set->fileType      = CELL_SAVEDATA_FILETYPE_SECUREFILE;
            ITF_Memcpy(_set->secureFileId, Savegame_SecureId, sizeof(Savegame_SecureId));

            _set->reserved      = NULL;      
            _set->fileOffset    = (pSaveGame->m_chooseDevice_MinFreeSize*pSaveGame->m_currentSlot)+getHeaderSize();
            _set->fileSize      = pSaveGame->m_currentSavegame.size();
            _set->fileName      = (char*)pSaveGame->m_loadSaveBaseName.cStr();

            _set->fileBufSize   = pSaveGame->m_currentSavegame.size();
            _set->fileBuf       = &pSaveGame->m_currentSavegame[0];

            pSaveGame->m_currentLoadOperation = LOAD_OPERATION_STEP_END;
            break;
        case LOAD_OPERATION_STEP_END:
        case LOAD_OPERATION_STEP_FAILED:
            _cbResult->result = CELL_SAVEDATA_CBRESULT_OK_LAST;
            break;
        case LOAD_OPERATION_IDLE: 
        default:
            break;
        }
    }    
    ///////////////////////////////////////////////////////////////////////////////////////////
    DWORD Adapter_Savegame_PS3::saveThreadRoutine(void *_param)
    {
        Adapter_Savegame_PS3* pSaveGame = (Adapter_Savegame_PS3*) _param;

        CellSaveDataSetBuf  SetBuff;
        u32                 NbFiles = pSaveGame->m_FilesToSave;
        u32                 res = 1;
                
        SetBuff.dirListMax  = 0;
        SetBuff.fileListMax = NbFiles;
        SetBuff.bufSize     = NbFiles * sizeof(CellSaveDataFileStat);
        SetBuff.buf         = Memory::malloc(SetBuff.bufSize);
        
        ITF_MemSet(SetBuff.reserved, 0, sizeof(SetBuff.reserved));
        String8 path = Adapter_Savegame_PS3::getSavegamePath("");

        int ret = cellSaveDataAutoSave2(
            CELL_SAVEDATA_VERSION_CURRENT,
            path.cStr(), 
            //CELL_SAVEDATA_ERRDIALOG_ALWAYS,
            CELL_SAVEDATA_ERRDIALOG_NONE,
            &SetBuff,
            saveDataStatCallback,
            saveDataFileCallback,
            SYS_MEMORY_CONTAINER_ID_INVALID,
            _param);

#ifndef ITF_FINAL
        DumpSaveError(ret);
#endif //ITF_FINAL

        if(ret != CELL_SAVEDATA_RET_OK)
        {
            pSaveGame->m_lastLoadSaveError = sdkErrorToSystemError(ret);            
            res = 0;
        } else
            pSaveGame->m_lastLoadSaveError = Error_Ok;

        Memory::free(SetBuff.buf);

        if ( !pSaveGame->m_isCreatingBootFiles )
        {
            pSaveGame->m_threadRunning = bfalse;
            ITF_MemoryBarrier();
        }       

		return res;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    DWORD Adapter_Savegame_PS3::loadThreadRoutine(void *_param)
    {
        Adapter_Savegame_PS3* pSaveGame = (Adapter_Savegame_PS3*) _param;
        CellSaveDataSetBuf  SetBuff;
        u32                 NbFiles = pSaveGame->m_FilesToSave;
        u32                 res = 1;

        SetBuff.dirListMax  = 0;
        SetBuff.fileListMax = NbFiles;
        SetBuff.bufSize     = NbFiles * sizeof(CellSaveDataFileStat);
        SetBuff.buf         = Memory::malloc(SetBuff.bufSize);

        ITF_MemSet(SetBuff.reserved, 0, sizeof(SetBuff.reserved));
        String8 path = Adapter_Savegame_PS3::getSavegamePath("");

        int ret = cellSaveDataAutoLoad2(
            CELL_SAVEDATA_VERSION_CURRENT,
            path.cStr(), 
            CELL_SAVEDATA_ERRDIALOG_NONE,
            &SetBuff,
            loadDataStatCallback,
            loadDataFileCallback,
            SYS_MEMORY_CONTAINER_ID_INVALID,
            _param);

#ifndef ITF_FINAL
        DumpSaveError(ret);
#endif //ITF_FINAL

        if(ret != CELL_SAVEDATA_RET_OK)
        {
            pSaveGame->m_lastLoadSaveError = sdkErrorToSystemError(ret);
            res = 0;
        } else
            pSaveGame->m_lastLoadSaveError = Error_Ok;

        Memory::free(SetBuff.buf);

        if ( !pSaveGame->m_isCreatingBootFiles )
        {
            pSaveGame->m_threadRunning = bfalse;
            ITF_MemoryBarrier();
        }
        return res;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    DWORD Adapter_Savegame_PS3::retryThreadRoutine(void *_param)
    {
        Adapter_Savegame_PS3* pSaveGame = (Adapter_Savegame_PS3*) _param;
        pSaveGame->m_firstLoading = btrue;
        if ( pSaveGame->initForBoot() )
        {
            pSaveGame->m_lastLoadSaveError = Error_Ok;
            LOG ("pSaveGame->initForBoot() >> OK");
        }
        else
        {
            pSaveGame->m_lastLoadSaveError = Error_LoadSave_IOError;
            LOG ("pSaveGame->initForBoot() >> FAIL");
        }

        pSaveGame->m_threadRunning = bfalse;
        ITF_MemoryBarrier();
        return 0;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    DWORD Adapter_Savegame_PS3::deleteSaveThreadRoutine(void *_param)
    {
        Adapter_Savegame_PS3* pSaveGame = (Adapter_Savegame_PS3*) _param;
        CellSaveDataSetBuf  SetBuff;
        u32                 NbFiles = 0;
        u32                 res = 1;
        String8 path = Adapter_Savegame_PS3::getSavegamePath("");

        SetBuff.dirListMax  = 0;
        SetBuff.fileListMax = NbFiles;
        SetBuff.bufSize     = NbFiles * sizeof(CellSaveDataFileStat);
        SetBuff.buf         = NULL;
        ITF_MemSet(SetBuff.reserved, 0, sizeof(SetBuff.reserved));

        CellSaveDataSetList SetList;
        SetList.sortType = CELL_SAVEDATA_SORTTYPE_SUBTITLE;
        SetList.sortOrder = CELL_SAVEDATA_SORTORDER_DESCENT;
        SetList.dirNamePrefix = (char*)path.cStr();
        SetList.reserved = NULL;

        int ret = cellSaveDataFixedDelete(
            &SetList,
            &SetBuff,
            deleteSaveFixedCallback,
            deleteSaveDoneCallback,
            SYS_MEMORY_CONTAINER_ID_INVALID,
            _param);


#ifndef ITF_FINAL
        DumpSaveError(ret);
#endif //ITF_FINAL

        if(ret != CELL_SAVEDATA_RET_OK)
        {
            pSaveGame->m_lastLoadSaveError = sdkErrorToSystemError(ret);
            res = 0;
        } else if( !pSaveGame->m_hasToExit )
        {

            pSaveGame->m_isSaving = btrue;
            
            // then create the save again
            if ( pSaveGame->initForBoot() )
            {
                pSaveGame->m_lastLoadSaveError = Error_Ok;
                LOG ("pSaveGame->initForBoot() >> OK");
            }
            else
            {
                pSaveGame->m_lastLoadSaveError = Error_LoadSave_IOError;
                LOG ("pSaveGame->initForBoot() >> FAIL");
                res = 0;
            }
        }

        ITF_MemoryBarrier();
        pSaveGame->m_threadRunning = bfalse;        
        return res;
    }
    
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    DWORD Adapter_Savegame_PS3::loadHeaderThreadRoutine(void *_param)
    {
        Adapter_Savegame_PS3* pSaveGame = (Adapter_Savegame_PS3*) _param;
        CellSaveDataSetBuf  SetBuff;
        u32                 NbFiles = pSaveGame->m_FilesToSave;
        u32                 res = 1;

        SetBuff.dirListMax  = 0;
        SetBuff.fileListMax = NbFiles;
        SetBuff.bufSize     = NbFiles * sizeof(CellSaveDataFileStat);
        SetBuff.buf         = Memory::malloc(SetBuff.bufSize);

        ITF_MemSet(SetBuff.reserved, 0, sizeof(SetBuff.reserved));
        String8 path = Adapter_Savegame_PS3::getSavegamePath("");

        int ret = cellSaveDataAutoLoad2(
            CELL_SAVEDATA_VERSION_CURRENT,
            path.cStr(), 
            CELL_SAVEDATA_ERRDIALOG_NONE,
            &SetBuff,
            loadHeaderStatCallback,
            loadHeaderFileCallback,
            SYS_MEMORY_CONTAINER_ID_INVALID,
            _param);

#ifndef ITF_FINAL
        DumpSaveError(ret);
#endif //ITF_FINAL

        if(ret != CELL_SAVEDATA_RET_OK)
        {
            pSaveGame->m_lastLoadSaveError = sdkErrorToSystemError(ret);
            res = 0;
        } else
            pSaveGame->m_lastLoadSaveError = Error_Ok;

        Memory::free(SetBuff.buf);
        if ( !pSaveGame->m_isCreatingBootFiles )
        {
            pSaveGame->m_threadRunning = bfalse;
            ITF_MemoryBarrier();
        }
        return res;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_PS3::handle_WritingSaveFile()
    {
        if(!m_threadRunning)
        {
            m_thread = NULL;
            TRC_ADAPTER->enableRESETandPOWER();
            setMainState(State_Idle);
            m_isSaving = bfalse;
        }        
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_PS3::handle_DeletingFile()
    {
        if(!m_threadRunning)
        {
            m_thread = NULL;
            TRC_ADAPTER->enableRESETandPOWER();
            setMainState(State_Idle);
            m_isSaving = bfalse;

            // Check for Space Disk
            TRC_ADAPTER_PS3->checkForBootSpaceDisk();
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_PS3::handle_RetryReadingFile()
    {
        if(!m_threadRunning)
        {
            m_thread = NULL;
            setMainState(State_Idle);
            m_isSaving = bfalse;

            if (isSaveCorrupted())
            {
                TRC_ADAPTER->addMessage(TRCManagerAdapter::Sav_CorruptedFile);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_PS3::setMainState(MainState _newState)
    {
        m_mainState = _newState;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_PS3::update()
    {        
        switch(m_mainState)
        {
        case Save_WritingFile:
            handle_WritingSaveFile();
            break;
        case Load_WaitingBeforeIdle:
            handle_Load_WaitingBeforeIdle();
            break;
        case HeaderLoad_WaitingBeforeIdle:
            handle_HeaderLoad_WaitingBeforeIdle();
            break;
        case DeletingFile:
            handle_DeletingFile();
            break;
        case RetryReadingFile:
            handle_RetryReadingFile();
            break;
        }
    }

#ifndef ITF_FINAL
    void Adapter_Savegame_PS3::DumpSaveError( int ret )
    {
        String8 error ("");
        switch(ret)
        {
            case CELL_SAVEDATA_RET_OK:              error = "OK"; break;
            case CELL_SAVEDATA_RET_CANCEL:          error = "CELL_SAVEDATA_RET_CANCEL"; break;
            case CELL_SAVEDATA_ERROR_CBRESULT:      error = "CELL_SAVEDATA_ERROR_CBRESULT"; break;
            case CELL_SAVEDATA_ERROR_ACCESS_ERROR:  error = "CELL_SAVEDATA_ERROR_ACCESS_ERROR"; break;
            case CELL_SAVEDATA_ERROR_INTERNAL:      error = "CELL_SAVEDATA_ERROR_INTERNAL"; break;
            case CELL_SAVEDATA_ERROR_PARAM:         error = "CELL_SAVEDATA_ERROR_PARAM"; break;
            case CELL_SAVEDATA_ERROR_NOSPACE:       error = "CELL_SAVEDATA_ERROR_NOSPACE"; break;
            case CELL_SAVEDATA_ERROR_BROKEN:        error = "CELL_SAVEDATA_ERROR_BROKEN"; break;
            case CELL_SAVEDATA_ERROR_FAILURE:       error = "CELL_SAVEDATA_ERROR_FAILURE"; break;
            case CELL_SAVEDATA_ERROR_BUSY:          error = "CELL_SAVEDATA_ERROR_BUSY"; break;
            default:                                error = "CELL_SAVEDATA_ERROR -> UNKNOWN"; break;
        }

        LOG ("SaveResult: %s (0x%x)\n", error.cStr(), ret);
    }
#endif //!ITF_FINAL

    bbool Adapter_Savegame_PS3::initForBoot()
    {  
        m_loadSaveBaseName = SAVEGAME_NAME;
        m_displayedName = "empty";
        m_fileIsCorrupted = bfalse;
        m_isCreatingBootFiles = btrue;
        m_isNotOwnerOfTheSave = bfalse;

        LocalisationId id;
        if ( SYSTEM_ADAPTER_PS3->isHDDMode() )
        {
            id = LOCALISATION_ID_GAME_NAME_HDD;
            m_saveName = LOCALISATIONMANAGER->getText(id);
        } 
        else
        {
            id = LOCALISATION_ID_SAVE_NAME; // Save Game [slotNumber]
            m_saveName = LOCALISATIONMANAGER->getText(id);
            m_saveName.replace("[slotNumber]", "");    
        }

        // Fake empty save
        m_currentSavegame.resize(m_chooseDevice_MinFreeSize*MAX_SLOTS);
        ITF_MemSet(&m_currentSavegame[0], 1, m_chooseDevice_MinFreeSize*MAX_SLOTS);

        Header h;        
        ITF_Memcpy(h.displayedName, m_displayedName.cStr(), m_displayedName.getLen()*sizeof(u16));
        h.displayedName[m_displayedName.getLen()] = 0;
        h.emptySave = btrue;
        h.fileSize = 0;
        
        for(u32 i = 0; i < MAX_SLOTS; i++)
        {
            ITF_Memcpy(&m_currentSavegame[0] + m_chooseDevice_MinFreeSize * i, &h, getHeaderSize());
        }

        // Copy PIC files
        bbool readIsSuccess = btrue; 
        for (u32 i = FILE_ICON0; i < FILE_COUNT; i++)
        {
            hwFile f ; 
            if (readIsSuccess && f.open(PathFilesSystem[i], ITF_FILE_ATTR_READ))
            {
                u32 FSize = f.getLength();
                DataFilesSystem[i].pData = (u8*)Memory::malloc(FSize);            
                DataFilesSystem[i].dataSize = FSize;
                if (!f.read(DataFilesSystem[i].pData, FSize))
                {
                    readIsSuccess = bfalse;
                }
                f.close();
            }
            else
            {
                readIsSuccess = bfalse;
            }
        }

        LOG (" Adapter_Savegame_PS3::initForBoot -> Write process starting ");

        // Read/Write save/headers
        if( readIsSuccess && 
            saveThreadRoutine(this) &&
            loadHeaderThreadRoutine(this) )
        {
            readHeader();
            m_disableSystemDefinitively = bfalse;
            UseSaveSystem();
        } else
        {
            if( m_lastLoadSaveError == Error_LoadSave_IOError )
            {
                m_fileIsCorrupted = btrue;
            }

            DoNotUseSaveSystem();
        }

        // And free PIC
        for (u32 i = FILE_ICON0; i < FILE_COUNT; i++)
        {
            if(DataFilesSystem[i].pData)
            {
                Memory::free( DataFilesSystem[i].pData );
                DataFilesSystem[i].pData = NULL;
            }
        }

        m_isCreatingBootFiles = bfalse;
        return IsSaveSystemEnable();
    }

    void Adapter_Savegame_PS3::initSystemPath()
    {
        PathFilesSystem[FILE_GAMEFILE] = SAVEGAME_NAME;
        String pathName (""); 

        pathName.setTextFormat("%s/%s", SAVEGAME_DIR, "ICON0.PNG");
        FILEMANAGER->TranslatePath(PathFilesSystem[FILE_ICON0], pathName);
         
        /*pathName.setTextFormat("%s/%s", SAVEGAME_DIR, "PIC1.PNG");
        FILEMANAGER->TranslatePath(PathFilesSystem[FILE_PIC1], pathName);*/

        DataFilesSystem[FILE_ICON0].pData = NULL;
        //DataFilesSystem[FILE_PIC1].pData = NULL;
    }

    Adapter_Savegame_PS3::ErrorCode Adapter_Savegame_PS3::sdkErrorToSystemError( int err )
    {
        switch(err)
        {
            case CELL_SAVEDATA_ERROR_BROKEN:
            case CELL_SAVEDATA_ERROR_FAILURE:
                return Error_LoadSave_IOError;
                break;

            case CELL_SAVEDATA_RET_OK:
            case CELL_SAVEDATA_RET_CANCEL:
            default:
                return Error_Ok;
        }
    }

    void Adapter_Savegame_PS3::preLoad( u32 _player, u32 _maxPlayersToUse /*= 1*/, bbool _force /*= bfalse*/ )
    {
        if (isSaveCorrupted())
        {
            TRC_ADAPTER->addMessage(TRCManagerAdapter::Sav_CorruptedFile);
        }
        else if(m_isNotOwnerOfTheSave)
        {
            // Display a warning only for rewards usage and save
            TRC_ADAPTER->addMessage(TRCManagerAdapter::Sav_NotOwnerOfTheSave);
            REWARD_ADAPTER->setEnable(bfalse);
        }
    }

    bbool Adapter_Savegame_PS3::IsSaveProcessEnable() const
    {
        return ( IsSaveSystemEnable() && !m_isNotOwnerOfTheSave );
    }

    u32 Adapter_Savegame_PS3::getMaxSlot() const
    {
        return MAX_SLOTS;
    }

    void Adapter_Savegame_PS3::deleteSaveFixedCallback( CellSaveDataCBResult *_cbResult, CellSaveDataListGet *_get, CellSaveDataFixedSet *_set )
    {
        _set->dirName = (char*)Adapter_Savegame_PS3::getSavegamePath("").cStr();
        _set->newIcon = NULL;
        _set->option = CELL_SAVEDATA_OPTION_NOCONFIRM;

        
    }

    void Adapter_Savegame_PS3::deleteSaveDoneCallback( CellSaveDataCBResult *_cbResult, CellSaveDataDoneGet *_get )
    {
        _cbResult->result = CELL_SAVEDATA_CBRESULT_OK_LAST_NOCONFIRM;

        // update space disk        
        u32 NeededKB = _get->sizeKB;
        TRC_ADAPTER_PS3->updateFakeFreeSpace(NeededKB);
    }

    void Adapter_Savegame_PS3::deleteSaveFile()
    {
        if(!m_thread)
        {
            m_filteredDeviceContent.clear();

            m_loadSaveBaseName = SAVEGAME_NAME;            
            m_thread = THREADMANAGER->createThread(deleteSaveThreadRoutine, this, ThreadSettings::m_settings[eThreadId_SaveManager]);
            m_threadRunning = btrue;
            setMainState(DeletingFile);

            TRC_ADAPTER->disableRESETandPOWER();
            m_readerUpdated = bfalse;
        }
    }

    void Adapter_Savegame_PS3::retryReadingSaveFile()
    {
        if(!m_thread)
        {
            m_loadSaveBaseName = SAVEGAME_NAME;            
            m_thread = THREADMANAGER->createThread(retryThreadRoutine, this, ThreadSettings::m_settings[eThreadId_SaveManager]);
            m_threadRunning = btrue;
            setMainState(RetryReadingFile);

            m_isSaving = btrue;
        }
    }

    bbool Adapter_Savegame_PS3::isSaveCorrupted()
    {
        if ( m_fileIsCorrupted )
            return btrue;

        // check for corruption 
        for ( u32 i = 0; i < m_filteredDeviceContent.size(); i ++)
        {   
            if ( m_filteredDeviceContent[i].codeCRC != 0 && m_filteredDeviceContent[i].codeCRC != m_codeCRC)
            {
                return btrue;
            }
        }
        return bfalse;
    }
}
