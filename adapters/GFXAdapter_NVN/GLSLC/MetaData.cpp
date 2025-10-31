#include "MetaData.h"

#include <nvn/nvn_Cpp.h>


namespace ITF::NVN::ShaderMetaData
{

    UniformBlockDesc UniformBlockDesc::build(const GLSLCuniformBlockInfo& _uniformBlock, const char* _stringPool)
    {
        UniformBlockDesc result{};
        result.name = _stringPool + _uniformBlock.nameInfo.nameOffset;
        result.size = _uniformBlock.size;
        result.numActiveVariables = _uniformBlock.numActiveVariables;
        result.vertexBlockBinding = _uniformBlock.bindings[nvn::ShaderStage::VERTEX];
        result.fragmentBlockBinding = _uniformBlock.bindings[nvn::ShaderStage::FRAGMENT];
        return result;
    }

    Vector<UniformBlockDesc> UniformBlockDesc::buildCollection(const GLSLCuniformBlockInfo* _uniformBlockArray, uint32_t _nbBlock, const char* _stringPool)
    {
        Vector<UniformBlockDesc> result;

        for (auto* uniformBlock = _uniformBlockArray; uniformBlock != _uniformBlockArray + _nbBlock; ++uniformBlock)
        {
            result.emplace_back(build(*uniformBlock, _stringPool));
        }
        return result;
    }

    UniformDesc UniformDesc::build(const GLSLCuniformInfo& _uniform, const char* _stringPool)
    {
        UniformDesc result{};
        result.name = _stringPool + _uniform.nameInfo.nameOffset;
        result.kind = _uniform.kind;
        result.type = _uniform.type;
        result.isInUBO = _uniform.isInUBO;
        result.blockNdx = _uniform.blockNdx;
        result.blockOffset = _uniform.blockOffset;
        result.isArray = _uniform.isArray;
        result.sizeOfArray = _uniform.sizeOfArray;
        result.arrayStride = _uniform.arrayStride;
        result.matrixStride = _uniform.matrixStride;
        result.isRowMajor = _uniform.isRowMajor;
        result.textureBinding = _uniform.bindings[nvn::ShaderStage::FRAGMENT];

        return result;
    }

    Vector<UniformDesc> UniformDesc::buildCollection(const GLSLCuniformInfo* _uniformArray, uint32_t _nbUniforms, const char* _stringPool)
    {
        Vector<UniformDesc> result;

        for (auto* uniform = _uniformArray; uniform != _uniformArray + _nbUniforms; ++uniform)
        {
            result.emplace_back(build(*uniform, _stringPool));
        }
        return result;
    }

    InputDesc InputDesc::build(const GLSLCprogramInputInfo& _input, const char* _stringPool)
    {
        InputDesc result{};
        result.name = _stringPool + _input.nameInfo.nameOffset;
        result.type = _input.type;
        result.sizeOfArray = _input.sizeOfArray; // Missing field: sizeOfArray
        result.location = _input.location;
        result.isArray = _input.isArray;
        result.isPerPatch = _input.isPerPatch;

        return result;
    }

    Vector<InputDesc> InputDesc::buildCollection(const GLSLCprogramInputInfo* _inputArray, uint32_t _nbInputs, const char* _stringPool)
    {
        Vector<InputDesc> result;

        for (auto* input = _inputArray; input != _inputArray + _nbInputs; ++input)
        {
            result.emplace_back(build(*input, _stringPool));
        }
        return result;
    }


    OutputDesc OutputDesc::build(const GLSLCprogramOutputInfo& _output, const char* _stringPool)
    {
        OutputDesc result{};
        result.name = _stringPool + _output.nameInfo.nameOffset;
        result.type = _output.type;
        result.sizeOfArray = _output.sizeOfArray;
        result.location = _output.location;
        result.isArray = _output.isArray;
        result.isPerPatch = _output.isPerPatch;

        return result;
    }

    Vector<OutputDesc> OutputDesc::buildCollection(const GLSLCprogramOutputInfo* _outputArray, uint32_t _nbOutputs, const char* _stringPool)
    {
        Vector<OutputDesc> result;

        for (auto* output = _outputArray; output != _outputArray + _nbOutputs; ++output)
        {
            result.emplace_back(build(*output, _stringPool));
        }
        return result;
    }

}
