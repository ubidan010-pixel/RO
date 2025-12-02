#pragma once

#include "core/Macros.h"
#include "core/utility.h"
#include "DX12/Helpers.h"
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#include "engine/display/Texture.h"

namespace ITF
{
    inline [[nodiscard]] D3D12_RECT itfRectToDX12Rect(GFX_RECT _viewportRect)
    {
        D3D12_RECT rect{};
        rect.left   = _viewportRect.left;
        rect.top    = _viewportRect.top;
        rect.right  = _viewportRect.right;
        rect.bottom = _viewportRect.bottom;
        return rect;
    }

    inline [[nodiscard]] D3D12_VIEWPORT itfRectToDX12Viewport(GFX_RECT _viewportRect)
    {
        D3D12_VIEWPORT viewport{};
        viewport.TopLeftX = static_cast<FLOAT>(_viewportRect.left);
        viewport.TopLeftY = static_cast<FLOAT>(_viewportRect.top);
        viewport.Width    = static_cast<FLOAT>(_viewportRect.right - _viewportRect.left);
        viewport.Height   = static_cast<FLOAT>(_viewportRect.bottom - _viewportRect.top);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        return viewport;
    }

    inline [[nodiscard]] D3D12_FILTER itfToDX12Filter(bool _isBilinear)
    {
        return _isBilinear ? D3D12_FILTER_MIN_MAG_MIP_LINEAR : D3D12_FILTER_MIN_MAG_MIP_POINT; // Anisotropic is probably not useful for a 2D game.
    }

    inline [[nodiscard]] D3D12_TEXTURE_ADDRESS_MODE itfToDX12(GFX_TEXADRESSMODE _texAddressMode)
    {
        switch (_texAddressMode)
        {
        case GFXTADDRESS_WRAP:
            return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        case GFXTADDRESS_MIRROR:
            return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        case GFXTADDRESS_CLAMP:
            return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        case GFXTADDRESS_BORDER:
            return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        default:
            break;
        }
        ITF_ASSERT_CRASH(false, "Invalid primitive type %d", toUnderlying(_texAddressMode));
        return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    }

    inline [[nodiscard]] Texture::PixFormat dx12ToITFTexFormat(DXGI_FORMAT _format)
    {
        switch (_format)
        {
        case DXGI_FORMAT_BC1_UNORM:
            return Texture::PF_DXT1;
        case DXGI_FORMAT_BC2_UNORM:
            return Texture::PF_DXT3;
        case DXGI_FORMAT_BC3_UNORM:
            return Texture::PF_DXT5;
        case DXGI_FORMAT_R8G8B8A8_UNORM:
            return Texture::PF_RGBA;
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
            return Texture::PF_RGBA; // Assuming sRGB maps to RGBA
        case DXGI_FORMAT_R8G8_UNORM:
            return Texture::PF_LA8;
        case DXGI_FORMAT_R8_UNORM:
            return Texture::PF_L8;
        default:
            break;
        }
        ITF_ASSERT_CRASH(false, "Unknown DX12 texture format %d", toUnderlying(_format));
        return Texture::PF_RGBA;
    }

    inline [[nodiscard]] DXGI_FORMAT itfToDX12TexFormat(Texture::PixFormat _pixformat)
    {
        DXGI_FORMAT pformat = DXGI_FORMAT_R8G8B8A8_UNORM; // Default format

        switch (_pixformat)
        {
        case Texture::PF_DXT1:
            pformat = DXGI_FORMAT_BC1_UNORM;
            break;
        case Texture::PF_DXT2:
        case Texture::PF_DXT3:
            pformat = DXGI_FORMAT_BC2_UNORM;
            break;
        case Texture::PF_DXT4:
        case Texture::PF_DXT5:
            pformat = DXGI_FORMAT_BC3_UNORM;
            break;
        case Texture::PF_RGB:
            {
                ITF_ASSERT(0); // assume RGA format
                pformat = DXGI_FORMAT_R8G8B8A8_UNORM;
            }
            break;
        case Texture::PF_RGBA:
            pformat = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case Texture::PF_L8:
            pformat = DXGI_FORMAT_R8_UNORM;
            break;
        case Texture::PF_A8:
            ITF_ASSERT(0); // need more efforts (swizzle)
            pformat = DXGI_FORMAT_R8_UNORM;
            break;
        default:
            ITF_ASSERT_CRASH(false, "Unknown texture ITF texture format %d", toUnderlying(_pixformat));
            break;
        }
        return pformat;
    }

    inline [[nodiscard]] D3D_PRIMITIVE_TOPOLOGY itfToDX12PrimitiveType(PRIMITIVETYPE _type)
    {
        switch (_type)
        {
        case GFX_TRIANGLES:
            return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        case GFX_POINTS:
            return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
        case GFX_LINES:
            return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
        case GFX_LINE_STRIP:
            return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
        case GFX_TRIANGLE_STRIP:
            return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
    #ifdef ITF_XBOX_SERIES
        case GFX_TRIANGLE_FAN:
            return D3D_PRIMITIVE_TOPOLOGY_TRIANGLEFAN; // Xbox extension
        case GFX_QUADS:
            return D3D_PRIMITIVE_TOPOLOGY_QUADLIST;
    #endif
        default:
            break;
        }
        ITF_ASSERT_CRASH(false, "Invalid ITF primitive type %d", toUnderlying(_type));
        return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    }
}
