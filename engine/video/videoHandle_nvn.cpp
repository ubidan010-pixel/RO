#include "precompiled_engine.h"

#ifdef ITF_NINTENDO

#include "engine/video/videoHandle_nvn.h"

#include "adapters/GFXAdapter_NVN/GFXAdapter_NVN.h"
#include "engine/resources/ResourceManager.h"

#ifdef ITF_SUPPORT_IMGUI
#include "engine/AdaptersInterfaces/ImGui/ImGuiAdapter.h"
#include "engine/imgui/ImGuiVideoDebugTool.h"
#include "imgui/imgui.h"
#include <chrono>
#endif

namespace ITF
{
    videoHandle::videoHandle()
    {
#ifdef ITF_SUPPORT_IMGUI
        registerToImGui();
#endif
    }

    videoHandle::~videoHandle()
    {
#ifdef ITF_SUPPORT_IMGUI
        unRegisterToImGui();
#endif
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
		if (m_decodeThreadRunning)
		{
			m_decodeThreadRunning = false;
			m_queueCv.notify_all();
			if (m_decodeThread.joinable())
				m_decodeThread.join();
		}

		{
			std::lock_guard<std::mutex> lock(m_queueMutex);
			m_frameQueue.clear();
		}

        destroyTexturesAndVB();
        m_decoder.close();

        m_opened = bfalse;
        m_stopped = btrue;
        m_isPaused = bfalse;
        m_bufferPts[0] = 0.0f;
        m_bufferPts[1] = 0.0f;
        m_currentTimeInSeconds = 0.0f;
        m_totalTimeInSeconds = 0.0f;

        resetPlaybackClock();

#ifdef ITF_SUPPORT_IMGUI
        onVideoClosed();
#endif
    }

    void videoHandle::stop()
    {
        videoHandleBase::stop();
    }

    bbool videoHandle::warmupFirstFrame()
    {
        const vpx_image_t* img = nullptr;
        f64 pts = 0.0f;

        if (!m_decoder.decodeNextFrame(img, pts) || !img)
            return bfalse;

        m_currentTexIndex = 0;
        m_bufferPts[0] = pts;
        m_bufferPts[1] = pts;
        m_currentTimeInSeconds = 0.0f;
        m_totalTimeInSeconds = 0.0f;

        if (!pushDecodedFrame(img, pts))
            return bfalse;

        return btrue;
    }

    bbool videoHandle::open(const String& _filename)
    {
        close();

        String fullName = _filename + "_nx.webm";

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

		m_textureWidth = width;
		m_textureHeight = height;

		{
			std::lock_guard<std::mutex> lock(m_queueMutex);

			m_framePool.clear();
			m_framePool.resize(m_maxQueuedFrames);

			const u32 wY = width;
			const u32 hY = height;
			const u32 wUV = width >> 1;
			const u32 hUV = height >> 1;

			for (auto& f : m_framePool)
			{
				f.width = width;
				f.height = height;
				f.yStride = wY;
				f.uvStride = wUV;
				f.pts = 0.0f;

				f.yPlane.resize(wY * hY);
				f.uPlane.resize(wUV * hUV);
				f.vPlane.resize(wUV * hUV);
			}

			m_frameQueue.clear();
			m_nextPoolIndex = 0;
		}

        m_totalTimeInSeconds = (f32)m_decoder.getDurationSeconds();
        m_currentTexIndex = 0;
        resetPlaybackClock();

        if (!warmupFirstFrame())
        {
            close();
            return bfalse;
        }

		m_opened = btrue;
		m_stopped = bfalse;
		m_isPaused = bfalse;

#ifdef ITF_SUPPORT_IMGUI
        onVideoOpened(fullName);
#endif

		m_decodeThreadRunning = true;
		m_decodeThread = std::thread(&videoHandle::decodeThreadFunc, this);

        return btrue;
    }

	void videoHandle::decodeThreadFunc()
	{
		while (m_decodeThreadRunning)
		{
			if (!m_opened || m_stopped)
			{
				std::unique_lock<std::mutex> lock(m_queueMutex);
				m_queueCv.wait_for(lock, std::chrono::milliseconds(10));
				continue;
			}

			if (m_isPaused)
			{
				std::unique_lock<std::mutex> lock(m_queueMutex);
				m_queueCv.wait_for(lock, std::chrono::milliseconds(10));
				continue;
			}

			{
				std::unique_lock<std::mutex> lock(m_queueMutex);
				if (m_frameQueue.size() >= m_maxQueuedFrames)
				{
					m_queueCv.wait_for(lock, std::chrono::milliseconds(5));
					continue;
				}
			}

			const vpx_image_t* img = nullptr;
			f64 pts = 0.0f;

			if (!m_decoder.decodeNextFrame(img, pts) || !img)
			{
				if (isLoop())
				{
					if (!m_decoder.reset())
					{
						m_stopped = btrue;
						continue;
					}

					continue;
				}
				else
				{
					m_stopped = btrue;
					continue;
				}
			}

			if (!pushDecodedFrame(img, pts))
			{
				continue;
			}

#ifdef ITF_SUPPORT_IMGUI
			auto t1 = Clock::now();
			const auto ms = std::chrono::duration<float, std::milli>(t1 - t0).count();
			m_lastDecodeTimeMs = ms;
			++m_decodeFrameCount;
			m_avgDecodeTimeMs += (ms - m_avgDecodeTimeMs) / (f32)m_decodeFrameCount;
			if (ms > m_maxDecodeTimeMs)
				m_maxDecodeTimeMs = ms;
#endif
		}
	}

