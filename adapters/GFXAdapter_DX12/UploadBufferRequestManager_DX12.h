#pragma once

#include "core/types.h"
#include "core/container/extendedVector.h"
#include "core/container/extendedMap.h"

#include "VRAMAllocator/VRAMRelocHdl.h"

#include "core/system/Synchronize.h"

#include "core/memory/UniquePtr.h"

#include <functional>

namespace ITF::DX12
{

    class UploadBufferRequestManager
    {
    public:
        explicit UploadBufferRequestManager(ComPtr<ID3D12Device> _device);
        UploadBufferRequestManager(const UploadBufferRequestManager&) = delete;
        ~UploadBufferRequestManager() = default;

        UploadBufferRequestManager& operator = (const UploadBufferRequestManager&) = delete;

        enum : ux { NB_MAX_MIP_LEVEL = 17 };

        struct UploadRequest
        {
            DefaultResourceBufferVRAMRelocHandle destination{};
            u64 fenceValue = 0;
        };

        // Create an upload request to copy the data into the destination buffer.
        // _data buffer is CPU copied into an internal upload buffer (so can be freed then).
        // The data is copied when pushCopyCommands() is called.
        // Use it for vertex and index buffers, or any other buffer that is not a texture.
        [[nodiscard]] UploadRequest allocCopyAndRequestUpload(const u8 * _data, uSize _dataSize, DX12::VramMemoryCategory _category);

        // Same as above, but a functor is invoked to fill the upload buffer (for instance by file loading operation).
        using DataFillCallback = std::function<void(u8* _data, uSize _dataSize)>;
        [[nodiscard]] UploadRequest allocCopyAndRequestUpload(uSize _dataSize, DX12::VramMemoryCategory _category, DataFillCallback _fillCallback);

        // Shortcut of the above when the data is already in an upload buffer.
        // Requires that _sourceBuffer.getStorage().size == _destinationBuffer.getStorage().size
        [[nodiscard]] UploadRequest requestUpload(DX12::UploadBufferVRAMRelocHandle _sourceBuffer, DX12::DefaultResourceBufferVRAMRelocHandle _destinationBuffer);

        // Uploads for textures.
        [[nodiscard]] UploadRequest requestTextureUpload(u32 _width, u32 _height, u32 _nbMipLevel, DXGI_FORMAT _format, D3D12_SUBRESOURCE_DATA * _data);
        // Same as above, but a functor is invoked to fill the upload buffer (for instance by file loading operation).
        using TextureDataFillCallback = std::function<void(u8* _dest, ux _mipLevel, ux _row, u64 _rowSize)>;
        [[nodiscard]] UploadRequest requestTextureUpload(u32 _width, u32 _height, u32 _nbMipLevel, DXGI_FORMAT _format, TextureDataFillCallback _fillCallback);

        // Shortcut of the above when the data is already in an upload buffer.
        [[nodiscard]] UploadRequest requestTextureUpload(DX12::UploadBufferVRAMRelocHandle _sourceBuffer, DX12::DefaultResourceBufferVRAMRelocHandle _destinationBuffer);

        void pushRequestedCopyCommands(); // To be called once per frame, to push the copy commands registered to the GPU.

        void syncCopyCommandsWithGFX(ID3D12GraphicsCommandList* _gfxCmdListForTransition, ID3D12CommandQueue * _gfxQueueForWait);
        void waitUntilAllCopyDone(ID3D12GraphicsCommandList* _gfxCmdListForTransition, ID3D12CommandQueue* _gfxQueueForWait); // CPU wait until all copy commands are done.

        void waitFence(u64 _fenceValue);

    #ifdef ITF_WIN64
        using CopyCommandList = ID3D12GraphicsCommandList;
    #else
        using CopyCommandList = ID3D12XboxDmaCommandList;
    #endif

        ID3D12CommandQueue * getCopyQueue() const
        {
            return m_copyQueue.Get();
        }

        CopyCommandList* getCopyCommandList() const
        {
            return m_commandList.Get();
        }

    private:
        ComPtr<ID3D12Device> m_device{};
        ComPtr<ID3D12CommandQueue> m_copyQueue{};

        ComPtr<CopyCommandList> m_commandList{};

        struct CopyCommand
        {
            UploadBufferVRAMRelocHandle source{};
            DefaultResourceBufferVRAMRelocHandle destination{};

            bool isTexture = false;
            u32 nbMipLevel = 0;
            D3D12_PLACED_SUBRESOURCE_FOOTPRINT layouts[NB_MAX_MIP_LEVEL];
            u32 nbRows[NB_MAX_MIP_LEVEL];
            u64 rowSizes[NB_MAX_MIP_LEVEL];

            void addUploadCommands(CopyCommandList* _cmdList);
            void addRegularUploadCommand(CopyCommandList* _cmdList);
            void addTextureUploadCommands(CopyCommandList* _cmdList);
        };

        struct CommandSet
        {
            static CommandSet buildWithAllocator(ID3D12Device* _device);
            Vector<CopyCommand> inProgressBatch;
            ComPtr<ID3D12CommandAllocator> allocator{};
            u64 fenceValue = 0;
        };

        CommandSet getNewOrUnusedCommandSet();
        // m_unusedCommandSets => Command sets that are available to be used (avoid excessive allocations).
        Vector<CommandSet> m_unusedCommandSets{};
        // m_gpuRunningCommandSets => Commands sets that have been submitted to the GPU but with no wait in the queue yet.
        Vector<CommandSet> m_gpuRunningCommandSets{};
        // m_gpuSyncedCommandSets => Commands sets that have been submitted to the GPU with a wait in the gfx queue.
        // Once the fence value has been reached on the GPU, they are moved to m_unusedCommandSets.
        Vector<CommandSet> m_gpuSyncedCommandSets{};

        // syncCommandSet:
        // - Moves the command set finished from running to unused.
        void syncCommandSetWithQueue(ID3D12CommandQueue* _gfxQueueForWait, CommandSet & _commandSet);
        // - Transition all destination resources (in the gfx cmd list).
        void transitionCommandSet(ID3D12GraphicsCommandList* _gfxCmdListForTransition, CommandSet & _commandSet);

        u64 pushCopyCommandsToList(CommandSet& _commandSet);
        void syncCommandSetsDoneForReuse();

        ComPtr<ID3D12Fence> m_fence{};
        HANDLE m_fenceEvent = nullptr;

        Mutex m_mutex;
        Vector<CopyCommand> m_pendingBatch;
        u64 m_newBatchFence = 1; // we assume 0 is an invalid/always signaled fence value.

        f64 m_lastCPUWaitForCopyCompletionDuration = 0;

        Vector<D3D12_RESOURCE_BARRIER> m_barriers{};
    };
}
