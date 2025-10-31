#include "precompiled_SystemAdapter_WII.h"

#ifndef _ITF_WII_DRAWPRIM_H_
#include "adapters/GFXAdapter_WII/GFXAdapter_WII.h"
#endif //_ITF_WII_DRAWPRIM_H_

#ifndef _ITF_SYSTEMADAPTER_HOMEBUTTON_WII_H_
#include "adapters/SystemAdapter_WII/SystemAdapter_HomeButton_WII.h"
#endif //_ITF_SYSTEMADAPTER_HOMEBUTTON_WII_H_

#ifndef _ITF_SYNCHRONIZE_WII_H_
#include "engine/system/Synchronize.h"
#endif //_ITF_SYNCHRONIZE_WII_H_

#ifndef _ITF_FILE_WII_H_
#include "core/file/wii/File_WII.h"
#endif //_ITF_FILE_WII_H_

#ifndef __MEM_MACROS_WII_H__
#include "core/memory/WII/MEM_macros_WII.h"
#endif //__MEM_MACROS_WII_H__

#ifndef _ITF_WARNINGMANAGER_H_
#include "engine/common/WarningManager.h"
#endif // _ITF_WARNINGMANAGER_H_

#ifndef _ITF_INPUTADAPTER_WII_H_
#include "adapters/InputAdapter_WII/InputAdapter_WII.h"
#endif //_ITF_INPUTADAPTER_WII_H_

#ifndef _ITF_SYSTEMADAPTER_WII_H_
#include "adapters/SystemAdapter_WII/SystemAdapter_WII.h"
#endif //_ITF_SYSTEMADAPTER_WII_H_

#ifndef _ITF_SYSTEMADAPTER_TPL_WII_H_
#include "adapters/SystemAdapter_WII/SystemAdapter_TPL_WII.h"
#endif //_ITF_SYSTEMADAPTER_TPL_WII_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_TRCADAPTER_WII_H_
#include "adapters/TRCAdapter_WII/TRCAdapter_WII.h"
#endif //_ITF_TRCADAPTER_WII_H_

#ifndef _ITF_SOUNDMANAGER_H_
#include "engine/sound/SoundManager.h"
#endif //_ITF_SOUNDMANAGER_H_

namespace ITF
{
	SystemAdapter_HomeButton::SystemAdapter_HomeButton()
		: _inHome(bfalse), _isHomeProhibited(0), _isHomeInitialized(bfalse), _IconAlpha(0.0f),
		m_soundArchive(NULL), m_soundBuffer(NULL), _isRenderedOnce(bfalse), _leaveMode(LeaveWithSaveWarning)  
	{
		ITF_MemSet(m_homeButtonPressed,0,sizeof(m_homeButtonPressed));
		Synchronize::createCriticalSection(&_CriticalSection);
	}

	SystemAdapter_HomeButton::~SystemAdapter_HomeButton ()
	{
		DestroyHomeMenu();	
	}

	bbool SystemAdapter_HomeButton::bIsProhibited () const
	{
		return (_isHomeProhibited > 0);
	}

	bbool SystemAdapter_HomeButton::bIsInHome () const
	{
		return _inHome;
	}	
	
	void SystemAdapter_HomeButton::SetProhibited (bbool _isProhibited)
	{	
		Synchronize::enterCriticalSection(&_CriticalSection);
		if (_isProhibited)
			_isHomeProhibited++;
		else if (_isHomeProhibited>0)
			_isHomeProhibited--;
		Synchronize::leaveCriticalSection(&_CriticalSection);
	}

	void SystemAdapter_HomeButton::SetAllowed()
	{
		SetProhibited( bfalse );
	}

	/* Change the adjust value depending on the display mode */
	void SystemAdapter_HomeButton::SetAdjustValue()
	{
	    if( SCGetAspectRatio() != SC_ASPECT_RATIO_16x9 )
	    {
	        /* 4:3 */
	        _hbmInfo.adjust.x       = 1.0f;
	        _hbmInfo.adjust.y       = 1.0f;
	    }
	    else
	    {
	        /* 16:9 */
	        _hbmInfo.adjust.x       = 812.f / 608.f;
	        _hbmInfo.adjust.y       = 1.0f;
	    }
	    
	    switch (VIGetTvFormat())
		{
			case VI_EURGB60:
			case VI_NTSC:	// 60H
				_hbmInfo.frameDelta     = 1.0f;
				break;
			case VI_MPAL:
			case VI_PAL: 	// 50H
				_hbmInfo.frameDelta     = 1.2f;
				break;
			default: ITF_ASSERT_CRASH(0, "Unknown TV format");	
		}
	}

