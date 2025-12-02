#include "precompiled_engine.h"

#ifdef ITF_XBOX_SERIES
#include "engine/video/videoHandle_dx12.h"

#include "adapters/GFXAdapter_DX12/GFXAdapter_DX12.h"
#include "adapters/SoundAdapter_XAudio/SoundAdapter_XAudio.h"
#include "engine/resources/ResourceManager.h"

namespace ITF
{
    videoHandle::videoHandle()
    {
    }

    videoHandle::~videoHandle()
    {
        close();
    }

    void videoHandle::update()
    {
    }

    void videoHandle::play()
    {
        if (!m_opened)
            return;

        m_isPaused = bfalse;
    }

    void videoHandle::pause()
    {
        if (!m_opened)
            return;

        m_isPaused = btrue;
    }

    void videoHandle::close()
    {
        destroyTexturesAndVB();
        m_decoder.close();

        m_opened = bfalse;
        m_stopped = btrue;
        m_isPaused = bfalse;
        m_bufferPts[0] = 0.0;
        m_bufferPts[1] = 0.0;
        m_currentTimeInSeconds = 0.0f;
        m_totalTimeInSeconds = 0.0f;
    }

    void videoHandle::stop()
    {
        videoHandleBase::stop();
    }

    bbool videoHandle::warmupFirstFrame()
    {
        const vpx_image_t* img = nullptr;
        f64 pts = 0.0;

        if (!m_decoder.decodeNextFrame(img, pts) || !img)
            return bfalse;

        m_currentTexIndex = 0;
        m_bufferPts[0] = pts;
        m_bufferPts[1] = pts;
        m_currentTimeInSeconds = 0;

        if (!uploadFrameToTextures(*img, 0))
            return bfalse;
        if (!uploadFrameToTextures(*img, 1))
            return bfalse;

        return btrue;
    }

    bbool videoHandle::open(const String& _filename)
    {
        close();

        String fullName = _filename + ".webm";

        if (!m_decoder.open(fullName))
            return bfalse;

        const u32 width = m_decoder.getWidth();
        const u32 height = m_decoder.getHeight();
        if (width == 0 || height == 0)
        {
            m_decoder.close();
            return bfalse;
        }

        if (!createTexturesAndVB(width, height))
        {
            m_decoder.close();
            return bfalse;
        }

        m_totalTimeInSeconds = (f32)m_decoder.getDurationSeconds();
        m_currentTexIndex = 0;

        if (!warmupFirstFrame())
        {
            close();
            return bfalse;
        }

        m_opened = btrue;
        m_stopped = bfalse;
        m_isPaused = bfalse;

        return btrue;
    }

    void videoHandle::render()
    {
        if (!m_opened || m_stopped || m_isPaused)
            return;

        const vpx_image_t* img = nullptr;
        f64 pts = 0.0;

        if (!m_decoder.decodeNextFrame(img, pts) || !img)
        {
            if (isLoop())
            {
                if (!m_decoder.reset() || !warmupFirstFrame())
                {
                    stop();
                    return;
                }

                return;
            }
            else
            {
                stop();
                return;
            }
        }

        const u32 uploadIndex = m_currentTexIndex ^ 1;

        if (!uploadFrameToTextures(*img, uploadIndex))
        {
            return;
        }
        m_bufferPts[uploadIndex] = pts;

        Texture* texArr[3];
        texArr[0] = static_cast<Texture*>(m_texY[m_currentTexIndex].getResource());
        texArr[1] = static_cast<Texture*>(m_texU[m_currentTexIndex].getResource());
        texArr[2] = static_cast<Texture*>(m_texV[m_currentTexIndex].getResource());

        GFX_ADAPTER->drawMovie(nullptr, getAlpha(), m_pVertexBuffer, texArr, 3);

        m_currentTimeInSeconds = (f32)m_bufferPts[m_currentTexIndex];

        m_currentTexIndex ^= 1;
    }

    bbool videoHandle::getCurrentTime(f32& _timeInSeconds)
    {
        if (!m_opened)
            return bfalse;

        _timeInSeconds = m_currentTimeInSeconds;
        return btrue;
    }

