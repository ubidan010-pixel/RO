#include "precompiled_inputadapter_WII.h"

#ifndef _ITF_INPUTADAPTER_WII_H_
#include "adapters/InputAdapter_WII/InputAdapter_WII.h"
#endif //_ITF_INPUTADAPTER_WII_H_

#include <revolution.h>
#include <revolution/wpad.h>
#include <revolution/kpad.h>
#include <revolution/mem.h>

#ifndef __MEM_MACROS_WII_H__
#include "core/memory/WII/MEM_macros_WII.h"
#endif //__MEM_MACROS_WII_H__

#ifndef __MTH_INT_WII_H__
#include "core/math/WII/MTH_int_WII.h"
#endif //__MTH_INT_WII_H__

#ifndef _ITF_TRCManagerAdapter_H_
#include "engine/AdaptersInterfaces/TRCManager_Adapter.h"
#endif //_ITF_TRCManagerAdapter_H_

#ifndef _ITF_SYSTEMADAPTER_WII_H_
#include "adapters/SystemAdapter_WII/SystemAdapter_WII.h"
#endif //_ITF_SYSTEMADAPTER_WII_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_


namespace ITF
{
    /**
	 =======================================================================================================================
		WPad allocator *
	 =======================================================================================================================
	 */
	static void *WPADalloc32(unsigned long size)
	{
        MEM_M_PushExt(MEM_C_MEM2_ALLOC|MEM_C_HARD_ALLOC);
        void* p = Memory::alignedMallocCategory(size, 32, MemoryId::mId_System);
        MEM_M_PopExt();
		return p;
	}

	static u8 WPADfree32(void *addr)
	{
        Memory::alignedFree(addr);
		return 1;
	}

	static ITF_THREAD_CRITICAL_SECTION 	Wii_KPADMutex; //KPAD is not thread safe
	static CallbackConnectStatus	Wii_JoyConnectStatus[MAX_PAD];
	static CallbackExtensionStatus  Wii_JoyExtensionStatus[MAX_PAD];
	
    /// Constructor.
	InputAdapter_WII::InputAdapter_WII() : m_pauseRumble(bfalse)
    {
    	//Callback state
        ITF_MemSet(Wii_JoyConnectStatus, 0, sizeof(Wii_JoyConnectStatus));
        ITF_MemSet(Wii_JoyExtensionStatus, 0, sizeof(Wii_JoyExtensionStatus));
		
		mayInitAPI();
		
#ifdef WII_SUPPORT_GAMECUBE_PAD
		wpadInit();
#endif //WII_SUPPORT_GAMECUBE_PAD
    }

    /// Destructor.
    InputAdapter_WII::~InputAdapter_WII()
    {    
    	closeAPI();
    }
    
    void InputAdapter_WII::mayInitAPI()
    {    			
		//A mutex is needed because the KPAD lib is not ThreadSafe.(rumble/update)
		Synchronize::createCriticalSection(&Wii_KPADMutex);
		Synchronize::enterCriticalSection(&Wii_KPADMutex);
		
		WPADRegisterAllocator(WPADalloc32, WPADfree32);		
		KPADInitEx(m_KPADunifiedStatus, sizeof(m_KPADunifiedStatus)/sizeof(KPADUnifiedWpadStatus));
		KPADReset();
		
		for(int i = 0; i < maxPad; i++) m_pads[i].Init(i);
				
		Synchronize::leaveCriticalSection(&Wii_KPADMutex);
		m_speaker.ProcessStart();
    }

	void InputAdapter_WII::closeAPI()
	{
		Synchronize::enterCriticalSection(&Wii_KPADMutex);
		for(int i = 0; i < maxPad; i++) 
		{		
			m_speaker.TurnOff(i);	    
			m_speaker.WaitForShutdown(i);
			m_pads[i].Close();			
		}
		Synchronize::leaveCriticalSection(&Wii_KPADMutex);
		Synchronize::destroyCriticalSection(&Wii_KPADMutex);
		m_speaker.ProcessStop();
		KPADShutdown();
	}

    void InputAdapter_WII::startRumble(u32 _numPad, f64 _time, f32 _leftMotorSpeed, f32 _rightMotorSpeed)
    {
        if(m_pads[_numPad].isConnected() && getPadType(_numPad) != Pad_WiiClassic)
        {
            m_pads[_numPad].RumbleSet(_time);
        }
    }

    void InputAdapter_WII::stopRumble(u32 _numPad)
    {
        ITF_ASSERT(_numPad<maxPad);
        m_pads[_numPad].StopMotor();		
    }

	void InputAdapter_WII::pauseAllRumble(bbool _pause)
	{
		m_pauseRumble = _pause;
		if (_pause)
		{
			for(u32 _pad = 0; _pad < JOY_MAX_COUNT; _pad++) 
				m_pads[_pad].StopMotor(btrue); 
		}
	}

    void InputAdapter_WII::getWiimoteAcc(u32 _pad, Vec3d* _acc)
    {   
        if(m_pads[_pad].isConnected())
        {
            _acc->m_x = m_pads[_pad].GetKStatus().acc.x;
            _acc->m_y = m_pads[_pad].GetKStatus().acc.y;
            _acc->m_z = m_pads[_pad].GetKStatus().acc.z;
        }        
    }

	void InputAdapter_WII::getPointer(u32 _pad, Vec3d* _pointer)
	{		
		const Vec2	*pVec = &m_pads[_pad].GetKStatus().pos;

		_pointer->m_x = pVec->x;
		_pointer->m_y = pVec->y + WiiPad::f32_PointerShiftGet(WiiPad::u8_GetSensorBarPosition());
		_pointer->m_z = m_pads[_pad].GetKStatus().dist;
	}

    void InputAdapter_WII::getExtensivePointer(u32 _pad, Vec3d* _pointer)
	{
		_pointer->m_x = m_pads[_pad].GetDPDExt().fPointer[0];
		_pointer->m_y = m_pads[_pad].GetDPDExt().fPointer[1] + WiiPad::f32_PointerShiftGet(WiiPad::u8_GetSensorBarPosition());
		_pointer->m_z = m_pads[_pad].GetDPDExt().fPointer[2];
	}

