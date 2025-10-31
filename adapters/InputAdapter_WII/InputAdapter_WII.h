#ifndef _ITF_INPUTADAPTER_WII_H_
#define _ITF_INPUTADAPTER_WII_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_VEC3D_H_
#include "core/math/vec3d.h"
#endif //_ITF_VEC3D_H_

#include <revolution.h>
#include <revolution/wpad.h>
#include <os.h>
#include <kpad.h>
#include <wenc.h>

namespace ITF
{
	#define WII_STICK_THRESHOLD      48

	#define WII_STICK_UP             0x1000
	#define WII_STICK_DOWN           0x2000
	#define WII_STICK_LEFT           0x4000
	#define WII_STICK_RIGHT          0x8000
	#define WII_SUBSTICK_UP          0x0100
	#define WII_SUBSTICK_DOWN        0x0200
	#define WII_SUBSTICK_LEFT        0x0400
	#define WII_SUBSTICK_RIGHT       0x0800
    
	// battery states 
	#define WII_BatteryState_Unknown	0x00000001
	#define WII_BatteryState_Low		0x00010002
	#define WII_BatteryState_High		0x00010003

	// Don't read pad data for some time
	#define WII_WAIT_TIME_AFTER_CONNECT		0.50
	
	// Speaker alarm code
	#define WII_SPEAKER_SAMPLES_PER_AUDIO_PACKET	40
	#define WII_SPEAKER_AUDIO_PACKET_MAX_LEN		20

	// Do not start and stop the rumble too often
	#define WII_RUMBLE_TIMETOWAIT			0.05
	#define WII_DPD_TIMETOWAIT				1.50
	
	#define KPAD_BUF_SIZE   KPAD_MAX_READ_BUFS

    #define MAX_PAD         WPAD_MAX_CONTROLLERS
	
	//#define WII_SUPPORT_GAMECUBE_PAD	
	
	class WiiSpeaker
	{
	public:
		WiiSpeaker() : 
			_isAlarmOn(bfalse) {}
		
		typedef enum eState
		{
			State_OFF,
			State_TURNING_ON,
			State_PLAY_START,
			State_PLAY_FIRST,
			State_PLAYING,
			State_TURNING_OFF,
			State_HOME_MENU
		} eState;

		typedef enum eMuteState
		{
		    Mute_OFF,
		    Mute_IN_PROGRESS,
		    Mute_ON
		} eMuteState;

		typedef struct	Info
		{
			WENCInfo			encInfo;
			volatile u32        u_UserAlive;
			volatile u32        u_MuteState; // mute = off / in progress / on
			volatile eState		state;
		} Info;

		// Periodic Alarms for audio streaming
		static OSAlarm	SpeakerAlarm;

		// Audio buffers 
		static s16		AudioBuffer[MAX_PAD][WII_SPEAKER_SAMPLES_PER_AUDIO_PACKET];

	
		static void Update(OSAlarm *alarm, OSContext *context);
		
		void ProcessStart();
		void ProcessStop();
		void HomeButtonStart();
		void HomeButtonFinish();
		
		void TurnOn(s32 _chan);
		void TurnOff(s32 _chan);
		void WaitForShutdown(s32 _chan);
		
		void Connect(s32 _chan);
		void Disconnect(s32 _chan);


		//Callback
		static void OffCallback(s32 _chan, s32 _result);		//TurnOff
		static void OnCallback(s32 _chan, s32 _result);		//TurnOn
		static void PlayCallback(s32 _chan, s32 _result);				
		static void MuteOnCallback(s32 _chan, s32 _result);
		static void MuteOffCallback(s32 _chan, s32 _result);
		
		
		static Info	_info[MAX_PAD];
	private:
		
		
		bbool	_isAlarmOn;
	};
	

	typedef struct
	{
	    // contains PADStatus structure
	    PADStatus   pst;

	    // extended field
	    u16         buttonDown;
	    u16         buttonUp;
	    u16         dirs;
	    u16         dirsNew;
	    u16         dirsReleased;
	    s16         stickDeltaX;
	    s16         stickDeltaY;
	    s16         substickDeltaX;
	    s16         substickDeltaY;
	} WiiPadStatus;    


	typedef struct 
	{
		f32 fPointerRef[3];
		f32 fPointer[3];
	} stWiimoteDPDExt ;
    

	class	WiiPad
	{
	public:
		/* Connection State */
		enum eConState { nodevice, connected, connecting, disconnecting };
		// Extension Controllers
		enum enExtensionControllers
		{
			IO_ExtensionType_None	   = 0,
			IO_ExtensionType_Unknown,
            IO_ExtensionType_Unset,
			IO_ExtensionType_WiiNunchuk,
			IO_ExtensionType_WiiClassic,
			IO_ExtensionType_WiiMotionPlus,
			IO_ExtensionType_WiiMotionPlusAndNunchuk,
			IO_ExtensionType_MAX
		};