    void videoHandle::destroyTexturesAndVB()
    {
        if (m_pVertexBuffer)
        {
            GFX_ADAPTER->removeVertexBufferDelay(m_pVertexBuffer, 1);
            m_pVertexBuffer = nullptr;
        }

        for (int i = 0; i < 2; ++i)
        {
            if (m_texY[i].getResource())
                Resource::forceDeletion(m_texY[i].getResource());
            if (m_texU[i].getResource())
                Resource::forceDeletion(m_texU[i].getResource());
            if (m_texV[i].getResource())
                Resource::forceDeletion(m_texV[i].getResource());

            m_texY[i] = ResourceID();
            m_texU[i] = ResourceID();
            m_texV[i] = ResourceID();
        }
    }
    bbool videoHandle::createTexturesAndVB(u32 videoW, u32 videoH)
    {
        GFXAdapter_DX12* gfx = static_cast<GFXAdapter_DX12*>(GFX_ADAPTER);

        m_textureWidth = videoW;
        m_textureHeight = videoH;

        m_textureWidth &= ~1;
        m_textureHeight &= ~1;

        const u32 wY = m_textureWidth;
        const u32 hY = m_textureHeight;
        const u32 wUV = m_textureWidth >> 1;
        const u32 hUV = m_textureHeight >> 1;

        for (int i = 0; i < 2; ++i)
        {
            // Y
            m_texY[i] = RESOURCE_MANAGER->newEmptyResource(Resource::ResourceType_Texture)->getID();
            gfx->createTexture(static_cast<Texture*>(m_texY[i].getResource()), wY, hY, 1, Texture::PF_L8);

            // U
            m_texU[i] = RESOURCE_MANAGER->newEmptyResource(Resource::ResourceType_Texture)->getID();
            gfx->createTexture(static_cast<Texture*>(m_texU[i].getResource()), wUV, hUV, 1, Texture::PF_L8);

            // V
            m_texV[i] = RESOURCE_MANAGER->newEmptyResource(Resource::ResourceType_Texture)->getID();
            gfx->createTexture(static_cast<Texture*>(m_texV[i].getResource()), wUV, hUV, 1, Texture::PF_L8);
        }

        m_pVertexBuffer = GFX_ADAPTER->createVertexBuffer(4, VertexFormat_PT, sizeof(VertexPT), bfalse, VB_T_INTERNAL);

        if (!m_pVertexBuffer)
            return bfalse;

        const float screenW = (float)gfx->getScreenWidth();
        const float screenH = (float)gfx->getScreenHeight();

        const float videoAspect = (float)videoW / (float)videoH;
        const float screenAspect = screenW / screenH;

        float quadW = screenW;
        float quadH = screenH;

        if (videoAspect > screenAspect)
        {
            quadH = screenW / videoAspect;
        }
        else
        {
            quadW = screenH * videoAspect;
        }

        const float halfQuadW = quadW * 0.5f;
        const float halfQuadH = quadH * 0.5f;
        const float cx = screenW * 0.5f;
        const float cy = screenH * 0.5f;

        const float left = cx - halfQuadW;
        const float right = cx + halfQuadW;
        const float top = cy - halfQuadH;
        const float bottom = cy + halfQuadH;

        VertexPT* vtx = nullptr;
        m_pVertexBuffer->Lock((void**)&vtx);

        vtx[0].m_pos.set(left, top, 0.0f);
        vtx[0].m_uv.set(0.0f, 0.0f);

        vtx[1].m_pos.set(right, top, 0.0f);
        vtx[1].m_uv.set(1.0f, 0.0f);

        vtx[2].m_pos.set(right, bottom, 0.0f);
        vtx[2].m_uv.set(1.0f, 1.0f);

        vtx[3].m_pos.set(left, bottom, 0.0f);
        vtx[3].m_uv.set(0.0f, 1.0f);

        m_pVertexBuffer->Unlock();

        return btrue;
    }