	i32	InputAdapter_WII::getValidDPD(u32 _pad)
	{
		// note : data_format == WPADGetDataFormat
		switch(m_pads[_pad].GetKStatus().data_format)
		{
		case WPAD_FMT_CORE_ACC_DPD:
		case WPAD_FMT_FREESTYLE_ACC_DPD:
		case WPAD_FMT_CLASSIC_ACC_DPD:
		case WPAD_FMT_CORE_ACC_DPD_FULL:
		case WPAD_FMT_MPLS:
			{
				return m_pads[_pad].GetKStatus().dpd_valid_fg ;
			}
		default:
			return -1;
		}
	}

#ifdef WII_SUPPORT_GAMECUBE_PAD
	void InputAdapter_WII::wpadInit()
	{
		PADInit();
		ITF_MemSet(m_gcpad, 0, sizeof(m_gcpad));
		
		PadChanMask[0] = PAD_CHAN0_BIT; 
		PadChanMask[1] = PAD_CHAN1_BIT;
		PadChanMask[2] = PAD_CHAN2_BIT;
		PadChanMask[3] = PAD_CHAN3_BIT;
		
		ITF_MemSet(m_wpadMapping, -1, sizeof(i32)*JOY_MAX_BUT);
		m_wpadMapping[MTH_PowerOfTwo(PAD_BUTTON_LEFT)]	= m_joyButton_DPadL;
		m_wpadMapping[MTH_PowerOfTwo(PAD_BUTTON_RIGHT)]	= m_joyButton_DPadR;
		m_wpadMapping[MTH_PowerOfTwo(PAD_BUTTON_DOWN)]	= m_joyButton_DPadD;
		m_wpadMapping[MTH_PowerOfTwo(PAD_BUTTON_UP)]	= m_joyButton_DPadU;
		m_wpadMapping[MTH_PowerOfTwo(PAD_BUTTON_START)]	= m_joyButton_Start;
		m_wpadMapping[MTH_PowerOfTwo(PAD_BUTTON_X)]		= m_joyButton_X;
		m_wpadMapping[MTH_PowerOfTwo(PAD_BUTTON_A)]		= m_joyButton_A;
		m_wpadMapping[MTH_PowerOfTwo(PAD_BUTTON_B)]		= m_joyButton_B;
		m_wpadMapping[MTH_PowerOfTwo(PAD_BUTTON_Y)]		= m_joyButton_Y;
		m_wpadMapping[MTH_PowerOfTwo(PAD_TRIGGER_Z)]	= m_joyButton_Back;
		m_wpadMapping[MTH_PowerOfTwo(PAD_TRIGGER_R)]	= m_joyButton_ThumbRight;
		m_wpadMapping[MTH_PowerOfTwo(PAD_TRIGGER_L)]	= m_joyButton_ThumbLeft;	
	}
	
	void InputAdapter_WII::updatePadStatus()
	{
		u32 ResetReq = 0;
		u32 rumbleMask = PADRead(m_gcpads);
		PADClamp(m_gcpads);
		for (u32 _pad = 0; _pad < PAD_MAX_CONTROLLERS; _pad++)
		{
	        // Connection check
	        if ( m_gcpads[_pad].err == PAD_ERR_NONE ||
	             m_gcpads[_pad].err == PAD_ERR_TRANSFER )
	        {
	            
	        }
	        else if ( m_gcpads[_pad].err == PAD_ERR_NO_CONTROLLER )
	        {
	            ResetReq |= PadChanMask[_pad];
	        }

	        padCopy(&m_gcpads[_pad], &m_gcpad[_pad]);			
	    	  	
	   		if(_pad == 0)	// TEMP,TODO: handle all 4 pad if needed
	   		{
		        m_virtualPadAxes[0] = (f32)m_gcpads[_pad].stickX / 0x40;
		        m_virtualPadAxes[1] = (f32)m_gcpads[_pad].stickY / 0x40;
		    	
		        // Check buttons.
				ITF_Memcpy(m_oldVirtualPadStatus, m_virtualPadStatus, sizeof(m_oldVirtualPadStatus));        
				
				u16 state = m_gcpads[_pad].button;
				u32 button = 0;
				ITF_MemSet(m_virtualPadStatus, 0, sizeof(bool)*JOY_MAX_BUT);		
				while(state)
				{
					if(m_wpadMapping[button]>=0)
						m_virtualPadStatus[m_wpadMapping[button]] = (state & 1);
					state >>= 1;
					button++;
				}		
			}
		} 

	    // Try resetting pad channels which have been not valid
	    if ( ResetReq )
	    {
	        PADReset( ResetReq );
	    }  
    }

	void InputAdapter_WII::padCopy( PADStatus* pad, WiiPadStatus* dmpad )
	{
	    u16  dirs;

	    if ( pad->err != PAD_ERR_TRANSFER )
	    {
	        // Detects which direction is the stick(s) pointing.
	        // This can be used when we want to use a stick as direction pad.
	        dirs = 0;
	        if ( pad->stickX    < - WII_STICK_THRESHOLD )
	            dirs |= WII_STICK_LEFT;
	        if ( pad->stickX    >   WII_STICK_THRESHOLD )
	            dirs |= WII_STICK_RIGHT;
	        if ( pad->stickY    < - WII_STICK_THRESHOLD )
	            dirs |= WII_STICK_DOWN;
	        if ( pad->stickY    >   WII_STICK_THRESHOLD )
	            dirs |= WII_STICK_UP;
	        if ( pad->substickX < - WII_STICK_THRESHOLD )
	            dirs |= WII_SUBSTICK_LEFT;
	        if ( pad->substickX >   WII_STICK_THRESHOLD )
	            dirs |= WII_SUBSTICK_RIGHT;
	        if ( pad->substickY < - WII_STICK_THRESHOLD )
	            dirs |= WII_SUBSTICK_DOWN;
	        if ( pad->substickY >   WII_STICK_THRESHOLD )
	            dirs |= WII_SUBSTICK_UP;

	        // Get the direction newly detected / released
	        dmpad->dirsNew      = PADButtonDown(dmpad->dirs, dirs);
	        dmpad->dirsReleased = PADButtonUp(dmpad->dirs, dirs);
	        dmpad->dirs         = dirs;

	        // Get DOWN/UP status of all buttons
	        dmpad->buttonDown = PADButtonDown(dmpad->pst.button, pad->button);
	        dmpad->buttonUp   = PADButtonUp(dmpad->pst.button, pad->button);

	        // Get delta of analogs
	        dmpad->stickDeltaX = (s16)(pad->stickX - dmpad->pst.stickX);
	        dmpad->stickDeltaY = (s16)(pad->stickY - dmpad->pst.stickY);
	        dmpad->substickDeltaX = (s16)(pad->substickX - dmpad->pst.substickX);
	        dmpad->substickDeltaY = (s16)(pad->substickY - dmpad->pst.substickY);

	        // Copy current status into DEMOPadStatus field
	        dmpad->pst = *pad;
	    }
	    else
	    {
	        // Get the direction newly detected / released
	        dmpad->dirsNew = dmpad->dirsReleased = 0;

	        // Get DOWN/UP status of all buttons
	        dmpad->buttonDown = dmpad->buttonUp = 0;

	        // Get delta of analogs
	        dmpad->stickDeltaX =    dmpad->stickDeltaY    = 0;
	        dmpad->substickDeltaX = dmpad->substickDeltaY = 0;
	    }		
	}
#endif	//WII_SUPPORT_GAMECUBE_PAD

