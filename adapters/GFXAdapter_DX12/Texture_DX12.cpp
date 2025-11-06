#include "precompiled_GFXAdapter_DX12.h"

#include "DX12/Helpers.h"
#include "Texture_DX12.h"
#include "RenderTarget_DX12.h"
#include "core/itfstring.h"
#include "core/math/BitTweak.h"
#include "VRAMAllocator/VRAMAllocator.h"

#include "engine/display/ddsformat.h"

namespace ITF::DX12
{
    Texture::Texture()
    #if defined(ITF_ENABLE_DX12_GRAPHICS_DEBUGGING) && ITF_ENABLE_DX12_GRAPHICS_DEBUGGING
        : m_debugName{"NotInit"}
    #endif
    {
    }

    static DXGI_FORMAT ddsHeaderToDX12Format(const DDS_HEADER & _header)
    {
        // simple version, only a few formats supported

        DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;

        if (_header.format.dwFlags & DDPIXELFORMAT::FOURCC)
        {
            switch (_header.format.dwFourCC)
            {
            case MAKEFOURCC('D', 'X', 'T', '1'):
                format = DXGI_FORMAT_BC1_UNORM;
                break;
            case MAKEFOURCC('D', 'X', 'T', '2'):
                format = DXGI_FORMAT_BC2_UNORM;
                break;
            case MAKEFOURCC('D', 'X', 'T', '3'):
                format = DXGI_FORMAT_BC2_UNORM;
                break;
            case MAKEFOURCC('D', 'X', 'T', '4'):
                format = DXGI_FORMAT_BC3_UNORM;
                break;
            case MAKEFOURCC('D', 'X', 'T', '5'):
                format = DXGI_FORMAT_BC3_UNORM;
                break;
            }
        }
        else if (_header.format.dwFlags & DDPIXELFORMAT::RGB)
        {
            if (_header.format.dwRGBBitCount == 32)
            {
                if (   _header.format.dwRBitMask && 0x000000FF
                    && _header.format.dwGBitMask && 0x0000FF00
                    && _header.format.dwBBitMask && 0x00FF0000
                    && _header.format.dwABitMask && 0xFF000000)
                    format = DXGI_FORMAT_R8G8B8A8_UNORM;
            }
        }

        return format;
    }

    u32 getBytesPerPixelFromFormat(DXGI_FORMAT _format)
    {
        switch (_format)
        {
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_B8G8R8A8_UNORM:
            return 4;

        case DXGI_FORMAT_R16G16_UNORM:
        case DXGI_FORMAT_R32_FLOAT:
            return 4;

        case DXGI_FORMAT_R16G16B16A16_FLOAT:
        case DXGI_FORMAT_R32G32_FLOAT:
            return 8;

        case DXGI_FORMAT_R32G32B32A32_FLOAT:
            return 16;

        default:
            {
                ITF_ASSERT_CRASH(false, "DX12 getBytesPerPixelFromFormat unsupported format %d (block compressed?)", toUnderlying(_format));
            }
            return 0;
        }
    }

    static bool isBlockCompressed(DXGI_FORMAT _format)
    {
        switch (_format)
        {
        case DXGI_FORMAT_BC1_UNORM:
        case DXGI_FORMAT_BC2_UNORM:
        case DXGI_FORMAT_BC3_UNORM:
        case DXGI_FORMAT_BC4_UNORM:
        case DXGI_FORMAT_BC5_UNORM:
        case DXGI_FORMAT_BC6H_UF16:
        case DXGI_FORMAT_BC7_UNORM:
            return true;
        default:
            return false;
        }
    }

    static u32 getBlockSize(DXGI_FORMAT _format)
    {
        switch (_format)
        {
        case DXGI_FORMAT_BC1_UNORM:
        case DXGI_FORMAT_BC4_UNORM:
            return 8;
        case DXGI_FORMAT_BC2_UNORM:
        case DXGI_FORMAT_BC3_UNORM:
        case DXGI_FORMAT_BC5_UNORM:
        case DXGI_FORMAT_BC6H_UF16:
        case DXGI_FORMAT_BC7_UNORM:
            return 16;
        default:
            {
                ITF_ASSERT_CRASH(false, "Unsupported DX12 block compressed texture format %d", toUnderlying(_format));
            }
            return 0;
        }
    }


