#include "precompiled_GFXAdapter_NVN.h"

#include "adapters/GFXAdapter_NVN/GFXAdapter_device_context_NVN.h"
#include "core/math/Mathf32.h"

#include "engine/profiler/profilerMacro.h"

#include "NVN/Helpers.h"

#ifdef ITF_DEBUG
#ifndef ITF_ENABLE_GPU_CAPTURE
    #define ITF_ENABLE_GPU_CAPTURE 1
#endif
#endif

#include "core/utility.h"

#include "core/math/BitTweak.h"

#include "core/memory/MemMng/MemMngPage_Nintendo.h"

#include "VRAMAllocator/VRAMAllocator.h"


namespace ITF
{

    NVNGfxDeviceContext::NVNGfxDeviceContext(nvn::Device& _nvnDevice)
        : m_nvnDevice(_nvnDevice)
    {
        create();
        m_displayReleaseSync.Initialize(&m_nvnDevice);
    }

    NVNGfxDeviceContext::~NVNGfxDeviceContext() = default;


    void NVNGfxDeviceContext::create()
    {
        initQueue();
        initCommandBuffer();
        initFrameStampAndLabels();
    }

    void NVNGfxDeviceContext::initQueue()
    {

        // Do not set the memory Queue.
        // It should be automatically allocated, calling functions set in nv::SetGraphicsAllocator.
        nvn::QueueBuilder queueBuilder;
        queueBuilder.SetDefaults()
            .SetDevice(&m_nvnDevice);

        m_queue = NVN::makeBuilderInitialized<nvn::Queue>(queueBuilder);
    }

    NVNGfxDeviceContext::CommandBufferMemoryBlock* NVNGfxDeviceContext::getAvailableCommandBufferMemoryBlock()
    {
        if (m_availableCBMemBlocks.empty())
        {
            addNewCommandBufferMemoryBlock();
        }
        auto result = m_availableCBMemBlocks.back();
        m_availableCBMemBlocks.pop_back();
        return result;
    }

    void NVNGfxDeviceContext::addNewCommandBufferMemoryBlock()
    {
        uPtr memSize = COMMAND_BUFFER_MEMORY_CHUNK_SIZE;

        CommandBufferMemoryBlock * cbMemoryBlock = new (MemoryId::mId_GfxAdapter) CommandBufferMemoryBlock();
        cbMemoryBlock->cpuAddress = MemoryPageAreaAllocator::allocArea(memSize);
        ITF_ASSERT(BitTweak::IsAligned(cbMemoryBlock->cpuAddress, m_cbMemoryBlockAlignment));
        cbMemoryBlock->offset = 0;
        cbMemoryBlock->size = memSize;
        ITF_VERIFY(cbMemoryBlock->sync.Initialize(&m_nvnDevice));

        nvn::MemoryPoolBuilder poolBuilder;
        poolBuilder.SetDefaults()
            .SetDevice(&m_nvnDevice)
        #ifdef ITF_NX
            .SetFlags(nvn::MemoryPoolFlags::CPU_UNCACHED | nvn::MemoryPoolFlags::GPU_UNCACHED)
        #else
            // Cache coherency on Ounce
            .SetFlags(nvn::MemoryPoolFlags::CPU_CACHED | nvn::MemoryPoolFlags::GPU_UNCACHED)
        #endif
            .SetStorage(cbMemoryBlock->cpuAddress, memSize);

        ITF_VERIFY(cbMemoryBlock->pool.Initialize(&poolBuilder));

        m_availableCBMemBlocks.push_back(cbMemoryBlock);
    }

    NVNGfxDeviceContext::CommandBufferControlBlock NVNGfxDeviceContext::getAvailableCommandBufferControlBlock()
    {
        if (m_availableCBControlBlocks.empty())
        {
            addNewCommandBufferControlBlock();
        }
        auto result = m_availableCBControlBlocks.back();
        m_availableCBControlBlocks.pop_back();
        return result;
    }

    void NVNGfxDeviceContext::addNewCommandBufferControlBlock()
    {
        uPtr memSize = COMMAND_BUFFER_CONTROL_CHUNK_SIZE;
        uPtr alignment = m_cbControlBlockAlignment;

        u8* data = reinterpret_cast<u8*>(Memory::alignedMallocCategory(memSize, alignment, MemoryId::mId_GfxAdapter));

        m_availableCBControlBlocks.push_back(CommandBufferControlBlock{ data, memSize });
    }

