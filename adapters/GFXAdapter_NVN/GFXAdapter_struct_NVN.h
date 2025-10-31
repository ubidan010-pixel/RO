#pragma once

#include "core/types.h"
#include "NVN/Helpers.h"

namespace ITF
{
    //////////////////////////////////////////////////////////////////////////
    /// Constant
    //////////////////////////////////////////////////////////////////////////

    enum : ux { GFX_NB_MAX_SAMPLER = 8 };
    enum : ux { GFX_NB_MAX_VB_ELEMENTS = 8 };

    //////////////////////////////////////////////////////////////////////////
    /// Struct
    //////////////////////////////////////////////////////////////////////////

    // Descriptor for vertex format elements

    struct VertexFormatElementDesc
    {
        const char* name;
        nvn::Format format;
        u32 offset;
        u32 stream;
    };

    struct VertexFormatDescriptor
    {
        const u16 vertexByteSize;
        const i8 fxTechInDefaultShader; // -1 if not valid. See Tech number in renderPCT.fx
        const u8 numElements;
        const VertexFormatElementDesc * elements; // ptr on array of numElements
        nvn::VertexAttribState vertexAttribState[GFX_NB_MAX_VB_ELEMENTS]{};
        nvn::VertexStreamState vertexStreamState[1];
        bool attribIsInit = false;
        void bindState(nvn::CommandBuffer* _cb);
        void initStateArray();
    };

} // namespace ITF
