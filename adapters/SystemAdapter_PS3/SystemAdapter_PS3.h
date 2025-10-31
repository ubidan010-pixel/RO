#ifndef _ITF_SYSTEMADAPTER_PS3_H_
#define _ITF_SYSTEMADAPTER_PS3_H_

#ifdef ITF_PS3

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#include <cell/spurs/types.h>

namespace ITF
{

namespace Purs
{
    class SpursInstance;
}

class SystemAdapter_PS3 : public SystemAdapter
{
public:
    bbool   initialize          (   );
    bbool   openGraphics        (   int _width, int _height, int _alphaBits, int _depthBits, int _stencilBits, bbool _fullscreen = bfalse, bbool _mouseCursor = bfalse, const String& _name = "", bbool _waitVBL=btrue );
    void    terminate           (   );
    void    present             (   );
    f64     getTime             (   );
    f64     getElapsedTime      (   );
    f32     getfPs              (   );
    void    update              (   bbool _minimal  );
    bbool   getWindowSize       (   int& _width, int& _height   );

    void    timerStart          (   );
    void    timerStop           (   );

    String  userName()const;
    virtual String  dateTime() const;
    void    getTime(Time & _time) const;

    void setGameTitle(const char* _gameTitle, u32 _gameTitleSize) ;
    void setTitleID(const char* _titleID, u32 _titleIDSize) ;
    void setBootType(u32 _bootType, u32 _bootAttributes, bool _fromAppHome, const char * m_bootPath);

    virtual bbool isHDDMode() const;
    virtual ITF_TERRITORY getSystemTerritory()  const;

    virtual bbool isBackAndSelectButtonsInverted() const { return m_invertedAssignButtons; }

    bbool isLaunchedFromAppHome() const { return m_launchedFromAppHome; }

    const char* getGameTitle() const { return m_gameTitle; }
    const char* getTitleID() const { return m_titleID; }
    const char* getBootPath() const { return m_bootPath; }

    const char* getPlatformName() {return "PS3";}

    SystemAdapter_PS3  (   );
    ~SystemAdapter_PS3   (   );


    void    queryPerformanceFrequency(LARGE_INTEGER *TicksPerSecond);
    void    queryPerformanceCounter(LARGE_INTEGER *count);

    LARGE_INTEGER   GetAdjustedCurrentTime();

    i32 messageBox( const String& _title, const String& _msg, u32 _type, ITF_MESSAGEBOX_RESULT silentResult);

    virtual ITF_LANGUAGE getSystemLanguage(const ITF_LANGUAGE * /*_supportedLanguages*/, u32 /*_supportedCount*/, ITF_LANGUAGE /*_defaultLanguage*/) const;

    CellSpurs * getMainSpurs() const;
    // the secondary one only has 1 spu assigned to it.
    // Right now it's used for file decompression (note that decompression tasks take several milliseconds)
    CellSpurs * getSecondarySpurs() const;

    enum NetworkServiceProduct  //must match the same order NetworkServices_PS3::commid /s_communication_sig
    {
        Product_Standard = 0,
        Product_Japanese = 1,
        Product_PolishRussian = 2
    };

    const NetworkServiceProduct getNetworkServiceProduct() const {return m_NetworkServiceProduct;}

protected:
    i64 m_QPFTicksPerSec;
    i64 m_stopTime;
    i64 m_lastElapsedTime;
    i64 m_baseTime;
    f64 m_LastFPSTime;
    bbool m_timerStopped;
    u64 m_NumFrames;
    f64 m_fPs;

    bbool m_invertedAssignButtons;

    // From application_framework
    char*       m_gameTitle;
    char*       m_titleID;
    u32         m_bootType;
    u32         m_bootAttributes;
    bool        m_launchedFromAppHome;
    char*       m_bootPath;

    NetworkServiceProduct m_NetworkServiceProduct;
    

    Purs::SpursInstance * m_mainSpurs;
    Purs::SpursInstance * m_secondarySpurs;
};
#define SYSTEM_ADAPTER_PS3		(static_cast< SystemAdapter_PS3* > (SYSTEM_ADAPTER))

} // namespace ITF

#endif //ITF_PS3

#endif //_ITF_SYSTEMADAPTER_PS3_H_