    bbool videoHandle::uploadFrameToTextures(const vpx_image_t& img, u32 texIndex)
    {
        if (img.fmt != VPX_IMG_FMT_I420 && img.fmt != VPX_IMG_FMT_I42016)
            return bfalse;

        const int idx = (int)texIndex;

        Texture* texY = static_cast<Texture*>(m_texY[idx].getResource());
        Texture* texU = static_cast<Texture*>(m_texU[idx].getResource());
        Texture* texV = static_cast<Texture*>(m_texV[idx].getResource());

        LOCKED_TEXTURE lockY{};
        LOCKED_TEXTURE lockU{};
        LOCKED_TEXTURE lockV{};

        GFX_ADAPTER->lockTexture(texY, &lockY);
        GFX_ADAPTER->lockTexture(texU, &lockU);
        GFX_ADAPTER->lockTexture(texV, &lockV);

        const u32 displayW = (u32)img.d_w;
        const u32 displayH = (u32)img.d_h;

        // Y
        {
            const u8* srcPlane = img.planes[VPX_PLANE_Y];
            const int srcStride = img.stride[VPX_PLANE_Y];
            u8* dstBase = (u8*)lockY.mp_Bits;
            const u32 dstPitch = (u32)lockY.m_pitch;

            const u32 copyWidth = (displayW <= m_textureWidth) ? displayW : m_textureWidth;
            const u32 copyHeight = (displayH <= m_textureHeight) ? displayH : m_textureHeight;

            for (u32 row = 0; row < copyHeight; ++row)
            {
                const u8* srcRow = srcPlane + (row * srcStride);
                u8* dstRow = dstBase + (row * dstPitch);

                memcpy(dstRow, srcRow, copyWidth);

                if (copyWidth < dstPitch)
                {
                    memset(dstRow + copyWidth, 16, dstPitch - copyWidth);
                }
            }

            if (copyHeight < m_textureHeight)
            {
                for (u32 row = copyHeight; row < m_textureHeight; ++row)
                {
                    u8* dstRow = dstBase + (row * dstPitch);
                    memset(dstRow, 16, dstPitch);
                }
            }
        }

        // U
        {
            const u32 displayW_uv = displayW >> 1;
            const u32 displayH_uv = displayH >> 1;

            const u8* srcPlane = img.planes[VPX_PLANE_U];
            const int srcStride = img.stride[VPX_PLANE_U];
            u8* dstBase = (u8*)lockU.mp_Bits;
            const u32 dstPitch = (u32)lockU.m_pitch;

            const u32 texW_uv = m_textureWidth >> 1;
            const u32 texH_uv = m_textureHeight >> 1;

            const u32 copyWidth = (displayW_uv <= texW_uv) ? displayW_uv : texW_uv;
            const u32 copyHeight = (displayH_uv <= texH_uv) ? displayH_uv : texH_uv;

            for (u32 row = 0; row < copyHeight; ++row)
            {
                const u8* srcRow = srcPlane + (row * srcStride);
                u8* dstRow = dstBase + (row * dstPitch);
                memcpy(dstRow, srcRow, copyWidth);

                if (copyWidth < dstPitch)
                {
                    memset(dstRow + copyWidth, 128, dstPitch - copyWidth);
                }
            }

            if (copyHeight < texH_uv)
            {
                for (u32 row = copyHeight; row < texH_uv; ++row)
                {
                    u8* dstRow = dstBase + (row * dstPitch);
                    memset(dstRow, 128, dstPitch);
                }
            }
        }

        // V
        {
            const u32 displayW_uv = displayW >> 1;
            const u32 displayH_uv = displayH >> 1;

            const u8* srcPlane = img.planes[VPX_PLANE_V];
            const int srcStride = img.stride[VPX_PLANE_V];
            u8* dstBase = (u8*)lockV.mp_Bits;
            const u32 dstPitch = (u32)lockV.m_pitch;

            const u32 texW_uv = m_textureWidth >> 1;
            const u32 texH_uv = m_textureHeight >> 1;

            const u32 copyWidth = (displayW_uv <= texW_uv) ? displayW_uv : texW_uv;
            const u32 copyHeight = (displayH_uv <= texH_uv) ? displayH_uv : texH_uv;

            for (u32 row = 0; row < copyHeight; ++row)
            {
                const u8* srcRow = srcPlane + (row * srcStride);
                u8* dstRow = dstBase + (row * dstPitch);
                memcpy(dstRow, srcRow, copyWidth);

                if (copyWidth < dstPitch)
                {
                    memset(dstRow + copyWidth, 128, dstPitch - copyWidth);
                }
            }

            if (copyHeight < texH_uv)
            {
                for (u32 row = copyHeight; row < texH_uv; ++row)
                {
                    u8* dstRow = dstBase + (row * dstPitch);
                    memset(dstRow, 128, dstPitch);
                }
            }
        }

        GFX_ADAPTER->unlockTexture(texY);
        GFX_ADAPTER->unlockTexture(texU);
        GFX_ADAPTER->unlockTexture(texV);

        return btrue;
    }
} // namespace ITF

#endif