	bbool SystemAdapter_HomeButton::InitHomeButtonInfo()
	{
		NandFile file;
		u32 size;
		ITF_VERIFY(file.fileToBuffer("homebtn.arc",_hbmInfo.layoutBuf,size,ITF_FILE_FLAGS_ATTR_TMP,MEM_C_AFTERFX_ALLOC));
		ITF_VERIFY(file.fileToBuffer("speaker.arc",_hbmInfo.spkSeBuf,size,ITF_FILE_FLAGS_ATTR_TMP,MEM_C_AFTERFX_ALLOC));
		ITF_VERIFY(file.fileToBuffer("config.txt",_hbmInfo.configBuf,_hbmInfo.configBufSize,ITF_FILE_FLAGS_ATTR_TMP,MEM_C_AFTERFX_ALLOC));
		ITF_VERIFY(file.fileToBuffer("home.csv",_hbmInfo.msgBuf,size,ITF_FILE_FLAGS_ATTR_TMP,MEM_C_AFTERFX_ALLOC));
		//ITF_VERIFY(file.fileToBuffer("network.csv",_hbmInfo.configBuf,size,ITF_FILE_FLAGS_ATTR_TMP,MEM_C_AFTERFX_ALLOC));

	    _hbmInfo.sound_callback = NULL;
	    
	    //If set to 0, the background is displayed as translucent; if set to 1, the background is filled.
	    _hbmInfo.backFlag       = 0;//1;  
	    _hbmInfo.cursor         = 0;
	    
		MEM_M_PushExt(MEM_C_AFTERFX_ALLOC);
		_hbmInfo.mem = ITF::Memory::alignedMalloc(HBM_MEM_SIZE, 32);
		MEM_M_PopExt();
		_hbmInfo.memSize = HBM_MEM_SIZE;
		_hbmInfo.pAllocator = NULL;

		_hbmInfo.region=SCGetLanguage();
		_hbmInfo.messageFlag = 0;

		SetAdjustValue();

		return btrue;
	}

	bbool DVDToNAND(const char *DVDdir, const char *DVDname, const char *NANDname)
	{
		String8 nameSrc(DVDdir);
		nameSrc += DVDname;

		void *buffer;
		u32 size;

		// Read from DVD
		hwFile DVDfile; 
		if (!DVDfile.fileToBuffer(nameSrc.cStr(),buffer,size,0,0))
		{
			LOG("Failed to read %s\n",nameSrc.cStr());
			return bfalse;
		}

		// Write to NAND
		NandFile NANDFile;
		bbool success = NANDFile.bufferToFile(NANDname,buffer,size,ITF_FILE_FLAGS_ATTR_TMP);
        if (!success)
			LOG("Failed to write %s\n",NANDname);
        ITF::Memory::alignedFree(buffer);
	    return success;
	}

	// copy every file to /tmp
	bbool SystemAdapter_HomeButton::copyHBMtoTMP()
	{
		const char directory[] = WII_HOMEBUTTON_HBMDIR "/";
		String8 filename;

		const u32 lang = SCGetLanguage();
		switch (lang)
		{
			case SC_LANG_JAPANESE: filename = "homeBtn.arc"; break;
			case SC_LANG_ENGLISH: filename = "homeBtn_ENG.arc" ; break;
			case SC_LANG_GERMAN: filename = "homeBtn_GER.arc" ; break;
			case SC_LANG_FRENCH: filename = "homeBtn_FRA.arc" ; break;
			case SC_LANG_SPANISH: filename = "homeBtn_SPA.arc" ; break;
			case SC_LANG_ITALIAN: filename = "homeBtn_ITA.arc" ; break;
			case SC_LANG_DUTCH: filename = "homeBtn_NED.arc" ; break;
			case SC_LANG_KOREAN: filename = "homeBtn_KOR.arc" ; break;
			default: filename = "homeBtn.arc" ; break;
		}
		if (!DVDToNAND(directory,filename.cStr(),"homebtn.arc"))
			return bfalse;

		if (!DVDToNAND(directory,"SpeakerSe.arc","speaker.arc"))
			return bfalse;

		if (!DVDToNAND(directory,"config.txt","config.txt"))
			return bfalse;

		if(_leaveMode == LeaveWithSaveWarning)
			filename = "home.csv";
		else 
			filename = "home_nosave.csv";
		if (!DVDToNAND(directory,filename.cStr(),"home.csv"))
			return bfalse;

		// Uncomment if you have network.
		// if (!DVDToNAND(directory,"network.csv","network.csv"))
		//	return bfalse;

		if (!DVDToNAND(directory,"HomeButtonSe.arc","hbmse.arc"))
			return bfalse;

		return btrue;
	}

