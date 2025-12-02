#ifndef _ITF_VIDEOHANDLE_DX12_H_
#define _ITF_VIDEOHANDLE_DX12_H_

#ifdef ITF_XBOX_SERIES

#include "engine/video/videoHandle.h"
#include "engine/video/webmVp9Decoder.h"
#include "engine/resources/ResourceManager.h"
#include <AdaptersInterfaces/GFXAdapter_VertexBufferManager.h>

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
        bbool warmupFirstFrame();
        void destroyTexturesAndVB();
        bbool createTexturesAndVB(u32 _width, u32 _height);
        bbool uploadFrameToTextures(const vpx_image_t& _img, u32 _texIndex);
        u32 m_textureWidth = 0;
        u32 m_textureHeight = 0;

    private:
        ResourceID  m_texY[2];
        ResourceID  m_texU[2];
        ResourceID  m_texV[2];

        ITF_VertexBuffer* m_pVertexBuffer = nullptr;
        u32               m_currentTexIndex = 0;   // 0 / 1

        webmVp9Decoder    m_decoder;
        f64               m_bufferPts[2] = { 0.0, 0.0 };
        f32               m_currentTimeInSeconds = 0.0f;
        f32               m_totalTimeInSeconds = 0.0f;
    };

} // namespace ITF

#endif

#endif //_ITF_VIDEOHANDLE_DX12_H_
