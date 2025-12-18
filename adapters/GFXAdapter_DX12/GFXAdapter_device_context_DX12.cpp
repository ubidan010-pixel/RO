#include "precompiled_GFXAdapter_DX12.h"

#include "adapters/GFXAdapter_DX12/GFXAdapter_device_context_DX12.h"
#include "core/math/Mathf32.h"
#include "engine/profiler/profilerMacro.h"
#include "DX12/Helpers.h"
#include "core/utility.h"
#include "core/math/BitTweak.h"

namespace ITF::DX12
{

    GfxDeviceContext::GfxDeviceContext(ComPtr<ID3D12Device> _device)
        : m_device(std::move(_device))
    {
        createQueue();
        createCommandList();
        createFrameStampsAndFences();
    }

    GfxDeviceContext::~GfxDeviceContext() = default;


    void GfxDeviceContext::createQueue()
    {
        D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
        commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        ITF_VERIFY_HR_CALL(m_device->CreateCommandQueue(&commandQueueDesc, DX12_IID_COMPTR_ARGS(m_commandQueue)));
    }

    void GfxDeviceContext::createCommandList()
    {
        for (u32 i = 0; i < MAX_FRAME_IN_ADVANCE; ++i)
        {
            ITF_VERIFY_HR_CALL(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, DX12_IID_COMPTR_ARGS(m_commandAllocators[i])));
        }

        ITF_VERIFY_HR_CALL(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocators[0].Get(), nullptr, DX12_IID_COMPTR_ARGS(m_commandList)));
        ITF_VERIFY_HR_CALL(m_commandList.As(&m_commandList2));

        m_commandList->Close();
    }

    void GfxDeviceContext::createFrameStampsAndFences()
    {
        new DX12::LabelManager(64, m_device);
        new DX12::TimeStampManager<MAX_FRAME_IN_ADVANCE>(64, m_device, m_commandQueue);

        m_renderingTimeStamp.init();
        m_flipTimeStamp.init();

        for(auto & fence : m_frameFences)
        {
            ITF_VERIFY_HR_CALL(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, DX12_IID_COMPTR_ARGS(fence)));
        }

        std::fill(m_frameFenceSignaledValues, m_frameFenceSignaledValues + ITF_ARRAY_SIZE(m_frameFenceSignaledValues), 0);
        m_frameFenceEvent = CreateEventA(nullptr /* LPSECURITY_ATTRIBUTES */, false /* bManualReset */, false /* bInitialState */, nullptr /* lpName */);
    }

    u64 GfxDeviceContext::getNBGPURenderedFrame() const
    {
        u64 frameDoneNumber = m_frameFences[0]->GetCompletedValue();
        for (u32 i = 1; i < MAX_FRAME_IN_ADVANCE; ++i)
        {
            u64 otherFrameDoneNumber = m_frameFences[i]->GetCompletedValue();
            if (otherFrameDoneNumber > frameDoneNumber)
            {
                frameDoneNumber = otherFrameDoneNumber;
            }
        }
        return frameDoneNumber;
    }

    void GfxDeviceContext::waitUntilNBGPURenderedFrame(u64 _gpuFrameToWait)
    {
        ITF_ASSERT_MSG(getNbSubmittedFrame() >= _gpuFrameToWait, "Requesting to wait on a frame not even submitted to the GPU.");

        if (getNbSubmittedFrame() < _gpuFrameToWait)
            return;

        for (u32 i = 0; i < MAX_FRAME_IN_ADVANCE; ++i)
        {
            u64 signaledValue = m_frameFenceSignaledValues[i];
            if (signaledValue == _gpuFrameToWait)
            {
                u64 completedValue = m_frameFences[i]->GetCompletedValue();
                if (completedValue != signaledValue)
                {
                    m_frameFences[getOlderGPUBufferIndex()]->SetEventOnCompletion(_gpuFrameToWait, m_frameFenceEvent);
                    WaitForSingleObject(m_frameFenceEvent, INFINITE);
                    return;
                }
            }
        }
        // The requested frame is not in the fences, so is older... No wait needed.
    }

    void GfxDeviceContext::waitUntilGPUStalled()
    {
        waitUntilNBGPURenderedFrame(getNbSubmittedFrame());
    }

    void GfxDeviceContext::startNewFrame()
    {
        SimpleProfileTimerScope cpuWaitForGPUProfilerScope(m_lastCPUWaitForGPUDuration);

        m_frameStarted = true;

        // At CPU start of frame, we ensure that the older GPU frame is finished.
        if (getNbSubmittedFrame() >= MAX_FRAME_IN_ADVANCE) // do not wait until 2 frames produced
        {
            waitUntilNBGPURenderedFrame(getNbSubmittedFrame() - (MAX_FRAME_IN_ADVANCE - 1));
        }

    #if !defined(ITF_WIN64)
        m_framePipelineToken = D3D12XBOX_FRAME_PIPELINE_TOKEN_NULL;
        m_device->WaitFrameEventX(D3D12XBOX_FRAME_EVENT_ORIGIN, INFINITE, nullptr, D3D12XBOX_WAIT_FRAME_EVENT_FLAG_NONE, &m_framePipelineToken);
    #endif

        m_commandAllocators[getCurrentBufferIndex()]->Reset();
        m_commandList->Reset(m_commandAllocators[getCurrentBufferIndex()].Get(), nullptr);

        if (getNbSubmittedFrame() >= 1)
            m_flipTimeStamp.stop(m_commandList.Get());
        m_renderingTimeStamp.start(m_commandList.Get());
    }