    void InputAdapter_WII::getGamePadPos( u32 _environment,u32 _pad, float *_pos, u32 _numAxes ) const
    {
		for (u32 i = 0 ; i < _numAxes ; i++) _pos[i] = 0.0f;
		
		if (m_pads[_pad].isConnected() && ((m_environmentInput & _environment) != 0) )
		{
			if(m_pads[_pad].isNunchuk())
			{
				_pos[m_joyStickLeft_X] = m_pads[_pad].GetKStatus().ex_status.fs.stick.x;
				_pos[m_joyStickLeft_Y] = m_pads[_pad].GetKStatus().ex_status.fs.stick.y;							
			} 
			else if (m_pads[_pad].isClassic())
			{
				_pos[m_joyStickLeft_X] = m_pads[_pad].GetKStatus().ex_status.cl.lstick.x;
				_pos[m_joyStickLeft_Y] = m_pads[_pad].GetKStatus().ex_status.cl.lstick.y;
				
				if(_numAxes>m_joyStickRight_X)
				{
					_pos[m_joyStickRight_X] = m_pads[_pad].GetKStatus().ex_status.cl.rstick.x;
					_pos[m_joyStickRight_Y] = m_pads[_pad].GetKStatus().ex_status.cl.rstick.y;	
				}				
			}
			else // Sideway
			{
				// --> no joystick
			}
		}
		
#ifdef WII_SUPPORT_GAMECUBE_PAD
        // For now, only 1 pad gamecube is allowed
        // + handle it only if no wiimote is connected on same index
		if ((_pad == 0) && !m_pads[_pad].isConnected())
		{
			//This environment is enable ,fill the buffer
			if ((m_environmentInput & _environment) != 0) 
	        {
	            for (u32 i = 0 ; i < _numAxes ; i++)
	            {
	                _pos[i] = m_virtualPadAxes[i];
	            }
	        }
	        else
	        {
	            for (u32 i = 0 ; i < _numAxes ; i++)
	                _pos[i] = 0.0f;
	        }  
        }
#endif //WII_SUPPORT_GAMECUBE_PAD
    }

    u32 InputAdapter_WII::getConnectedPadIndex (u32 _padIndex) const
    {
		u32 padIndex = 0;
		u32 activeCount = 0;
		for (; padIndex < maxPad; padIndex++)
		{
			if (m_pads[padIndex].isConnected())
			{
				if (activeCount == _padIndex)
					break;
				activeCount++;
			}
		}
        return padIndex;
    }

#ifdef WII_SUPPORT_GAMECUBE_PAD
    void InputAdapter_WII::logPadStatus(u32 _pad) const
	{
#ifndef ITF_FINAL
		static bool _padConnectedStatus[4] = { true, true, true, true };
		
		if (m_pads[_pad].isConnected())
		{
			if(!_padConnectedStatus[_pad])	
			{
				OSReport("PadStatus[%d]: connected \n", _pad);
				_padConnectedStatus[_pad] = true;
			}
			
			// buttons  ****************
			static InputAdapter::PressStatus	_buttons[4][JOY_MAX_BUT];
			static bool _buttonsINIT = false;
			if(!_buttonsINIT) 
			{
				ITF_MemSet(_buttons, 0, sizeof(InputAdapter::PressStatus) * JOY_MAX_BUT * 4);
				_buttonsINIT= true;
			}
			
			
			bool changed = false;
			for (u32 i = 0; i < JOY_MAX_BUT && !changed; i++)
			{
				if(m_pads[_pad].getMixedButton(i) != _buttons[_pad][i]) changed = true;
			}
			
			if(changed)
			{
				OSReport("PadStatus[%d]: ", _pad);
				for (u32 i = 0; i < JOY_MAX_BUT; i++)
				{					
					OSReport ( " %d ", (u32) m_pads[_pad].getMixedButton(i) );
					_buttons[_pad][i] = m_pads[_pad].getMixedButton(i);
				}
				OSReport("\n");
			}
			
			// extension ******************
			static WiiPad::enExtensionControllers _padConnectedStatus[4] = { WiiPad::IO_ExtensionType_None, WiiPad::IO_ExtensionType_None, WiiPad::IO_ExtensionType_None, WiiPad::IO_ExtensionType_None };
			if(_padConnectedStatus[_pad] != m_pads[_pad].GetCurrentExtensionType())
			{		
				OSReport("ExtensionStatus[%d]: ", _pad);
				switch(m_pads[_pad].GetCurrentExtensionType())
				{
				case WiiPad::IO_ExtensionType_None: OSReport("IO_ExtensionType_None"); break;
				case WiiPad::IO_ExtensionType_Unknown: OSReport("IO_ExtensionType_Unknown"); break;
				case WiiPad::IO_ExtensionType_WiiNunchuk: OSReport("IO_ExtensionType_WiiNunchuk"); break;
				case WiiPad::IO_ExtensionType_WiiClassic: OSReport("IO_ExtensionType_WiiClassic"); break;
				case WiiPad::IO_ExtensionType_WiiMotionPlus: OSReport("IO_ExtensionType_WiiMotionPlus"); break;
				case WiiPad::IO_ExtensionType_WiiMotionPlusAndNunchuk: OSReport("IO_ExtensionType_WiiMotionPlusAndNunchuk"); break;
				default:OSReport("IO_ExtensionType_Unknown"); break;
				}
				_padConnectedStatus[_pad] = m_pads[_pad].GetCurrentExtensionType();
				OSReport("\n");
			}			
		} else {
			if(_padConnectedStatus[_pad])
			{
				OSReport("PadStatus[%d]: not connected\n", _pad);
				_padConnectedStatus[_pad] = false;	
			}
		}
#endif  //ITF_FINAL
	}
#endif // WII_SUPPORT_GAMECUBE_PAD

