#include "precompiled_GFXAdapter_PS5.h"

#include "adapters/GFXAdapter_PS5/GFXAdapter_device_context_PS5.h"

#include <video_out.h>

#include "Gnmp_Helpers.h"

#ifdef ITF_DEBUG
#ifndef ITF_ENABLE_GPU_CAPTURE
    #define ITF_ENABLE_GPU_CAPTURE 1
#endif
#endif

#include "core/utility.h"
#include "core/memory/MemMng/MemMngPage_PS5.h"


namespace ITF
{

    GfxDeviceContext_PS5::GfxDeviceContext_PS5(MemoryPageManager& _memoryPageMgr)
        : m_memoryPageMgr(_memoryPageMgr)
    {
        create();
    }

    GfxDeviceContext_PS5::~GfxDeviceContext_PS5() = default;


    void GfxDeviceContext_PS5::create()
    {
        initFrameStampAndLabels();
        initCommandBuffer();
    }

    void GfxDeviceContext_PS5::initFrameStampAndLabels()
    {
        m_nbSubmittedFrame = 0;

        m_renderingTimeStamp.init("TimeStamp_rendering");
        m_flipTimeStamp.init("TimeStamp_flip");
        m_nbGPURenderedFrame.init(0, "Label_nbGPURenderedFrame");
    }

    void GfxDeviceContext_PS5::initCommandBuffer()
    {
        m_bufferIndex = 0;

        for (Gnmx::GfxContext* & context : m_context)
        {
            size_t idx = distInRange(m_context, context);
            ITF_UNUSED(idx);


            u8* commandBufferArea = m_memoryPageMgr.AllocArea(COMMAND_BUFFER_BYTE_SIZE, MemoryPageManager::Protection::allRW(), Gnmp::kAlignmentOfBufferInBytes);
            ITF_ASSERT_MSG(commandBufferArea!=nullptr, "Failed to allocate Command Buffer %zu", idx);

            context = newAlloc(mId_GfxAdapter, Gnmx::GfxContext{});
            context->init(commandBufferArea, COMMAND_BUFFER_BYTE_SIZE, nullptr, nullptr);
            ITF_AGC_REGISTER_RESOURCE(&context->m_agcCtx.m_dcb, "Context %zu", idx);

            // Removed setGlobalResourceTableAddr... Was used on PS4, but don't know what the PS5 usage could be...
            // u8*& globalRessourceTable = m_globalRessourceTable[idx];
            // globalRessourceTable = m_memoryPageMgr.AllocArea(SCE_GNMP_SHADER_GLOBAL_TABLE_SIZE, MemoryPageManager::Protection::allRW(), Gnmp::kAlignmentOfBufferInBytes);
            // context->setGlobalResourceTableAddr(globalRessourceTable);

            context->reset();
            context->initializeDefaultHardwareState();
        }
    }

    void GfxDeviceContext_PS5::waitUntilNBGPURenderedFrame(u64 _gpuFrameToWait)
    {
        m_nbGPURenderedFrame.cpuWaitUntil(
            [&](u64 lastGPUProcessedFrame)
            {
                return lastGPUProcessedFrame >= _gpuFrameToWait;
            });
    }

    void GfxDeviceContext_PS5::startNewFrame(i32 videoOutHandle, u32 displayBufferIndex)
    {
        Gnmp::TimerProfilerScope cpuWaitForGPUProfilerScope(m_lastCPUWaitForGPU);

        if (m_nbSubmittedFrame >= 2) // do not wait until 2 frames produced
        {
            // Ensure the GPU has finished processing at least the nb submitted frames - 1
            waitUntilNBGPURenderedFrame(m_nbSubmittedFrame - 1);
        }

        getContext()->reset();

        if (m_nbSubmittedFrame == 0)
        {
            getContext()->initializeDefaultHardwareState();
        }

        getContext()->m_agcCtx.m_dcb.waitUntilSafeForRendering(videoOutHandle, displayBufferIndex);

        m_flipTimeStamp.stop(*getContext());
        m_renderingTimeStamp.start(*getContext());
    }

    void GfxDeviceContext_PS5::endFrameAndFlip(i32 videoOutHandle, u32 displayBufferIndex, u32 flipMode)
    {
        m_renderingTimeStamp.stop(*getContext());
        m_flipTimeStamp.start(*getContext());

        getContext()->m_agcCtx.m_dcb.setFlip(videoOutHandle, displayBufferIndex, flipMode, m_nbSubmittedFrame);

        m_nbSubmittedFrame++;

        sce::Agc::Core::gpuSyncEvent
        (
            &getContext()->m_agcCtx.m_dcb,
            sce::Agc::Core::SyncWaitMode::kAsynchronous,
            sce::Agc::Core::SyncCacheOp::kFlushCbDbNonMetadataWritesToMemory,
            sce::Agc::Core::SyncLabelVisibility::kCpu,
            m_nbGPURenderedFrame.getAgcLabel(),
            m_nbSubmittedFrame
        );

        checkProfilingStart();
        ITF_VERIFY_SCE_CALL(getContext()->submit());
        ITF_VERIFY_SCE_CALL(sce::Agc::suspendPoint());

        advanceToNextContext();
    }

    void GfxDeviceContext_PS5::checkProfilingStart()
    {
    #if ITF_ENABLE_GPU_CAPTURE
        static const char gs_gpuTraceFileName[] = "/hostapp/thread_trace.rtt";

        // save the thread trace file for the frame and reset ready to perform another trace.
        if (m_traceFrame)
        {
            ITF_VERIFY_SCE_CALL(sce::Agc::triggerCapture(sce::Agc::CaptureMode::kCaptureBetweenSuspendPoints, gs_gpuTraceFileName));
            m_traceFrame = false;
        }
    #endif // ITF_ENABLE_GPU_CAPTURE
    }

} // namespace ITF