	bbool videoHandle::pushDecodedFrame(const vpx_image_t* img, f64 pts)
	{
		if (!img)
			return bfalse;

		if (img->fmt != VPX_IMG_FMT_I420 && img->fmt != VPX_IMG_FMT_I42016)
			return bfalse;

		const u32 w = m_textureWidth;
		const u32 h = m_textureHeight;
		const u32 wUV = w >> 1;
		const u32 hUV = h >> 1;

		//if ((img->d_w && img->d_w != w) || (img->d_h && img->d_h != h))
		//{
		//}

		int slot = -1;

		{
			std::lock_guard<std::mutex> lock(m_queueMutex);

			if (m_framePool.empty())
				return bfalse;

			slot = m_nextPoolIndex;
			m_nextPoolIndex = (m_nextPoolIndex + 1) % (int)m_framePool.size();

			CpuYuvFrame& frame = m_framePool[slot];

			frame.pts = pts;

			const u8* srcY = img->planes[VPX_PLANE_Y];
			const int srcSY = img->stride[VPX_PLANE_Y];
			const u8* srcU = img->planes[VPX_PLANE_U];
			const int srcSU = img->stride[VPX_PLANE_U];
			const u8* srcV = img->planes[VPX_PLANE_V];
			const int srcSV = img->stride[VPX_PLANE_V];

			// --- copy Y ---
			for (u32 row = 0; row < h; ++row)
			{
				memcpy(&frame.yPlane[row * w], srcY + row * srcSY, w);
			}

			// --- copy U ---
			for (u32 row = 0; row < hUV; ++row)
			{
				memcpy(&frame.uPlane[row * wUV], srcU + row * srcSU, wUV);
			}

			// --- copy V ---
			for (u32 row = 0; row < hUV; ++row)
			{
				memcpy(&frame.vPlane[row * wUV], srcV + row * srcSV, wUV);
			}

			if (m_frameQueue.size() >= m_maxQueuedFrames)
				m_frameQueue.pop_front();

			m_frameQueue.push_back(slot);
		}

		m_queueCv.notify_all();
		return btrue;
	}

	void videoHandle::render()
	{
		if (!m_opened || m_stopped)
			return;

		if (!m_pVertexBuffer)
			return;

        tickPlaybackClock();
        const f64 targetTime = getPlaybackTimeSeconds();

		int frameIndex = -1;

        if (!m_isPaused)
        {
            std::lock_guard<std::mutex> lock(m_queueMutex);

            while (!m_frameQueue.empty())
            {
                int idx = m_frameQueue.front();
                CpuYuvFrame& f = m_framePool[idx];

                if (f.pts <= targetTime + 0.0001)
                {
                    frameIndex = idx;
                    m_frameQueue.pop_front();
                }
                else
                {
                    break;
                }
            }
        }

		if (frameIndex >= 0)
		{
			CpuYuvFrame& frame = m_framePool[frameIndex];

			const u32 uploadIndex = m_currentTexIndex ^ 1;
			if (uploadFrameToTextures(frame, uploadIndex))
			{
				m_bufferPts[uploadIndex] = frame.pts;
				m_currentTexIndex = uploadIndex;
				m_currentTimeInSeconds = static_cast<f32>(frame.pts);
			}
		}

		const u32 drawIndex = m_currentTexIndex;

		Texture* texArr[3];
		texArr[0] = static_cast<Texture*>(m_texY[drawIndex].getResource());
		texArr[1] = static_cast<Texture*>(m_texU[drawIndex].getResource());
		texArr[2] = static_cast<Texture*>(m_texV[drawIndex].getResource());

		if (!texArr[0] || !texArr[1] || !texArr[2])
			return;

		GFX_ADAPTER->drawMovie(nullptr, getAlpha(), m_pVertexBuffer, texArr, 3);
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
        GFXAdapter_NVN* gfx = static_cast<GFXAdapter_NVN*>(GFX_ADAPTER);

        m_textureWidth  = videoW & ~1u;
        m_textureHeight = videoH & ~1u;

        const u32 wY   = m_textureWidth;
        const u32 hY   = m_textureHeight;
        const u32 wUV  = m_textureWidth >> 1;
        const u32 hUV  = m_textureHeight >> 1;

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

        GFX_RECT viewport{};
        GFX_ADAPTER->getViewport(&viewport);

        const float screenW = (float)(viewport.right  - viewport.left);
        const float screenH = (float)(viewport.bottom - viewport.top);

        const float videoAspect  = (float)videoW / (float)videoH;
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

        const float left   = cx - halfQuadW;
        const float right  = cx + halfQuadW;
        const float top    = cy - halfQuadH;
        const float bottom = cy + halfQuadH;

        VertexPT* vtx = nullptr;
        m_pVertexBuffer->Lock((void**)&vtx);

        vtx[0].m_pos.set(left,  top,    0.0f);
        vtx[0].m_uv.set(0.0f,   0.0f);

        vtx[1].m_pos.set(right, top,    0.0f);
        vtx[1].m_uv.set(1.0f,   0.0f);

        vtx[2].m_pos.set(right, bottom, 0.0f);
        vtx[2].m_uv.set(1.0f,   1.0f);

        vtx[3].m_pos.set(left,  bottom, 0.0f);
        vtx[3].m_uv.set(0.0f,   1.0f);

        m_pVertexBuffer->Unlock();

        return btrue;
    }

