#pragma once

#include "core/types.h"
#include "DX12/Helpers.h"
#include "engine/AdaptersInterfaces/GFXAdapter.h"

namespace ITF
{
    enum : ux { GFX_NB_MAX_VB_ELEMENTS = 8 };

    namespace DX12
    {
        enum class VertexFormat : i32
        {
            P = VertexFormat_P,
            PC = VertexFormat_PC,
            PCT = VertexFormat_PCT,
            PT = VertexFormat_PT,
            PCBT = VertexFormat_PCBT,
            PNCT = VertexFormat_PNCT, // this one is obsolete, and should not be used
            PNC3T = VertexFormat_PNC3T,
        };

        inline constexpr VertexFormat toVertexFormat(i32 gfxAdapterVertexFormat)
        {
            return static_cast<VertexFormat>(gfxAdapterVertexFormat);
        }

        D3D12_INPUT_LAYOUT_DESC getInputLayoutDesc(VertexFormat _format);
        uSize getVertexByteSize(VertexFormat _format);

        i8 getFXTechInDefaultShader(VertexFormat _format);
    }

} // namespace ITF