#if defined(ITF_WIN64)
    void GfxDeviceContext::endFrameAndFlip(IDXGISwapChain3* _swapChain, bool _vsyncOn, u32 _displayBufferIndex)
#else
    void GfxDeviceContext::endFrameAndFlip(ID3D12Resource* _backBuffer, u32 _displayBufferIndex)
#endif
    {
        ITF_ASSERT_CRASH(_displayBufferIndex == getCurrentBufferIndex(), "Desynch of fences and render target indices");

        m_renderingTimeStamp.stop(m_commandList.Get());
        m_flipTimeStamp.start(m_commandList.Get());

        m_commandList->Close();
        ID3D12CommandList* commandLists[] = { m_commandList.Get() };
        m_commandQueue->ExecuteCommandLists(1, commandLists);

    #if defined(ITF_WIN64)

        ITF_ASSERT(_swapChain->GetCurrentBackBufferIndex() == getCurrentBufferIndex());
        _swapChain->Present(_vsyncOn ? 1 : 0, 0);

    #else

        // no swapchain with GDK. Use PresentX instead
        D3D12XBOX_PRESENT_PLANE_PARAMETERS planeParameters = {};
        planeParameters.Token = m_framePipelineToken;
        planeParameters.ResourceCount = 1;
        planeParameters.ppResources = &_backBuffer;

        ITF_VERIFY_HR_CALL(m_commandQueue->PresentX(1, &planeParameters, nullptr));

    #endif

        m_commandQueue->Signal(m_frameFences[getCurrentBufferIndex()].Get(), m_nbSubmittedFrame);
        m_frameFenceSignaledValues[getCurrentBufferIndex()] = m_nbSubmittedFrame;
        m_nbSubmittedFrame++;

        m_frameStarted = false;
    }

    void GfxDeviceContext::resetFramePipelineAfterSuspendResume()
    {
        m_nbSubmittedFrame = 0;
        m_frameStarted = false;

        for (u32 i = 0; i < MAX_FRAME_IN_ADVANCE; ++i)
            m_frameFenceSignaledValues[i] = 0;

#if !defined(ITF_WIN64)
        m_framePipelineToken = D3D12XBOX_FRAME_PIPELINE_TOKEN_NULL;
#endif
    }
} // namespace ITF
