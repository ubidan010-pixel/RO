#pragma once

#if defined(ITF_WIN64)
#include <dxgi1_6.h>
#endif

#include "core/math/MathTools.h"
#include "DX12/Helpers.h"
#include "Label_DX12.h"
#include "TimeStamp_DX12.h"


namespace ITF::DX12
{

    class GfxDeviceContext
    {
    public:
        explicit GfxDeviceContext(ComPtr<ID3D12Device> _device);
        GfxDeviceContext(const GfxDeviceContext &) = delete;
        ~GfxDeviceContext();

        GfxDeviceContext & operator = (const GfxDeviceContext &) = delete;

        enum : u32
        {
            MAX_FRAME_IN_ADVANCE = 2,
            FRAME_BUFFER_COUNT = MAX_FRAME_IN_ADVANCE,
        };

        void startNewFrame();
    #if defined(ITF_WIN64)
        void endFrameAndFlip(IDXGISwapChain3 * _swapChain, bool _vsyncOn, u32 _displayBufferIndex);
    #else
        void endFrameAndFlip(ID3D12Resource* _backBuffer, u32 _displayBufferIndex);
    #endif

        void resetWindow(); // when window changed because of docking/undocking

        void waitUntilNBGPURenderedFrame(u64 _nbGPUFrame);
        void waitUntilGPUStalled();

        [[nodiscard]] ID3D12GraphicsCommandList * getContext() const { return m_commandList.Get(); }
        [[nodiscard]] ID3D12CommandQueue * getQueue() const { return m_commandQueue.Get(); }

        // getCurrentBufferIndex is the command list wrapped index being filled by the CPU.
        [[nodiscard]] u32 getCurrentBufferIndex() const { return m_nbSubmittedFrame % FRAME_BUFFER_COUNT; }
        // getOlderGPUBufferIndex is the older command list wrapped index the CPU is executing.
        [[nodiscard]] u32 getOlderGPUBufferIndex() const { return (getCurrentBufferIndex() + FRAME_BUFFER_COUNT - 1) % FRAME_BUFFER_COUNT; }
        // getNextAvailableGPUBufferIndex is the next command list wrapped index that will be available for the CPU to use.
        // If FRAME_BUFFER_COUNT==2, this is the same as getOlderGPUBufferIndex().
        [[nodiscard]] u32 getNextAvailableGPUBufferIndex() const { return (getCurrentBufferIndex() + 1) % FRAME_BUFFER_COUNT; }

        [[nodiscard]] u64 getNbSubmittedFrame() const { return m_nbSubmittedFrame; }
        [[nodiscard]] u64 getCPUFrameNumber() const { return getNbSubmittedFrame(); } // The frame being produced by the CPU.
        [[nodiscard]] u64 getNBGPURenderedFrame() const;

        [[nodiscard]] f64 getLastCPUWaitForGPU() const { return m_lastCPUWaitForGPUDuration; }
        [[nodiscard]] f64 getLastGPURenderingTime() const { return m_renderingTimeStamp.getTime(); }
        [[nodiscard]] f64 getLastGPUFlipTime() const { return m_flipTimeStamp.getTime(); }

        [[nodiscard]] bool isFrameStarted() const { return m_frameStarted; }

    private:
        void create();

        ComPtr<ID3D12Device> m_device{};
        int m_currentRTIndex = 0;

        void createQueue();
        ComPtr<ID3D12CommandQueue> m_commandQueue{};

        void createCommandList();
        ComPtr<ID3D12CommandAllocator> m_commandAllocators[MAX_FRAME_IN_ADVANCE]{};
        ComPtr<ID3D12GraphicsCommandList> m_commandList{};
        ComPtr<ID3D12GraphicsCommandList2> m_commandList2{};

        void createFrameStampsAndFences();
        DX12::TimeStamp<2> m_renderingTimeStamp{};
        DX12::TimeStamp<2> m_flipTimeStamp{};

    #if !defined(ITF_WIN64)
        D3D12XBOX_FRAME_PIPELINE_TOKEN m_framePipelineToken = D3D12XBOX_FRAME_PIPELINE_TOKEN_NULL;
    #endif

        // m_nbSubmittedFrame is the number of frames sent to the GPU, ie. the number of command list executed on the queue.
        // It is also the frame number being produced on the CPU side: for instance at startup the first frame is number 0, and 0 frames are submitted to GPU.
        // m_frameFences[m_nbSubmittedFrame%MAX_FRAME_IN_ADVANCE] is requesting a signal in the queue just after the command list execution.
        u64 m_nbSubmittedFrame = 0;
        ComPtr<ID3D12Fence> m_frameFences[MAX_FRAME_IN_ADVANCE]{}; // The value of the fence is the nb of frame the GPU has finished processing.
        u64 m_frameFenceSignaledValues[MAX_FRAME_IN_ADVANCE] = { 0 }; // The value to signal for each frame fence.
        // m_frameFenceEvent is used to wait for the GPU to reach a given fence value.
        HANDLE m_frameFenceEvent{};
        f64 m_lastCPUWaitForGPUDuration = 0.; // Time (in seconds) spent waiting for the GPU before producing new gfx command.

        bool m_frameStarted = false;
    };

} // namespace ITF
