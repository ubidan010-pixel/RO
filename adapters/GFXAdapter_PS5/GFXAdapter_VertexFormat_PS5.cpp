#include "precompiled_GFXAdapter_PS5.h"

#include "GFXAdapter_PS5.h"

#include "Gnmp_Helpers.h"

#include "GFXAdapter_struct_PS5.h"

namespace ITF
{
    // Vertex format descriptions for PS5
    static const VertexFormatElementDesc gs_vertexFormatElements_P[] =
    {
        { "Position0"    , Gnmp::kDataFormatR32G32B32Float    , 0    , 0},
    };
    static const Gnmp::FetchShaderInstancingMode gs_vertexFormatInstancingMode_P[] =
    {
        Gnmp::kFetchShaderUseVertexIndex,
    };
    static const VertexFormatDescriptor gs_vertexFormatDescP =
    {
        sizeof(VertexP),
        -1, // No tech entry for this format in default shader
        ITF_ARRAY_SIZE(gs_vertexFormatElements_P),
        gs_vertexFormatElements_P,
        gs_vertexFormatInstancingMode_P,
    };

    static const VertexFormatElementDesc gs_vertexFormatElements_PC[] =
    {
        { "Position0", Gnmp::kDataFormatR32G32B32Float   , 0    , 0},
        { "COLOR0"   , Gnmp::kDataFormatB8G8R8A8Unorm    , 12   , 0},
    };
    static const Gnmp::FetchShaderInstancingMode gs_vertexFormatInstancingMode_PC[] =
    {
        Gnmp::kFetchShaderUseVertexIndex,
        Gnmp::kFetchShaderUseVertexIndex,
    };
    static const VertexFormatDescriptor gs_vertexFormatDescPC =
    {
        sizeof(VertexPC),
        1, // entry default_PC_VS
        ITF_ARRAY_SIZE(gs_vertexFormatElements_PC),
        gs_vertexFormatElements_PC,
        gs_vertexFormatInstancingMode_PC,
    };

    static const VertexFormatElementDesc gs_vertexFormatElements_PCT[] =
    {
        { "Position0", Gnmp::kDataFormatR32G32B32Float   , 0    , 0},
        { "COLOR0"   , Gnmp::kDataFormatB8G8R8A8Unorm    , 12   , 0},
        { "TEXCOORD0", Gnmp::kDataFormatR32G32Float      , 16   , 0},
    };
    static const Gnmp::FetchShaderInstancingMode gs_vertexFormatInstancingMode_PCT[] =
    {
        Gnmp::kFetchShaderUseVertexIndex,
        Gnmp::kFetchShaderUseVertexIndex,
        Gnmp::kFetchShaderUseVertexIndex,
    };
    static const VertexFormatDescriptor gs_vertexFormatDescPCT =
    {
        sizeof(VertexPCT),
        0, // entry default_PCT_VS
        ITF_ARRAY_SIZE(gs_vertexFormatElements_PCT),
        gs_vertexFormatElements_PCT,
        gs_vertexFormatInstancingMode_PCT,
    };

    static const VertexFormatElementDesc gs_vertexFormatElements_PT[] =
    {
        { "Position0", Gnmp::kDataFormatR32G32B32Float   , 0    , 0},
        { "TEXCOORD0", Gnmp::kDataFormatR32G32Float      , 12   , 0},
    };
    static const Gnmp::FetchShaderInstancingMode gs_vertexFormatInstancingMode_PT[] =
    {
        Gnmp::kFetchShaderUseVertexIndex,
        Gnmp::kFetchShaderUseVertexIndex,
    };
    static const VertexFormatDescriptor gs_vertexFormatDescPT =
    {
        sizeof(VertexPT),
        2, // entry default_PT_VS
        ITF_ARRAY_SIZE(gs_vertexFormatElements_PT),
        gs_vertexFormatElements_PT,
        gs_vertexFormatInstancingMode_PT,
    };

    const VertexFormatElementDesc gs_vertexFormatElements_PCBT[] =
    {
        { "Position0"    , Gnmp::kDataFormatR32G32B32Float, 0    , 0},
        { "COLOR0"       , Gnmp::kDataFormatB8G8R8A8Unorm , 12   , 0},
        { "BLENDINDICES0", Gnmp::kDataFormatR32Sint       , 16   , 0},
        { "TEXCOORD0"    , Gnmp::kDataFormatR32G32Float   , 20   , 0},
    };
    const Gnmp::FetchShaderInstancingMode gs_vertexFormatInstancingMode_PCBT[] =
    {
        Gnmp::kFetchShaderUseVertexIndex,
        Gnmp::kFetchShaderUseVertexIndex,
        Gnmp::kFetchShaderUseVertexIndex,
        Gnmp::kFetchShaderUseVertexIndex,
    };
    static const VertexFormatDescriptor gs_vertexFormatDescPCBT =
    {
        sizeof(VertexPCBT),
        12, // entry OVERLAY_PCBT_VS (?). Is it correct?
        ITF_ARRAY_SIZE(gs_vertexFormatElements_PCBT),
        gs_vertexFormatElements_PCBT,
        gs_vertexFormatInstancingMode_PCBT,
    };

