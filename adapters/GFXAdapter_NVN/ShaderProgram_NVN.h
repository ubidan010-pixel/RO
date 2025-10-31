#ifndef ITF_SHADERPROGRAM_NVN_H_
#define ITF_SHADERPROGRAM_NVN_H_

#pragma once

#include "core/types.h"
#include "NVN/Helpers.h"
#include "VRAMAllocator/VRAMRelocHdl.h"
#include "core/container/extendedMap.h"

#include "FxParameterDB_NVN.h"
#include "ShaderParameterMapping_NVN.h"

#include "UniformBufferPool_NVN.h"

namespace ITF
{
    class FxParameterDB;
    struct VertexFormatDescriptor;

    class ShaderProgram
    {
    public:
        ShaderProgram();
        ~ShaderProgram();

        void load(const String& fileName);

        nvn::ShaderData getShaderData() const;

        const Vector<NVN::ShaderMetaData::UniformDesc>& getUniformsMetaData() const { return m_metaDataUniforms; }

        // bind the shader with the parameter DB, so that the apply method can fill the constant buffer (call at shader init)
        bool bindParameterDB(const FxParameterDB& _paramDB);

        bool isLoaded() const { return !m_loadedFileName.isEmpty(); }

    protected:

        void loadShaderGPUCode(const u8* _gpuCodeDataBlock, u32 _dataSize);
        void loadControlData(const u8* _controlDataBlock, u32 _controlSize);

        template <nvn::ShaderStage::Enum stage>
        void applyParamDB(nvn::CommandBuffer& _rdrCtxt, NVNUniformBufferPool& _uniformPool, const FxParameterDB& _paramDB);

        String8 m_loadedFileName;
    #if ITF_ENABLE_NVN_GRAPHICS_DEBUGGING
        String8 m_debugShortName; // assigned to the shader program, do not change after loading
    #endif
        Vector<NVN::ShaderMetaData::UniformBlockDesc> m_metaDataUniformBlocks;
        Vector<NVN::ShaderMetaData::UniformDesc> m_metaDataUniforms;
        Vector<NVN::ShaderMetaData::InputDesc> m_metaDataInputs;
        Vector<NVN::ShaderMetaData::OutputDesc> m_metaDataOutputs;
        Map<StringID, ShaderParameterMapping> m_paramMapping; // contains the way to fill constant buffers from FxParameterDB data

        Vector<u8> m_controlData;
        NVN::BufferVRAMRelocHandle m_gpuCode;
        u32 m_effectiveGPUCodeSize = 0; // m_gpuCode block is biggger because of padding requirement, m_effectiveGPUCodeSize is the real size of the shader code.
    };

    class VertexProgram final : public ShaderProgram
    {
    public:
        VertexProgram();
        ~VertexProgram();

        VertexProgram(const VertexProgram&) = delete;
        VertexProgram& operator = (const VertexProgram&) = delete;

        void checkVertexDeclCompatibility(const VertexFormatDescriptor& _vertexDecl);

        // Fill the constant buffer and the texture samplers from the FxParameterDB and assign it in the command buffer
        void applyParamDB(nvn::CommandBuffer& _rdrCtxt, NVNUniformBufferPool& _uniformPool, const FxParameterDB& _paramDB);

    private:
        NVN::SimpleVRAMRelocHandle m_fetchShaderAddr;
    };

    class PixelProgram final : public ShaderProgram
    {
    public:

        PixelProgram();
        ~PixelProgram();

        PixelProgram(const PixelProgram&) = delete;
        PixelProgram & operator = (const PixelProgram&) = delete;

        // Fill the constant buffer and the texture samplers from the FxParameterDB and assign it in the command buffer
        void applyParamDB(nvn::CommandBuffer& _rdrCtxt, NVNUniformBufferPool& _uniformPool, const FxParameterDB& _paramDB);
    };

#if 0
    // missing, might be useful at a time?
    class ComputeProgram final : public ShaderProgram
    {
    public:

        ComputeProgram() = default;
        ComputeProgram(const ComputeProgram&) = delete;
        ComputeProgram& operator = (const ComputeProgram&) = delete;

        ~ComputeProgram() = default;

        void load(const String& fileName);
        bool bind(const FxParameterDB& _paramDB);
        void apply(Gnmx::GfxContext& _rdrCtxt);
    };
#endif

    // This class is a repository of shaders
    // It's goal is :
    // - to avoid multiple loading of shaders from disk between techniques
    // - to be able to test identity of shaders at runtime by just testing address equality
    // (in order to not resend the shader to the GPU)
    class ShaderProgramRepository : public TemplateSingleton<ShaderProgramRepository>
    {
    public:
        ShaderProgramRepository();
        ~ShaderProgramRepository();

        void clear(); // beware to not keep VertexProgram* and PixelProgram* after this call (will be dangling)

        VertexProgram* getVPO(const String& _vpoName);
        PixelProgram* getFPO(const String& _fpoName);

    private:
        using VertexProgRepo = Map<String, VertexProgram>;
        using PixelProgRepo = Map<String, PixelProgram>;

        VertexProgRepo m_vpoRepo;
        PixelProgRepo m_fpoRepo;
    };
}

#endif
