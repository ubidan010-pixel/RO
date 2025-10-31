#include "precompiled_Adapter_Savegame_WII.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_ADAPTER_SAVEGAME_WII_H__
#include "adapters/Adapter_Savegame_WII/Adapter_Savegame_WII.h"
#endif

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_FILE_WII_H_
#include "core/file/wii/File_WII.h"
#endif //_ITF_FILE_WII_H_

#ifndef _ITF_SYSTEMADAPTER_TPL_WII_H_
#include "adapters/SystemAdapter_WII/SystemAdapter_TPL_WII.h"
#endif //_ITF_SYSTEMADAPTER_TPL_WII_H_

#ifndef _ITF_TRCADAPTER_WII_H_
#include "adapters/TRCAdapter_WII/TRCAdapter_WII.h"
#endif //_ITF_TRCADAPTER_WII_H_

namespace ITF
{
#ifndef ITF_FINAL
//	#define WII_DEBUG_SIMUL_CORRUPFILE
//	#define WII_DEBUG_SIMUL_FILE_NOINODE
//	#define WII_DEBUG_SIMUL_FILE_NOSPACE	
//	#define WII_DEBUG_SIMUL_BANNER_NOINODE
//	#define WII_DEBUG_SIMUL_BANNER_NOSPACE
//  #define WII_DEBUG_SIMUL_FIRST_BOOT
#endif


#define SAVEFILE_SIZE	            20*1024	// Slot Size
#define SAVEFILE_BANNERSIZE         29344 // banner.bin size

    Adapter_Savegame_WII::Adapter_Savegame_WII() :
        m_mainState(State_Idle),
        m_contentEnumeration_PlayerIndex(0),
        m_frameSinceLastStateChange(0),
        m_IOOperationThread(0),
        m_wasEnumerated(bfalse),
        m_choseDevice_PlayerIndex(0),
		m_workingbuff(NULL),
		m_lastError(0),
        m_internalDisable(bfalse)
        {
        }

    void Adapter_Savegame_WII::init()
    {
		setDeviceChosenFlags(btrue, btrue);
        m_currentSavegame.push_back(0);

#ifdef WII_DEBUG_SIMUL_FIRST_BOOT
        if ( 1 )
#else
		if( !check_banner() )
#endif
		{						
            init_createAllFiles();
		} 
        else
        {
            // Check for corruption
            if (getWantedFilesSize() != getCurrentFilesSize() ||
                getWantedBannerSize() != getCurrentBannerSize() )
            {
                disableSaveSystem();
                m_internalDisable = btrue;
                TRC_ADAPTER->addMessage(TRCManagerAdapter::Sav_ReadFiles);
            }
        }

        if(IsSaveSystemEnable())
            read_headers();
    }

    bbool Adapter_Savegame_WII::init_createAllFiles()
    {        
        // Get TPL datas	
        TPLPalettePtr  pPalette;
        if(!SystemAdapter_TPL::readAndAlloc(&pPalette, String(WII_SAVEGAME_BANNERPATH)))
        {
            // Memory or DVD error
            disableSaveSystem();
            return bfalse;// FATAL ERROR
        }
        SystemAdapter_TPL::unpack(pPalette);	

        wait_forDVDDiscError();
        TRC_ADAPTER->disableRESETandPOWER();

        // 
        if ( init_checkSize(pPalette) && init_createFiles() && init_createBanner(pPalette) )
        {
            LOG ("SaveSystem initialized");
            enableSaveSystem();
        } else
        {
            disableSaveSystem();
            m_internalDisable = btrue;
        }

        Memory::alignedFree(pPalette);
        TRC_ADAPTER->enableRESETandPOWER();

        return IsSaveSystemEnable();
    }

