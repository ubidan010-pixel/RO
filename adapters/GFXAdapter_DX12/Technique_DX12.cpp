#include "precompiled_GFXAdapter_DX12.h"

#include "Technique_DX12.h"
#include "ShaderProgram_DX12.h"
#include "VertexFormat_DX12.h"
#include "core/XML/PugiXMLWrap.h"
#include "engine/common/WarningManager.h"

#ifndef ITF_FINAL
#define CHECK_VERTEXDECL_SHADER_COMPATIBILITY
#endif

namespace ITF::DX12
{

#if defined(ITF_ENABLE_DX12_GRAPHICS_DEBUGGING) && ITF_ENABLE_DX12_GRAPHICS_DEBUGGING
    static String8 getShortTechFileName(const String8& fileName)
    {
        const char* shortFileNamePtr = ::strrchr(fileName.cStr(), '/');
        if (shortFileNamePtr == nullptr)
            shortFileNamePtr = ::strrchr(fileName.cStr(), '\\');

        if (shortFileNamePtr != nullptr)
        {
            shortFileNamePtr++;
            return String8(shortFileNamePtr);
        }

        return fileName;
    }
#endif


    void PSOCache::setIfChanged(ID3D12GraphicsCommandList* _rdrCtxt, Technique& _technique, BlendState _blendState)
    {
        if (&_technique != m_lastTechnique || _blendState != m_lastBlendState)
        {
            setMarker(_rdrCtxt, _technique.getPSOName(_blendState));
            ID3D12RootSignature* newRootSig = _technique.getRootSignature()->get();
            if (newRootSig != m_lastRootSignature)
            {
                _rdrCtxt->SetGraphicsRootSignature(newRootSig);
                m_lastRootSignature = newRootSig;
            }
            _rdrCtxt->SetPipelineState(_technique.getOrCreatePSO(_blendState));
            m_lastTechnique = &_technique;
            m_lastBlendState = _blendState;
        }
    }

    void PSOCache::reset()
    {
        m_lastTechnique = nullptr;
        m_lastRootSignature = nullptr;
        m_lastBlendState = BlendState();
    }


    Technique::Technique(ComPtr<ID3D12Device> _device, PugiXMLWrap& _xmlParser, const String& _fxPathAndName)
        : m_device(std::move(_device))
    {
        const char* techName = _xmlParser.readAttribute("name");
        String techFullName;
        techFullName.setTextFormat("%s_%s", StringToUTF8(_fxPathAndName).get(), techName);
        m_techFullName = techFullName;
        m_techFullName8 = StringToUTF8(m_techFullName).get();

        if (_strnicmp("Tech_", techName, strlen("Tech_")))
        {
            ITF_WARNING(NULL, bfalse, "Cannot get an ordinal number for Technique %s", m_techFullName8.cStr());
            m_techNum = INVALID_TECH_NUMBER;
        }
        m_techNum = atoi(techName + strlen("Tech_"));

        if (_xmlParser.enterElement("pass"))
        {
            u32 vtxCount = 0, fragCount = 0;
            if (_xmlParser.enterElement("entry"))
            {
                do
                {
                    // the first (and supposedly only) element is our entry point
                    _xmlParser.enterFirstElement();
                    const char* entry = _xmlParser.readElementValue();
                    _xmlParser.exitElement();

                    if (!_strcmpi(_xmlParser.readAttribute("type", "Vertex"), "Fragment"))
                    {
                        loadFragmentProgram(_fxPathAndName, entry);
                        fragCount++;
                    }
                    else
                    {
                        loadVertexProgram(_fxPathAndName, entry);
                        vtxCount++;
                    }
                } while (_xmlParser.nextSameElement(true));
            }
            if (vtxCount != 1 && fragCount != 1)
            {
                ITF_WARNING(NULL, bfalse, "Entries in fx file %s ill formed, last taken", StringToUTF8(_fxPathAndName).get());
            }

            // check for a second pass ?
            if (_xmlParser.nextSameElement(false))
            {
                ITF_WARNING(NULL, bfalse, "Too much pass used in fx file %s (additionals ignored)", StringToUTF8(_fxPathAndName).get());
            }
            _xmlParser.exitElement();
        }
        else
        {
            ITF_WARNING(NULL, bfalse, "No pass found in a technique in fx file %s", StringToUTF8(_fxPathAndName).get());
        }

        if (m_vertexProg)
        {
            m_vertexFormat = m_vertexProg->getVertexFormat();
            m_inputLayoutDesc = DX12::getInputLayoutDesc(m_vertexFormat);
            createPSOVariants();
        }
    }

    void Technique::createPSOVariants()
    {
        createPSOVariant(BlendState{ GFX_BLEND_COPY });
        createPSOVariant(BlendState{ GFX_BLEND_ALPHA });
        createPSOVariant(BlendState{ GFX_BLEND_ADDALPHA });
    }