	void SystemAdapter_HomeButton::InitSound()
	{
		ITF_ASSERT(!m_soundArchive && !m_soundBuffer);

		MEM_M_PushExt(MEM_C_AFTERFX_ALLOC);
		m_soundBuffer = ITF::Memory::alignedMalloc(HBM_MEM2_SIZE_MEMORY, 32);
		MEM_M_PopExt();
	
		u32 size;
		NandFile file;
		ITF_VERIFY(file.fileToBuffer("hbmse.arc",m_soundArchive,size,ITF_FILE_FLAGS_ATTR_TMP,MEM_C_AFTERFX_ALLOC));
	}

	void SystemAdapter_HomeButton::InitControllerData()
	{
	    for( u32 i = 0; i < WPAD_MAX_CONTROLLERS; i++ )
	    {
	        _conData.wiiCon[i].kpad = NULL;
	        _conData.wiiCon[i].pos.x = 0.f;
	        _conData.wiiCon[i].pos.y = 0.f;
	        _conData.wiiCon[i].use_devtype = WPAD_DEV_NOT_FOUND;
	    }
	}

	void SystemAdapter_HomeButton::InitProjection( u32 flag )
	{
	    Mtx44 projMtx;
	    if( flag )
	    {
	        MTXOrtho(projMtx, 228.0f, -228.0f, -406.0f, 406.0f, 0.0f, 500.0f);
	    }
	    else
	    {
	        MTXOrtho(projMtx, 228.0f, -228.0f, -304.0f, 304.0f, 0.0f, 500.0f);
	    }
	    GXSetProjection(projMtx, GX_ORTHOGRAPHIC);
	}

	void SystemAdapter_HomeButton::InitHomeMenu ()
	{	
		ITF_ASSERT_MSG(!_isHomeInitialized,"Home Button Menu has already been initialized!");
		
		_Root = WII_HOMEBUTTON_HBMDIR;
		
		Synchronize::enterCriticalSection(&_CriticalSection);
	        
		// Icon init
		String IconPath = _Root + "/homeBtnIcon.tpl";	    
        if(!SystemAdapter_TPL::readAndAlloc(&_icon.tpl, IconPath))
        {
            ITF_ASSERT(0);
            return ;// FATAL ERROR
        }
		SystemAdapter_TPL::unpack(_icon.tpl);
		
		TPLGetGXTexObjFromPalette(_icon.tpl, &_icon.texObj, 0);
		
        ForceProhibitedExit();
		
		// Copy HBM files from DVD to NAND
		ITF_VERIFY_MSG(copyHBMtoTMP(),"Could not copy Home Button Menu to tmp NAND");

		_isHomeInitialized = btrue;
	    Synchronize::leaveCriticalSection(&_CriticalSection);
	}

	void SystemAdapter_HomeButton::CreateHomeMenu()
	{
		// Tell the display that we want to use the AfterFX buffer, and wait until it is available.
		u32 size = GFX_ADAPTER_WII->getAFXBufferSize();
		void *buffer = GFX_ADAPTER_WII->lockAFXBuffer(size);
#ifdef ENABLE_ALLOCATOR
		MEMory()->CreateTempHeap(MEM_TempHeapAfterFX,buffer,size);
#else //ENABLE_ALLOCATOR
        ITF::MEMory()->CreateTempHeap(MEM_TempHeapAfterFX,buffer,size);
#endif // ENABLE_ALLOCATOR

		InitHomeButtonInfo();
		HBMCreate( &_hbmInfo );

		InitSound();
		HBMCreateSound(m_soundArchive, m_soundBuffer, HBM_MEM2_SIZE_MEMORY);
	}

	void SystemAdapter_HomeButton::DeleteHomeMenu()
	{
		HBMDeleteSound();
		HBMDelete();

		ITF::Memory::alignedFree(_hbmInfo.layoutBuf);
		ITF::Memory::alignedFree(_hbmInfo.spkSeBuf);
		ITF::Memory::alignedFree(_hbmInfo.configBuf);
		ITF::Memory::alignedFree(_hbmInfo.msgBuf);
		ITF::Memory::alignedFree(_hbmInfo.mem);	
		ITF::Memory::alignedFree(m_soundArchive);
		ITF::Memory::alignedFree(m_soundBuffer);

		_hbmInfo.layoutBuf = NULL;
		_hbmInfo.spkSeBuf= NULL;
		_hbmInfo.configBuf= NULL;
		_hbmInfo.msgBuf= NULL;	
		_hbmInfo.mem = NULL;
		_hbmInfo.memSize = 0;
		m_soundArchive = NULL;
		m_soundBuffer = NULL;		

		// Inform the display manager that the AfterFX buffer is available
#ifdef ENABLE_ALLOCATOR
        ITF_VERIFY(MEMory()->bDestroyTempHeap(MEM_TempHeapAfterFX));
#else //ENABLE_ALLOCATOR
        ITF_VERIFY(ITF::MEMory()->bDestroyTempHeap(MEM_TempHeapAfterFX));
#endif // ENABLE_ALLOCATOR
        GFX_ADAPTER_WII->unlockAFXBuffer();
	}

