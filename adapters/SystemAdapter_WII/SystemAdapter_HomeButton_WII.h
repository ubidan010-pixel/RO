#ifdef ITF_WII

#ifndef _ITF_SYSTEMADAPTER_HOMEBUTTON_WII_H_
#define _ITF_SYSTEMADAPTER_HOMEBUTTON_WII_H_

#ifndef TPL_H
#include <revolution/tpl.h>
#endif //TPL_H

namespace ITF
{

	#include <revolution/vi.h>
	#include <revolution/wpad.h>
	#include <revolution.h>
	#include <revolution/kpad.h>
	#include <revolution/mem.h>
	#include <revolution/hbm.h>
	#include <revolution/sc.h>

	typedef u32  (*SystemAdapter_HomeButton_PreCallback_Fn)(void);
	typedef void (*SystemAdapter_HomeButton_PostCallback_Fn)(u32);
	
	#define WII_HOMEBUTTON_PREFIX 				""
	#define WII_HOMEBUTTON_SOUND_PREFIX 		""
	#define WII_HOMEBUTTON_HBMDIR				"HomeButton2"	
	#define WII_HOMEBUTTON_FADEIN				0.25
	#define WII_HOMEBUTTON_FADEOUT				0.25
	#define WII_HOMEBUTTON_SHOWTIME				1.25
		
	class SystemAdapter_HomeButton
	{
	  public:
	  	SystemAdapter_HomeButton();
		~SystemAdapter_HomeButton();

		enum LeaveMode
		{
			LeaveWithoutWarning = 0,
			LeaveWithSaveWarning
		};
		
		void SetLeaveMode(LeaveMode mode) { _leaveMode = mode; }
		void InitHomeMenu (); 	// This should be called once during game launch
		void DestroyHomeMenu(); // This frees the memory , should be called when exiting the game only

		// is the HBM open ?
		bbool	bIsInHome () const; 
		
		// Check if we should enter home menu
		void 	ProcessHomeMenu(); 
		void 	HandleProhibited();
		void	ShowProhibited();
        void    ActiveProhibitedIcon();
		
		// These functions prevents or allows the user from opening the HBM
		// IMPORTANT : this function is incremental. This means that if you call SetProhibited twice, you'll have to call SetEnabled twice as well !		
		void	SetProhibited (bbool = btrue) ;
		void	SetAllowed () ;		
		
		void	ForceProhibitedExit ();		  	
	  	bbool	bIsProhibited () const;
		u32		bIsAnyHomeJustPressed();	
        ITF_INLINE bbool bIsInitialized() const { return _isHomeInitialized; }

	  private:
		bbool		_inHome;
		bbool		_isHomeInitialized;
		bbool		_isRenderedOnce;
		u32			_isHomeProhibited;
		LeaveMode	_leaveMode;
		String		_Root;
        f32         _IconAlpha;
		HBMDataInfo _hbmInfo;
		HBMControllerData _conData;
		void* 		m_soundArchive;
		void* 		m_soundBuffer;
		bbool		m_homeButtonPressed[WPAD_MAX_CONTROLLERS];

		ITF_THREAD_CRITICAL_SECTION _CriticalSection;
		
		//internal mangement
	  	void Close();
	  	void Open();		
		u32	 Update();
		void Loop();			
		bbool InitHomeButtonInfo ();
		void CreateHomeMenu();
		void DeleteHomeMenu();
		bbool copyHBMtoTMP();
		void InitSound();
		void UpdatePad();
		void Render();
			
		//helpers
		void InitProjection(u32 flag);
		void InitControllerData();
		void SetAdjustValue();
		void DrawBanIcon(u8 alpha);
		
		//Icon manager
		struct HomeIcon
		{
			TPLPalettePtr	tpl;
			GXTexObj 		texObj;
			double			showStartTime;
			double			showEndTime;
			bbool			enable;
		};
		
		HomeIcon	_icon;
	};
} // namespace ITF


#endif //_ITF_SYSTEMADAPTER_HOMEBUTTON_WII_H_

#endif //ITF_WII