    Texture Texture::createFromMemoryDDS(TextureDescriptorPool& _texPool, UploadBufferRequestManager& _uploadMgr, const u8* _srcRawData, u64 _rawDataSize, const char* _debugName)
    {
        Texture texture{};

    #if defined (ITF_ENABLE_DX12_GRAPHICS_DEBUGGING) && ITF_ENABLE_DX12_GRAPHICS_DEBUGGING
        texture.m_debugName = _debugName;
    #endif

        const DDS_HEADER & header = *reinterpret_cast<const DDS_HEADER*>(_srcRawData);

        DXGI_FORMAT format = ddsHeaderToDX12Format(header);

        _srcRawData += sizeof(DDS_HEADER);

        D3D12_SUBRESOURCE_DATA subResourcesData[UploadBufferRequestManager::NB_MAX_MIP_LEVEL]{};

        u32 mipWidth = header.dwWidth;
        u32 mipHeight = header.dwHeight;
        u32 nbMipMap = (header.dwFlags & DDSD_MIPMAPCOUNT) != 0 ? header.dwMipMapCount : 1;
        ITF_ASSERT(nbMipMap != 0);
        ITF_ASSERT(nbMipMap < UploadBufferRequestManager::NB_MAX_MIP_LEVEL);
        nbMipMap = Clamp<u32>(nbMipMap, 1, UploadBufferRequestManager::NB_MAX_MIP_LEVEL);

        bool blockCompressed = isBlockCompressed(format);
        u32 blockSize = blockCompressed ? getBlockSize(format) : 0;
        u32 bytesPerPixel = blockCompressed ? 0 : getBytesPerPixelFromFormat(format);

        for (u32 mipLevel = 0; mipLevel < nbMipMap; ++mipLevel)
        {
            D3D12_SUBRESOURCE_DATA & subRes = subResourcesData[mipLevel];
            subRes.pData = _srcRawData;

            if (blockCompressed)
            {
                u32 numBlocksWide = BitTweak::GetAboveAlign(mipWidth, 4) / 4;
                u32 numBlocksHeight = BitTweak::GetAboveAlign(mipHeight, 4) / 4;
                subRes.RowPitch = numBlocksWide * blockSize;
                subRes.SlicePitch = subRes.RowPitch * numBlocksHeight;
            }
            else
            {
                subRes.RowPitch = mipWidth * bytesPerPixel;
                subRes.SlicePitch = subRes.RowPitch * mipHeight;
            }

            mipWidth /= 2;
            mipHeight /= 2;
            _srcRawData += subRes.SlicePitch;

            if (!mipWidth || !mipHeight)
            {
                nbMipMap = mipLevel + 1;
                break;
            }
        }

        auto uploadReq = _uploadMgr.requestTextureUpload(header.dwWidth, header.dwHeight, nbMipMap, format, subResourcesData);

        texture.m_memHdl = std::move(uploadReq.destination);
        texture.m_uploadFence = uploadReq.fenceValue;

        texture.m_resource = texture.m_memHdl.getD3DResource();

    #if defined (ITF_ENABLE_DX12_GRAPHICS_DEBUGGING) && ITF_ENABLE_DX12_GRAPHICS_DEBUGGING
        ITF_VERIFY_HR_CALL(texture.m_resource->SetName(String8ToWChar(texture.m_debugName).get()));
    #endif

        texture.m_descriptorHandle = _texPool.registerTexture(texture.m_resource.Get(), texture.getDebugName());
        texture.m_desc = texture.m_resource->GetDesc();

        return texture;
    }

    Texture Texture::createFromRenderTarget(TextureDescriptorPool& _texPool, RenderTarget& _renderTarget)
    {
        Texture texture{};

    #if defined (ITF_ENABLE_DX12_GRAPHICS_DEBUGGING) && ITF_ENABLE_DX12_GRAPHICS_DEBUGGING
        texture.m_debugName = _renderTarget.getDebugName();
    #endif

        texture.m_resource = _renderTarget.getResource();

        texture.m_descriptorHandle = _texPool.registerTexture(texture.m_resource.Get(), texture.getDebugName());
        texture.m_desc = texture.m_resource->GetDesc();

        return texture;
    }

    Texture Texture::createCommitted(ComPtr<ID3D12Device> _device, TextureDescriptorPool& _texPool, u32 _width, u32 _height, DXGI_FORMAT _format, D3D12_HEAP_TYPE _heapType, const char* _debugName)
    {
        Texture texture{};

    #if defined (ITF_ENABLE_DX12_GRAPHICS_DEBUGGING) && ITF_ENABLE_DX12_GRAPHICS_DEBUGGING
        texture.m_debugName = _debugName;
    #endif

        // Create a committed resource for the texture
        D3D12_RESOURCE_DESC texDesc = {};
        texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        texDesc.Alignment = 0;
        texDesc.Width = _width;
        texDesc.Height = _height;
        texDesc.DepthOrArraySize = 1;
        texDesc.MipLevels = 1;
        texDesc.Format = _format;
        texDesc.SampleDesc.Count = 1;
        texDesc.SampleDesc.Quality = 0;
        texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = _heapType;
        heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        heapProps.CreationNodeMask = 1;
        heapProps.VisibleNodeMask = 1;

        HRESULT hr = _device->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &texDesc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            DX12_IID_COMPTR_ARGS(texture.m_resource)
        );

        ITF_ASSERT_CRASH(SUCCEEDED(hr), "Failed to create committed texture resource");

        #if defined (ITF_ENABLE_DX12_GRAPHICS_DEBUGGING) && ITF_ENABLE_DX12_GRAPHICS_DEBUGGING
        ITF_VERIFY_HR_CALL(texture.m_resource->SetName(String8ToWChar(texture.m_debugName).get()));
        #endif

        texture.m_descriptorHandle = _texPool.registerTexture(texture.m_resource.Get(), texture.getDebugName());
        texture.m_desc = texture.m_resource->GetDesc();

        texture.m_descriptorHandle = _texPool.registerTexture(texture.m_resource.Get(), texture.getDebugName());
        texture.m_desc = texture.m_resource->GetDesc();

        return texture;
    }

}
