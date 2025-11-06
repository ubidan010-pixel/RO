#pragma once

#include <wrl/client.h>

#ifdef ITF_XBOX_SERIES
    #include <d3d12_xs.h>
    #include <d3dx12_xs.h>
    #include <xg_xs.h>
    #define DX12_IID_PPV_ARGS IID_GRAPHICS_PPV_ARGS
#else
    #include <d3d12.h>
    #include <d3dx12.h>
    #define DX12_IID_PPV_ARGS IID_PPV_ARGS
#endif

#define DX12_IID_COMPTR_ARGS(comPtr_) DX12_IID_PPV_ARGS((comPtr_).ReleaseAndGetAddressOf())

#define DXC_IID_PPV_COMPTR_ARGS(comPtr_) IID_PPV_ARGS((comPtr_).ReleaseAndGetAddressOf())

#if !defined(ITF_ENABLE_DX12_GRAPHICS_DEBUGGING)
    #if defined(ITF_DEBUG) || defined(ITF_RELEASE)
        #define ITF_ENABLE_DX12_GRAPHICS_DEBUGGING 1
    #else
        #define ITF_ENABLE_DX12_GRAPHICS_DEBUGGING 0
    #endif
#endif

#if defined(ITF_ENABLE_DX12_GRAPHICS_DEBUGGING) && ITF_ENABLE_DX12_GRAPHICS_DEBUGGING
    #include <pix3.h>
#endif

using Microsoft::WRL::ComPtr;

namespace ITF::DX12
{
    // Add a simple enum for shader stages, as none in D3D12 is really appropriate.
    enum class ShaderStage : u8
    {
        Vertex = 0,
        Pixel,
    };

    inline void setMarker(ID3D12GraphicsCommandList* _cmdList, const char* _markerName)
    {
    #if defined(ITF_ENABLE_DX12_GRAPHICS_DEBUGGING) && ITF_ENABLE_DX12_GRAPHICS_DEBUGGING
        PIXSetMarker(_cmdList, 0xFF0000FFu, "%s", _markerName);
    #endif
    }

    inline void pushMarker(ID3D12GraphicsCommandList* _cmdList, const char* _markerName)
    {
    #if defined(ITF_ENABLE_DX12_GRAPHICS_DEBUGGING) && ITF_ENABLE_DX12_GRAPHICS_DEBUGGING
        PIXBeginEvent(_cmdList, 0xFF00FFFFu, "%s", _markerName);
    #endif
    }

    inline void popMarker(ID3D12GraphicsCommandList* _cmdList)
    {
    #if defined(ITF_ENABLE_DX12_GRAPHICS_DEBUGGING) && ITF_ENABLE_DX12_GRAPHICS_DEBUGGING
        PIXEndEvent(_cmdList);
    #endif
    }
}
