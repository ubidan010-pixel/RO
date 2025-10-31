#ifdef ITF_WII

#ifndef _ITF_SYSTEMADAPTER_WII_H_
#define _ITF_SYSTEMADAPTER_WII_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_SYSTEMADAPTER_HOMEBUTTON_WII_H_
#include "adapters/SystemAdapter_WII/SystemAdapter_HomeButton_WII.h"
#endif //_ITF_SYSTEMADAPTER_HOMEBUTTON_WII_H_

#ifndef _ITF_SYSTEMADAPTER_SPLASHSCREEN_WII_H_
#include "adapters/SystemAdapter_WII/SystemAdapter_Splashscreen_WII.h"
#endif //_ITF_SYSTEMADAPTER_SPLASHSCREEN_WII_H_

namespace ITF
{

//--------------------------------------------------------------------------------------
class SystemAdapter_WII : public SystemAdapter
{
public:
    bbool   initialize          (   );
    bbool   openGraphics        (   int _width, int _height, int _alphaBits, int _depthBits, int _stencilBits, bbool _fullscreen = bfalse,      bbool _mouseCursor = bfalse, const String& _name = "", bbool _waitVBL=btrue );
    void    terminate           (   );
    void    present             (   );

    void    timerStart          (   );
    void    timerStop           (   );
    f64     getTime             (   );
    void    getTime             (Time & _time) const;
    f64     getEllaspedFromStart(  );
    f64     getElapsedTime      (   );
    f32     getfPs              (   );
#ifndef ITF_FINAL
    f32     getfPsNoVsync       (   ) { return m_fPs_NoVsync; }
#endif // ITF_FINAL

    void    update              (   bbool _minimal  );
    bbool   gameUpdateAllowed   (   );
    bbool   getWindowSize       (   int& _width, int& _height   );
	i32		messageBox( const String& _title, const String& _msg, u32 _type, ITF_MESSAGEBOX_RESULT _result);

    virtual bbool isTrialMode();

    ITF_LANGUAGE getSystemLanguage(const ITF_LANGUAGE *_supportedLanguages, u32 _supportedCount, ITF_LANGUAGE _defaultLanguage) const;
    String  userName()const;
    virtual String  dateTime() const;

    const  char* getPlatformName() {return "WII";}

    void *allocatePhysicalMemory(u32 _size, u32 _alignment);
    void freePhysicalMemory(void *_pointer);

    SystemAdapter_WII    (   );
    ~SystemAdapter_WII   (   );

    /// Timer.
    LARGE_INTEGER   GetAdjustedCurrentTime();
    i64 m_stopTime;
    i64 m_lastElapsedTime;
    i64 m_baseTime;
    f64 m_LastFPSTime;
    bbool m_timerStopped;
    u64 m_NumFrames;
    f64 m_fPs;
#ifndef ITF_FINAL
    f64 m_VBLSyncTime;
    f64 m_fPs_NoVsync;
#endif // ITF_FINAL

    bbool m_isFullTitle;
    
    // Home button management 
    void updateHomeButton(); 
    void renderHomeButtonIcons();
    void initHomeButton();
    bbool disableHomeButton();
    bbool enableHomeButton();
	bbool isInHomeMenu() const; 

    void handleEpilepsyScreen();

    // Sound mode
    enum SoundMode
    {
        SoundMode_Unknown=0,
        SoundMode_Mono,
        SoundMode_Stereo,
        SoundMode_Surround
    };
    SoundMode    getSoundModeFromWiiSettings() const; 

    // SplashScreen component
    SystemAdapter_SplashScreen  m_splashScreen;
    void updateSplashScreen();
    void renderSplashScreen();
    void initSplashScreen();
    void pauseSplashScreen();
    void unpauseSplashScreen();

    bbool isInSplashScreen();

    void    queryPerformanceFrequency(LARGE_INTEGER *TicksPerSecond);
    void    queryPerformanceCounter(LARGE_INTEGER *counter);


protected:
    void checkLicense();
    
private:
	SystemAdapter_HomeButton	_HomeButton;
};

#define SYSTEM_ADAPTER_WII		(static_cast< SystemAdapter_WII* > (SYSTEM_ADAPTER))


} // namespace ITF


#endif //_ITF_SYSTEMADAPTER_WII_H_

#endif //ITF_WII