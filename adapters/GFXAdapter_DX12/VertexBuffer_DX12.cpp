#include "precompiled_GFXAdapter_DX12.h"

#include "VertexBuffer_DX12.h"
#include "core/types.h"
#include "DX12/Helpers.h"

namespace ITF::DX12
{

    void Buffer::checkInvariant() const
    {
        ITF_ASSERT(!(m_vramHdl.isValid() && m_dynamicHdl.isValid()));  // only one of them is valid or none (NAND)

        ITF_ASSERT(imply(m_vramHdl.isValid(), m_vramHdl.getD3DResource() != nullptr));
        ITF_ASSERT(imply(m_dynamicHdl.isValid(), m_dynamicHdl.getD3DResource() != nullptr));

        ITF_ASSERT(imply(m_dynamicHdl.isValid(), m_uploadFence == 0)); // upload buffer should not have a fence

        ITF_ASSERT(m_lastUsedFrameCount == -1 || m_lastUsedFrameCount >= 0);
    }

    void Buffer::init(UploadBufferRequestManager& _uploadManager, const u8* _data, uSize _dataSize, DX12::VramMemoryCategory _category)
    {
        auto request = _uploadManager.allocCopyAndRequestUpload(_data, _dataSize, _category);
        m_vramHdl = std::move(request.destination);
        m_uploadFence = std::move(request.fenceValue);
    }

    void Buffer::initDynamic(uSize _dataSize, DX12::VramMemoryCategory _category)
    {
        m_dynamicHdl.alloc(_dataSize, _category);
    }

    void Buffer::upload(UploadBufferRequestManager& _uploadManager, DX12::VramMemoryCategory _category)
    {
        ITF_ASSERT(isDynamic());

        m_vramHdl.alloc(m_dynamicHdl.getStorage().size, _category);

        auto request = _uploadManager.requestUpload(m_dynamicHdl, m_vramHdl);

        ITF_ASSERT(request.destination.getD3DResource() == m_vramHdl.getD3DResource());
        m_uploadFence = request.fenceValue;
        m_dynamicHdl = {}; // release the dynamic handle (it is owned by the upload manager now)
    }
}