    void InputAdapter_WII::getGamePadButtons( u32 _environment ,u32 _pad, PressStatus* _buttons, u32 _numButtons ) const
    {   
		for (u32 i = 0; i < _numButtons; i++)
		{			
			if (m_pads[_pad].isConnected())
            {
                if ( m_buttonMode == MixedMode || i == m_joyButton_Home )
				    _buttons[i] = m_pads[_pad].getMixedButton(i);
                else // ExtensionOnly
                {
                    if(m_pads[_pad].isClassic())
                        _buttons[i] = m_pads[_pad].getClassicButton(i);
                    else
                        _buttons[i] = m_pads[_pad].getMixedButton(i);
                }
            }
            else
				_buttons[i] = InputAdapter::Released;				
			
		}

#ifdef WII_SUPPORT_GAMECUBE_PAD
        // For now, only 1 pad gamecube is allowed
        // + handle it only if no wiimote is connected on same index
		if ((_pad == 0) && !m_pads[_pad].isConnected())
		{
			for (u32 i = 0; i < _numButtons; i++)
			{
				bool bPressed		= m_virtualPadStatus[i];
				bool bWasPressed	= m_oldVirtualPadStatus[i];

				if( bPressed && !bWasPressed )				
					_buttons[i] = InputAdapter::JustPressed;
				else if ( bPressed )
					_buttons[i] = InputAdapter::Pressed ; 
				else if ( bWasPressed )
					_buttons[i] = InputAdapter::JustReleased; 
				else
					_buttons[i] = InputAdapter::Released;
			}
		}
#endif //WII_SUPPORT_GAMECUBE_PAD
    }

    u32 InputAdapter_WII::getGamePadCount()
    {
        return m_numValidPads;
    }

    void InputAdapter_WII::updateMouse()
    {
    }
    
    void InputAdapter_WII::homeMenuStart()
    {
    	m_speaker.HomeButtonStart();
    	    	
    	for(u32 _chan = 0; _chan < maxPad; _chan ++)
    	{
    		if(m_pads[_chan].isConnected())
            {
                m_pads[_chan].SetDPDState(btrue);
                stopRumble(_chan);
            }
    	}    		
    }
    
    void InputAdapter_WII::homeMenuStop()
    {
    	m_speaker.HomeButtonFinish();    
    	
    	for(u32 _chan = 0; _chan < maxPad; _chan ++)
    	{
    		if(m_pads[_chan].isConnected())
    			m_pads[_chan].SetDPDState(bfalse);
    	} 	
    }

    void InputAdapter_WII::setIsPadEnable( u32 _numPad, bbool _enable )
    {
        if(_numPad>=maxPad)
            return;

        if( !_enable && isPadConnected(_numPad) )
        {
            WPADDisconnect(_numPad);
            //setPadConnected(_numPad, bfalse);
        }

        m_pads[_numPad].setIsEnable(_enable);
    }

    bbool InputAdapter_WII::getIsPadEnable( u32 _numPad ) const
    {
        if(_numPad>=maxPad)
            return bfalse;

        return m_pads[_numPad].getIsEnable();
    }


    void InputAdapter_WII::updateAllInputState()
    {	    	
    	Synchronize::enterCriticalSection(&Wii_KPADMutex);

		m_numValidPads = 0 ; 
		for(int i = 0; i < maxPad; i++) 
		{
			bool wasConnected = (m_pads[i].isConnected());
			m_pads[i].Update();
			bool isConnected  = (m_pads[i].isConnected());

            if(isConnected)
                m_numValidPads ++;
			
			//Speaker update status
			if (!wasConnected && isConnected)  m_speaker.Connect(i);
			if (wasConnected && !isConnected)  m_speaker.Disconnect(i);
		}
		
		Synchronize::leaveCriticalSection(&Wii_KPADMutex); 	
		
#ifdef WII_SUPPORT_GAMECUBE_PAD
		updatePadStatus();
#endif //WII_SUPPORT_GAMECUBE_PAD
    }

    //----------------------------------------------------------------------------//
	void IO_ExtensionConnectCallback(s32 _iChan, s32 _iResult)
	{
		switch(_iResult)
		{
		case WPAD_DEV_FREESTYLE:
			Wii_JoyExtensionStatus[_iChan].extension = WiiPad::IO_ExtensionType_WiiNunchuk;
			break;
		case WPAD_DEV_MPLS:
			Wii_JoyExtensionStatus[_iChan].extension = WiiPad::IO_ExtensionType_WiiMotionPlus;
			break;
		case WPAD_DEV_MPLS_FREESTYLE:
			Wii_JoyExtensionStatus[_iChan].extension = WiiPad::IO_ExtensionType_WiiMotionPlusAndNunchuk;
			break;
		case WPAD_DEV_CORE:
			Wii_JoyExtensionStatus[_iChan].extension = WiiPad::IO_ExtensionType_None;
			break;
		case WPAD_DEV_CLASSIC:
			Wii_JoyExtensionStatus[_iChan].extension = WiiPad::IO_ExtensionType_WiiClassic;
			break;
		default:
			Wii_JoyExtensionStatus[_iChan].extension = WiiPad::IO_ExtensionType_Unknown;	
		}		
				
		Wii_JoyExtensionStatus[_iChan].updated = true;
	}	

    void IO_ControllerConnectCallback(s32 _iChan, s32 _iReason)
	{	
		Wii_JoyConnectStatus[_iChan].reason 	= _iReason; 
		Wii_JoyConnectStatus[_iChan].updated 	= true;
	}
	
    //----------------------------------------------------------------------------//
    void InputAdapter_WII::onHBMHasReassignedControllers()
    {
        // Force to set the extension controller again
        LOG(" onHBMHasReassignedControllers called >> ");
        for(u32 _pad = 0; _pad < JOY_MAX_COUNT; _pad++)
        {
            if(isPadConnected(_pad))
            {
                // extension controller callback
                WPADSetExtensionCallback(_pad, IO_ExtensionConnectCallback);
            }
        }
    }
	
