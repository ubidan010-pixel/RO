#ifndef ITF_SHADERPROGRAM_PS5_H_
#define ITF_SHADERPROGRAM_PS5_H_

#pragma once

#include "core/types.h"
#include "Gnmp_Helpers.h"
#include "VRAMAllocator/VRAMRelocHdl.h"
#include "core/container/extendedMap.h"

#include "FxParameterDB_PS5.h"
#include "ShaderParameterMapping_PS5.h"

namespace ITF
{
    namespace AGS
    {
        struct ResourceDesc;
    }

    class FxParameterDB;
    struct VertexFormatDescriptor;

    class ShaderProgram
    {
    public:
        ShaderProgram();
        ~ShaderProgram();

        void load(const String& fileName);

        const Vector<AGS::ResourceDesc>& getMetaDataResources() const { return m_metaDataResources; }
        sce::Agc::Shader* getAgcShader() const { return m_agcShader; }
        Gnmp::VRAMAddress getMicroCodeAddr() const { return m_microCodeAddr.getAddress(); }

        // bind the shader with the parameter DB, so that the apply method can fill the constant buffer (call at shader init)
        bool bindParameterDB(const FxParameterDB& _paramDB);

    protected:
        template <Gnmp::ShaderStage stage>
        void applyParamDB(Gnmx::GfxContext& _rdrCtxt, const FxParameterDB& _paramDB);

        String8 m_loadedFileName;
        String8 m_markerString;
        Vector<AGS::ResourceDesc> m_metaDataResources;
        Map<StringID, ShaderParameterMapping> m_paramMapping; // contains the way to fill constant buffers from FxParameterDB data

        sce::Agc::Shader* m_agcShader = nullptr;
        Gnmp::SimpleVRAMRelocHandle m_headerAddr;
        Gnmp::SimpleVRAMRelocHandle m_microCodeAddr;
        Gnmp::AutoResourceHandler<sce::Agc::Shader> m_resHandler;
    };


    class VertexProgram final : public ShaderProgram
    {
    public:
        VertexProgram();
        ~VertexProgram();

        VertexProgram(const VertexProgram&) = delete;
        VertexProgram& operator = (const VertexProgram&) = delete;

        void load(const String& fileName); // overload ShaderProgram::load to handle the fetch shader
        
        void checkVertexDeclCompatibility(const VertexFormatDescriptor& _vertexDecl);

        void set(Gnmx::GfxContext& _context);

        // Fill the constant buffer and the texture samplers from the FxParameterDB and assign it in the command buffer
        void applyParamDB(Gnmx::GfxContext& _rdrCtxt, const FxParameterDB& _paramDB);

    private:
        Gnmp::SimpleVRAMRelocHandle m_fetchShaderAddr;
    };

    class PixelProgram final : public ShaderProgram
    {
    public:

        PixelProgram();
        ~PixelProgram();

        PixelProgram(const PixelProgram&) = delete;
        PixelProgram & operator = (const PixelProgram&) = delete;

        // Fill the constant buffer and the texture samplers from the FxParameterDB and assign it in the command buffer
        void applyParamDB(Gnmx::GfxContext& _rdrCtxt, const FxParameterDB& _paramDB);
        void set(Gnmx::GfxContext& _context);
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
        using VertexProgRepo = ITF::Map<String, VertexProgram>;
        using PixelProgRepo = ITF::Map<String, PixelProgram>;

        VertexProgRepo m_vpoRepo;
        PixelProgRepo m_fpoRepo;
    };
}

#endif // ITF_SHADERPROGRAM_PS5_H_
