#pragma once

#include "core/container/extendedVector.h"
#include <type_traits>

#include <nvnTool/nvnTool_GlslcInterface.h>

namespace ITF::NVN::ShaderMetaData
{

    struct UniformBlockDesc
    {
        String8 name = "anonymous";
        u32 size = 0;
        u32 numActiveVariables = 0;
        u32 vertexBlockBinding = -1; // The index to give at BindUniformBuffer
        u32 fragmentBlockBinding = -1; // The index to give at BindUniformBuffer

        static UniformBlockDesc build(const GLSLCuniformBlockInfo& _uniformBlock, const char* _stringPool);
        static Vector<UniformBlockDesc> buildCollection(const GLSLCuniformBlockInfo* _uniformBlockArray, uint32_t _nbBlock, const char* _stringPool);
    };
    static_assert(std::is_move_assignable_v<UniformBlockDesc>);

    struct UniformDesc
    {
        String8 name = "anonymous";
        GLSLCpiqUniformKind kind = GLSLC_PIQ_UNIFORM_KIND_PLAIN;
        GLSLCpiqTypeEnum type = GLSLC_PIQ_TYPE_FLOAT;
        bool isInUBO = false; // True if this uniform is part of a uniform block.
        i32 blockNdx = -1; // Index of the uniform block this uniform belongs to, if any (-1 if not in a uniform block).
        i32 blockOffset = -1; // Offset in the uniform block this uniform belongs to, if any (-1 if not in a uniform block).
        bool isArray = false;
        i32 sizeOfArray = 1; // Size of the array if this uniform is an array, 1 otherwise.
        i32 arrayStride = -1; // Stride in bytes between uniforms in the array.
        i32 matrixStride = -1; // Specifies stride between columns/rows of matrix depending on _isRowMajor_.
        bool isRowMajor = false;
        i32 textureBinding = -1; // Binding index. Used for samplers and images.

        static UniformDesc build(const GLSLCuniformInfo& _uniformArray, const char* _stringPool);
        static Vector<UniformDesc> buildCollection(const GLSLCuniformInfo* _uniformArray, uint32_t _nbUniform, const char* _stringPool);
    };
    static_assert(std::is_move_assignable_v<UniformDesc>);

    struct InputDesc
    {
        std::string name = "anonymous";
        GLSLCpiqTypeEnum type = GLSLC_PIQ_TYPE_FLOAT;
        uint32_t sizeOfArray = 1;
        int32_t location = -1;
        bool isArray = false;
        bool isPerPatch = false;

        static InputDesc build(const GLSLCprogramInputInfo& _input, const char* _stringPool);
        static Vector<InputDesc> buildCollection(const GLSLCprogramInputInfo* _inputArray, uint32_t _nbInputs, const char* _stringPool);
    };
    static_assert(std::is_move_assignable_v<InputDesc>);

    struct OutputDesc
    {
        std::string name = "anonymous";
        GLSLCpiqTypeEnum type = GLSLC_PIQ_TYPE_FLOAT;
        uint32_t sizeOfArray = 1;
        int32_t location = -1;
        int32_t locationNdx = -1; // Fragment color index of the output variable. -1 if not applicable.
        bool isArray = false;
        bool isPerPatch = false;

        static OutputDesc build(const GLSLCprogramOutputInfo& _output, const char* _stringPool);
        static Vector<OutputDesc> buildCollection(const GLSLCprogramOutputInfo* _outputArray, uint32_t _nbOutputs, const char* _stringPool);
    };
    static_assert(std::is_move_assignable_v<OutputDesc>);
}