	// Static init
	//----------------------------------------------------------------------------//
	bool WiiPad::ms_bMPLSInitialized = false;
	u8 WiiPad::mu8_SensorBarPosition = 0;
	f32 WiiPad::mf32_PointerShift[2] = {0.f, 0.f};
	
	// WiiPad Init 
	//----------------------------------------------------------------------------//
	WiiPad::WiiPad() :
		m_conState(nodevice), m_eCurrentExtensionType(IO_ExtensionType_Unknown), m_ePreviousExtensionType(IO_ExtensionType_Unset),
		m_bWPADInfoValid(1), m_type(WPAD_DEV_NOT_FOUND), m_ValidTime(0),
		m_lastAIMChanging (0.0), m_needAIMToChange (bfalse), m_isEnable(btrue)
	{
		ITF_MemSet(&m_PadStatus, 0, sizeof(m_PadStatus));
		ITF_MemSet(m_PadBuffsEX, 0, sizeof(m_PadBuffsEX));
		ITF_MemSet(&m_RumbleState, 0, sizeof(m_RumbleState));
		ITF_MemSet(&m_WPADInfo, 0, sizeof(m_WPADInfo));
		m_fLastCheckTime = (f64)SYSTEM_ADAPTER->getTime();
		m_DPDStatus.desired = enDPDStatus_ON;
		m_DPDStatus.current = enDPDStatus_UNSET;
	}

	WiiPad::~WiiPad()
	{
		KPADSetConnectCallback(m_chan, NULL);
	}

	void WiiPad::Init(int _chan)
	{
		m_chan = _chan;

		ITF_MemSet(m_buttons, 0, sizeof(m_buttons));
		ITF_MemSet(m_ButtonMapping, -1, sizeof(i32)*JOY_MAX_BUT);

		ITF_MemSet(m_classicButtons, 0, sizeof(m_classicButtons));
		ITF_MemSet(m_ClassicButtonMapping, -1, sizeof(i32)*JOY_MAX_BUT);
		
		// Default button mapping.		
		m_ButtonMapping[MTH_PowerOfTwo(WPAD_BUTTON_LEFT)]	= m_joyButton_DPadL;
		m_ButtonMapping[MTH_PowerOfTwo(WPAD_BUTTON_RIGHT)]	= m_joyButton_DPadR;
		m_ButtonMapping[MTH_PowerOfTwo(WPAD_BUTTON_DOWN)]	= m_joyButton_DPadD;
		m_ButtonMapping[MTH_PowerOfTwo(WPAD_BUTTON_UP)]		= m_joyButton_DPadU;
		m_ButtonMapping[MTH_PowerOfTwo(WPAD_BUTTON_PLUS)]	= m_joyButton_Plus;
		m_ButtonMapping[MTH_PowerOfTwo(WPAD_BUTTON_2)]		= m_joyButton_2;
		m_ButtonMapping[MTH_PowerOfTwo(WPAD_BUTTON_1)]		= m_joyButton_1;
		m_ButtonMapping[MTH_PowerOfTwo(WPAD_BUTTON_B)]		= m_joyButton_B;
		m_ButtonMapping[MTH_PowerOfTwo(WPAD_BUTTON_A)]		= m_joyButton_A;
		m_ButtonMapping[MTH_PowerOfTwo(WPAD_BUTTON_MINUS)]	= m_joyButton_Minus;
		m_ButtonMapping[MTH_PowerOfTwo(WPAD_BUTTON_HOME)]	= m_joyButton_Home;
		m_ButtonMapping[MTH_PowerOfTwo(WPAD_BUTTON_Z)]		= m_joyButton_Z;
		m_ButtonMapping[MTH_PowerOfTwo(WPAD_BUTTON_C)]		= m_joyButton_C;
			
		// Classic button mapping
		m_ClassicButtonMapping[MTH_PowerOfTwo(WPAD_CL_BUTTON_LEFT)]		= m_joyButton_DPadL;
		m_ClassicButtonMapping[MTH_PowerOfTwo(WPAD_CL_BUTTON_RIGHT)]	= m_joyButton_DPadR;
		m_ClassicButtonMapping[MTH_PowerOfTwo(WPAD_CL_BUTTON_DOWN)]		= m_joyButton_DPadD;
		m_ClassicButtonMapping[MTH_PowerOfTwo(WPAD_CL_BUTTON_UP)]		= m_joyButton_DPadU;
		m_ClassicButtonMapping[MTH_PowerOfTwo(WPAD_CL_BUTTON_PLUS)]		= m_joyButton_Start;
		m_ClassicButtonMapping[MTH_PowerOfTwo(WPAD_CL_BUTTON_B)]		= m_joyButton_B;
		m_ClassicButtonMapping[MTH_PowerOfTwo(WPAD_CL_BUTTON_Y)]		= m_joyButton_Y;
		m_ClassicButtonMapping[MTH_PowerOfTwo(WPAD_CL_BUTTON_A)]		= m_joyButton_A;
		m_ClassicButtonMapping[MTH_PowerOfTwo(WPAD_CL_BUTTON_X)]		= m_joyButton_X;
		m_ClassicButtonMapping[MTH_PowerOfTwo(WPAD_CL_BUTTON_MINUS)]	= m_joyButton_Back;
		m_ClassicButtonMapping[MTH_PowerOfTwo(WPAD_CL_BUTTON_HOME)]		= m_joyButton_Home;		
		m_ClassicButtonMapping[MTH_PowerOfTwo(WPAD_CL_TRIGGER_ZR)]		= m_joyButton_ZR;
		m_ClassicButtonMapping[MTH_PowerOfTwo(WPAD_CL_TRIGGER_ZL)]		= m_joyButton_ZL;
		m_ClassicButtonMapping[MTH_PowerOfTwo(WPAD_CL_TRIGGER_R)]		= m_joyButton_R;
		m_ClassicButtonMapping[MTH_PowerOfTwo(WPAD_CL_TRIGGER_L)]		= m_joyButton_L;
					
		// register motion+
		//KPADSetControlMplsCallback(m_chan, &ControlMotionPlusCallback);
		
		// connect callback
		KPADSetConnectCallback(m_chan, IO_ControllerConnectCallback);
		KPADSetSamplingCallback(m_chan, NULL);
		
		// Only one controller required at startup. This project specific data should be set by the scripts.
		m_bIsRequired			 = btrue;
		m_eRequiredExtensionType = IO_ExtensionType_None;
					
		m_DPDExt.fPointerRef[2]	= 0;
		m_DPDExt.fPointerRef[0]	= 0;
		m_DPDExt.fPointerRef[1]	= 0;
		m_DPDExt.fPointer[0]	= 0;
		m_DPDExt.fPointer[1]	= 0;
		m_DPDExt.fPointer[2]	= 0;
		
		WiiPad::mu8_SensorBarPosition = WPADGetSensorBarPosition();
	}