    bbool Adapter_Savegame_WII::read_headers()
    {
        m_filteredDeviceContent.clear();
        for (u32 _player = 0; _player < MAX_SLOTS; _player++)
        {
            Header h ATTRIBUTE_ALIGN(32); 
            if ( readHeader( _player, &h ) )
                m_filteredDeviceContent.push_back( h );
        }

        if (m_filteredDeviceContent.size() == MAX_SLOTS)
        {
            return btrue;
        }

        // Headers are wrong
        disableSaveSystem();
        m_internalDisable = btrue;
        TRC_ADAPTER->addMessage(TRCManagerAdapter::Sav_ReadFiles);

        return bfalse;
    }

	void Adapter_Savegame_WII::recreate_files()
	{
		if (init_createAllFiles())
        {
            enableSaveSystem();
            m_internalDisable = bfalse;
        }
	}

    void Adapter_Savegame_WII::tests()
    {
    }

    void Adapter_Savegame_WII::term()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_WII::handle_MustChooseDevice()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Adapter_Savegame_WII::getEnumeratedContent(u32 /*_playerIndex*/, u32 _index, String8 &_baseName, String &_displayName, bbool &_emptySave)
    {
        if(m_filteredDeviceContent.size()<=_index) 
        {
            static const String strEmpty ("empty");
            _displayName = strEmpty;
            _emptySave = btrue;
            return btrue;
        }

        _displayName = m_filteredDeviceContent[_index].displayedName;
        _emptySave = m_filteredDeviceContent[_index].emptySave;
        return btrue;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Adapter_Savegame_WII::startContentEnumeration(u32 _player)
    {
        return btrue;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_WII::handle_JustClosedDeviceSelectorGUI()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    Adapter_Savegame::ErrorCode Adapter_Savegame_WII::startChooseDevice(u32 _playerIndex, u32 _minFreeSize, bbool _isSilentWhenChosenNone)
    {
        return Error_Ok;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_WII::handle_AsynchronousContentEnumeration()
    {
		if ( m_frameSinceLastStateChange > 10 && m_opState >= Op_Ended )
		{
			setMainState(State_Idle);
			NANDClose(&m_fileInfo);
			bbool isFailed = bfalse;
			
	#ifdef WII_DEBUG_SIMUL_CORRUPFILE
			TRC_ADAPTER->addMessage(TRCManagerAdapter::Sav_ReadFiles);
			isFailed = true;
	#endif	//WII_DEBUG_SIMUL_CORRUPFILE
			
			if(m_lastError > 0 && m_lastError != buffSize())	// Bad read
			{
				TRC_ADAPTER->addMessage(TRCManagerAdapter::Sav_ReadFiles);
				isFailed = btrue;
			} 
			else if(m_lastError < 0)
			{
				TRC_ADAPTER->checkIOError(TRCManagerAdapter::Sav_ReadFiles, m_lastError);
				isFailed = btrue;
			}
					
			if(!isFailed)
			{
				buff2slot(m_filteredDeviceContent[m_contentEnumeration_PlayerIndex].fileSize);
			}
		}
    }
	
	
	// Some helpers to handle 32bits aligned buffer during write/read operation
	void Adapter_Savegame_WII::preparebuff()
	{
		if(m_workingbuff)
			Memory::alignedFree(m_workingbuff);
		m_workingbuff = (u8*)Memory::alignedMallocCategory(totalSize(), 32, MemoryId::mId_SavegameManager);
        ITF_MemSet(m_workingbuff, 0, totalSize());
	}
	
	u32 Adapter_Savegame_WII::buffSize()
	{
		return OSRoundUp32B(SAVEFILE_SIZE) - getHeaderSize();
	}

    u32 Adapter_Savegame_WII::totalSize()
    {
        return buffSize() + getHeaderSize();
    }
	
	void Adapter_Savegame_WII::buff2slot(u32 dataSize)
	{
		if(m_workingbuff)
		{            
            m_currentSavegame.resize(dataSize);
			ITF_Memcpy(&m_currentSavegame[0], m_workingbuff, dataSize);		
		}
	}
	void Adapter_Savegame_WII::data2buff(const void* data, u32 dataSize )
	{
		if(m_workingbuff)
		{
            if ( dataSize > buffSize() )
            {
                ITF_ASSERT_MSG(0, "SAVEGAME SIZE too big, please grow up buffSize()");
            }

            ITF_Memcpy(m_workingbuff, &m_workingheader, sizeof(Header));
            if(dataSize>0 && data!=NULL)
			    ITF_Memcpy(m_workingbuff+getHeaderSize(), data, dataSize);
		}
	}
	
	
    Adapter_Savegame::ErrorCode Adapter_Savegame_WII::startLoad(u32 _playerIndex, u32 _slotIndex, const String8 &baseName)
    {
    	if(!canUseSaveSystem()) return Error_Internal;
    		
		m_contentEnumeration_PlayerIndex = _slotIndex;
        if(m_codeCRC && m_filteredDeviceContent[_slotIndex].CodeCRC != m_codeCRC)
        {
            return Error_LoadSave_IOError;
        }

		s32 res = NANDOpen(getPlayerSlotPath(_slotIndex).cStr(), &m_fileInfo, NAND_ACCESS_READ);
		if ( res != NAND_RESULT_OK )
		{
			return Error_SavegameNotFound;
		}
		
		if(1)	// async
		{
			NANDSetUserData(&m_commandBlock, this);
			preparebuff();
					
			m_opState = Op_Running;
            NANDSeek(&m_fileInfo, getHeaderSize(), NAND_SEEK_SET);
			res = NANDReadAsync(&m_fileInfo, m_workingbuff, buffSize(), loadThreadRoutine, &m_commandBlock);
			if (res != NAND_RESULT_OK)
			{
				// Error?
				setMainState(State_Idle);
				return Error_SavegameNotFound;		
			} 

			setMainState(EnumeratingContent);
		} else
		{
			preparebuff();
            NANDSeek(&m_fileInfo, getHeaderSize(), NAND_SEEK_SET);
			res = NANDRead(&m_fileInfo, m_workingbuff, buffSize());
			if (res == buffSize())
				buff2slot(m_filteredDeviceContent[_slotIndex].fileSize);
			NANDClose(&m_fileInfo);
			setMainState(State_Idle);		
		}
		return Error_Ok;
	}
	
	Adapter_Savegame::ErrorCode Adapter_Savegame_WII::startSavegameEnumerationUpdate(u32 _player)
	{
		if(!canUseSaveSystem()) return Error_LoadOrSaveForbiddenForPlayer;
		return Error_Ok;
	}

    ///////////////////////////////////////////////////////////////////////////////////////////
    Adapter_Savegame::ErrorCode Adapter_Savegame_WII::startSave(u32 _playerIndex, u32 _slotIndex, const String8 &baseName, const String &displayName, const void *_data, u32 _dataSize, bbool _warnIfOverwrite)
    {
    	if(!canUseSaveSystem()) return Error_Internal;
    	
		m_contentEnumeration_PlayerIndex = _slotIndex;
        
		s32 res = NANDOpen(getPlayerSlotPath(_slotIndex).cStr(), &m_fileInfo, NAND_ACCESS_WRITE);	
		if ( res != NAND_RESULT_OK )
		{
			setMainState(State_Idle);
			return Error_SavegameNotFound;
		}
		
        ITF_MemSet(&m_workingheader, 0, sizeof(Header));

        u32 nameSize = displayName.getLen()*sizeof(u16);
        m_workingheader.emptySave   = (_dataSize==0);
        m_workingheader.fileSize    = _dataSize;
        m_workingheader.CodeCRC     = m_codeCRC;
        m_workingheader.DataCRC     = m_dataCRC;
        ITF_Memcpy(m_workingheader.displayedName, displayName.cStr(), nameSize);
        m_currentSavegame.resize(_dataSize);
        

		preparebuff();
		data2buff(_data, _dataSize);
        m_chooseDevice_MinFreeSize = _dataSize;
			
        // -- 
        /*res = NANDWrite(&m_fileInfo, m_workingbuff, totalSize());
        setMainState(State_Idle);
        NANDClose(&m_fileInfo);
        return Error_Ok;*/

        //--
		setMainState(Save_CreatingContent);
		
		//TRC_ADAPTER->addMessage(TRCManagerAdapter::Sav_OperationRunning);
        return Error_Ok;
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_WII::handle_Save_CreateContent()
    {
		if (m_frameSinceLastStateChange>10)
		{	
            wait_forDVDDiscError();
            TRC_ADAPTER->disableRESETandPOWER();

			NANDSetUserData(&m_commandBlock, this);	
			m_opState = Op_Running;
            
			s32 res = NANDWriteAsync(&m_fileInfo, m_workingbuff, totalSize(), saveThreadRoutine, &m_commandBlock);
			if (res != NAND_RESULT_OK)
			{
				// Error?
				setMainState(State_Idle);
				m_opState = Op_Idle;
                TRC_ADAPTER->enableRESETandPOWER();
				return ;
			} 
		
            
			setMainState(Save_WritingFile);	
		}
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_WII::handle_Load_WaitingBeforeIdle()
    {
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    String8 Adapter_Savegame_WII::getSavegamePath()
    {
		static String8 _savegamePath ("");
		if(_savegamePath.isEmpty())
		{
			_savegamePath  = hwFile::getNANDHomeDir();
			_savegamePath +=  "/index.dat";
		}
        return _savegamePath;
    }
	///////////////////////////////////////////////////////////////////////////////////////////
	String8 Adapter_Savegame_WII::getPlayerSlotPath(u32 _slotIndex) const
	{
		String8 _slotPath;
		_slotPath.setTextFormat(WII_SAVEGAME_SLOTPATH, hwFile::getNANDHomeDir().cStr(), _slotIndex);
		return _slotPath;
	}	
	///////////////////////////////////////////////////////////////////////////////////////////
	String8 Adapter_Savegame_WII::getPlayerTmpSlotPath(u32 _slotIndex) const
	{
		String8 _slotPath;
		_slotPath.setTextFormat(WII_SAVEGAME_SLOTPATH, "/tmp", _slotIndex);
		return _slotPath;
    }	
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_WII::saveThreadRoutine(s32 _result, NANDCommandBlock* _commandBlock)
    {
		Adapter_Savegame_WII* pCaller = (Adapter_Savegame_WII*)NANDGetUserData(_commandBlock);
        if(pCaller)
        {
		    pCaller->m_lastError 	= (i32)_result;
		    pCaller->m_opState 		= Adapter_Savegame_WII::Op_Ended;
        }
    }

	///////////////////////////////////////////////////////////////////////////////////////////
	void Adapter_Savegame_WII::loadThreadRoutine(s32 _result, NANDCommandBlock* _commandBlock)
	{
		Adapter_Savegame_WII* pCaller = (Adapter_Savegame_WII*)NANDGetUserData(_commandBlock);
		if(pCaller)
        {
		    pCaller->m_lastError 	= (i32)_result;
		    pCaller->m_opState 		= Adapter_Savegame_WII::Op_Ended;
        }
	}	

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_WII::handle_WritingSaveFile()
    {
		if ( m_frameSinceLastStateChange > 10 && m_opState == Op_Ended )
		{
			setMainState(State_Idle);
			NANDClose(&m_fileInfo);
            TRC_ADAPTER->enableRESETandPOWER();
		}
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_WII::setMainState(MainState _newState)
    {
		m_mainState = _newState;
		m_frameSinceLastStateChange = 0;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_WII::update()
    {        
    	if(!canUseSaveSystem()) return;
    	
		m_frameSinceLastStateChange++;

		//If there's a posted Choose Device request, try to display the device selector
		switch(m_mainState)
		{
		case ChooseDevice_ShowingSelector:
		case ChooseDevice_PostedShowSelector:
			setMainState(State_Idle);
			break;
		// LOAD **************
		case EnumeratingContent:
			handle_AsynchronousContentEnumeration();
			break;

		// SAVE **************
		case Save_CreatingContent:
			handle_Save_CreateContent();
			break;
		case Save_WritingFile:
			handle_WritingSaveFile();
			break;

        // IDLE **************
        case State_Idle:
        case Load_WaitingBeforeIdle:
            break;

		default:
			ITF_ASSERT_MSG(0, "State not supported");
			break;
		}
    }
    
	bbool Adapter_Savegame_WII::init_createBanner( TPLPalettePtr _pal )
	{
		i32 rv;
		static NANDBanner Banner ATTRIBUTE_ALIGN(32);
		TPLDescriptorPtr pDescriptor;
		
		//First, init the structure ..
		NANDInitBanner(&Banner,
            WII_SAVEGAME_BANNERFLAG, 
            (unsigned short*)WII_SAVEGAME_TITLE, 
            (unsigned short*)WII_SAVEGAME_COMMENT);	
		
		// ... and fill the TPL descriptor
		pDescriptor = TPLGet(_pal, 0);
		ITF_Memcpy(Banner.bannerTexture, pDescriptor->textureHeader->data, NAND_BANNER_TEXTURE_SIZE);
		
		// IconsBanner
		u16 IconsCount = _pal->numDescriptors - 1; // First descriptor is the Banner
		for (u16 i = 1; i < 9 && i <= IconsCount; ++i)
		{
			pDescriptor = TPLGet(_pal, i);
			ITF_Memcpy(&Banner.iconTexture[i-1], pDescriptor->textureHeader->data, NAND_BANNER_ICON_SIZE);
			NANDSetIconSpeed(&Banner, i-1, NAND_BANNER_ICON_ANIM_SPEED_NORMAL);
		}
		// When there are less than eight, enter END
		if(IconsCount<8)
			NANDSetIconSpeed(&Banner, IconsCount, NAND_BANNER_ICON_ANIM_SPEED_END);
		
		
		NANDFileInfo bannerInfo;
		rv = NANDCreate(getTmpBannerPath().cStr(), NAND_PERM_OWNER_MASK|NAND_PERM_GROUP_MASK, 0);
		if (rv != NAND_RESULT_EXISTS && rv != NAND_RESULT_OK)
		{
			ITF_ASSERT_MSG(0, "NANDCreate failed %d\n", rv);
			TRC_ADAPTER->checkIOError(TRCManagerAdapter::Sav_InitCreatingBanner, rv);
			return bfalse;
		}

		u8 copyBuf[NAND_FSBLOCK_SIZE] ATTRIBUTE_ALIGN(32);
		rv = NANDSimpleSafeOpen(getTmpBannerPath().cStr(), &bannerInfo, NAND_ACCESS_WRITE, copyBuf, NAND_FSBLOCK_SIZE);
		if (rv != NAND_RESULT_OK)
		{
			ITF_ASSERT_MSG(0, "NANDSimpleSafeOpen: Could not open 'banner.bin' for writing\n");
			TRC_ADAPTER->checkIOError(TRCManagerAdapter::Sav_InitCreatingBanner, rv);
			return bfalse;
		}
		
		u32 bannerSize = NAND_BANNER_SIZE( IconsCount );
		rv = NANDWrite(&bannerInfo, &Banner, bannerSize);
		if (rv != bannerSize)
		{
			ITF_ASSERT_MSG(0, "NANDWrite: Could not write banner");
			if(rv < 0)
			{
				TRC_ADAPTER->checkIOError(TRCManagerAdapter::Sav_InitCreatingBanner, rv);
			} else
			{
				TRC_ADAPTER->addMessage(TRCManagerAdapter::Sav_InitCreatingBanner);
			}
			return bfalse;
		}
		rv = NANDSimpleSafeClose(&bannerInfo);
		
		// And finally move to home dir
		if(!move2home(getTmpBannerPath()))
		{
			ITF_ASSERT_MSG(0, "NANDMove: Could not move banner");			
			return bfalse;
		}

		return btrue;
	}

#define BYTES2FS(x)     ceil (x / (f32)NAND_FSBLOCK_SIZE)


    u32 Adapter_Savegame_WII::getCurrentBannerSize() const
    {
        NANDFileInfo info;
        unsigned long bannerSize;
        s32 res = NANDOpen(getBannerPath().cStr(), &info, NAND_ACCESS_READ);
        if(res==NAND_RESULT_OK)
        {
            NANDGetLength(&info, &bannerSize);
            NANDClose(&info);
        }
        return (u32)bannerSize;
    }

    u32 Adapter_Savegame_WII::getWantedBannerSize() const
    {
        return SAVEFILE_BANNERSIZE;
    }

    u32 Adapter_Savegame_WII::getCurrentFilesSize() const
    {
        u32 totalFilesSize = 0;
        for (u32 slot = 0; slot < MAX_SLOTS; slot++)
        {
            NANDFileInfo info;
            s32 res = NANDOpen(getPlayerSlotPath(slot).cStr(), &info, NAND_ACCESS_READ) ;
            if( res == NAND_RESULT_OK )
            {
                unsigned long fileSize = 0;
                NANDGetLength(&info, &fileSize);
                totalFilesSize += fileSize;
                NANDClose(&info);
            }
        }
        return totalFilesSize;
    }

    u32 Adapter_Savegame_WII::getWantedFilesSize() const
    {
        return SAVEFILE_SIZE * MAX_SLOTS;
    }

    bbool Adapter_Savegame_WII::init_checkSize(TPLPalettePtr _pal)
    {
        // Get required size
        u32 bannerSize  = NAND_BANNER_SIZE( _pal->numDescriptors - 1 );
        ITF_ASSERT_MSG(SAVEFILE_BANNERSIZE==bannerSize, "Banner size has changed, please update SAVEFILE_BANNERSIZE");        

        // Banner + Files 
        u32 totalFsblock = BYTES2FS(bannerSize) + BYTES2FS(SAVEFILE_SIZE) * MAX_SLOTS;
        u32 totalInodes  = MAX_SLOTS + 1;    // files count + banner

#ifdef WII_DEBUG_SIMUL_FILE_NOINODE
        totalInodes = NAND_MAX_INODES;
#endif
#ifdef WII_DEBUG_SIMUL_FILE_NOSPACE        
        totalFsblock = NAND_MAX_CAPACITY;
#endif
        return TRC_ADAPTER->checkSpaceDisk(TRCManagerAdapter::Sav_CheckSpace, totalFsblock, totalInodes);
    }

	bbool Adapter_Savegame_WII::move2home(const String8& src)
	{
		return (NANDMove(src.cStr(), hwFile::getNANDHomeDir().cStr()) == NAND_RESULT_OK)	;
	}

	String8 Adapter_Savegame_WII::getTmpBannerPath()
	{		
		static String8 str_BannerPath("/tmp/banner.bin");
		return str_BannerPath;
	}
	
	String8 Adapter_Savegame_WII::getBannerPath()
	{		
		static String8 str_BannerPath("");		
		if(str_BannerPath.isEmpty())
		{			
			str_BannerPath  = hwFile::getNANDHomeDir();
			str_BannerPath += "/banner.bin";		
		}

		ITF_ASSERT(!str_BannerPath.isEmpty());
		return str_BannerPath;
	}

	bbool Adapter_Savegame_WII::isSaveFileExists()
	{
		bbool res = btrue;
		for (u32 _player = 0; _player < MAX_SLOTS; _player++)
		{
			NANDFileInfo info;
			s32 OpenRes = NANDOpen(getPlayerSlotPath(_player).cStr(), &info, NAND_ACCESS_READ) ;
			if( OpenRes == NAND_RESULT_OK )
				NANDClose(&info);
				
			res = res && ( OpenRes != NAND_RESULT_NOEXISTS );
		}
		return res;
	}
	
	bbool Adapter_Savegame_WII::check_banner()
	{
		NANDFileInfo info;
		s32 OpenRes = NANDOpen(getBannerPath().cStr(), &info, NAND_ACCESS_READ) ;
		if( OpenRes == NAND_RESULT_OK )
			NANDClose(&info);

		return ( OpenRes != NAND_RESULT_NOEXISTS );
	}

    u32 Adapter_Savegame_WII::getHeaderSize() 
    {
        return OSRoundUp32B ( sizeof( Header ) );
    }

    bbool Adapter_Savegame_WII::readHeader(u32 slot, Header* pHeader)
    {
        NANDFileInfo info;
        i32 res = NANDOpen(getPlayerSlotPath(slot).cStr(), &info, NAND_ACCESS_READ);
        if(res != NAND_RESULT_OK)
        {
            return bfalse; 
        }

        const u32 HEADER_SIZE = getHeaderSize();
        u8* buf = (u8*)Memory::alignedMallocCategory(HEADER_SIZE, 32, MemoryId::mId_SavegameManager);

        s32 sizeReaded = NANDRead(&info, buf, HEADER_SIZE);
        NANDClose(&info);

        if(sizeReaded<HEADER_SIZE)
        {
            Memory::alignedFree(buf);
            return bfalse;
        }
      
        ITF_Memcpy(pHeader, buf, sizeof(Header));
        Memory::alignedFree(buf);

        if(pHeader->CodeCRC != m_codeCRC && 
            pHeader->CodeCRC != 0 && m_codeCRC != 0)
        {
            return bfalse;
        }
        return btrue;
    }

	
	bbool Adapter_Savegame_WII::init_createFiles()
	{	
        for (u32 _player = 0; _player < MAX_SLOTS; _player++)
		{
			i32 res = NANDCreate(getPlayerTmpSlotPath(_player).cStr(), NAND_PERM_OWNER_READ|NAND_PERM_OWNER_WRITE|NAND_PERM_GROUP_READ, 0);
			if(res != NAND_RESULT_OK && res != NAND_RESULT_EXISTS)
				return bfalse;
				
			NANDFileInfo info;
			
			res = NANDOpen(getPlayerTmpSlotPath(_player).cStr(), &info, NAND_ACCESS_WRITE);
			if(res != NAND_RESULT_OK)
			{
				TRC_ADAPTER->checkIOError(TRCManagerAdapter::Sav_InitCreatingFiles, res);	
			}
			else
			{
				u8 buf[SAVEFILE_SIZE] ATTRIBUTE_ALIGN(32);
	    		ITF_MemSet(buf, 0, SAVEFILE_SIZE);

                static const String strEmpty ("empty");
                Header h ; 
                u32 len = strEmpty.getLen() * sizeof(u16);
                ITF_Memcpy(h.displayedName, strEmpty.cStr(), len);                
                h.emptySave = btrue;
                h.fileSize = 0;
                h.CodeCRC = 0;
                h.DataCRC = 0;
                ITF_Memcpy(buf, &h, sizeof(Header));
	    		
	    		res = NANDWrite(&info, buf, SAVEFILE_SIZE);
	    		if(res != SAVEFILE_SIZE)
	    		{
	    			// Handle error with TRC check
	    			TRC_ADAPTER->checkIOError(TRCManagerAdapter::Sav_InitCreatingFiles, res);
	    		}	    		
    		}
    		
			NANDClose(&info);
		}
		
		// Move to home dir
		for (u32 _player = 0; _player < MAX_SLOTS; _player++)
		{
			if(!move2home(getPlayerTmpSlotPath(_player).cStr()))
			{
				return bfalse;
			}
		}	

		return btrue;
	}

    void Adapter_Savegame_WII::preLoad( u32 _player, u32 _maxPlayersToUse /*= 1*/, bbool _force /*= bfalse*/ )
    {
        // Load headers to fill the slot
        if(IsSaveSystemEnable() && m_internalDisable)
        {
            disableSaveSystem();
        } else if (IsSaveSystemEnable())
            read_headers();
    }

    void Adapter_Savegame_WII::wait_forDVDDiscError()
    {
        // Wait for the DVDTDisc error
        while ( TRC_ADAPTER->isAFatalErrorRunning() )
        {
            sleep ( 10 );
        }
    }

}

