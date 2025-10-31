#include "precompiled_GFXAdapter_NVN.h"

#include "GFXAdapter_NVN.h"
#include "NVN/Helpers.h"
#include "GFXAdapter_struct_NVN.h"

#include "VertexBuffer_NVN.h"

namespace ITF
{
    // Vertex format descriptions for NVN
    static const VertexFormatElementDesc gs_vertexFormatElements_P[] =
    {
        { "Position0", nvn::Format::RGB32F,  0,  0},
    };

    static VertexFormatDescriptor gs_vertexFormatDescP =
    {
        sizeof(VertexP),
        -1, // No tech entry for this format in default shader
        ITF_ARRAY_SIZE(gs_vertexFormatElements_P),
        gs_vertexFormatElements_P
    };

    static const VertexFormatElementDesc gs_vertexFormatElements_PC[] =
    {
        { "Position0", nvn::Format::RGB32F,    0,   0},
        { "COLOR0"   , nvn::Format::RGBA8,    12,   0},
    };
    static VertexFormatDescriptor gs_vertexFormatDescPC =
    {
        sizeof(VertexPC),
        1, // entry default_PC_VS
        ITF_ARRAY_SIZE(gs_vertexFormatElements_PC),
        gs_vertexFormatElements_PC
    };

    static const VertexFormatElementDesc gs_vertexFormatElements_PCT[] =
    {
        { "Position0", nvn::Format::RGB32F,    0,   0},
        { "COLOR0"   , nvn::Format::RGBA8,    12,   0},
        { "TEXCOORD0", nvn::Format::RG32F,    16,   0},
    };

    static VertexFormatDescriptor gs_vertexFormatDescPCT =
    {
        sizeof(VertexPCT),
        0, // entry default_PCT_VS
        ITF_ARRAY_SIZE(gs_vertexFormatElements_PCT),
        gs_vertexFormatElements_PCT
    };

    static const VertexFormatElementDesc gs_vertexFormatElements_PT[] =
    {
        { "Position0", nvn::Format::RGB32F,    0,   0},
        { "TEXCOORD0", nvn::Format::RG32F,    12,   0},
    };
    static VertexFormatDescriptor gs_vertexFormatDescPT =
    {
        sizeof(VertexPT),
        2, // entry default_PT_VS
        ITF_ARRAY_SIZE(gs_vertexFormatElements_PT),
        gs_vertexFormatElements_PT
    };

    const VertexFormatElementDesc gs_vertexFormatElements_PCBT[] =
    {
        { "Position0"    , nvn::Format::RGB32F,  0, 0},
        { "COLOR0"       , nvn::Format::RGBA8,  12, 0},
        { "BLENDINDICES0", nvn::Format::R32I,   16, 0},
        { "TEXCOORD0"    , nvn::Format::RG32F,  20, 0},
    };
    static VertexFormatDescriptor gs_vertexFormatDescPCBT =
    {
        sizeof(VertexPCBT),
        12, // entry OVERLAY_PCBT_VS (?). Is it correct?
        ITF_ARRAY_SIZE(gs_vertexFormatElements_PCBT),
        gs_vertexFormatElements_PCBT
    };

    // PNC3T --> PC4T
    static const VertexFormatElementDesc gs_vertexFormatElements_PNC3T[] =
    {
        { "Position0"    , nvn::Format::RGB32F,   0, 0},
        { "COLOR0"       , nvn::Format::RGBA8,   12, 0},
        { "TEXCOORD0"    , nvn::Format::RG32F,   16, 0},
        { "TEXCOORD1"    , nvn::Format::RGBA32F, 24, 0},
        { "TEXCOORD2"    , nvn::Format::RGBA32F, 40, 0},
        { "TEXCOORD3"    , nvn::Format::RG32F  , 56, 0},
    };
    static VertexFormatDescriptor gs_vertexFormatDescPNC3T =
    {
        sizeof(VertexPNC3T),
        5, // entry frize_PNC3T_VS
        ITF_ARRAY_SIZE(gs_vertexFormatElements_PNC3T),
        gs_vertexFormatElements_PNC3T
    };

    VertexFormatDescriptor& GFXAdapter_NVN::getVertexFormatDescriptor(VertexFormatAsEnum _vertexFormat) const
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

    VertexFormatDescriptor& GFXAdapter_NVN::getVertexFormatDescriptor(u32 _vertexFormat) const
    {
        return getVertexFormatDescriptor(static_cast<VertexFormatAsEnum>(_vertexFormat));
    }

    void VertexFormatDescriptor::initStateArray()
    {
        if (!attribIsInit)
        {
            for (ux i = 0; i < numElements; i++)
            {
                vertexAttribState[i].SetDefaults()
                    .SetFormat(elements[i].format, elements[i].offset)
                    .SetStreamIndex(elements[i].stream);
            }
            attribIsInit = true;
            vertexStreamState[0].SetDefaults()
                .SetStride(vertexByteSize);

        }
    }

    void VertexFormatDescriptor::bindState(nvn::CommandBuffer* _cb)
    {
        initStateArray();
        _cb->BindVertexAttribState(numElements, vertexAttribState);
        _cb->BindVertexStreamState(1, vertexStreamState);
    }

    void GFXAdapter_NVN::nvnBindVertexBuffer(NVNVertexBuffer* _vertexBuffer, u32 _offset)
    {
        _vertexBuffer->markAsUsedAtFrame(m_mainContext->getNbSubmittedFrame() + 1u); // $GB: todo, check/explain why +1 here ?

        nvn::Buffer* buffer = _vertexBuffer->vramHdl.getBuffer();
        ITF_ASSERT_CRASH(buffer->GetSize() >= _offset, "Offset in buffer is bigger than the buffer size");
        const auto bufferAddress = buffer->GetAddress() + _offset;
        const auto bufferSize = buffer->GetSize() - _offset;
        getRenderingContext()->BindVertexBuffer(0, bufferAddress, bufferSize);
    }
} // namespace ITF
