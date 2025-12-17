#ifndef _ITF_VIDEOHANDLE_H_
#define _ITF_VIDEOHANDLE_H_

#include "core/types.h"

#include "core/itfstring.h"
#include "engine/AdaptersInterfaces/ImGui/IDebugImgui.h"

namespace  ITF
{
    class  videoHandleBase: IDebugImGui
    {
    public:

        videoHandleBase() = default;

        const bbool isLoop() const {return m_loop;}
        const bbool isStopped() const {return m_stopped;}
        const bbool isOpened() const { return m_opened;}
        const bbool isPlayedFromMemory() const {return m_playFromMemory;}
        void setPlayedFromMemory(bbool _bPlayFromMemory) {m_playFromMemory = _bPlayFromMemory;}
        
        void stop() {m_stopped = btrue;}
        ITF_INLINE bbool isPaused() const { return m_isPaused; }

        void setAlpha(f32 _alpha)               {m_alpha = _alpha;}
        const f32 getAlpha()         const      {return m_alpha;}

        const f32 getTotalTime()     const      {return m_totalTimeInSeconds;}
 
    protected:

        bbool   m_opened = bfalse;
        bbool   m_loop = bfalse;
        bbool   m_stopped = btrue;
        bbool   m_isPaused = bfalse;
        f32     m_alpha = 1.0f;
        bbool   m_playFromMemory = bfalse;

        f32     m_fps = 1.0f;
        f32     m_totalTimeInSeconds = 0.0f;

        f64     m_playbackTimeSeconds = 0.0;
        bbool   m_useExternalClock = bfalse;
        f64     m_externalClockMs = 0.0;

        void    resetPlaybackClock();
        void    setExternalClockMs(f64 ms);
        void    clearExternalClock();
        void    tickPlaybackClock();
        f64     getPlaybackTimeSeconds() const { return m_playbackTimeSeconds; }

#ifdef ITF_SUPPORT_IMGUI
    public:
        virtual void registerToImGui() override;
        virtual void unRegisterToImGui() override;
        virtual void displayImGui() override;

    protected:
        f32 m_lastDecodeTimeMs = 0.0f;
        f32 m_avgDecodeTimeMs = 0.0f;
        f32 m_maxDecodeTimeMs = 0.0f;
        u32 m_decodeFrameCount = 0;
        String m_lastOpenedFile;
        f64 m_openedAtSeconds = 0.0;
        void onVideoOpened(const String& _fullFilename);
        void onVideoClosed();
#endif // ITF_SUPPORT_IMGUI
    };
}


#ifdef ITF_SUPPORT_BINK

    #ifdef ITF_X360
        #include "engine/video/videoHandle_X360.h"
    #elif defined(ITF_WINDOWS)
        #include "engine/video/videoHandle_WIN.h"
    #elif defined(ITF_PS3)
        #include "engine/video/videoHandle_PS3.h"
    #elif defined(ITF_WII)
        #include "engine/video/videoHandle_WII.h"
    #else
        #error "Platform not supported"
    #endif
#elif defined(ITF_XBOX_SERIES) || defined(ITF_NINTENDO) || defined(ITF_PS5)
    #include "engine/video/videoHandle_sw.h"
#else
//not supported

namespace ITF
{
    class videoHandle : public videoHandleBase
    {
    public:

        void destroy();
        void update() {};
        void play();
        void pause() {}
        void close() {};

        bbool open(const String& _filename) {return btrue;}
        void stop() {};
        void render() {};

        bbool getCurrentTime(f32& _timeInSeconds) {return bfalse;};

        void OnResetDevice() {}
        void OnLostDevice() {}
    };

}

#endif

#endif //_ITF_VIDEOHANDLE_H_
