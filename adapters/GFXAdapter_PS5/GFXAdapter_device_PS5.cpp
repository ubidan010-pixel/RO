#include "precompiled_GFXAdapter_PS5.h"

#include "adapters/GFXAdapter_PS5/GFXAdapter_PS5.h"

#include "core/utility.h"

#include <video_out.h>

namespace ITF
{

    bbool GFXAdapter_PS5::createDevice(i32 _videoOutHdl)
    {
        m_videoOutHandle = _videoOutHdl;

        m_mainContext = newAlloc(mId_GfxAdapter, GfxDeviceContext_PS5(m_memoryBigPageMgrDirectMemory));

        fetchScreenResolution();
        createRenderTargets();
        m_backBufferIndex = 0;

        registerRenderTargetsAsVideoOut();

        return btrue;
    }

    bbool GFXAdapter_PS5::isDeviceLost()
    {
        // probably never lost on PS5...
        return bfalse;
    }

    bbool GFXAdapter_PS5::resetDevice(bbool _forceReset)
    {
        // Called by at resolution change, nothing useful needed on PS5.
        return btrue;
    }

    void GFXAdapter_PS5::fetchScreenResolution()
    {
        i32 resAllowOutputResolutionWqhd = sceVideoOutAllowOutputResolutionWqhdDetection(m_videoOutHandle);
        if (resAllowOutputResolutionWqhd != SCE_OK && resAllowOutputResolutionWqhd != SCE_VIDEO_OUT_ERROR_UNSUPPORTED_OPERATION)
        {
            ITF_ASSERT_SCE_RESULT(resAllowOutputResolutionWqhd);
        }

        SceVideoOutOutputStatus outputStatus{};
        i32 resVideoOutGetOutput = sceVideoOutGetOutputStatus(m_videoOutHandle, &outputStatus);
        if (resVideoOutGetOutput != SCE_OK && resVideoOutGetOutput != SCE_VIDEO_OUT_ERROR_UNSUPPORTED_OPERATION)
        {
            ITF_ASSERT_SCE_RESULT(resVideoOutGetOutput);
        }
        // Assume 4K by default.
        // It's responsibility of Video-out Info in the param file (param.json) to allow not 4K resolution (not recommended)
        u32 screenWidth = 3840;
        u32 screenHeight = 2160;
        if (resVideoOutGetOutput == SCE_OK)
        {
            switch (outputStatus.resolution)
            {
            case SCE_VIDEO_OUT_OUTPUT_RESOLUTION_HD:
                screenWidth = 1920;
                screenHeight = 1080;
                break;
            case SCE_VIDEO_OUT_OUTPUT_RESOLUTION_WQHD:
                screenWidth = 2560;
                screenHeight = 1440;
                break;
            default:
                break;
            }
            // Right now we assume SDR at 60Hz.
            // If these asserts trigger, it means the Video-out Info in the param file (param.json) allow HDR and/or HFR.
            ITF_ASSERT_MSG(outputStatus.dynamicRange == SCE_VIDEO_OUT_OUTPUT_DYNAMIC_RANGE_SDR, "Only SDR is supported");
            ITF_ASSERT_MSG(outputStatus.refreshRate == SCE_VIDEO_OUT_REFRESH_RATE_59_94HZ, "Only 60Hz is supported");
        }
        setResolution(screenWidth, screenHeight);
    }

    void GFXAdapter_PS5::createRenderTargets()
    {
        Gnmp::RenderTargetSpec rtSpec;
        rtSpec.init();
        rtSpec.m_width = getScreenWidth();
        rtSpec.m_height = getScreenHeight();
        rtSpec.m_colorFormat = Gnmp::kDataFormatR8G8B8A8Unorm;
        rtSpec.m_colorTileModeHint = sce::Agc::CxRenderTarget::TileMode::kRenderTarget;
        //rtSpec.m_flags.enableCmaskFastClear = true;
        rtSpec.m_flags.enableCmaskFastClear = false;
        rtSpec.m_flags.enableDccCompression = false; // TODO: Consider replace enableCmaskFastClear with enableDccCompression
        rtSpec.m_flags.enableFmaskCompression = false; // No MSAA so not useful

        for (PS5RenderTarget & renderTarget : m_renderTargets)
        {
            ITF_VERIFY_SCE_CALL(renderTarget.init(&rtSpec));

            renderTarget.baseMemHdl.alloc<Gnmp::RENDER_TARGET>(renderTarget.getColorSizeAlign(), Gnmp::VRAM_RenderTarget);
            //renderTarget.cmaskMemHdl.alloc<Gnmp::RENDER_TARGET>(renderTarget.getCmaskSizeAlign(), Gnmp::VRAM_RenderTarget);

            char renderTargetName[] = "RenderTarget_X";
            renderTargetName[ITF_ARRAY_SIZE(renderTargetName) - 2] = '0' + distInRange(m_renderTargets, renderTarget);
            renderTarget.m_resHandler.set(renderTargetName);

            renderTarget.buildTexture();
            renderTarget.asTexture->m_resHandler.set(renderTargetName);
        }
    }

    void GFXAdapter_PS5::registerRenderTargetsAsVideoOut()
    {
        SceVideoOutBufferAttribute2 attribute;
        sceVideoOutSetBufferAttribute2
        (
            &attribute,
            SCE_VIDEO_OUT_PIXEL_FORMAT2_R8_G8_B8_A8_SRGB,
            SCE_VIDEO_OUT_TILING_MODE_TILE,
            getScreenWidth(),
            getScreenHeight(),
            SCE_VIDEO_OUT_BUFFER_ATTRIBUTE_OPTION_NONE,
            SCE_VIDEO_OUT_DCC_CONTROL_NONE,
            0
        );

        for (ux i = 0; i < NB_SCANOUT_RENDER_TARGETS; ++i)
        {
            m_videoOutBuffersAddress[i].data = m_renderTargets[i].getBaseAddress();
        }

        const u32 setIndex = 0;
        const u32 bufferIndexStart = 0;
        ITF_VERIFY_SCE_CALL(sceVideoOutRegisterBuffers2(
                                m_videoOutHandle,
                                setIndex, bufferIndexStart,
                                m_videoOutBuffersAddress, NB_SCANOUT_RENDER_TARGETS,
                                &attribute,
                                SCE_VIDEO_OUT_BUFFER_ATTRIBUTE_CATEGORY_UNCOMPRESSED, nullptr));
    }

} // namespace ITF