	void WiiPad::Close()
	{
		StopMotor();
		WPADDisconnect(GetChan());
	}

	void WiiPad::EnableAimingMode(bbool _enable)
	{		
		if(!_enable && KPADIsEnableAimingMode(m_chan))
		{
			KPADDisableAimingMode(m_chan);
		} else if(_enable)
		{
			KPADEnableAimingMode(m_chan);
		}		
	}	

	void WiiPad::ComputeKStatus()
	{
		// Start with the last data.
		ITF_Memcpy(&m_PadStatus, &m_PadBuffsEX[0], sizeof(m_PadStatus));

		for(int i = 1; i < m_read; ++i)
		{
			// all buttons from sample
			m_PadStatus.hold |= m_PadBuffsEX[i].hold;

			// Copy best most recent valid dpd value.
			if(m_PadBuffsEX[i].dpd_valid_fg > m_PadStatus.dpd_valid_fg)
			{
				m_PadStatus.pos          = m_PadBuffsEX[i].pos;
				m_PadStatus.vec          = m_PadBuffsEX[i].vec;
				m_PadStatus.speed        = m_PadBuffsEX[i].speed;
				m_PadStatus.horizon      = m_PadBuffsEX[i].horizon;
				m_PadStatus.hori_vec     = m_PadBuffsEX[i].hori_vec;
				m_PadStatus.hori_speed   = m_PadBuffsEX[i].hori_speed;
				m_PadStatus.dist         = m_PadBuffsEX[i].dist;
				m_PadStatus.dist_vec     = m_PadBuffsEX[i].dist_vec;
				m_PadStatus.dist_speed   = m_PadBuffsEX[i].dist_speed;
				m_PadStatus.dpd_valid_fg = m_PadBuffsEX[i].dpd_valid_fg;
			}
			
			if (isClassic())
			{			
				m_PadStatus.ex_status.cl.hold |= m_PadBuffsEX[i].ex_status.cl.hold;				
			}
		}
		
		//WPADClampStick(GetChan(), &m_PadStatus, WPAD_STICK_CLAMP_CIRCLE_WITH_PLAY);
	}

    void WiiPad::SetConState(eConState _eConState)
    {
        m_conState = _eConState;
        INPUT_ADAPTER->setPadConnected(m_chan, isConnected());
    }
	
	void WiiPad::SetConnecting()
	{
		if(GetConState() == nodevice)
		{
			double ValidTime = SYSTEM_ADAPTER->getTime() + WII_WAIT_TIME_AFTER_CONNECT;
			if(m_ValidTime < ValidTime) m_ValidTime = ValidTime;
			SetConState(connecting);
			// extension controller callback
			WPADSetExtensionCallback(GetChan(), IO_ExtensionConnectCallback);

            ITF_MemSet(&m_PadStatus, 0, sizeof(m_PadStatus));
            ITF_MemSet(m_PadBuffsEX, 0, sizeof(m_PadBuffsEX));
		}
	}

    /*
	 =======================================================================================================================
	 =======================================================================================================================
	 */  

	void WiiPad::UpdateStatus()
	{
		BOOL intr = OSDisableInterrupts(); //disable the interrupts between KPADRead and KMPLSRead
		
		// Connected status
		CallbackConnectStatus &connectStatus = Wii_JoyConnectStatus[GetChan()];
		if(connectStatus.updated)
		{
			if(connectStatus.reason == WPAD_ERR_NO_CONTROLLER)
			{
				ResetExtensionController();
				SetConState(nodevice);
                m_type = WPAD_DEV_NOT_FOUND;
			}
			// Controller connected	
			else if(connectStatus.reason == WPAD_ERR_NONE)
			{
				if(GetConState() != WiiPad::connected)		
                {
                    if(!getIsEnable())
                    {
                        WPADDisconnect(GetChan());
                    }
                    else
                    {
                        SetConnecting();
                    }                    
                }
			}
			connectStatus.updated = false;
		}	
		
		// Extension status
		CallbackExtensionStatus &extensionStatus = Wii_JoyExtensionStatus[GetChan()];
		if(extensionStatus.updated)
		{		
			WiiPad::enExtensionControllers extensionType = extensionStatus.extension;
			if(extensionType != m_eCurrentExtensionType)
			{
				SetCurrentExtensionType(extensionType);
                StopMotor();
			}
			
			if( (extensionType != WiiPad::IO_ExtensionType_None) && 
				(extensionType != WiiPad::IO_ExtensionType_Unknown)
					&& (GetConState() == connecting))
			{
				SetConnecting();
			}
			
			// Be sur Mpls is disable
			if (KPADGetMplsStatus(GetChan()) != WPAD_MPLS_OFF)
			{
				// Mpls seems to be in a not connected state after plugged in
				KPADDisableMpls(GetChan());
			}
			
			extensionStatus.updated = false;
		}	
		
		if (isConnected())
        {
		    s32 err;
		    m_read = KPADReadEx(GetChan(), m_PadBuffsEX, KPAD_MAX_READ_BUFS * 2, &err);		    
		    if(err == KPAD_READ_ERR_NONE && m_read>0)
		    {
			    ComputeKStatus();  
		    }
        }

		if(m_bIsRequired && (m_conState == connecting) && (SYSTEM_ADAPTER->getTime() > m_ValidTime))
		{
			// Connected state
			SetConState(connected);
			
			// RO do not use Pointer
			SetDPDState(SYSTEM_ADAPTER_WII->isInHomeMenu());			
			m_ValidTime = 0;
		}

		if (m_bIsRequired && (m_conState == connected) && (m_type==WPAD_DEV_NOT_FOUND))
		{
			CheckExtensionControllerType();
		}

        if((m_ePreviousExtensionType!=m_eCurrentExtensionType || m_eCurrentExtensionType==IO_ExtensionType_Unknown) && (m_conState == connected) )
            OnExtensionChange();
			
		if (m_bIsRequired)
		{
			UpdateWPADInfo();
			// Check low level
			
			if (GetBatteryState() != m_lastBatteryState)
			{
				m_lastBatteryState = GetBatteryState();
				if(m_lastBatteryState == WII_BatteryState_Low)
				{
					TRC_ADAPTER->addMessage(TRCManagerAdapter::Pad_LowBattery);
				}
			}
		}
				
		OSRestoreInterrupts(intr);
	}

