#ifndef _ITF_VIDEOHANDLE_SW_H_
#define _ITF_VIDEOHANDLE_SW_H_

#if defined(ITF_XBOX_SERIES) || defined(ITF_NINTENDO) || defined(ITF_PS5)

#include "engine/video/videoHandle.h"
#include "engine/video/webmDecoder.h"
#include "engine/resources/ResourceManager.h"

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <vector>

namespace ITF
{
    class videoHandle : public videoHandleBase
    {
    public:
        videoHandle();
        ~videoHandle();

        void    update();
        void    play();
        void    pause();
        void    close();

        bbool   open(const String& _filename);
        void    stop();
        void    render();

        bbool   getCurrentTime(f32& _timeInSeconds);

    private:
        struct CpuYuvFrame
        {
            ITF_VECTOR<u8>  yPlane;
            ITF_VECTOR<u8>  uPlane;
            ITF_VECTOR<u8>  vPlane;

            u32 width       = 0;
            u32 height      = 0;
            u32 yStride     = 0;
            u32 uvStride    = 0;
            f64 pts         = 0.0;
        };

        std::thread                 m_decodeThread;
        std::atomic<bool>           m_decodeThreadRunning{ false };
        std::mutex                  m_queueMutex;
        std::condition_variable     m_queueCv;
        ITF_DEQUE<int>              m_frameQueue;
        size_t                      m_maxQueuedFrames = 3;
        ITF_VECTOR<CpuYuvFrame>     m_framePool;
        int                         m_nextPoolIndex = 0;

        void                        decodeThreadFunc();
        bbool                       pushDecodedFrame(const vpx_image_t* img, f64 pts);
        bbool                       warmupFirstFrame();

        void                        destroyTexturesAndVB();
        bbool                       createTexturesAndVB(u32 _width, u32 _height);
        bbool                       uploadFrameToTextures(const CpuYuvFrame& _img, u32 _texIndex);

        u32                         m_textureWidth  = 0;
        u32                         m_textureHeight = 0;

        ResourceID                  m_texY[2];
        ResourceID                  m_texU[2];
        ResourceID                  m_texV[2];

        ITF_VertexBuffer*           m_pVertexBuffer = nullptr;
        u32                         m_currentTexIndex = 0;   // 0 / 1

        webmDecoder                 m_decoder;
        f64                         m_bufferPts[2] = { 0.0, 0.0 };
        f32                         m_currentTimeInSeconds = 0.0;
    };

} // namespace ITF

#endif

#endif // _ITF_VIDEOHANDLE_SW_H_
