#pragma once

#include "core/types.h"
#include "core/memory/UniquePtr.h"
#include "core/itfstring.h"
#include "core/container/extendedMap.h"
#include "ShaderProgram_DX12.h"
#include "RootSignature_DX12.h"
#include "SamplerDescriptorPool_DX12.h"
#include "UniformBufferPool_DX12.h"
#include "BlendState_DX12.h"

namespace ITF
{
    class PugiXMLWrap;

    namespace DX12
    {
        class FxParameterDB;
        class VertexProgram;
        class PixelProgram;
        struct VertexFormatDescriptor;

        class Technique;

        // Todo => handle also root signature changes?
        class PSOCache
        {
        public:
            // This class is used to cache the last Technique set in the command buffer.
            PSOCache() = default;
            ~PSOCache() = default;

            void setIfChanged(ID3D12GraphicsCommandList * _rdrCtxt, Technique& _technique, BlendState _blendState);

            // reset the cache so that the next call to setIfChanged will reassign the program in the cmd buffer.
            // Call reset each time a shader program is changed without using this cache.
            void reset();

        private:
            Technique* m_lastTechnique = nullptr;
            ID3D12RootSignature* m_lastRootSignature = nullptr;
            BlendState m_lastBlendState;
        };

        // A Technique is in practice a root signature + vertex + pixel shaders, ie. a PSO (Pipeline State Object) with variable blend state.
        // As the blend state is variable, it contains in practice several PSO (Pipeline State Objects), indexed by blend state key.
        // Most common blend states are created at init, but some can be added dynamically.
        // The Technique does not own the shaders, as they are stored in a shader repository (thus allowing several techniques to share the same shaders).
        // The DX12RootSignature object is owned by the technique, but the D3D object is shared referenced.
        class Technique
        {
        public:

            explicit Technique::Technique(ComPtr<ID3D12Device> _device, PugiXMLWrap& _xmlParser, const String& _fxPathAndName);
            Technique(const Technique &) = delete;
            ~Technique() = default; // not owning the programs (Shader repository role)

            Technique & operator = (const Technique&) = delete;

            enum : ux { INVALID_TECH_NUMBER = UX_INVALID };
            [[nodiscard]] ux getTechNumber() const { return m_techNum; }

            bool bind(const FxParameterDB & _paramDB, const SamplerDescriptorPool& _samplerPool); // return false if parameters are missing

            [[nodiscard]] DX12::VertexFormat getVertexFormat() const { return m_vertexProg == nullptr ? DX12::VertexFormat::P : m_vertexProg->getVertexFormat(); }

            [[nodiscard]] bool isValid() const { return m_vertexProg != nullptr && m_pixelProg != nullptr; }
            [[nodiscard]] const VertexProgram * getVertexProgram() const { return m_vertexProg; }
            [[nodiscard]] const PixelProgram* getPixelProgram() const { return m_pixelProg; }

            [[nodiscard]] const RootSignature * getRootSignature() const { return m_vertexProg == nullptr ? nullptr : &m_vertexProg->getRootSignature(); }

            void set(ID3D12GraphicsCommandList* _rdrCtxt, BlendState _blendState, PSOCache & psoCache); // set the PSO in the command buffer, using the cache
            void apply(ID3D12GraphicsCommandList* _rdrCtxt, UniformBufferPool & _uniformBufferPool, const FxParameterDB & paramDB); // set the parameters in the command buffer (call it after the set)

            ID3D12PipelineState* getOrCreatePSO(BlendState _blendState);
            [[nodiscard]] const char * getPSOName(BlendState _blendState);

            [[nodiscard]] const String & getName() const { return m_techFullName; }
            [[nodiscard]] const String8 & getName8() const { return m_techFullName8; }

        private:
            void loadVertexProgram(const String& _fxPathAndName, const char* _entry);
            void loadFragmentProgram(const String& _fxPathAndName, const char* _entry);
            void checkUniformBufferMappingCompatibility(const FxParameterDB& _paramDB);
            void createPSOVariants();
            void createPSOVariant(BlendState _blendState);

            ComPtr<ID3D12Device> m_device{};
            Map<u32, ComPtr<ID3D12PipelineState>> m_pipelineStatesByBlendStateKey{};
        #if defined(ITF_ENABLE_DX12_GRAPHICS_DEBUGGING) && ITF_ENABLE_DX12_GRAPHICS_DEBUGGING
            void createPSODebugName(BlendState _blendState);
            Map<u32, String8> m_pipelineStatesNames{};
        #else
            void createPSODebugName(BlendState _blendState) {}
        #endif

            VertexProgram * m_vertexProg = nullptr;
            DX12::VertexFormat m_vertexFormat = DX12::VertexFormat::PCT; // Got from the vertex shader input signature
            D3D12_INPUT_LAYOUT_DESC m_inputLayoutDesc{};
            PixelProgram * m_pixelProg = nullptr;

            i32 m_techNum = INVALID_TECH_NUMBER;

            String m_techFullName{};
            String8 m_techFullName8{};
        };
    } // DX12
} // ITF