		// Motion plus modes
		enum enMotionPlusMode
		{
			IO_MotionPlusMode_Off = 0,
			IO_MotionPlusMode_Std,
			IO_MotionPlusMode_Exp
		};

		enum enDPDStatus
		{
			enDPDStatus_OFF 			= 0,
			enDPDStatus_ON,
			enDPDStatus_CONFIGURING,
			enDPDStatus_UNSET			// use to force update
		};

		typedef struct	RumbleState_
		{			
			f64 	duringTime; 
			f64 	startTime; 
			f64		lastEndTime;
			bool	motorOn;
		} RumbleState;
		
		typedef struct stDPDStatus_
		{
			u32		current;
			u32 	desired;
			u32 	desiredBackup;
		} stDPDStatus;
	
		WiiPad();
		~WiiPad();		
		void		Init(int _chan);
		void		Close();
		void		Update();
	
		ITF_INLINE i32 GetChan() const { return m_chan; }
		u32 GetType() const ;



		void		EnableAimingMode(bbool _enable);
		
		
		void		RumbleSet(f64 _time);
		void		StartMotor(bbool _bForce = bfalse);
		void		StopMotor(bbool _bForce = bfalse);


        static ITF_INLINE u8 	u8_GetSensorBarPosition()
        {
            return WiiPad::mu8_SensorBarPosition;
        }

        static ITF_INLINE void  PointerShiftSet(bbool _b_IsSensorBarTop, f32 _f32_Shift)
        {
            WiiPad::mf32_PointerShift[!_b_IsSensorBarTop] = _f32_Shift;
        }

        static ITF_INLINE f32   f32_PointerShiftGet(bbool _b_IsSensorBarTop)
        {
            return WiiPad::mf32_PointerShift[!_b_IsSensorBarTop];
        }

		ITF_INLINE bool isMotorEnable() const { return WPADIsMotorEnabled(); }
		ITF_INLINE bool isMotorOn() const { return m_RumbleState.motorOn;}
		ITF_INLINE eConState GetConState() const	{		return m_conState;	}
		void            SetConState(eConState _eConState);
		void            SetConnecting();
	
		ITF_INLINE enExtensionControllers GetCurrentExtensionType() const
		{
			return m_eCurrentExtensionType;
		}

		ITF_INLINE void SetCurrentExtensionType(enExtensionControllers _eType)
		{
			// on a transition M+ -> no M+ , disable the motion plus detection 
			if( 	(		m_eCurrentExtensionType == IO_ExtensionType_WiiMotionPlus 
						|| 	m_eCurrentExtensionType == IO_ExtensionType_WiiMotionPlusAndNunchuk) 
				&& !(		_eType 					== IO_ExtensionType_WiiMotionPlus 
						||  _eType 					== IO_ExtensionType_WiiMotionPlusAndNunchuk) )
			{
				StopMotionPlus();
			}
			
			m_eCurrentExtensionType = _eType;
		}

		ITF_INLINE void CpyKStatus(KPADStatus* dest) 
		{
			ITF_ASSERT(dest);
			ITF_Memcpy(dest, &m_PadStatus, sizeof(KPADStatus));
		}
		
		ITF_INLINE const KPADStatus &GetKStatus() const
		{
			return m_PadStatus;
		}


		ITF_INLINE const stWiimoteDPDExt &GetDPDExt() const
		{
			return m_DPDExt;
		}

		ITF_INLINE KPADStatus *pGetKBuff(int _iSample)
		{
			return m_PadBuffsEX + _iSample;
		}
	
		ITF_INLINE void ResetExtensionController()
		{
			// extension controller callback
			WPADSetExtensionCallback(m_chan, NULL);
            m_eCurrentExtensionType = IO_ExtensionType_Unknown;
			if(m_bMotionPlusEnable || m_bMotionPlusReady)
			{
				KPADDisableMpls(m_chan);
			}
		}
		ITF_INLINE bool isClassic() const { return GetCurrentExtensionType() == IO_ExtensionType_WiiClassic; }
        ITF_INLINE bool isNunchuk() const { return GetCurrentExtensionType() == IO_ExtensionType_WiiNunchuk 
            || GetCurrentExtensionType() == IO_ExtensionType_WiiMotionPlusAndNunchuk; }
        ITF_INLINE bool isAlone() const { return GetCurrentExtensionType() == IO_ExtensionType_None; }

        ITF_INLINE bool isConnected() const { return m_conState == connected; }

		void CheckExtensionControllerType();
	
		ITF_INLINE int GetSampleNb() const	{return m_read;	}	
		ITF_INLINE i32 GetBatteryState() const {return (m_bWPADInfoValid ? (m_WPADInfo.lowBat ? WII_BatteryState_Low : WII_BatteryState_High) : WII_BatteryState_Unknown);	}	
		ITF_INLINE void ValidateWPADInfo(bbool _bValid){m_bWPADInfoValid = (_bValid ? 1 : 0);}
	
