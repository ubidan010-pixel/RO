#ifdef ITF_CTR

#ifndef _ITF_SYSTEMADAPTER_CTR_H_
#define _ITF_SYSTEMADAPTER_CTR_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

namespace ITF
{

//--------------------------------------------------------------------------------------

class SystemAdapter_CTR : public SystemAdapter
{
public:
    bbool   initialize          (   );
    bbool   openGraphics        (   int _width, int _height, int _alphaBits, int _depthBits, int _stencilBits, bbool _fullscreen = bfalse,      bbool _mouseCursor = bfalse, const String& _name = "", bbool _waitVBL=btrue );
    void    terminate           (   );
    void    present             (   );
    f64     getTime             (   );
    f64     getElapsedTime      (   );
    f32     getfPs              (   );
    void    update              (   bbool _minimal  );
    bbool   getWindowSize       (   int& _width, int& _height   );
    void    getTime(Time & _time) const;
    i32     messageBox( const String& _title, const String& _msg, u32 _type,ITF_MESSAGEBOX_RESULT /*silentResult*/);
    f64     getEllaspedFromStart (  );

    virtual bbool isTrialMode();


    ITF_LANGUAGE getSystemLanguage(const ITF_LANGUAGE *_supportedLanguages, u32 _supportedCount, ITF_LANGUAGE _defaultLanguage) const;
    String  userName()const;
    virtual String  dateTime() const;

    const  char* getPlatformName() {return "CTR";}

    void *allocatePhysicalMemory(u32 _size, u32 _alignment);
    void freePhysicalMemory(void *_pointer);

    SystemAdapter_CTR    (   );
    ~SystemAdapter_CTR   (   );

    /// Timer.
    LARGE_INTEGER   GetAdjustedCurrentTime();
    i64 m_lastElapsedTime;
    i64 m_baseTime;
    f64 m_LastFPSTime;
    bbool m_timerStopped;
    u64 m_NumFrames;
    f64 m_fPs;
    bbool m_isFullTitle;

protected:
    void checkLicense();
};

} // namespace ITF


#endif //_ITF_SYSTEMADAPTER_W32DIRECTX_H_

#endif //ITF_WIN32