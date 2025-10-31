#ifndef ITF_GFX_ADAPTER_DEVICE_CONTEXT_PS5_H_
#define ITF_GFX_ADAPTER_DEVICE_CONTEXT_PS5_H_

#pragma once

#include "core/math/MathTools.h"
#include "Gnmp_Helpers.h"
#include "Label_PS5.h"

namespace ITF
{
    class MemoryPageManager;

    // Device Context
    // (CUE : Constant update Engine / DCB : Draw Command Buffer / CCB : Constant command Buffer)

    class GfxDeviceContext_PS5
    {
    public:
        explicit GfxDeviceContext_PS5(MemoryPageManager & _memoryPageMgr);
        ~GfxDeviceContext_PS5();

        void startNewFrame(i32 videoOutHandle, u32 displayBufferIndex);
        void endFrameAndFlip(i32 videoOutHandle, u32 displayBufferIndex, u32 flipMode);

        void waitUntilNBGPURenderedFrame(u64 _nbGPUFrame);
        void waitUntilGPUStalled() { waitUntilNBGPURenderedFrame(getNbSubmittedFrame()); }

        void triggerGPUProfilingCaptureAtNextFrame() { m_traceFrame = true; }

        Gnmx::GfxContext* getContext() const { return m_context[m_bufferIndex]; }

        u64 getNbSubmittedFrame() const { return m_nbSubmittedFrame; } // aka frame in production by the cpu
        u64 getNBGPURenderedFrame() const { return m_nbGPURenderedFrame.getValue(); }

        f32 getLastCPUWaitForGPU() const { return m_lastCPUWaitForGPU; }
        f32 getLastGPURenderingTime() const { return m_renderingTimeStamp.getTime(); }
        f32 getLastGPUFlipTime() const { return m_flipTimeStamp.getTime(); }

        enum : ux
        {
            COMMAND_BUFFER_COUNT = 2,
            COMMAND_BUFFER_BYTE_SIZE = 8*1024*1024,
        };

    private:
        void create();
        ux advanceToNextContext() { m_bufferIndex = (m_bufferIndex + 1) % COMMAND_BUFFER_COUNT; return m_bufferIndex; }
        void checkProfilingStart();

        MemoryPageManager& m_memoryPageMgr;

        void initCommandBuffer();
        Gnmx::GfxContext*           m_context[COMMAND_BUFFER_COUNT];
        u32                         m_bufferIndex = 0; // 0 to COMMAND_BUFFER_COUNT-1
        u64                         m_nbSubmittedFrame = 0; // nbFrame sent to the GPU. Also the frame number producing command on the CPU side.
        f32                         m_lastCPUWaitForGPU = 0.f; // Time (in seconds) spent waiting for the GPU before producing new gfx command.

        bool                        m_traceFrame = false;

        void initFrameStampAndLabels();
        Gnmp::TimeStamp<2>          m_renderingTimeStamp;
        Gnmp::TimeStamp<2>          m_flipTimeStamp;

        Gnmp::Label                 m_nbGPURenderedFrame;
    };
} // namespace ITF

#endif // ITF_GFX_ADAPTER_DEVICE_CONTEXT_PS5_H_
