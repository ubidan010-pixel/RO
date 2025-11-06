#pragma once

#ifdef ITF_WIN64
#include <d3d12shader.h>
#elif defined(ITF_XBOX_SERIES)
#include <d3d12shader_xs.h>
#else
#include <d3d12shader_x.h>
#endif

#include "core/types.h"

#include "DX12/Helpers.h"

#include "VRAMAllocator/VRAMRelocHdl.h"
#include "core/container/extendedMap.h"

#include "FxParameterDB_DX12.h"
#include "ShaderParameterMapping_DX12.h"
#include "UniformBufferPool_DX12.h"
#include "VertexFormat_DX12.h"

#include "RootSignature_DX12.h"

namespace ITF::DX12
{
    class FxParameterDB;
    struct VertexFormatDescriptor;

    class ShaderProgram
    {
    public:
        ShaderProgram();
        virtual ~ShaderProgram();

        virtual void load(const String& _fileName);

        [[nodiscard]] D3D12_SHADER_BYTECODE getShaderData() const { return CD3DX12_SHADER_BYTECODE(m_shaderRawData.data(), m_shaderRawData.size()); }

        // bind the shader with the parameter DB, so that the apply method can fill the constant buffer (call at shader init)
        bool bindParameterDB(const FxParameterDB& _paramDB, const SamplerDescriptorPool& _samplerPool, const RootSignatureDescriptor& _rootSignatureDescriptor);

        [[nodiscard]] bool isLoaded() const { return !m_loadedFileName.isEmpty(); }

        [[nodiscard]] const char* getName() const;

        [[nodiscard]] ShaderParameterMapping * getMappingForParamDB(const FxParameterDB& _paramDB);

    protected:
        void createBlob(); // create m_dxcBlob pointing to m_shaderRawData
        void createShaderReflectionFromRawData();

        template <ShaderStage stage>
        void applyParamDB(ID3D12GraphicsCommandList * _rdrCtxt, UniformBufferPool& _uniformPool, const FxParameterDB& _paramDB);

        String8 m_loadedFileName;
    #if defined(ITF_ENABLE_DX12_GRAPHICS_DEBUGGING) && ITF_ENABLE_DX12_GRAPHICS_DEBUGGING
        String8 m_debugShortName = "NotLoaded"; // assigned to the shader program, do not change after loading
    #endif
        Map<StringID, ShaderParameterMapping> m_paramMapping; // contains the way to fill constant buffers from FxParameterDB data

        Vector<u8> m_shaderRawData;
        ComPtr<IDxcBlob> m_dxcBlob; // pointing to the same data as m_shaderRawData, but as a DXC blob (for reflection purpose)
        ComPtr<ID3D12ShaderReflection> m_shaderReflection{};
    };

    class VertexProgram final : public ShaderProgram
    {
    public:
        VertexProgram();
        ~VertexProgram() override;

        VertexProgram(const VertexProgram&) = delete;
        VertexProgram& operator = (const VertexProgram&) = delete;

        void load(ComPtr<ID3D12Device> _device, const String& _fileName);

        [[nodiscard]] DX12::VertexFormat getVertexFormat() const { return m_vertexFormat; }

        [[nodiscard]] const RootSignature & getRootSignature() const { return m_rootSignature; }

        // Fill the constant buffer and the texture samplers from the FxParameterDB and assign it in the command buffer
        void applyParamDB(ID3D12GraphicsCommandList * _rdrCtxt, UniformBufferPool& _uniformPool, const FxParameterDB& _paramDB);

    private:
        void fetchVertexFormatFromReflection();
        void buildRootSignature(ComPtr<ID3D12Device> _device);

        enum class SemanticKind
        {
            POSITION,
            NORMAL,
            COLOR,
            TEXCOORD,
            BLENDINDICES,
        };

        struct Semantic
        {
            Semantic() = default;
            static Semantic build(const D3D12_SIGNATURE_PARAMETER_DESC& _desc);

            SemanticKind kind = SemanticKind::POSITION;
            ux index = 0;
        };

        static [[nodiscard]] DX12::VertexFormat findVertexFormat(const Vector<Semantic>& _inputSemantics);

        Vector<Semantic> m_inputSemantics;
        DX12::VertexFormat m_vertexFormat = DX12::VertexFormat::PCT;
        RootSignature m_rootSignature;
    };

    class PixelProgram final : public ShaderProgram
    {
    public:

        PixelProgram();
        ~PixelProgram() override;

        PixelProgram(const PixelProgram&) = delete;
        PixelProgram & operator = (const PixelProgram&) = delete;

        void load(ComPtr<ID3D12Device> _device, const String& _fileName)
        {
            ShaderProgram::load(_fileName);
        }

        // Fill the constant buffer and the texture samplers from the FxParameterDB and assign it in the command buffer
        void applyParamDB(ID3D12GraphicsCommandList* _rdrCtxt, UniformBufferPool& _uniformPool, const FxParameterDB& _paramDB);
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

        [[nodiscard]] VertexProgram* getVPO(ComPtr<ID3D12Device> _device, const String& _vpoName);
        [[nodiscard]] PixelProgram* getFPO(ComPtr<ID3D12Device> _device, const String& _fpoName);

    private:
        using VertexProgRepo = Map<String, VertexProgram>;
        using PixelProgRepo = Map<String, PixelProgram>;

        VertexProgRepo m_vpoRepo;
        PixelProgRepo m_fpoRepo;
    };
}