	void SystemAdapter_HomeButton::Open()
	{
	    ITF_ASSERT_MSG(_isHomeInitialized,"Trying to open HBM while it's not initialized");		
		if(!_isHomeInitialized || _isHomeProhibited) return;		

        GFX_ADAPTER_WII->resetGX();

		_inHome = btrue;
			
		INPUT_ADAPTER_WII->homeMenuStart();	
		
		VISetBlack(GX_TRUE);
		_isRenderedOnce = bfalse;

		CreateHomeMenu();

		HBMInit();

		InitControllerData();
		UpdatePad();
				
		VIWaitForRetrace();
	}

    // --------------------------------------------------------------------------------
    // From SDK Sample ---------------------------------
    static const f32 scStickMoveCoe =  2048.0f/72.0f; /* Analog stick movement coefficient*/

    static f32 AbsClamp( f32 val, f32 max )
    {
        return ( ( val > max ) ? max : ( val < -max ) ? -max : val );
    }

    static int calcAnalogCursorPos( f32 stickX, f32 stickY, Vec2* pos )
    {
        f32 x,y;
        x = ( stickX / scStickMoveCoe );
        y = ( stickY / scStickMoveCoe );
        x = AbsClamp( x, 1.0f );
        y = AbsClamp( y, 1.0f );
        if( x == 0.0f && y == 0.0f ) return FALSE;
        pos->x = AbsClamp( pos->x + x, 1.0f );
        pos->y = AbsClamp( pos->y - y, 1.0f );
        return TRUE;
    }

    static int calcDigitalCursorPos( u32 button, Vec2* pos )
    {
        const float spd =1.0f / scStickMoveCoe;
        const float spd2= spd * 0.7071f;

        button&=KPAD_CL_BUTTON_UP|KPAD_CL_BUTTON_LEFT|KPAD_CL_BUTTON_DOWN|KPAD_CL_BUTTON_RIGHT;
        switch (button)
        {
        case KPAD_CL_BUTTON_UP:     pos->y-=spd; break;
        case KPAD_CL_BUTTON_LEFT:   pos->x-=spd; break;
        case KPAD_CL_BUTTON_DOWN:   pos->y+=spd; break;
        case KPAD_CL_BUTTON_RIGHT:  pos->x+=spd; break;
        case KPAD_CL_BUTTON_UP  |KPAD_CL_BUTTON_LEFT:   pos->y-=spd2; pos->x-=spd2; break;
        case KPAD_CL_BUTTON_UP  |KPAD_CL_BUTTON_RIGHT:  pos->y-=spd2; pos->x+=spd2; break;
        case KPAD_CL_BUTTON_DOWN|KPAD_CL_BUTTON_LEFT:   pos->y+=spd2; pos->x-=spd2; break;
        case KPAD_CL_BUTTON_DOWN|KPAD_CL_BUTTON_RIGHT:  pos->y+=spd2; pos->x+=spd2; break;
        default: return FALSE;
        }
        pos->x = AbsClamp( pos->x, 1.0f );
        pos->y = AbsClamp( pos->y, 1.0f );
        return TRUE;
    }

    // --------------------------------------------------------------------------------

