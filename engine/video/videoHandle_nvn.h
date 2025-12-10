#ifndef _ITF_VIDEOHANDLE_NVN_H_
#define _ITF_VIDEOHANDLE_NVN_H_

#ifdef ITF_NINTENDO

#include "engine/video/videoHandle.h"
#include "engine/video/webmDecoder.h"
#include "engine/resources/ResourceManager.h"
#include <thread>

namespace ITF
{
    class videoHandle : public videoHandleBase
    {
    public:
        videoHandle();
        ~videoHandle();

        void update();
        void play();
        void pause();
        void close();

        bbool open(const String& _filename);
        void stop();
        void render();

        bbool getCurrentTime(f32& _timeInSeconds);

    private:
        struct CpuYuvFrame
        {
            std::vector<u8> yPlane;
            std::vector<u8> uPlane;
            std::vector<u8> vPlane;

            u32 width = 0;
            u32 height = 0;
            u32 yStride = 0;
            u32 uvStride = 0;
            f64 pts = 0.0;
        };

        std::thread m_decodeThread;
        std::atomic<bool> m_decodeThreadRunning{ false };
        std::mutex m_queueMutex;
        std::condition_variable m_queueCv;
        std::deque<int> m_frameQueue;
        size_t m_maxQueuedFrames = 3;
        std::vector<CpuYuvFrame> m_framePool;
        int m_nextPoolIndex = 0;

        void decodeThreadFunc();
        bbool pushDecodedFrame(const vpx_image_t* img, f64 pts);
        bbool warmupFirstFrame();
        void destroyTexturesAndVB();
        bbool createTexturesAndVB(u32 _width, u32 _height);
        bbool uploadFrameToTextures(const CpuYuvFrame& _img, u32 _texIndex);

        u32 m_textureWidth = 0;
        u32 m_textureHeight = 0;

    private:
        ResourceID m_texY[2];
        ResourceID m_texU[2];
        ResourceID m_texV[2];

        ITF_VertexBuffer* m_pVertexBuffer = nullptr;
        u32 m_currentTexIndex = 0;   // 0 / 1

        webmDecoder m_decoder;
        f64 m_bufferPts[2] = { 0.0, 0.0 };
        f32 m_currentTimeInSeconds = 0.0f;
        f32 m_totalTimeInSeconds = 0.0f;
    };

} // namespace ITF

#endif // ITF_NINTENDO

#endif //_ITF_VIDEOHANDLE_NVN_H_