    // PNC3T --> PC4T
    static const VertexFormatElementDesc gs_vertexFormatElements_PNC3T[] =
    {
        { "Position0"    , Gnmp::kDataFormatR32G32B32Float    , 0    , 0},
        { "COLOR0"       , Gnmp::kDataFormatB8G8R8A8Unorm     , 12   , 0},
        { "TEXCOORD0"    , Gnmp::kDataFormatR32G32Float       , 16   , 0},
        { "TEXCOORD1"    , Gnmp::kDataFormatR32G32B32A32Float , 24   , 0},
        { "TEXCOORD2"    , Gnmp::kDataFormatR32G32B32A32Float , 40   , 0},
        { "TEXCOORD3"    , Gnmp::kDataFormatR32G32Float       , 56   , 0},
    };
    static const Gnmp::FetchShaderInstancingMode gs_vertexFormatInstancingMode_PNC3T[] =
    {
        Gnmp::kFetchShaderUseVertexIndex,
        Gnmp::kFetchShaderUseVertexIndex,
        Gnmp::kFetchShaderUseVertexIndex,
        Gnmp::kFetchShaderUseVertexIndex,
        Gnmp::kFetchShaderUseVertexIndex,
        Gnmp::kFetchShaderUseVertexIndex,
    };
    static const VertexFormatDescriptor gs_vertexFormatDescPNC3T =
    {
        sizeof(VertexPNC3T),
        5, // entry frize_PNC3T_VS
        ITF_ARRAY_SIZE(gs_vertexFormatElements_PNC3T),
        gs_vertexFormatElements_PNC3T,
        gs_vertexFormatInstancingMode_PNC3T,
    };

    const VertexFormatDescriptor& GFXAdapter_PS5::getVertexFormatDescriptor(VertexFormatAsEnum _vertexFormat) const
    {
        switch (_vertexFormat)
        {
        case VertexFormatAsEnum::P:
            return gs_vertexFormatDescP;
        case VertexFormatAsEnum::PC:
            return gs_vertexFormatDescPC;
        case VertexFormatAsEnum::PCT:
            return gs_vertexFormatDescPCT;
        case VertexFormatAsEnum::PT:
            return gs_vertexFormatDescPT;
        case VertexFormatAsEnum::PCBT:
            return gs_vertexFormatDescPCBT;
        case VertexFormatAsEnum::PNCT:
        {
            ITF_ASSERT_CRASH(false, "Invalid vertex format PNCT");
            return gs_vertexFormatDescP;
        }
        case VertexFormatAsEnum::PNC3T:
            return gs_vertexFormatDescPNC3T;
        case VertexFormatAsEnum::PatchAnim:
        {
            ITF_ASSERT_CRASH(false, "Invalid vertex format PatchAnim");
            return gs_vertexFormatDescP;
        }
        case VertexFormatAsEnum::PTa:
            return gs_vertexFormatDescPT;
        }

        ITF_ASSERT_CRASH(false, "Invalid vertex format 0x%08x", _vertexFormat);
        return gs_vertexFormatDescP;
    }

    const VertexFormatDescriptor& GFXAdapter_PS5::getVertexFormatDescriptor(u32 _vertexFormat) const
    {
        return getVertexFormatDescriptor(static_cast<VertexFormatAsEnum>(_vertexFormat));
    }

    void GFXAdapter_PS5::fillGnmpVertexBufferFormat(Gnmp::Buffer* _vertexBuffer, u8* _vertexData[], VertexFormatAsEnum _vertexFormat, u32 _nbVertex, u32 &_numElem)
    {
        const VertexFormatDescriptor & vertexFormatDesc = getVertexFormatDescriptor(_vertexFormat);
        ITF_ASSERT(vertexFormatDesc.numElements > 0);
        ITF_ASSERT(vertexFormatDesc.numElements <= GFX_NB_MAX_VB_ELEMENTS);
        const VertexFormatElementDesc * elts = vertexFormatDesc.elements;
        const u32 vertexStructSize = vertexFormatDesc.vertexByteSize;
        _numElem = vertexFormatDesc.numElements;
        
        for(ux i = 0; i < _numElem; i++)
        {
            _vertexBuffer[i].initAsVertexBuffer(_vertexData[elts[i].stream] + elts[i].offset, elts[i].format, vertexStructSize, _nbVertex);
        }
    }

} // namespace ITF
