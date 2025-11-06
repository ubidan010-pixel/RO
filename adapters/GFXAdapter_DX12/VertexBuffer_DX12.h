#pragma once

#include "core/types.h"

#include "core/math/BooleanLogic.h"
#include "DX12/Helpers.h"
#include "VRAMAllocator/VRAMRelocHdl.h"

#include "UploadBufferRequestManager_DX12.h"

namespace ITF::DX12
{
    class Buffer
    {
    public:

        Buffer() = default;
        Buffer(const Buffer&) = delete;
        ~Buffer()
        {
            checkInvariant();
        }

        Buffer& operator=(const Buffer&) = delete;

        [[nodiscard]] ID3D12Resource* getResource() const
        {
            if (m_vramHdl.isValid())
                return m_vramHdl.getD3DResource();
            else if (m_dynamicHdl.isValid())
                return m_dynamicHdl.getD3DResource();
            else
                return nullptr;
        }

        [[nodiscard]] uSize getByteSize() const
        {
            if (m_vramHdl.isValid())
                return m_vramHdl.getStorage().size;
            else if (m_dynamicHdl.isValid())
                return m_dynamicHdl.getStorage().size;
            else
                return 0;
        }

        void init(UploadBufferRequestManager& _uploadManager, const u8* _data, uSize _dataSize, DX12::VramMemoryCategory _category);
        void initDynamic(uSize _dataSize, DX12::VramMemoryCategory _category);

        // if isDynamic() is true, the buffer can be made static by uploading its content to VRAM.
        // It is not valid to call this on a non-dynamic buffer
        // After this call, isDynamic() will return false and an upload will be enqueued.
        // Be sure to ensureUploadDone() before the buffer use.
        void upload(UploadBufferRequestManager& _uploadManager, DX12::VramMemoryCategory _category);

        // call this to tag the VB as being used at a specific frame
        void markAsUsedAtFrame(u64 _cpuFrameCount) { m_lastUsedFrameCount = static_cast<i64>(_cpuFrameCount); }
        [[nodiscard]] i64 getLastUsedFrame() const { return m_lastUsedFrameCount; }

        [[nodiscard]] bool isDynamic() const { return m_dynamicHdl.isValid(); }

        [[nodiscard]] u64 getUploadFenceValue() const { return m_uploadFence; }

        void ensureUploadDone(UploadBufferRequestManager & _uploadManager)
        {
            /*
            if (m_uploadFence != 0)
            {
                _uploadManager.waitFence(m_uploadFence);
                m_uploadFence = 0;
            }
            */
        }

    protected:
        void checkInvariant() const;

        // Depending on the VB being dynamic or not, only one of these handles is valid.
        DefaultResourceBufferVRAMRelocHandle m_vramHdl; // not dynamic
        UploadBufferVRAMRelocHandle m_dynamicHdl;
        // Note that the upload buffer to send the data to the GPU (if applicable) is allocated in the UploadBufferRequestManager system, not here.
        u64 m_uploadFence = 0;
        i64 m_lastUsedFrameCount = -1;
    };


    class VertexBuffer : public Buffer
    {
    public:

        using Buffer::init;

        void init(UploadBufferRequestManager& _uploadManager, const u8* _data, uSize _dataSize)
        {
            init(_uploadManager, _data, _dataSize, VRAM_VertexBuffer);
        }

        using Buffer::initDynamic;
        void initDynamic(uSize _dataSize)
        {
            initDynamic(_dataSize, VRAM_VertexBuffer);
        }

        using Buffer::upload;
        void upload(UploadBufferRequestManager& _uploadManager)
        {
            upload(_uploadManager, VRAM_VertexBuffer);
        }
    };

    class IndexBuffer : public Buffer
    {
    public:

        using Buffer::init;
        void init(UploadBufferRequestManager& _uploadManager, const u8* _data, uSize _dataSize)
        {
            init(_uploadManager, _data, _dataSize, VRAM_IndexBuffer);
        }

        using Buffer::initDynamic;
        void initDynamic(uSize _dataSize)
        {
            initDynamic(_dataSize, VRAM_IndexBuffer);
        }

        using Buffer::upload;
        void upload(UploadBufferRequestManager& _uploadManager)
        {
            upload(_uploadManager, VRAM_IndexBuffer);
        }
    };
}