	bbool videoHandle::uploadFrameToTextures(const CpuYuvFrame& frame, u32 texIndex)
	{
		const u32 displayW = frame.width;
		const u32 displayH = frame.height;

		const u32 texW = m_textureWidth;
		const u32 texH = m_textureHeight;
		const u32 texW_uv = texW >> 1;
		const u32 texH_uv = texH >> 1;

		const u8* srcY = frame.yPlane.data();
		const u8* srcU = frame.uPlane.data();
		const u8* srcV = frame.vPlane.data();
		const u32 srcStrideY = frame.yStride;
		const u32 srcStrideUV = frame.uvStride;

		Texture* texY = static_cast<Texture*>(m_texY[texIndex].getResource());
		Texture* texU = static_cast<Texture*>(m_texU[texIndex].getResource());
		Texture* texV = static_cast<Texture*>(m_texV[texIndex].getResource());

		LOCKED_TEXTURE lockY{};
		LOCKED_TEXTURE lockU{};
		LOCKED_TEXTURE lockV{};

		GFX_ADAPTER->lockTexture(texY, &lockY);
		GFX_ADAPTER->lockTexture(texU, &lockU);
		GFX_ADAPTER->lockTexture(texV, &lockV);

		// --- Y ---
		{
			u8* dstBase = (u8*)lockY.mp_Bits;
			const u32 dstPitch = (u32)lockY.m_pitch;

			const u32 copyW = (displayW <= texW) ? displayW : texW;
			const u32 copyH = (displayH <= texH) ? displayH : texH;

			for (u32 row = 0; row < copyH; ++row)
			{
				const u8* srcRow = srcY + row * srcStrideY;
				u8* dstRow = dstBase + row * dstPitch;
				memcpy(dstRow, srcRow, copyW);

				if (copyW < dstPitch)
					memset(dstRow + copyW, 16, dstPitch - copyW);
			}

			for (u32 row = copyH; row < texH; ++row)
			{
				u8* dstRow = dstBase + row * dstPitch;
				memset(dstRow, 16, dstPitch);
			}
		}

		// --- U ---
		{
			u8* dstBase = (u8*)lockU.mp_Bits;
			const u32 dstPitch = (u32)lockU.m_pitch;

			const u32 displayW_uv = displayW >> 1;
			const u32 displayH_uv = displayH >> 1;

			const u32 copyW = (displayW_uv <= texW_uv) ? displayW_uv : texW_uv;
			const u32 copyH = (displayH_uv <= texH_uv) ? displayH_uv : texH_uv;

			for (u32 row = 0; row < copyH; ++row)
			{
				const u8* srcRow = srcU + row * srcStrideUV;
				u8* dstRow = dstBase + row * dstPitch;
				memcpy(dstRow, srcRow, copyW);

				if (copyW < dstPitch)
					memset(dstRow + copyW, 128, dstPitch - copyW);
			}

			for (u32 row = copyH; row < texH_uv; ++row)
			{
				u8* dstRow = dstBase + row * dstPitch;
				memset(dstRow, 128, dstPitch);
			}
		}

		// --- V ---
		{
			u8* dstBase = (u8*)lockV.mp_Bits;
			const u32 dstPitch = (u32)lockV.m_pitch;

			const u32 displayW_uv = displayW >> 1;
			const u32 displayH_uv = displayH >> 1;

			const u32 copyW = (displayW_uv <= texW_uv) ? displayW_uv : texW_uv;
			const u32 copyH = (displayH_uv <= texH_uv) ? displayH_uv : texH_uv;

			for (u32 row = 0; row < copyH; ++row)
			{
				const u8* srcRow = srcV + row * srcStrideUV;
				u8* dstRow = dstBase + row * dstPitch;
				memcpy(dstRow, srcRow, copyW);

				if (copyW < dstPitch)
					memset(dstRow + copyW, 128, dstPitch - copyW);
			}

			for (u32 row = copyH; row < texH_uv; ++row)
			{
				u8* dstRow = dstBase + row * dstPitch;
				memset(dstRow, 128, dstPitch);
			}
		}

		GFX_ADAPTER->unlockTexture(texY);
		GFX_ADAPTER->unlockTexture(texU);
		GFX_ADAPTER->unlockTexture(texV);

		return btrue;
	}

} // namespace ITF

#endif // ITF_NINTENDO