		ITF_INLINE void SetDPDState (u32 _bOnOff)
        {
            m_DPDStatus.desired = _bOnOff ? enDPDStatus_ON : enDPDStatus_OFF;
        }
        ITF_INLINE u32  GetDPDState() const
        {
            return m_DPDStatus.current;
        }
		void UpdateDPDState();
		ITF_INLINE void ForceDPDUpdate()
        {
            m_DPDStatus.current = enDPDStatus_UNSET;
        }
		ITF_INLINE void SaveDPDState()
        {
            m_DPDStatus.desiredBackup = m_DPDStatus.desired;
        }
		ITF_INLINE void RestoreDPDState()
        {
            m_DPDStatus.desired = m_DPDStatus.desiredBackup;
        }
		void ProcessDPDControl(s32 reason);
		void ProcessMotionPlusControl(s32 reason);

		ITF_INLINE void SetRequired(i32 _bRequired)
        {
            m_bIsRequired = _bRequired;
        }
		ITF_INLINE int  bIsRequired() const
        {
            return m_bIsRequired;
        }

		ITF_INLINE void SetRequiredExtension(enExtensionControllers _eRequired)
        {
            m_eRequiredExtensionType = _eRequired;
        }
		enExtensionControllers eRequiredExtension();
		ITF_INLINE bool bIsStickConnected() const { return m_WPADInfo.attach&&m_eCurrentExtensionType!=IO_ExtensionType_None;}
		
		
		/*===================================================================================================================
			WII MOTION PLUS
		 ===================================================================================================================*/		 	 	 
		void StartMotionPlus(u8 _Mode);
		ITF_INLINE void StopMotionPlus()	
		{
			KPADDisableMpls(m_chan);
			m_bMotionPlusEnable = bfalse;	
		}
		
		u8 GetMotionPlusStatus();
		void GetMotionPlusSpeeds(Vec* v);
		void GetMotionPlusAngles(Vec* v);
		void GetMotionPlusDirection(Vec* x, Vec* y, Vec* z);
		void ResetMotionPlus();
		static ITF_INLINE bool IsMotionPlusInitialized() 
		{
			return ms_bMPLSInitialized;
		}
		ITF_INLINE bool MplsCheck() { return m_bMotionPlusReady || m_bMotionPlusEnable; }
		ITF_INLINE bool MplsActive() { return m_bMotionPlusReady && m_bMotionPlusEnable; }

		ITF_INLINE bool isJustPressed   (u32 _button)   const
        {
            return (m_buttons[_button] == InputAdapter::JustPressed || m_classicButtons[_button] == InputAdapter::JustPressed);
        }
		ITF_INLINE bool isPressed       (u32 _button)   const
        {
            return (m_buttons[_button] == InputAdapter::Pressed || m_buttons[_button] == InputAdapter::Double_Press)
                || (m_classicButtons[_button] == InputAdapter::Pressed || m_classicButtons[_button] == InputAdapter::Double_Press);
        }

		InputAdapter::PressStatus	getButton(u32 _button) const { return m_buttons[_button]; }
		InputAdapter::PressStatus	getClassicButton(u32 _button) const { return m_classicButtons[_button]; }
		InputAdapter::PressStatus	getMixedButton(u32 _button) const;
		
        ITF_INLINE void setIsEnable(bbool _isEnable) { m_isEnable = _isEnable; }
        ITF_INLINE bbool getIsEnable() const { return m_isEnable; }


	private:
		void	UpdateStatus();
		void	UpdateWPADInfo();
		void	UpdateRumble();
		void	UpdateButtons();
		void    ComputeKStatus();
        void    OnExtensionChange();

		i32			m_chan;				//!< WPad chan id

		// WiiMote
		i32			m_ButtonMapping[JOY_MAX_BUT];
		InputAdapter::PressStatus	m_buttons[JOY_MAX_BUT];
		
		// Classic controler
		i32			m_ClassicButtonMapping[JOY_MAX_BUT];		
		InputAdapter::PressStatus	m_classicButtons[JOY_MAX_BUT];
		

		/* Kpadread and Kpadprobe return values */
		unsigned long	m_type;
		s32			m_result;
		s32			m_read;
		KPADStatus	m_PadStatus;		//!< current state
		stDPDStatus	m_DPDStatus;

		//-----------------------------------------------------------------
		u8			m_bIsRequired;    		//!< This pad is a required one
		//-----------------------------------------------------------------

		enExtensionControllers 	m_eRequiredExtensionType; 	//!< Ths pads requires a extension
		enExtensionControllers 	m_eCurrentExtensionType;
        enExtensionControllers 	m_ePreviousExtensionType;
		
