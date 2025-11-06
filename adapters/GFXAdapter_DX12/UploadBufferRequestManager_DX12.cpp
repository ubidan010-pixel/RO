#include "precompiled_GFXAdapter_DX12.h"

#include "engine/profiler/profilerMacro.h"
#include "DX12/Helpers.h"

#include "UploadBufferRequestManager_DX12.h"

namespace ITF::DX12
{

#ifdef ITF_WIN64
    D3D12_COMMAND_LIST_TYPE COPY_COMMAND_LIST_TYPE = D3D12_COMMAND_LIST_TYPE_COPY;
#else
    D3D12_COMMAND_LIST_TYPE COPY_COMMAND_LIST_TYPE = D3D12XBOX_COMMAND_LIST_TYPE_DMA;
#endif


    UploadBufferRequestManager::UploadBufferRequestManager(ComPtr<ID3D12Device> _device)
        : m_device(std::move(_device))
    {
        D3D12_COMMAND_QUEUE_DESC copyQueueDesc = {};
        copyQueueDesc.Type = COPY_COMMAND_LIST_TYPE;
        ITF_VERIFY_HR_CALL(m_device->CreateCommandQueue(&copyQueueDesc, DX12_IID_COMPTR_ARGS(m_copyQueue)));

        ITF_VERIFY_HR_CALL(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, DX12_IID_COMPTR_ARGS(m_fence)));
        m_fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);

        // we need a command list with an allocator, so we create one unused CommandSet and assign its allocator.
        m_unusedCommandSets.push_back(CommandSet::buildWithAllocator(m_device.Get()));

        ITF_VERIFY_HR_CALL(m_device->CreateCommandList(0, COPY_COMMAND_LIST_TYPE, m_unusedCommandSets.back().allocator.Get(), nullptr, DX12_IID_COMPTR_ARGS(m_commandList)));
        m_commandList->Close();

        // Reserve some space to avoid allocations during normal usage.
        m_pendingBatch.reserve(256);
    }


    UploadBufferRequestManager::UploadRequest UploadBufferRequestManager::allocCopyAndRequestUpload(const u8* _data, uSize _dataSize, DX12::VramMemoryCategory _category)
    {
        return allocCopyAndRequestUpload(_dataSize, _category, [_data](u8* destPtr, uSize size)
        {
            std::memcpy(destPtr, _data, size);
        });
    }

    UploadBufferRequestManager::UploadRequest UploadBufferRequestManager::allocCopyAndRequestUpload(uSize _dataSize, DX12::VramMemoryCategory _category, DataFillCallback _fillCallback)
    {
        DX12::UploadBufferVRAMRelocHandle source{};
        source.alloc(_dataSize, _category);

        DX12::DefaultResourceBufferVRAMRelocHandle destination{};
        destination.alloc(_dataSize, _category);

        void* mappedPtr = nullptr;
        D3D12_RANGE readRange{0, 0};
        ITF_VERIFY_HR_CALL(source.getD3DResource()->Map(0, &readRange, &mappedPtr));

        std::move(_fillCallback)(static_cast<u8*>(mappedPtr), _dataSize);

        source.getD3DResource()->Unmap(0, nullptr);

        return requestUpload(std::move(source), std::move(destination));
    }

    // Shortcut of the above when the data is already in an upload buffer.
    // Requires that _sourceBuffer.getStorage().size == _destinationBuffer.getStorage().size
    UploadBufferRequestManager::UploadRequest UploadBufferRequestManager::requestUpload(DX12::UploadBufferVRAMRelocHandle _sourceBuffer, DX12::DefaultResourceBufferVRAMRelocHandle _destinationBuffer)
    {
        UploadRequest req{};
        // Assuming UploadRequest contains source/destination members (no header context provided).
        req.destination = _destinationBuffer;
        {
            ScopeLockMutex lock(m_mutex);
            m_pendingBatch.push_back({ std::move(_sourceBuffer), std::move(_destinationBuffer) });
            req.fenceValue = m_newBatchFence;
        }
        return req;
    }

    u64 UploadBufferRequestManager::pushCopyCommandsToList(CommandSet& _commandSet)
    {
        u64 submittedBatchFence = 0;

        ITF_ASSERT(_commandSet.inProgressBatch.empty());

        {
            // limit the lock scope to avoid contention at maximum.
            ScopeLockMutex lock(m_mutex);
            submittedBatchFence = m_newBatchFence;
            m_newBatchFence++;
            // m_inProgressBatch assign could allocate, but should maximize after a while. We reserved in the constructor.
            _commandSet.inProgressBatch.assign(std::make_move_iterator(m_pendingBatch.begin()), std::make_move_iterator(m_pendingBatch.end()));
            m_pendingBatch.clear();
        }

        for (auto & copy : _commandSet.inProgressBatch)
        {
            copy.addUploadCommands(m_commandList.Get());
        }

        return submittedBatchFence;
    }

    UploadBufferRequestManager::CommandSet UploadBufferRequestManager::CommandSet::buildWithAllocator(ID3D12Device * _device)
    {
        CommandSet commandSet;
        commandSet.inProgressBatch.reserve(256); // avoid allocations during normal usage
        ITF_VERIFY_HR_CALL(_device->CreateCommandAllocator(COPY_COMMAND_LIST_TYPE, DX12_IID_COMPTR_ARGS(commandSet.allocator)));
        return commandSet;
    }

    UploadBufferRequestManager::CommandSet UploadBufferRequestManager::getNewOrUnusedCommandSet()
    {
        if (m_unusedCommandSets.empty())
        {
            m_unusedCommandSets.push_back(CommandSet::buildWithAllocator(m_device.Get()));
        }
        CommandSet commandSet = std::move(m_unusedCommandSets.back());
        m_unusedCommandSets.pop_back();
        return commandSet;
    }

    void UploadBufferRequestManager::pushRequestedCopyCommands()
    {
        syncCommandSetsDoneForReuse();

        {
            ScopeLockMutex lock(m_mutex);
            if (m_pendingBatch.empty())
            {
                return; // nothing to do
            }
        }

        m_gpuRunningCommandSets.push_back(getNewOrUnusedCommandSet());
        CommandSet & commandSet = m_gpuRunningCommandSets.back();

        m_commandList->Reset(commandSet.allocator.Get(), nullptr);

        u64 submittedBatchFence = pushCopyCommandsToList(commandSet);
        m_commandList->Close();
        ID3D12CommandList* lists[] = { m_commandList.Get() };
        m_copyQueue->ExecuteCommandLists(1, lists);
        commandSet.fenceValue = submittedBatchFence;
        m_copyQueue->Signal(m_fence.Get(), submittedBatchFence);
    }

    void UploadBufferRequestManager::syncCommandSetWithQueue(ID3D12CommandQueue* _gfxQueueForWait, CommandSet & _commandSet)
    {
        _gfxQueueForWait->Wait(m_fence.Get(), _commandSet.fenceValue);
    }

    void UploadBufferRequestManager::transitionCommandSet(ID3D12GraphicsCommandList* _gfxCmdListForTransition, CommandSet& _commandSet)
    {
        m_barriers.clear();
        m_barriers.reserve(_commandSet.inProgressBatch.size());
        for (auto& cmd : _commandSet.inProgressBatch)
        {
            D3D12_RESOURCE_STATES destState =
                cmd.isTexture ?
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE :
                (D3D12_RESOURCE_STATE_INDEX_BUFFER | D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
            m_barriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(cmd.destination.getD3DResource(),
                D3D12_RESOURCE_STATE_COPY_DEST,
                destState));
        }
        _gfxCmdListForTransition->ResourceBarrier(m_barriers.size(), m_barriers.data());
    }

    void UploadBufferRequestManager::syncCommandSetsDoneForReuse()
    {
        for (auto it = m_gpuSyncedCommandSets.begin(); it != m_gpuSyncedCommandSets.end();)
        {
            CommandSet& commandSet = *it;
            if (m_fence->GetCompletedValue() >= commandSet.fenceValue)
            {
                commandSet.allocator->Reset();
                commandSet.inProgressBatch.clear(); // delete the upload buffers here
                m_unusedCommandSets.push_back(std::move(commandSet));

                it = m_gpuSyncedCommandSets.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    void UploadBufferRequestManager::syncCopyCommandsWithGFX(ID3D12GraphicsCommandList* _gfxCmdListForTransition, ID3D12CommandQueue* _gfxQueueForWait)
    {
        for (CommandSet& commandSet : m_gpuRunningCommandSets)
        {
            syncCommandSetWithQueue(_gfxQueueForWait, commandSet);
            transitionCommandSet(_gfxCmdListForTransition, commandSet);
            m_gpuSyncedCommandSets.push_back(std::move(commandSet));
        }
        m_gpuRunningCommandSets.clear();
    }

    void UploadBufferRequestManager::waitUntilAllCopyDone(ID3D12GraphicsCommandList* _gfxCmdListForTransition, ID3D12CommandQueue* _gfxQueueForWait)
    {
        syncCopyCommandsWithGFX(_gfxCmdListForTransition, _gfxQueueForWait);
        u64 lastFenceValue = m_newBatchFence - 1;
        waitFence(lastFenceValue);
    }

    void UploadBufferRequestManager::waitFence(u64 _fenceValue)
    {
        if (m_fence->GetCompletedValue() < _fenceValue)
        {
            ITF_VERIFY_HR_CALL(m_fence->SetEventOnCompletion(_fenceValue, m_fenceEvent));
            ::WaitForSingleObject(m_fenceEvent, INFINITE);
        }
    }

    static void copyMipsToUploadTexture(
        ID3D12Resource* _uploadResource,
        ux _nbMipLevel, D3D12_PLACED_SUBRESOURCE_FOOTPRINT* _layouts, ux* _nbRows, u64* _rowSizes,
        UploadBufferRequestManager::TextureDataFillCallback _fillCallback)
    {
        void* mappedPtr = nullptr;
        D3D12_RANGE readRange{ 0, 0 };
        ITF_VERIFY_HR_CALL(_uploadResource->Map(0, &readRange, &mappedPtr));
        u8* dest = static_cast<u8*>(mappedPtr);

        for (u32 i = 0; i < _nbMipLevel; ++i)
        {
            u8* subDest = dest + _layouts[i].Offset;
            for (UINT row = 0; row < _nbRows[i]; ++row)
            {
                _fillCallback(subDest + row * _layouts[i].Footprint.RowPitch, i, row, _rowSizes[i]);
            }
        }

        _uploadResource->Unmap(0, nullptr);

    }

    UploadBufferRequestManager::UploadRequest UploadBufferRequestManager::requestTextureUpload(DX12::UploadBufferVRAMRelocHandle _sourceBuffer, DX12::DefaultResourceBufferVRAMRelocHandle _destinationBuffer)
    {
        CopyCommand cmd{};
        cmd.isTexture = true;
        cmd.source = std::move(_sourceBuffer);
        cmd.destination = std::move(_destinationBuffer);

        D3D12_RESOURCE_DESC desc = cmd.source.getD3DResource()->GetDesc();
        cmd.nbMipLevel = desc.MipLevels;

        u64 uploadBufferRequiredSize = 0;
        m_device->GetCopyableFootprints(
            &desc,
            0,
            cmd.nbMipLevel,
            0,
            cmd.layouts,
            cmd.nbRows,
            cmd.rowSizes,
            &uploadBufferRequiredSize);

        ITF_ASSERT(cmd.source.getStorage().size == uploadBufferRequiredSize);

        UploadRequest request{};
        request.destination = cmd.destination;
        {
            ScopeLockMutex lock(m_mutex);
            request.fenceValue = m_newBatchFence;
            m_pendingBatch.push_back(std::move(cmd));
        }

        return request;
    }

    UploadBufferRequestManager::UploadRequest UploadBufferRequestManager::requestTextureUpload(u32 _width, u32 _height, u32 _nbMipLevel, DXGI_FORMAT _format, D3D12_SUBRESOURCE_DATA* _data)
    {
        return requestTextureUpload(_width, _height, _nbMipLevel, _format,
            [_data](u8* _dest, ux _mipLevel, ux _row, u64 _rowSize)
            {
                const u8* src = static_cast<const u8*>(_data[_mipLevel].pData);
                std::memcpy(_dest, src + _row * _data[_mipLevel].RowPitch, _rowSize);
            });
    }

    UploadBufferRequestManager::UploadRequest UploadBufferRequestManager::requestTextureUpload(u32 _width, u32 _height, u32 _nbMipLevel, DXGI_FORMAT _format, TextureDataFillCallback _fillCallback)
    {
        CopyCommand cmd{};
        cmd.isTexture = true;
        cmd.nbMipLevel = _nbMipLevel;

        D3D12_RESOURCE_DESC desc = {};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        desc.Alignment = 0;
        desc.Width = _width;
        desc.Height = _height;
        desc.DepthOrArraySize = 1;
        desc.MipLevels = static_cast<UINT16>(cmd.nbMipLevel);
        desc.Format = _format;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        desc.Flags = D3D12_RESOURCE_FLAG_NONE;

        D3D12_RESOURCE_ALLOCATION_INFO allocInfo = m_device->GetResourceAllocationInfo(0, 1, &desc);

        cmd.destination.allocAligned(DX12::VRAMChunkSize(allocInfo.SizeInBytes), DX12::VRAMChunkSize(allocInfo.Alignment), desc, DX12::VRAM_Texture);

        u64 uploadBufferRequiredSize = 0;
        m_device->GetCopyableFootprints(
            &desc,
            0,
            cmd.nbMipLevel,
            0,
            cmd.layouts,
            cmd.nbRows,
            cmd.rowSizes,
            &uploadBufferRequiredSize);

        CD3DX12_RESOURCE_DESC uploadBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferRequiredSize);
        allocInfo = m_device->GetResourceAllocationInfo(0, 1, &uploadBufferDesc);
        cmd.source.allocAligned(DX12::VRAMChunkSize(allocInfo.SizeInBytes), DX12::VRAMChunkSize(allocInfo.Alignment), DX12::VRAM_Texture);

        copyMipsToUploadTexture(cmd.source.getD3DResource(), cmd.nbMipLevel, cmd.layouts, cmd.nbRows, cmd.rowSizes, std::move(_fillCallback));

        UploadRequest request{};
        request.destination = cmd.destination;
        {
            ScopeLockMutex lock(m_mutex);
            request.fenceValue = m_newBatchFence;
            m_pendingBatch.push_back(std::move(cmd));
        }

        return request;
    }

    void UploadBufferRequestManager::CopyCommand::addUploadCommands(CopyCommandList* _cmdList)
    {
        if (isTexture)
        {
            addTextureUploadCommands(_cmdList);
        }
        else
        {
            addRegularUploadCommand(_cmdList);
        }
    }
    void UploadBufferRequestManager::CopyCommand::addRegularUploadCommand(CopyCommandList* _cmdList)
    {
        _cmdList->CopyBufferRegion(destination.getD3DResource(), 0, source.getD3DResource(), 0, source.getResourceDesc().Width);
    }

    void UploadBufferRequestManager::CopyCommand::addTextureUploadCommands(CopyCommandList* _cmdList)
    {
        ID3D12Resource* uploadResource = source.getD3DResource();
        ID3D12Resource * destResource = destination.getD3DResource();

        for (ux i = 0; i < nbMipLevel; ++i)
        {
            D3D12_TEXTURE_COPY_LOCATION dst = {};
            dst.pResource = destResource;
            dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
            dst.SubresourceIndex = i;

            D3D12_TEXTURE_COPY_LOCATION srcLoc = {};
            srcLoc.pResource = uploadResource;
            srcLoc.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
            srcLoc.PlacedFootprint = layouts[i];

            _cmdList->CopyTextureRegion(&dst, 0, 0, 0, &srcLoc, nullptr);
        }
    }
}
