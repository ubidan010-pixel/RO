#include "precompiled_GFXAdapter_DX12.h"

#include "VertexFormat_DX12.h"

#include "core/types.h"
#include "DX12/Helpers.h"

namespace ITF::DX12
{
    namespace
    {
        struct VertexFormatDescriptor
        {
            const uSize vertexByteSize;
            const i8 fxTechInDefaultShader; // -1 if not valid. See Tech number in renderPCT.fx
            const D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
        };

        template <size_t N>
        static constexpr D3D12_INPUT_LAYOUT_DESC toInputLayoutDesc(const D3D12_INPUT_ELEMENT_DESC(&_elements)[N])
        {
            D3D12_INPUT_LAYOUT_DESC desc{};
            desc.pInputElementDescs = _elements;
            desc.NumElements = static_cast<UINT>(N);
            return desc;
        }
    }

    static constexpr D3D12_INPUT_ELEMENT_DESC gs_vertexFormatElements_P[] =
    {
        { "POSITION"        , 0, DXGI_FORMAT_R32G32B32_FLOAT    , 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };
    static constexpr VertexFormatDescriptor gs_vertexFormatDesc_P =
    {
        sizeof(VertexP),
        -1,
        toInputLayoutDesc(gs_vertexFormatElements_P)
    };

    static constexpr D3D12_INPUT_ELEMENT_DESC gs_vertexFormatElements_PC[] =
    {
        { "POSITION"        , 0, DXGI_FORMAT_R32G32B32_FLOAT    , 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR"           , 0, DXGI_FORMAT_R8G8B8A8_UNORM     , 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };
    static constexpr VertexFormatDescriptor gs_vertexFormatDesc_PC =
    {
        sizeof(VertexPC),
        1, // entry default_PC_VS
        toInputLayoutDesc(gs_vertexFormatElements_PC)
    };

    static constexpr D3D12_INPUT_ELEMENT_DESC gs_vertexFormatElements_PCT[] =
    {
        { "POSITION"        , 0, DXGI_FORMAT_R32G32B32_FLOAT    , 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR"           , 0, DXGI_FORMAT_R8G8B8A8_UNORM     , 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD"        , 0, DXGI_FORMAT_R32G32_FLOAT       , 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };
    static constexpr VertexFormatDescriptor gs_vertexFormatDesc_PCT =
    {
        sizeof(VertexPCT),
        0, // entry default_PCT_VS
        toInputLayoutDesc(gs_vertexFormatElements_PCT)
    };

    static constexpr D3D12_INPUT_ELEMENT_DESC gs_vertexFormatElements_PT[] =
    {
        { "POSITION"        , 0, DXGI_FORMAT_R32G32B32_FLOAT    , 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD"        , 0, DXGI_FORMAT_R32G32_FLOAT       , 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };
    static constexpr VertexFormatDescriptor gs_vertexFormatDesc_PT =
    {
        sizeof(VertexPT),
        2, // entry default_PT_VS
        toInputLayoutDesc(gs_vertexFormatElements_PT)
    };

    static constexpr D3D12_INPUT_ELEMENT_DESC gs_vertexFormatElements_PCBT[] =
    {
        { "POSITION"        , 0, DXGI_FORMAT_R32G32B32_FLOAT    , 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR"           , 0, DXGI_FORMAT_R8G8B8A8_UNORM     , 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "BLENDINDICES"    , 0, DXGI_FORMAT_R8G8B8A8_UINT      , 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD"        , 0, DXGI_FORMAT_R32G32_FLOAT       , 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };
    static constexpr VertexFormatDescriptor gs_vertexFormatDesc_PCBT =
    {
        sizeof(VertexPCBT),
        12, // entry OVERLAY_PCBT_VS (?). Is it correct?
        toInputLayoutDesc(gs_vertexFormatElements_PCBT)
    };

    static constexpr D3D12_INPUT_ELEMENT_DESC gs_vertexFormatElements_PNCT[] =
    {
        { "POSITION"        , 0, DXGI_FORMAT_R32G32B32_FLOAT    , 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL"          , 0, DXGI_FORMAT_R32G32B32_FLOAT    , 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR"           , 0, DXGI_FORMAT_R8G8B8A8_UNORM     , 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD"        , 0, DXGI_FORMAT_R32G32_FLOAT       , 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };
    static constexpr VertexFormatDescriptor gs_vertexFormatDesc_PNCT =
    {
        sizeof(VertexPNCT),
        4, // entry default_PNCT_VS
        toInputLayoutDesc(gs_vertexFormatElements_PNCT)
    };

    // PNC3T --> PC4T
    static constexpr D3D12_INPUT_ELEMENT_DESC gs_vertexFormatElements_PNC3T[] =
    {
        { "POSITION"        , 0, DXGI_FORMAT_R32G32B32_FLOAT    , 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR"           , 0, DXGI_FORMAT_R8G8B8A8_UNORM     , 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD"        , 0, DXGI_FORMAT_R32G32_FLOAT       , 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD"        , 1, DXGI_FORMAT_R32G32B32A32_FLOAT , 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD"        , 2, DXGI_FORMAT_R32G32B32A32_FLOAT , 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD"        , 3, DXGI_FORMAT_R32G32B32A32_FLOAT , 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };
    static constexpr VertexFormatDescriptor gs_vertexFormatDesc_PNC3T =
    {
        sizeof(VertexPNC3T),
        5, // entry frize_PNC3T_VS
        toInputLayoutDesc(gs_vertexFormatElements_PNC3T)
    };

    static const VertexFormatDescriptor& getDescriptor(VertexFormat _format)
    {
        switch (_format)
        {
        case VertexFormat::P:
            return gs_vertexFormatDesc_P;
        case VertexFormat::PC:
            return gs_vertexFormatDesc_PC;
        case VertexFormat::PCT:
            return gs_vertexFormatDesc_PCT;
        case VertexFormat::PT:
            return gs_vertexFormatDesc_PT;
        case VertexFormat::PCBT:
            return gs_vertexFormatDesc_PCBT;
        case VertexFormat::PNCT:
            return gs_vertexFormatDesc_PNCT;
        case VertexFormat::PNC3T:
            return gs_vertexFormatDesc_PNC3T;
        default:
            {
                ITF_ASSERT_CRASH(false, "Invalid vertex format");
                return gs_vertexFormatDesc_P;
            }
        }
    }


    D3D12_INPUT_LAYOUT_DESC getInputLayoutDesc(VertexFormat _format)
    {
        return getDescriptor(_format).inputLayoutDesc;
    }

    uSize getVertexByteSize(VertexFormat _format)
    {
        return getDescriptor(_format).vertexByteSize;
    }

    i8 getFXTechInDefaultShader(VertexFormat _format)
    {
        return getDescriptor(_format).fxTechInDefaultShader;
    }

} // namespace ITF::DX12