		//!
		// *  kpad device direct data buffers
		//KPADStatus	m_PadBuffs[KPAD_MAX_READ_BUFS];
		KPADStatus	m_PadBuffsEX[KPAD_MAX_READ_BUFS*2];
		
        bbool       m_isEnable;
		double		m_ValidTime;
		eConState	m_conState;
		RumbleState m_RumbleState;
		
		
		//!
		// * Static buffer used by the KMPLS library
		static bool ms_bMPLSInitialized;
		stWiimoteDPDExt	m_DPDExt; // if wm+ is enabled, extrapolation of dpd pointer
		static u8				mu8_SensorBarPosition;
		static f32				mf32_PointerShift[2];
		
		// Info updated by WPADGetInfo.
		WPADInfo				m_WPADInfo;
		u8	 					m_bWPADInfoValid;
		i32						m_lastBatteryState;
		f64						m_fLastCheckTime;

		// *  WiiMotionPlus status
		KPADMPStatus			m_MotionPlusStatus;
		KPADUnifiedWpadStatus	m_UWpadStatusBuffer[KPAD_MAX_READ_BUFS];
		bool					m_bMotionPlusReady;
		bool					m_bMotionPlusEnable;
		
		// * AIM mode feature
		f64						m_lastAIMChanging;
		bbool					m_needAIMToChange;
	};


	// Callback struct management
	typedef struct 
	{		
		volatile bool	updated;
		s32				reason; 
	} CallbackConnectStatus;


	typedef struct 
	{		
		volatile bool	updated;
		WiiPad::enExtensionControllers	extension; 
	} CallbackExtensionStatus;
	
	
	

    class InputAdapter_WII : public InputAdapter
    {
    private:
        static const u32        maxPad = MAX_PAD;
        u32                     getConnectedPadIndex (u32 _padIndex) const;
    public:
                                InputAdapter_WII (    );
                                ~InputAdapter_WII(    );

        void                    getGamePadPos       (  u32 _environment, u32 _pad, float *_pos, u32 _numAxes ) const;
        void                    getGamePadButtons   (  u32 _environment , u32 _pad, PressStatus* _buttons, u32 _numButtons ) const;
        u32                     getGamePadCount     (    );

        void                    updateAllInputState();
        
        // Specific behavior when home menu start/stop
        void					homeMenuStart();
        void					homeMenuStop();

        void                    updateMouse();

        void                    mayInitAPI();
		void					closeAPI();

		
        void                    getWiimoteAcc(u32 _pad, Vec3d* _acc);
		void					getPointer(u32 _pad, Vec3d* _pointer);// Z axis is the Dist from screen
		void					getExtensivePointer(u32 _pad, Vec3d* _pointer);// Z axis is the Dist from screen
		i32						getValidDPD(u32 _pad);
		
        void		            startRumble(u32 _numPad, f64 _time, f32 _leftMotorSpeed, f32 _rightMotorSpeed);
        void                    stopRumble(u32 _numPad);
		void					pauseAllRumble(bbool _pause);

        void                    setIsPadEnable(u32 _numPad, bbool _enable);
        bbool                   getIsPadEnable(u32 _numPad) const;

        void                    onHBMHasReassignedControllers();

#ifdef WII_SUPPORT_GAMECUBE_PAD
    	void 					wpadInit();
    	void					updatePadStatus();
		void		  			logPadStatus(u32 _pad = 0) const;
		
		
		PADStatus    			m_gcpads[maxPad]; // internal use only		
		WiiPadStatus       		m_gcpad[maxPad];		
		u32 					PadChanMask[PAD_MAX_CONTROLLERS] ;
		bool					m_virtualPadStatus[JOY_MAX_BUT];
		bool					m_oldVirtualPadStatus[JOY_MAX_BUT];
        f32                     m_virtualPadAxes[JOY_MAX_AXES];
        
		void 					padCopy( PADStatus* pad, WiiPadStatus* dmpad );
		i32						m_wpadMapping[JOY_MAX_BUT];
#endif // WII_SUPPORT_GAMECUBE_PAD

		WiiPad					m_pads[maxPad];

    private:
        WiiSpeaker				m_speaker;
		
		s32 					wpad_state;
		u32                 	m_numValidPads;
		KPADUnifiedWpadStatus	m_KPADunifiedStatus[maxPad*KPAD_BUF_SIZE];

		bbool					m_pauseRumble;
    };

	void IO_JOystickWiimoteComputeExtensivePointer(stWiimoteDPDExt *_pExtPointer,i32 _i32_JoyIndex, bbool _bCondition);

	#define INPUT_ADAPTER_WII		(static_cast< InputAdapter_WII* > (INPUT_ADAPTER))

} // namespace ITF

#endif //_ITF_INPUTADAPTER_WII_H_