	void SystemAdapter_HomeButton::UpdatePad()
	{				
		InputAdapter_WII* pInputAdapter = (InputAdapter_WII*)INPUT_ADAPTER;
		ITF_ASSERT(pInputAdapter);
		
		/// Update All controllers states.
		pInputAdapter->updateAllInputState();
		
		for(u32 _pad=0; _pad<WPAD_MAX_CONTROLLERS; _pad++)
		{	
			if(pInputAdapter->isPadConnected(_pad))
			{
				_conData.wiiCon[_pad].use_devtype = pInputAdapter->m_pads[_pad].GetType();
                if(! _conData.wiiCon[_pad].kpad )
                {
		 		    _conData.wiiCon[_pad].kpad = (KPADStatus*)ITF_NEW(KPADStatus);
                }

                pInputAdapter->m_pads[_pad].CpyKStatus(_conData.wiiCon[_pad].kpad);
                u32 input_classic = 0; 

                // NB[LaurentCou]: from SDSG_hbmdemos, proper support for classic controller, fixes RO-15569
                /* 
                DPD takes precedence over the Classic controller - this is the behavior expected by Lotcheck
                If the Classic is connected (and the game supports it) then
                    When DPD is zero, use Classic input directly, pass WPAD_DEV_CLASSIC as device type
                    When DPD is non-zero, use only WPAD_DEV_CORE input

                This will mimic the behavior of the Wii System Menu version of the HBM
                */

                if( _conData.wiiCon[_pad].use_devtype == WPAD_DEV_CLASSIC )
                {                   
                    input_classic = calcDigitalCursorPos(
                        _conData.wiiCon[_pad].kpad->ex_status.cl.hold,
                        &_conData.wiiCon[_pad].pos );

                    input_classic = input_classic | calcAnalogCursorPos(
                        _conData.wiiCon[_pad].kpad->ex_status.cl.lstick.x,
                        _conData.wiiCon[_pad].kpad->ex_status.cl.lstick.y,
                        &_conData.wiiCon[_pad].pos );

                    /* Mirror classic button presses as CORE input so they are read even if the DPD is valid */
                    if( KPAD_CL_BUTTON_HOME & _conData.wiiCon[_pad].kpad->ex_status.cl.trig )
                        _conData.wiiCon[_pad].kpad->trig |= KPAD_BUTTON_HOME;
                    if( KPAD_CL_BUTTON_A & _conData.wiiCon[_pad].kpad->ex_status.cl.trig )
                        _conData.wiiCon[_pad].kpad->trig |= KPAD_BUTTON_A;
                }

                if (_conData.wiiCon[_pad].kpad->dpd_valid_fg)
                {
                    /* If DPD is active, use only CORE input */
                    _conData.wiiCon[_pad].pos.x         = _conData.wiiCon[_pad].kpad->pos.x;
                    _conData.wiiCon[_pad].pos.y         = _conData.wiiCon[_pad].kpad->pos.y;
                    _conData.wiiCon[_pad].use_devtype   = WPAD_DEV_CORE;
                }
		 	}
			else 
			{
				_conData.wiiCon[_pad].pos.x = 0.0f;
		   		_conData.wiiCon[_pad].pos.y = 0.0f;
                if(_conData.wiiCon[_pad].kpad)
                {
                    SF_DEL(_conData.wiiCon[_pad].kpad);
                    _conData.wiiCon[_pad].kpad = NULL;
                }		   		
		   		_conData.wiiCon[_pad].use_devtype = WPAD_DEV_NOT_FOUND;
			}
		}
	}

	void SystemAdapter_HomeButton::Close()
	{
		GXSetCurrentMtx(GX_PNMTX0);
		GXSetZMode(GX_ENABLE,GX_LEQUAL,GX_ENABLE);
		GXSetBlendMode( GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_NOOP );
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0,GX_COLOR0A0);
		GXSetTevOp(GX_TEVSTAGE0,GX_MODULATE);
		GXSetNumTexGens(1);
	 	GXSetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_VTX, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
		GXSetAlphaUpdate(GX_TRUE);	
		
		
		// Alpha compare TEV settings
		GXSetAlphaCompare( GX_ALWAYS,0, GX_AOP_OR, GX_ALWAYS, 0 );
		
		// Z-compare mode settings
		GXSetZMode(GX_DISABLE, GX_ALWAYS, GX_DISABLE);
		
		// Settings for the number of indirect lookups to use
		GXSetNumIndStages(0);
		
		// Settings for the swap mode for swapping to a given TEV stage
		GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
		
		// Texture coordinate scale settings
		GXSetTexCoordScaleManually(GX_TEXCOORD0, GX_FALSE, 0, 0);
		GXSetTexCoordScaleManually(GX_TEXCOORD1, GX_FALSE, 0, 0);
		
		// Cylinder wrapping settings
		GXSetTexCoordCylWrap(GX_TEXCOORD0, GX_FALSE, GX_FALSE);
		GXSetTexCoordCylWrap(GX_TEXCOORD1, GX_FALSE, GX_FALSE);
		
		// Culling control
		GXSetCullMode(GX_CULL_NONE);
		
		// Gamma correction for pixels being copied from EFB to XFB
		GXSetDispCopyGamma(GX_GM_1_0);
		
		// Fog settings
		GXSetFog(GX_FOG_NONE, 0, 0, 0, 0, (GXColor){ 0, 0, 0, 255 });