    void Technique::createPSOVariant(BlendState _blendState)
    {
        D3D12_RASTERIZER_DESC rasterizer = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        rasterizer.CullMode = D3D12_CULL_MODE_NONE;

        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = m_inputLayoutDesc;
        psoDesc.pRootSignature = m_vertexProg->getRootSignature().get();
        psoDesc.VS = m_vertexProg->getShaderData();
        psoDesc.PS = m_pixelProg->getShaderData();
        psoDesc.RasterizerState = rasterizer;
        psoDesc.BlendState = _blendState.buildBlendDesc();
        psoDesc.DepthStencilState.DepthEnable = FALSE;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count = 1;

        ComPtr<ID3D12PipelineState> & pso = m_pipelineStatesByBlendStateKey[_blendState.getKeyAsUInt()];
        ITF_VERIFY_HR_CALL(m_device->CreateGraphicsPipelineState(&psoDesc, DX12_IID_COMPTR_ARGS(pso)));
        createPSODebugName(_blendState);
    }

    #if defined(ITF_ENABLE_DX12_GRAPHICS_DEBUGGING) && ITF_ENABLE_DX12_GRAPHICS_DEBUGGING
    void Technique::createPSODebugName(BlendState _blendState)
    {
        String8 str;
        str.setTextFormat("PSO %s %s VS=%s PS=%s", getShortTechFileName(m_techFullName8).cStr(), _blendState.getName(), m_vertexProg->getName(), m_pixelProg->getName());
        auto itFound = m_pipelineStatesByBlendStateKey.find(_blendState.getKeyAsUInt());
        if (itFound != m_pipelineStatesByBlendStateKey.end())
        {
            ID3D12PipelineState* pso = itFound->second.Get();
            pso->SetName(String8ToWChar(str).get());
        }
        m_pipelineStatesNames[_blendState.getKeyAsUInt()] = std::move(str);
    }
    #endif

    ID3D12PipelineState* Technique::getOrCreatePSO(BlendState _blendState)
    {
        auto itFound = m_pipelineStatesByBlendStateKey.find(_blendState.getKeyAsUInt());
        if (itFound != m_pipelineStatesByBlendStateKey.end())
        {
            return itFound->second.Get();
        }

        // Create a new PSO for this blend state.
        // A not valid behavior, as createPSOVariants() supposely created all needed variants.
        ITF_WARNING(NULL, bfalse, "Creating PSO on the fly for Technique %s and blend state %s. Update createPSOVariants().", m_techFullName8.cStr(), _blendState.getName());
        createPSOVariant(_blendState);
        itFound = m_pipelineStatesByBlendStateKey.find(_blendState.getKeyAsUInt());
        ITF_VERIFY(itFound != m_pipelineStatesByBlendStateKey.end());
        return itFound->second.Get();
    }

    const char* Technique::getPSOName(BlendState _blendState)
    {
    #if defined(ITF_ENABLE_DX12_GRAPHICS_DEBUGGING) && ITF_ENABLE_DX12_GRAPHICS_DEBUGGING
        auto itFound = m_pipelineStatesNames.find(_blendState.getKeyAsUInt());
        if (itFound != m_pipelineStatesNames.end())
        {
            return itFound->second.cStr();
        }
    #endif
        return "PSO Unknown";
    }

    void Technique::loadVertexProgram(const String & _fxPathAndName, const char * _entry)
    {
        String vpName;
        vpName.setTextFormat("%s_%s.vs.cso", StringToUTF8(_fxPathAndName).get(), _entry);
        m_vertexProg = ShaderProgramRepository::get().getVPO(m_device, vpName);
    }

    void Technique::loadFragmentProgram(const String & _fxPathAndName, const char * _entry)
    {
        String fpName;
        fpName.setTextFormat("%s_%s.ps.cso", StringToUTF8(_fxPathAndName).get(), _entry);
        m_pixelProg = ShaderProgramRepository::get().getFPO(m_device, fpName);
    }

    void Technique::checkUniformBufferMappingCompatibility(const FxParameterDB& _paramDB)
    {
    #if ASSERT_ENABLED
        ShaderParameterMapping *vsMapping = m_vertexProg->getMappingForParamDB(_paramDB);
        ShaderParameterMapping* psMapping = m_pixelProg->getMappingForParamDB(_paramDB);

        if (vsMapping == nullptr || psMapping == nullptr)
        {
            return;
        }

        vsMapping->checkCompatibilityWith(*psMapping);
    #endif

    }

    bool Technique::bind(const FxParameterDB & _paramDB, const SamplerDescriptorPool& _samplerPool)
    {
        bool successVtx = m_vertexProg->bindParameterDB(_paramDB, _samplerPool, getRootSignature()->getDescriptor());
        bool successFrag = m_pixelProg->bindParameterDB(_paramDB, _samplerPool, getRootSignature()->getDescriptor());


        return successVtx && successFrag;
    }

    void Technique::set(ID3D12GraphicsCommandList* _rdrCtxt, BlendState _blendState, PSOCache& psoCache)
    {
        psoCache.setIfChanged(_rdrCtxt, *this, _blendState);
    }

    void Technique::apply(ID3D12GraphicsCommandList* _rdrCtxt, UniformBufferPool& _uniformBufferPool, const FxParameterDB& paramDB)
    {
        if (m_vertexProg)
        {
            m_vertexProg->applyParamDB(_rdrCtxt, _uniformBufferPool, paramDB);
        }
        if (m_pixelProg)
        {
            m_pixelProg->applyParamDB(_rdrCtxt, _uniformBufferPool, paramDB);
        }
    }
}