    void NVNGfxDeviceContext::initCommandBuffer()
    {
        m_cbMemoryBlockAlignment = NVN::VRAMAllocator::getDeviceProperty(nvn::DeviceInfo::COMMAND_BUFFER_COMMAND_ALIGNMENT);
        m_cbControlBlockAlignment = NVN::VRAMAllocator::getDeviceProperty(nvn::DeviceInfo::COMMAND_BUFFER_CONTROL_ALIGNMENT);

        m_commandBuffer = NVN::makeUnique<nvn::CommandBuffer>();
        m_commandBuffer->Initialize(&m_nvnDevice);
        m_commandBuffer->SetMemoryCallback(methodToCallback<&NVNGfxDeviceContext::outOfMemoryCallback>());
        m_commandBuffer->SetMemoryCallbackData(this);
    }

    void NVNGfxDeviceContext::setNextCommandBuffer()
    {
        m_currentCBMemBlocks.push_back(getAvailableCommandBufferMemoryBlock());
        auto memBlock = m_currentCBMemBlocks.back();
        m_commandBuffer->AddCommandMemory(&memBlock->pool, memBlock->offset, memBlock->size);
    }

    void NVNGfxDeviceContext::outOfMemoryCallback(nvn::CommandBuffer* _cmdBuf, nvn::CommandBufferMemoryEvent::Enum _event, size_t _minSize)
    {
        ITF_ASSERT(_cmdBuf == m_commandBuffer.get());

        if (_event == nvn::CommandBufferMemoryEvent::OUT_OF_COMMAND_MEMORY)
        {
            setNextCommandBuffer();
        }
        else if (_event == nvn::CommandBufferMemoryEvent::OUT_OF_CONTROL_MEMORY)
        {
            m_currentCBControlBlocks.push_back(getAvailableCommandBufferControlBlock());
            auto controlBlock = m_currentCBControlBlocks.back();
            _cmdBuf->AddControlMemory(controlBlock.data, controlBlock.size);
        }
    }

    void NVNGfxDeviceContext::initFrameStampAndLabels()
    {
        m_nbSubmittedFrame = 0;

        m_renderingTimeStamp.init("TimeStamp_rendering");
        m_flipTimeStamp.init("TimeStamp_flip");
        m_nbGPURenderedFrame.init(0, "Label_nbGPURenderedFrame");
    }

    void NVNGfxDeviceContext::pushNBGPURenderedFrameCommand(u64 _newGpuFrameCount)
    {
        m_nbGPURenderedFrame.setWriteOnEOP(m_commandBuffer.get(), u32(_newGpuFrameCount));
    }

    void NVNGfxDeviceContext::waitUntilNBGPURenderedFrame(u64 _gpuFrameToWait)
    {
        u32 gpuFrameToWait32Bit = u32(_gpuFrameToWait);

        m_nbGPURenderedFrame.cpuWaitUntil(
            [&](u64 lastGPUProcessedFrame)
            {
                return lastGPUProcessedFrame >= gpuFrameToWait32Bit
                    // Try to survive a wrap of the counter as the command is only putting a 32 bit value:
                    // If the difference if too high, it means something was wrong, and we continue
                    || (lastGPUProcessedFrame <= _gpuFrameToWait && (_gpuFrameToWait - u32(lastGPUProcessedFrame) > 100000)) ;
            });
    }

    void NVNGfxDeviceContext::waitUntilGPUStalled()
    {
        m_queue->Flush();
        m_queue->Finish();
        waitUntilNBGPURenderedFrame(getNBGPURenderedFrame()); // probably not useful...
    }

