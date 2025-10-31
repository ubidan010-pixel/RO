#pragma once

#include "core/math/MathTools.h"
#include "NVN/Helpers.h"
#include "Label_NVN.h"

namespace ITF
{
    // Device Context
    class NVNGfxDeviceContext
    {
    public:
        explicit NVNGfxDeviceContext(nvn::Device& _nvnDevice);
        ~NVNGfxDeviceContext();

        void startNewFrame(nvn::Window * _nvnWindow, u32 displayBufferIndex);
        bool isRecording() const { return m_commandBuffer->IsRecording(); }
        void endFrameAndFlip(nvn::Window* _nvnWindow, u32 displayBufferIndex);

        void resetWindow(); // when window changed because of docking/undocking

        void waitUntilNBGPURenderedFrame(u64 _nbGPUFrame);
        void waitUntilGPUStalled();

        nvn::CommandBuffer * getContext() const { return m_commandBuffer.get(); }
        nvn::Queue * getQueue() const { return m_queue.get(); }

        u32 getCurrentBufferIndex() const { return m_bufferIndex; }
        u32 getCurrentGPUBufferIndex() const { return (m_bufferIndex + COMMAND_BUFFER_COUNT - 1) % COMMAND_BUFFER_COUNT; }
        u32 getNextAvailableGPUBufferIndex() const { return (m_bufferIndex + 1) % COMMAND_BUFFER_COUNT; }

        u64 getNbSubmittedFrame() const { return m_nbSubmittedFrame; } // aka frame in production by the cpu
        u64 getNBGPURenderedFrame() const { return m_nbGPURenderedFrame.getCounter(); }

        f32 getLastCPUWaitForGPU() const { return m_lastCPUWaitForGPU; }
        f32 getLastGPURenderingTime() const { return m_renderingTimeStamp.getTime(); }
        f32 getLastGPUFlipTime() const { return m_flipTimeStamp.getTime(); }

        enum : ux
        {
            COMMAND_BUFFER_COUNT = 2,
            COMMAND_BUFFER_MEMORY_CHUNK_SIZE = 64*1024*1024,
            COMMAND_BUFFER_CONTROL_CHUNK_SIZE = 64 * 1024 * 1024,
        };

    private:
        void create();
        ux advanceToNextContext() { m_bufferIndex = (m_bufferIndex + 1) % COMMAND_BUFFER_COUNT; return m_bufferIndex; }
        void checkProfilingStart();
        void outOfMemoryCallback(nvn::CommandBuffer* _cmdBuf, nvn::CommandBufferMemoryEvent::Enum _event, size_t _minSize);
        void pushNBGPURenderedFrameCommand(u64 _newGpuFrameCount);

        nvn::Device& m_nvnDevice;
        int m_currentRTIndex = 0;

        void initQueue();
        NVN::UniquePtr<nvn::Queue> m_queue;

        void initCommandBuffer();
        NVN::UniquePtr<nvn::CommandBuffer> m_commandBuffer;

        nvn::Sync m_displayReleaseSync;

        struct CommandBufferMemoryBlock
        {
            void* cpuAddress;
            nvn::MemoryPool pool;
            iPtr offset = 0;
            uPtr size = 0;
            nvn::Sync sync;
        };
        CommandBufferMemoryBlock* getAvailableCommandBufferMemoryBlock();
        void addNewCommandBufferMemoryBlock();
        void fenceCurrentCBMemBlocks();
        void setNextCommandBuffer();

        int m_cbMemoryBlockAlignment = 8;
        Vector<CommandBufferMemoryBlock*> m_availableCBMemBlocks; // Memory blocks available for command buffer allocation. Fetched and eventually allocated by out of memory callbacks.
        Vector<CommandBufferMemoryBlock*> m_currentCBMemBlocks; // Memory blocks currently used by the command buffers set in contruction.
        Vector<CommandBufferMemoryBlock*> m_submittedCommandBufferMemBlocks[COMMAND_BUFFER_COUNT]; // Memory blocks currently accessed by the GPU. m_submittedCommandBufferBlocks[m_bufferIndex] is the one that will be submitted soon or just was.
        // At end of the frame:
        // - we add a fencesync command of each element of m_currentCBMemBlocks
        // - we submit the command buffer to the queue
        // - we move m_currentCBMemBlocks to m_submittedCommandBufferBlocks[m_bufferIndex]
        // - we increment m_nbSubmittedFrame and m_bufferIndex (modulo COMMAND_BUFFER_COUNT)
        // At the start of the new frame:
        // - we wait on CPU for the GPU to finish processing all m_submittedCommandBufferBlocks[(m_bufferIndex-1)%COMMAND_BUFFER_COUNT] (by calling sync.wait() on each element)
        //      - this is also ensuring that at this point only 1 frame is in production by the GPU.
        // - we move the content to m_submittedCommandBufferBlocks[(m_bufferIndex-1)%COMMAND_BUFFER_COUNT] to the end of m_availableCBMemBlocks

        // Control buffer memory it just regular memory, never used by the GPU.
        // So we use the regular CPU memory allocator.
        struct CommandBufferControlBlock
        {
            u8* data = nullptr;
            uPtr size = 0;
        };
        CommandBufferControlBlock getAvailableCommandBufferControlBlock();
        void addNewCommandBufferControlBlock(); // add it to m_availableCBControlBlocks
        void setCurrentCBControlBlocksAsAvailable();
        int m_cbControlBlockAlignment = 8;
        Vector<CommandBufferControlBlock> m_availableCBControlBlocks; // Memory blocks available for command buffer allocation. Fetched by out of memory callbacks.
        Vector<CommandBufferControlBlock> m_currentCBControlBlocks; // Memory blocks currently used by the command buffers set in contruction.
        // At end of the frame, after the CommandBuffer :
        // - we move the content of m_currentCBControlBlocks to m_availableCBControlBlocks

        u32 m_bufferIndex = 0; // 0 to COMMAND_BUFFER_COUNT-1
        u32 m_nbSubmittedFrame = 0; // nbFrame sent to the GPU. Also the frame number producing command on the CPU side.
        f64 m_lastCPUWaitForGPU = 0.; // Time (in seconds) spent waiting for the GPU before producing new gfx command.

        void initFrameStampAndLabels();
        NVN::TimeStamp<2>          m_renderingTimeStamp;
        NVN::TimeStamp<2>          m_flipTimeStamp;

        NVN::Label                 m_nbGPURenderedFrame;
    };

} // namespace ITF