    void WiiPad::OnExtensionChange()
    {
        // If the pad task is not running, do not send message about pad now.
        if(!TRC_ADAPTER->isPadTaskEnable())
            return;

        switch(m_eCurrentExtensionType)
        {
        case WiiPad::IO_ExtensionType_WiiMotionPlusAndNunchuk:
        case WiiPad::IO_ExtensionType_WiiNunchuk:
            INPUT_ADAPTER->setPadType(m_chan, InputAdapter::Pad_WiiNunchuk);
            break;
        case WiiPad::IO_ExtensionType_WiiClassic:
            INPUT_ADAPTER->setPadType(m_chan, InputAdapter::Pad_WiiClassic);
            break;
        case WiiPad::IO_ExtensionType_None:         
        default:
            INPUT_ADAPTER->setPadType(m_chan, InputAdapter::Pad_WiiSideWay);
            break;
        }

        // To test changes of extension
        m_ePreviousExtensionType = m_eCurrentExtensionType;
    }
	
	void WiiPad::CheckExtensionControllerType()
	{		
		s32 res = WPADProbe((s32)GetChan(), &m_type);		
		if (res == WPAD_ERR_BUSY)
		{
			m_eCurrentExtensionType = IO_ExtensionType_Unknown;
			m_type = WPAD_DEV_NOT_FOUND;
			return;
		}	
		
		if (res == WPAD_ERR_NONE)
		{
			switch (m_type){
			case WPAD_DEV_FREESTYLE:
				m_eCurrentExtensionType = IO_ExtensionType_WiiNunchuk;
				break;
			case WPAD_DEV_MPLS:
				m_eCurrentExtensionType = IO_ExtensionType_WiiMotionPlus;
				break;
			case WPAD_DEV_MPLS_FREESTYLE:
				m_eCurrentExtensionType = IO_ExtensionType_WiiMotionPlusAndNunchuk;
				break;
			case WPAD_DEV_CORE:
				m_eCurrentExtensionType = IO_ExtensionType_None;
				break;
			case WPAD_DEV_CLASSIC:
				m_eCurrentExtensionType = IO_ExtensionType_WiiClassic;
			break;
			
			default:
				m_eCurrentExtensionType = IO_ExtensionType_None;
			};
		}
		else // WPAD_ERR_NO_CONTROLLER
		{
			m_eCurrentExtensionType = IO_ExtensionType_None;	
			m_type = WPAD_DEV_NOT_FOUND;
		}  
	}

    static void WPADGetInfo_Callback(s32 chan, s32 result)
	{
		switch (result)
		{	
			case WPAD_ERR_NONE:				
				((InputAdapter_WII*)INPUT_ADAPTER)->m_pads[chan].ValidateWPADInfo(btrue);
				break;
			case WPAD_ERR_NO_CONTROLLER:
				((InputAdapter_WII*)INPUT_ADAPTER)->m_pads[chan].ValidateWPADInfo(bfalse);
				break;
			case WPAD_ERR_BUSY:
			case WPAD_ERR_TRANSFER:
				break;
		}
	}

	inline void WiiPad::UpdateWPADInfo()
	{
 		f64	f64_CurTime	= SYSTEM_ADAPTER->getTime();

		// Check batteries every 5 seconds
		if (!isMotorOn() && (f64_CurTime - m_fLastCheckTime >= 5.f))
		{
			WPADGetInfoAsync(GetChan(), &m_WPADInfo, &WPADGetInfo_Callback);
			m_fLastCheckTime = f64_CurTime;
		}
	}
		
	void WiiPad::UpdateButtons()
	{
		if(isConnected())
		{
            // Check buttons.
            u32 state = m_PadStatus.hold & 0x0000ffff;
            for(u32 button=0 ; button<JOY_MAX_BUT ; ++button)
			{
				if(m_ButtonMapping[button]>=0)
				{	
					bool pressed = (state & 1);
		            bool waspressed = m_buttons[m_ButtonMapping[button]] == InputAdapter::Pressed || 
		                m_buttons[m_ButtonMapping[button]] == InputAdapter::JustPressed;

		            if(pressed && waspressed)
                        m_buttons[m_ButtonMapping[button]] = InputAdapter::Pressed;
                    else if(pressed)
                        m_buttons[m_ButtonMapping[button]] = InputAdapter::JustPressed;
		            else if(waspressed)
                        m_buttons[m_ButtonMapping[button]] = InputAdapter::JustReleased;
		            else
		                m_buttons[m_ButtonMapping[button]] = InputAdapter::Released;
                }
                
                if(state)
				    state >>= 1;
			}
		
		    if (isClassic())
		    {
			    // Check classic buttons.
			    state = m_PadStatus.ex_status.cl.hold & 0x0000ffff;
                for(u32 button=0 ; button<JOY_MAX_BUT ; ++button)
			    {						
				    if(m_ClassicButtonMapping[button]>=0)
				    {	
					    bool pressed = (state & 1);
		                bool waspressed = m_classicButtons[m_ClassicButtonMapping[button]] == InputAdapter::Pressed || 
		                    m_classicButtons[m_ClassicButtonMapping[button]] == InputAdapter::JustPressed;

		                if(pressed && waspressed)
		                    m_classicButtons[m_ClassicButtonMapping[button]] = InputAdapter::Pressed;
		                else if(pressed)
		                    m_classicButtons[m_ClassicButtonMapping[button]] = InputAdapter::JustPressed;
		                else if(waspressed)
		                    m_classicButtons[m_ClassicButtonMapping[button]] = InputAdapter::JustReleased;
		                else
		                    m_classicButtons[m_ClassicButtonMapping[button]] = InputAdapter::Released;
                    }
                
                    if(state)
				        state >>= 1;
			    }
		    }
        }

#ifdef WII_SUPPORT_GAMECUBE_PAD
        //INPUT_ADAPTER_WII->logPadStatus(GetChan());
#endif // WII_SUPPORT_GAMECUBE_PAD
	}

