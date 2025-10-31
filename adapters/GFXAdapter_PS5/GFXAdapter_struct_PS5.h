#ifndef ITF_GFXADAPTER_STRUCT_PS5_H_
#define ITF_GFXADAPTER_STRUCT_PS5_H_

#pragma once

#include "core/types.h"
#include "Gnmp_Helpers.h"

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

    struct ConstBuffers
    {
        u8* buffer;
        u32 size;
    };

    struct VertexFormatElementDesc
    {
        const char* name;
        Gnmp::DataFormat format;
        u32 offset;
        u32 stream;
    };

    struct VertexFormatDescriptor
    {
        const u16 vertexByteSize;
        const i8 fxTechInDefaultShader; // -1 if not valid. See Tech number in renderPCT.fx
        const u8 numElements;
        const VertexFormatElementDesc * elements; // ptr on array of numElements
        const Gnmp::FetchShaderInstancingMode * instancingModes; // ptr on array of numElements
    };

} // namespace ITF

#endif // ITF_GFXADAPTER_STRUCT_PS5_H_