		if (HBMIsReassignedControllers())
		{            
            INPUT_ADAPTER_WII->onHBMHasReassignedControllers();
		}
		
		DeleteHomeMenu();

        if (!TRC_ADAPTER_WII->IsShutdownRequested())
    		VISetBlack(GX_FALSE);
		
	    GFX_ADAPTER->clear(GFX_CLEAR_COLOR|GFX_CLEAR_DEFERRED, 0.f, 0.f, 0.f, 0.f);
	    
        GFX_ADAPTER_WII->setResolution_WII(GFX_ADAPTER->getScreenWidth(), GFX_ADAPTER->getScreenHeight(), SC_ASPECT_RATIO_16x9);
        GFX_ADAPTER_WII->resetGX();

		GXSetDrawDone();
		VIWaitForRetrace();
		
		INPUT_ADAPTER_WII->homeMenuStop();	
		
		_inHome = bfalse;
	}

	u32 SystemAdapter_HomeButton::Update()
	{
		UpdatePad();
        SoundManager::staticUpdate(LOGICDT);

		HBMSelectBtnNum oButton = HBMCalc(&_conData);		
		HBMUpdateSound();
				
		switch(oButton)
		{
			case HBM_SELECT_BTN1:
				TRCManagerAdapter_WII::RequestExit(TRCManagerAdapter::Exit_BackToMenu);

				// Stop here (3 methods, just to be sure). 
				OSSuspendThread(OSGetCurrentThread());
				OSSleepMilliseconds(100000); 
				while (1);

				break;
			case HBM_SELECT_BTN2:	
                TRCManagerAdapter_WII::RequestExit(TRCManagerAdapter::Exit_Restart);

				// Stop here (3 methods, just to be sure). 
				OSSuspendThread(OSGetCurrentThread());
				OSSleepMilliseconds(10000); 
				while (1);

				break;
			case HBM_SELECT_BTN4:	
				OSLaunchManualViewer(NULL);
				break;
			case HBM_SELECT_HOMEBTN:
    			_inHome = bfalse;
				break;			
		}
		
		return _inHome;
	}

	void SystemAdapter_HomeButton::Render()
	{
		u32 u32_ScreenWidth, u32_ScreenHeight, u32_ScreenLeft, u32_ScreenTop;
		Mtx mv;
		
        GFX_ADAPTER_WII->setResolution_WII((SCGetAspectRatio() == SC_ASPECT_RATIO_16x9) ? 812 : 608, 456, SCGetAspectRatio());

        GFX_ADAPTER->startRendering();    
	    GFX_ADAPTER->clear(GFX_CLEAR_COLOR|GFX_CLEAR_DEFERRED, 0.f, 0.f, 0.f, 0.f);
	    
	    u32_ScreenWidth 	= GFX_ADAPTER_WII->getRealScreenWidth();
		u32_ScreenHeight 	= GFX_ADAPTER_WII->getRealScreenHeight();
		u32_ScreenLeft 		= 0;	//TODO: check that
		u32_ScreenTop 		= 0;	//TODO: check that
		
		
	   	MTXIdentity(mv);
	    GXLoadPosMtxImm(mv, GX_PNMTX1);
	    
	    GXInvalidateVtxCache();
	    GXInvalidateTexAll();    
		GXSetColorUpdate(GX_TRUE);
		GXSetAlphaUpdate(GX_FALSE);	

        GFX_RECT saveViewport;
        GFX_ADAPTER->getViewport(&saveViewport);        

        GFX_RECT Viewport;
        Viewport.left   = 0;
        Viewport.top    = 0;
        Viewport.right  = u32_ScreenWidth;
        Viewport.bottom = u32_ScreenHeight;
        GFX_ADAPTER->setupViewport(&Viewport);
			
		bool is16x9 = SCGetAspectRatio()==SC_ASPECT_RATIO_16x9;
		
		//background
		InitProjection(is16x9);		
	    GXSetCullMode( GX_CULL_NONE );
	    GXSetZMode( GX_DISABLE, GX_LEQUAL, GX_DISABLE ) ;
	    GXSetZCompLoc( GX_TRUE );
	    GXSetZTexture( GX_ZT_DISABLE, GX_TF_Z8, 0 );
		
		//ui
		InitProjection(is16x9);
		
	    GXClearVtxDesc();
	    GXSetVtxAttrFmt(GX_VTXFMT4, GX_VA_POS,  GX_POS_XY,  GX_F32, 0);
	    GXSetVtxAttrFmt(GX_VTXFMT4, GX_VA_CLR0, GX_CLR_RGB, GX_RGB8, 0);
	    GXSetVtxDesc(GX_VA_POS,  GX_DIRECT);
	    GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);

	    GXSetNumChans(1);
	    GXSetNumTexGens(0);
	    GXSetNumTevStages(1);
	    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	    GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

	    GXSetBlendMode(GX_BM_NONE, GX_BL_ZERO, GX_BL_ZERO, GX_LO_CLEAR);
	    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	    GXSetCurrentMtx( GX_PNMTX1 );
	        
	    //GXColor clr = { 1.f, 1.f, 1.f, 1.f };
	    //GXSetFog(GX_FOG_NONE,0.f,1.f,0.f,1.f,clr);
	    SetAdjustValue();	
	    HBMSetAdjustFlag(is16x9);
		HBMDraw();	

		GFX_ADAPTER_WII->endRendering();
		GFX_ADAPTER_WII->present();

        GFX_ADAPTER->setupViewport(&saveViewport);
		
		if(!_isRenderedOnce)
		{
			_isRenderedOnce = btrue;
            if (!TRC_ADAPTER_WII->IsShutdownRequested())
            {
                VISetBlack(GX_FALSE);
            }
		}
	}

	void SystemAdapter_HomeButton::Loop()
	{
		while(_inHome && !TRCManagerAdapter_WII::IsShutdownRequested())
		{		
			if(Update())
			{
				Render();
			}
			sleep(10);
		}		
	}

	void SystemAdapter_HomeButton::ProcessHomeMenu()
	{
	    if(!bIsInHome())
	    {
			VIWaitForRetrace();
	        Open();	
			Loop();
            Close();
	    }
	}

	void SystemAdapter_HomeButton::DrawBanIcon(u8 alpha )
	{
        const f32 W = GFX_ADAPTER_WII->getRealScreenWidth();
        const f32 H = GFX_ADAPTER_WII->getRealScreenHeight();
		
        const f32 fscale      = f32_Inv(GFX_ADAPTER_WII->getEFBScaleHeight());
        const f32 fTopPos     = H - 50.f;
		const f32 fLeftPos    = 50.f;
		const f32 fIconWidth  = 56.f * fscale;
		const f32 fIconHeight = 56.f * fscale;

        const f32 fTop    = fTopPos;
		const f32 fBottom = fTop  - fIconHeight;
		const f32 fLeft   = fLeftPos;
		const f32 fRight  = fLeft + fIconWidth;
		
        // Not applicable on RAYMAN
		//if (SCGetAspectRatio() == SC_ASPECT_RATIO_16x9)
		//{
		//	float fCorrection = 812.f / 608.f;
		//	fLeft /= fCorrection;
		//	fRight /= fCorrection;
		//}		

		GXLoadTexObj(&_icon.texObj,GX_TEXMAP0);
		GXSetBlendMode(GX_BM_BLEND,GX_BL_SRCALPHA,GX_BL_INVSRCALPHA,GX_LO_NOOP);
        GXSetCullMode(GX_CULL_NONE);
        GXSetZMode(GX_FALSE,GX_LEQUAL,GX_FALSE);

        GFX_ADAPTER->setOrthoView(0.f, W, 0.f, H);

        GFX_RECT saveViewport;
        GFX_ADAPTER->getViewport(&saveViewport);

        GFX_RECT Viewport;
        Viewport.left   = 0;
        Viewport.top    = 0;
        Viewport.right  = (u32)W;
        Viewport.bottom = (u32)H;
        GFX_ADAPTER->setupViewport(&Viewport);

        GFX_ADAPTER_WII->resetUVAnim();

		GXSetNumChans(1);
		GXSetChanCtrl(GX_COLOR0A0,GX_DISABLE,GX_SRC_REG,GX_SRC_VTX,GX_LIGHT_NULL,GX_DF_NONE,GX_AF_NONE);

		GXSetColorUpdate(GX_TRUE);
		GXSetAlphaUpdate(GX_TRUE);
	
		for(u32 i=GX_TEVSTAGE0 ; i<8 ; ++i)
	    {
	        GXSetTevSwapMode((GXTevStageID)i, GX_TEV_SWAP0, GX_TEV_SWAP0);
	        GXSetTevDirect((GXTevStageID)i);
	    }
	
		GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_NEVER, 0);
		GXSetNumTevStages(1);
		
		GXSetTevOrder(GX_TEVSTAGE0,GX_TEXCOORD0,GX_TEXMAP0,GX_COLOR0A0);
		GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);		
		GXSetNumTexGens(1);

		GXClearVtxDesc();
		GXSetVtxDesc(GX_VA_POS,GX_DIRECT);
		GXSetVtxDesc(GX_VA_CLR0,GX_DIRECT);	
		GXSetVtxDesc(GX_VA_TEX0,GX_DIRECT);
		
		GXSetVtxAttrFmt(GX_VTXFMT0,GX_VA_POS,GX_POS_XY,GX_F32,0);
		GXSetVtxAttrFmt(GX_VTXFMT0,GX_VA_CLR0,GX_CLR_RGBA,GX_RGBA8,0);
		GXSetVtxAttrFmt(GX_VTXFMT0,GX_VA_TEX0,GX_TEX_ST,GX_F32,0);
	
		GXBegin(GX_QUADS,GX_VTXFMT0,4);		
			GXPosition2f32(fLeft,fTop);
			GXColor4u8(255,255,255,alpha);
			GXTexCoord2f32(0,0);
			
			GXPosition2f32(fRight,fTop);		
			GXColor4u8(255,255,255,alpha);
			GXTexCoord2f32(1,0);
			
			GXPosition2f32(fRight,fBottom);
			GXColor4u8(255,255,255,alpha);
			GXTexCoord2f32(1,1);
			
			GXPosition2f32(fLeft,fBottom);
			GXColor4u8(255,255,255,alpha);
			GXTexCoord2f32(0,1);
		GXEnd();

		GXSetZMode(GX_TRUE,GX_LEQUAL,GX_TRUE);
		GXSetNumTexGens(1);
		GXSetBlendMode(GX_BM_NONE,GX_BL_ONE,GX_BL_ZERO,GX_LO_NOOP);

        GFX_ADAPTER->setupViewport(&saveViewport);
    }

    void SystemAdapter_HomeButton::ActiveProhibitedIcon()
    {
        _icon.enable = btrue;
    }

	void SystemAdapter_HomeButton::ShowProhibited ()
	{	
		if(_icon.enable && _IconAlpha>0)
		    DrawBanIcon((u8)(_IconAlpha*255.0));
	}
	
	void SystemAdapter_HomeButton::ForceProhibitedExit()
	{
		_icon.enable = bfalse;
		_icon.showEndTime = 0;
		_icon.showStartTime = 0;
	}

	void SystemAdapter_HomeButton::HandleProhibited()
	{
		if(!_inHome && bIsProhibited() && _icon.enable)
		{
            _IconAlpha = 0; 		
            f64 currentTime = SYSTEM_ADAPTER->getTime();

            if(!_icon.showStartTime) 
            {
                _icon.showStartTime = currentTime;
                _IconAlpha = 0;	
            }
            else if(currentTime < _icon.showStartTime + WII_HOMEBUTTON_FADEIN)
            {
                _IconAlpha = (currentTime - _icon.showStartTime) / WII_HOMEBUTTON_FADEIN;
            }
            else if(_icon.showEndTime != 0 && currentTime < _icon.showEndTime + WII_HOMEBUTTON_FADEOUT)
            {
                _IconAlpha = 1 - (currentTime - _icon.showEndTime) / WII_HOMEBUTTON_FADEOUT;
            }
            else if(_icon.showEndTime != 0)
            {
                ForceProhibitedExit();
                return;
            }
            else
            {
                _IconAlpha = 1;

                // active fadeout
                if ( !_icon.showEndTime && 
                    currentTime >= _icon.showStartTime + WII_HOMEBUTTON_FADEIN + WII_HOMEBUTTON_SHOWTIME )
                    _icon.showEndTime = _icon.showStartTime + WII_HOMEBUTTON_FADEIN + WII_HOMEBUTTON_SHOWTIME;			
            }
		}
        else if(_icon.enable)
        {
            ForceProhibitedExit();
        }
	}
	u32 SystemAdapter_HomeButton::bIsAnyHomeJustPressed()
	{
		InputAdapter::PressStatus	buttons[JOY_MAX_BUT];	
		bool _homeButtonState = false;
		for (u32 _pad = 0; _pad < JOY_MAX_COUNT ; _pad ++)
		{
	   		INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentEngine, _pad, buttons, JOY_MAX_BUT);

			InputAdapter::PressStatus status = buttons[m_joyButton_Home];
			bbool pressed = (status == InputAdapter::Pressed) || (status == InputAdapter::Double_Press) || (status == InputAdapter::JustPressed);
			if (pressed && !m_homeButtonPressed[_pad])
				_homeButtonState = true;
			m_homeButtonPressed[_pad] = pressed;
		}
		return _homeButtonState;
	}

	void SystemAdapter_HomeButton::DestroyHomeMenu()
	{
		Synchronize::destroyCriticalSection(&_CriticalSection);
	}

} //namespace ITF