	inline void WiiPad::StartMotor(bbool _bForce)
	{
		if(_bForce || !isMotorOn())
		{
			WPADStartMotor(GetChan());
			m_RumbleState.motorOn = true;
		}
	}
	inline void WiiPad::StopMotor(bbool _bForce)
	{
		if(_bForce || isMotorOn())
		{
			WPADStopMotor(GetChan());
			m_RumbleState.lastEndTime = SYSTEM_ADAPTER->getTime();
			m_RumbleState.motorOn = false;
		}		
	}
	
	inline void WiiPad::UpdateRumble()
	{
		f64 CurTime = SYSTEM_ADAPTER->getTime();
		
		if(CurTime - m_RumbleState.startTime >= m_RumbleState.duringTime		
			 || SYSTEM_ADAPTER_WII->isInHomeMenu() 
			 || GAMEMANAGER->isInPause()
			 || TRC_ADAPTER->isDisplayingError()
			 /*|| RVL_DvdError::bIsDisplayingDvdError()*/)
			StopMotor();
		else		
			StartMotor();		
	}

	inline void WiiPad::RumbleSet(f64 _time)
	{
		f64 CurTime = SYSTEM_ADAPTER->getTime();
		if(isMotorEnable() && 
			m_RumbleState.lastEndTime + WII_RUMBLE_TIMETOWAIT < CurTime)
		{
			m_RumbleState.duringTime 	= _time;
			m_RumbleState.startTime 	= CurTime;
		}
	}

	void WiiPad::Update()
	{
		UpdateStatus();
		UpdateRumble();
		UpdateButtons();
		UpdateDPDState();
	}

	void WiiPad::UpdateDPDState()
	{
		if ((m_DPDStatus.desired != m_DPDStatus.current) &&
			isConnected())
		{
			 if (m_DPDStatus.desired)
		 		KPADEnableDPD(GetChan());
			 else
		 		KPADDisableDPD(GetChan());
		}

        m_DPDStatus.current = WPADIsDpdEnabled(GetChan());
	}

	void WiiPad::ProcessDPDControl(s32 reason)
	{
		if (reason == KPAD_STATE_CTRL_DPD_FINISHED)
			m_DPDStatus.current = WPADIsDpdEnabled(GetChan());
		else
			m_DPDStatus.current = enDPDStatus_CONFIGURING;
	}

	void WiiPad::ProcessMotionPlusControl(s32 reason)
	{
		if ( reason == KPAD_STATE_CTRL_MPLS_FINISHED ) 
		{
			m_bMotionPlusReady = true;
		}
	}
		
	void WiiPad::StartMotionPlus(u8 _Mode)	
	{
		//Start KMPLS lib if not already done
		if(!ms_bMPLSInitialized)
		{
			ms_bMPLSInitialized = true; 
		}
		
		//Start the module and set the good modes
		if(_Mode==IO_MotionPlusMode_Std)
		{
			m_eRequiredExtensionType=IO_ExtensionType_WiiMotionPlus;
		}
		else if(_Mode==IO_MotionPlusMode_Exp)
		{
			m_eRequiredExtensionType=IO_ExtensionType_WiiMotionPlusAndNunchuk;
		}
		else
		{
			ITF_ASSERT_MSG(0, "Unknown mode for motion plus");					
		}
	}

	u8 WiiPad::GetMotionPlusStatus()	
	{
		if(!ms_bMPLSInitialized)
			return IO_MotionPlusMode_Off;
		else
		{
			u8 s=KPADGetMplsStatus(m_chan);
			if(s==WPAD_MPLS_OFF)
				return IO_MotionPlusMode_Off;
			else if(s==WPAD_MPLS_STD)
				return IO_MotionPlusMode_Std;
			else if(s==WPAD_MPLS_EXP)
				return IO_MotionPlusMode_Exp;
			else
				return IO_MotionPlusMode_Off;
		}
	}

	void WiiPad::GetMotionPlusSpeeds(Vec* v)
	{
		v->x=0;
		v->y=0;
		v->z=0;
		for( int i=0;i<m_read;i++)
		{
			v->x+=m_PadBuffsEX[i].mpls.mpls.x/m_read;
			v->y+=m_PadBuffsEX[i].mpls.mpls.y/m_read;
			v->z+=m_PadBuffsEX[i].mpls.mpls.z/m_read;
		}
	}

	void WiiPad::GetMotionPlusAngles(Vec* v)
	{
		v->x=0;
		v->y=0;
		v->z=0;
		for( int i=0;i<m_read;i++)
		{
			v->x+=m_PadBuffsEX[i].mpls.angle.x/m_read;
			v->y+=m_PadBuffsEX[i].mpls.angle.y/m_read;
			v->z+=m_PadBuffsEX[i].mpls.angle.z/m_read;
		}
	}

	void WiiPad::GetMotionPlusDirection(Vec* x, Vec* y, Vec* z)
	{
		*x=m_PadBuffsEX[0].mpls.dir.X;
		*y=m_PadBuffsEX[0].mpls.dir.Y;
		*z=m_PadBuffsEX[0].mpls.dir.Z;
	}

	void WiiPad::ResetMotionPlus()
	{
		if(ms_bMPLSInitialized)
		{
			KPADResetMpls(GetChan());		
		}
	}

    u32 WiiPad::GetType() const
    {
        switch(m_eCurrentExtensionType)
        {
        case WiiPad::IO_ExtensionType_WiiMotionPlusAndNunchuk: return WPAD_DEV_MPLS_FREESTYLE;
        case WiiPad::IO_ExtensionType_WiiNunchuk: return WPAD_DEV_FREESTYLE;
        case WiiPad::IO_ExtensionType_WiiClassic: return WPAD_DEV_CLASSIC;
        case WiiPad::IO_ExtensionType_None: 
        default:
            return WPAD_DEV_CORE;
        }
    }

    InputAdapter::PressStatus WiiPad::getMixedButton( u32 _button ) const
    {
        if(isClassic())
        {
            return (m_buttons[_button] == InputAdapter::Released) ? m_classicButtons[_button] : m_buttons[_button];
        }
        
        return getButton(_button);
    }

} // namespace ITF