    void NVNGfxDeviceContext::startNewFrame(nvn::Window* _nvnWindow, u32 _displayBufferIndex)
    {
        SimpleProfileTimerScope cpuWaitForGPUProfilerScope(m_lastCPUWaitForGPU);

        if (m_nbSubmittedFrame >= COMMAND_BUFFER_COUNT) // do not wait until 2 frames produced
        {
            static const u64 kMaxWaitTime = 1000000000; // 1 second in nanoseconds
            Vector<CommandBufferMemoryBlock*> & memBlocks = m_submittedCommandBufferMemBlocks[getCurrentBufferIndex()];
            for (auto & memBlock : memBlocks)
            {
                nvn::SyncWaitResult waitRes = memBlock->sync.Wait(kMaxWaitTime);
                ITF_UNUSED(waitRes);
                ITF_ASSERT_CRASH(waitRes != nvn::SyncWaitResult::TIMEOUT_EXPIRED, "Timeout expired while waiting for command buffer memory block sync. GPU may be stalled or not responding.");
                ITF_ASSERT_CRASH(waitRes != nvn::SyncWaitResult::FAILED, "Command buffer memory block sync failed. This should not happen (missind fence ?).");
            }

            m_availableCBMemBlocks.insert(m_availableCBMemBlocks.end(), memBlocks.begin(), memBlocks.end());
            memBlocks.clear();

            // Ensure the GPU has finished processing at least the nb submitted frames - 1
            waitUntilNBGPURenderedFrame(m_nbSubmittedFrame - 1);
        }

        _nvnWindow->AcquireTexture(&m_displayReleaseSync, &m_currentRTIndex);
        ITF_ASSERT(m_bufferIndex == m_currentRTIndex);
        ITF_ASSERT(m_bufferIndex == _displayBufferIndex);

        setNextCommandBuffer();

        m_commandBuffer->BeginRecording();

        m_commandBuffer->WaitSync(&m_displayReleaseSync);

        if (m_nbSubmittedFrame >= 1)
            m_flipTimeStamp.stop(m_commandBuffer.get());
        m_renderingTimeStamp.start(m_commandBuffer.get());
    }

    void NVNGfxDeviceContext::setCurrentCBControlBlocksAsAvailable()
    {
        m_availableCBControlBlocks.insert(m_availableCBControlBlocks.end(), m_currentCBControlBlocks.begin(), m_currentCBControlBlocks.end());
        m_currentCBControlBlocks.clear();
    }

    void NVNGfxDeviceContext::fenceCurrentCBMemBlocks()
    {
        // Fence mem blocks of the submitted command buffer.
        for (auto& memBlock : m_currentCBMemBlocks)
        {
            m_queue->FenceSync(&memBlock->sync, nvn::SyncCondition::ALL_GPU_COMMANDS_COMPLETE, nvn::SyncFlagBits::FLUSH_FOR_CPU);
        }

        Vector<CommandBufferMemoryBlock*>& memBlockSubmitted = m_submittedCommandBufferMemBlocks[getCurrentBufferIndex()];
        ITF_ASSERT(memBlockSubmitted.empty());
        memBlockSubmitted.insert(memBlockSubmitted.end(), m_currentCBMemBlocks.begin(), m_currentCBMemBlocks.end());
        m_currentCBMemBlocks.clear();
    }

    void NVNGfxDeviceContext::endFrameAndFlip(nvn::Window* _nvnWindow, u32 displayBufferIndex)
    {
        m_renderingTimeStamp.stop(m_commandBuffer.get());
        m_flipTimeStamp.start(m_commandBuffer.get()); // we will stop it at the next frame start

        pushNBGPURenderedFrameCommand(m_nbSubmittedFrame + 1);  // +1 as it is the number of rendered frame (so never 0)

        nvn::CommandHandle cmdHandle = m_commandBuffer->EndRecording();
        m_queue->SubmitCommands(1, &cmdHandle);

        // After the submit the control blocks are available for reuse.
        setCurrentCBControlBlocksAsAvailable();

        // But for the memory blocks, we need to be sure the GPU is no more touching them.
        // So we add a fence, and move them in the submitted blocks.
        // We will wait for them at the N+2 frame start and put thme back to available list.
        fenceCurrentCBMemBlocks();

        m_queue->PresentTexture(_nvnWindow, getCurrentBufferIndex());

        m_nbSubmittedFrame++;
        advanceToNextContext();
    }

    void NVNGfxDeviceContext::resetWindow()
    {
        m_bufferIndex = 0;
    }

    void NVNGfxDeviceContext::checkProfilingStart()
    {
    #if ITF_ENABLE_GPU_CAPTURE
        ITF_ASSERT(0); // $GB todo
    #endif // ITF_ENABLE_GPU_CAPTURE
    }

} // namespace ITF
