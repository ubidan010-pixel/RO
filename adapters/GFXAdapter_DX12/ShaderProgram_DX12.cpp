#include "precompiled_GFXAdapter_DX12.h"

#include "ShaderProgram_DX12.h"

#ifdef ITF_WIN64
#include <dxcapi.h>
#elif defined(ITF_XBOX_SERIES)
#include <dxcapi_xs.h>
#else
#include <dxcapi_x.h>
#endif

#include "DX12/Helpers.h"
#include "core/Macros.h"
#include "core/UnicodeTools.h"
#include "core/AdaptersInterfaces/FileManager.h"
#include "engine/common/WarningManager.h"

#include "core/file/FileServer.h"

#include "core/Math/BitTweak.h"

#include "VRAMAllocator/VRAMAllocator.h"

namespace ITF::DX12
{

    ShaderProgram::ShaderProgram() = default;
    ShaderProgram::~ShaderProgram() = default;

    static Vector<u8> readShaderBinaryFileContentInBundle(const String& _fileName)
    {
        File* f = FILEMANAGER->openFile(_fileName, ITF_FILE_ATTR_READ);
        if (f == nullptr)
        {
            ITF_WARNING(NULL, bfalse, "Shader program file %s not found", StringToUTF8(_fileName).get());
            return Vector<u8>{};
        }

        Vector<u8> content;
        content.resize(f->getLength());

        u32 sizeRead = 0;
        f->read(content.data(), content.size(), &sizeRead);
        ITF_ASSERT(content.size() == sizeRead);
        FILEMANAGER->closeFile(f);
        return content;
    }

#ifndef ITF_FINAL
    static Vector<u8> readShaderBinaryFileContentDirect(const String& _fileName)
    {
        Vector<u8> content;

    #ifdef ITF_WIN64
        String directAccessFileName = FILESERVER->getDataDepot() + _fileName;
    #else
        String directAccessFileName;
        directAccessFileName.setStringFormat("g:\\%ls%ls", FILESERVER->getDataDepot().cStr(), _fileName.cStr());
    #endif

        hwFile f;
        if (f.open(directAccessFileName, ITF_FILE_ATTR_READ))
        {
            content.resize(f.getLength());
            u32 sizeRead = 0;
            f.read(content.data(), content.size(), &sizeRead);
            ITF_ASSERT(sizeRead == content.size()); // read error
            content.resize(sizeRead);
        }
        return content;
    }
#endif

    static Vector<u8> readShaderBinaryFileContent(const String & _fileName)
    {
        Vector<u8> content;

        // Try to read not bundled first to be able to change shaders without rebuilding the bundles
#ifndef ITF_FINAL
        content = readShaderBinaryFileContentDirect(_fileName);
#endif

        if (content.empty())
        {
            content = readShaderBinaryFileContentInBundle(_fileName);
        }

        return content;
    }

#if defined(ITF_ENABLE_DX12_GRAPHICS_DEBUGGING) && ITF_ENABLE_DX12_GRAPHICS_DEBUGGING
    static String8 getShortFileName(const String8 & fileName)
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

    static String8 getShortShaderName(const String8& fileName)
    {
        String8 shortName = getShortFileName(fileName);
        const char* shortFileNameDot = ::strchr(shortName.cStr(), '.');
        if (shortFileNameDot == nullptr)
            return shortName;
        return shortName.substr(0, u32(shortFileNameDot - shortName.cStr()));
    }
#endif

    void ShaderProgram::createBlob()
    {
        ComPtr<IDxcUtils> dxcUtils;
        ITF_VERIFY_HR_CALL(DxcCreateInstance(CLSID_DxcUtils, DXC_IID_PPV_COMPTR_ARGS(dxcUtils)));

        ComPtr<IDxcBlobEncoding> dxcBlob;
        ITF_VERIFY_HR_CALL(dxcUtils->CreateBlobFromPinned(m_shaderRawData.data(), m_shaderRawData.size(), DXC_CP_ACP, dxcBlob.ReleaseAndGetAddressOf()));
        m_dxcBlob = std::move(dxcBlob);
    }

    void ShaderProgram::createShaderReflectionFromRawData()
    {
        ComPtr<IDxcContainerReflection> containerReflection;
        ITF_VERIFY_HR_CALL(DxcCreateInstance(CLSID_DxcContainerReflection, DXC_IID_PPV_COMPTR_ARGS(containerReflection)));
        ITF_VERIFY_HR_CALL(containerReflection->Load(m_dxcBlob.Get()));

        UINT dxilPartIndex = 0;
        ITF_VERIFY_HR_CALL(containerReflection->FindFirstPartKind(DXC_PART_DXIL, &dxilPartIndex));

        ITF_VERIFY_HR_CALL(containerReflection->GetPartReflection(dxilPartIndex, DXC_IID_PPV_COMPTR_ARGS(m_shaderReflection)));
    }

    void ShaderProgram::load(const String& fileName)
    {
        m_loadedFileName.clear();
        m_shaderRawData = readShaderBinaryFileContent(fileName);

        if (m_shaderRawData.empty())
        {
            ITF_WARNING(NULL, bfalse, "Shader program file %s not found or empty", StringToUTF8(fileName).get());
            return;
        }

        m_loadedFileName = StringToUTF8(fileName.cStr()).get();

    #if defined(ITF_ENABLE_DX12_GRAPHICS_DEBUGGING) && ITF_ENABLE_DX12_GRAPHICS_DEBUGGING
        m_debugShortName = getShortShaderName(m_loadedFileName);
    #endif

        createBlob();
        createShaderReflectionFromRawData();
    }

    bool ShaderProgram::bindParameterDB(const FxParameterDB& _paramDB, const SamplerDescriptorPool& _samplerPool, const RootSignatureDescriptor& _rootSignatureDescriptor)
    {
        String8 cbName = _paramDB.getName();

        if (m_paramMapping.end() != m_paramMapping.find(_paramDB.getNameID()))
            return true; // already bound

        bool success = false;

        auto & paramMappingForDB = m_paramMapping[_paramDB.getNameID()];

        success = paramMappingForDB.bindParameterDB(_paramDB, _samplerPool, m_shaderReflection.Get(), _rootSignatureDescriptor);

        ITF_ASSERT_MSG(success, "Cannot bind constant buffer %s or texture samplers to shader %s", cbName.cStr(), m_loadedFileName.cStr());

        return success;
    }

    ShaderParameterMapping* ShaderProgram::getMappingForParamDB(const FxParameterDB& _paramDB)
    {
        auto itParamMapping = m_paramMapping.find(_paramDB.getNameID());
        if (itParamMapping != m_paramMapping.end())
        {
            return &itParamMapping->second;
        }
        return nullptr;
    }

    template <ShaderStage stage>
    void ShaderProgram::applyParamDB(ID3D12GraphicsCommandList* _rdrCtxt, UniformBufferPool& _uniformPool, const FxParameterDB& _paramDB)
    {
        auto itParamMapping = m_paramMapping.find(_paramDB.getNameID());
        ITF_ASSERT_MSG(itParamMapping != m_paramMapping.end(), "FX Parameter DB %s is not bound with shader %s", _paramDB.getName(), m_loadedFileName.cStr());
        if (itParamMapping != m_paramMapping.end())
        {
            itParamMapping->second.applyParams<stage>(_rdrCtxt, _uniformPool, _paramDB);
        }
    }

    const char* ShaderProgram::getName() const
    {
    #if defined(ITF_ENABLE_DX12_GRAPHICS_DEBUGGING) && ITF_ENABLE_DX12_GRAPHICS_DEBUGGING
        return m_debugShortName.cStr();
    #else
        return "Unknown";
    #endif
    }


    VertexProgram::VertexProgram() = default;
    VertexProgram::~VertexProgram() = default;

    void VertexProgram::load(ComPtr<ID3D12Device> _device, const String& _fileName)
    {
        ShaderProgram::load(_fileName);
        fetchVertexFormatFromReflection();
        buildRootSignature(std::move(_device));
    }

    VertexProgram::Semantic VertexProgram::Semantic::build(const D3D12_SIGNATURE_PARAMETER_DESC& _desc)
    {
        Semantic result{};
        if (::_stricmp(_desc.SemanticName, "POSITION") == 0)
            result.kind = SemanticKind::POSITION;
        else if (::_stricmp(_desc.SemanticName, "NORMAL") == 0)
            result.kind = SemanticKind::NORMAL;
        else if (::_stricmp(_desc.SemanticName, "COLOR") == 0)
            result.kind = SemanticKind::COLOR;
        else if (::_stricmp(_desc.SemanticName, "TEXCOORD") == 0)
            result.kind = SemanticKind::TEXCOORD;
        else if (::_stricmp(_desc.SemanticName, "BLENDINDICES") == 0)
            result.kind = SemanticKind::BLENDINDICES;
        else
            ITF_ASSERT_MSG(false, "Semantic %s not handled", _desc.SemanticName);
        result.index = _desc.SemanticIndex;
        return result;
    }

    DX12::VertexFormat VertexProgram::findVertexFormat(const Vector<Semantic>& _inputSemantics)
    {
        using VF = DX12::VertexFormat;

        struct SemanticsToFormat
        {
            VF format;
            Vector<Semantic> semantics;
        };

        static const SemanticsToFormat semanticsToVertexFormatMap[] =
        {
            {VF::P,
                Vector<Semantic>{
                    {SemanticKind::POSITION, 0}}},
            {VF::PC,
                Vector<Semantic>{
                    {SemanticKind::POSITION, 0},
                    {SemanticKind::COLOR, 0}}},
            {VF::PCT,
                Vector<Semantic>{
                    {SemanticKind::POSITION, 0},
                    {SemanticKind::COLOR, 0},
                    {SemanticKind::TEXCOORD, 0}}},
            {VF::PT,
                Vector<Semantic>{
                    {SemanticKind::POSITION, 0},
                    {SemanticKind::TEXCOORD, 0}}},
            {VF::PCBT,
                Vector<Semantic>{
                    {SemanticKind::POSITION, 0},
                    {SemanticKind::COLOR, 0},
                    {SemanticKind::BLENDINDICES, 0},
                    {SemanticKind::TEXCOORD, 0}}},
            {VF::PNCT,
                Vector<Semantic>{
                    {SemanticKind::POSITION, 0},
                    {SemanticKind::NORMAL, 0},
                    {SemanticKind::COLOR, 0},
                    {SemanticKind::TEXCOORD, 0}}},
            {VF::PNC3T,
                Vector<Semantic>{
                    {SemanticKind::POSITION, 0},
                    {SemanticKind::COLOR, 0},
                    {SemanticKind::TEXCOORD, 0},
                    {SemanticKind::TEXCOORD, 1},
                    {SemanticKind::TEXCOORD, 2},
                    {SemanticKind::TEXCOORD, 3}}},
        };

        for (const auto& semToFmt : semanticsToVertexFormatMap)
        {
            if (semToFmt.semantics.size() == _inputSemantics.size())
            {
                bool match = true;
                for (ux i = 0; i < semToFmt.semantics.size(); ++i)
                {
                    if (semToFmt.semantics[i].kind != _inputSemantics[i].kind ||
                        semToFmt.semantics[i].index != _inputSemantics[i].index)
                    {
                        match = false;
                        break;
                    }
                }
                if (match)
                    return semToFmt.format;
            }
        }

        ITF_ASSERT_MSG(false, "The vertex format of the shader is unknown");

        return VF::PCT;
    }

    void VertexProgram::fetchVertexFormatFromReflection()
    {
        D3D12_SHADER_DESC shaderDesc{};
        ITF_VERIFY_HR_CALL(m_shaderReflection->GetDesc(&shaderDesc));

        m_inputSemantics.clear();

        for (ux i = 0; i < shaderDesc.InputParameters; ++i)
        {
            D3D12_SIGNATURE_PARAMETER_DESC paramDesc{};
            ITF_VERIFY_HR_CALL(m_shaderReflection->GetInputParameterDesc(i, &paramDesc));
            m_inputSemantics.emplace_back(Semantic::build(paramDesc));
        }

        m_vertexFormat = findVertexFormat(m_inputSemantics);
    }

    void VertexProgram::buildRootSignature(ComPtr<ID3D12Device> _device)
    {
        m_rootSignature = RootSignature(std::move(_device), m_dxcBlob);
    }

    void VertexProgram::applyParamDB(ID3D12GraphicsCommandList* _rdrCtxt, UniformBufferPool& _uniformPool, const FxParameterDB& _paramDB)
    {
        ShaderProgram::applyParamDB<ShaderStage::Vertex>(_rdrCtxt, _uniformPool, _paramDB);
    }

    PixelProgram::PixelProgram() = default;
    PixelProgram::~PixelProgram() = default;

    void PixelProgram::applyParamDB(ID3D12GraphicsCommandList* _rdrCtxt, UniformBufferPool& _uniformPool, const FxParameterDB& _paramDB)
    {
        ShaderProgram::applyParamDB<ShaderStage::Pixel>(_rdrCtxt, _uniformPool, _paramDB);
    }

    // Shader program repository
    template <typename Map>
    static typename Map::mapped_type * getOrLoadShaderProg(ComPtr<ID3D12Device> _device, Map& map, const String& _shaderName)
    {
        using ShaderProg = typename Map::mapped_type;
        
        auto itFound = map.find(_shaderName);
        if (itFound == map.end())
        {
            ShaderProg & newShaderProg = map[_shaderName];
            newShaderProg.load(std::move(_device), _shaderName);
            if (!newShaderProg.isLoaded()) // not able to load ?
            {
                map.erase(_shaderName); // remove it and return NULL
                return nullptr;
            }
            // shader loaded, return it
            return &newShaderProg;
        }
        else
        {
            // shader already loaded, return it
            return &itFound->second;
        }
    }

    ShaderProgramRepository::ShaderProgramRepository() = default;
    ShaderProgramRepository::~ShaderProgramRepository() = default;

    VertexProgram* ShaderProgramRepository::getVPO(ComPtr<ID3D12Device> _device, const String& _vpoName)
    {
        return getOrLoadShaderProg(std::move(_device), m_vpoRepo, _vpoName);
    }

    PixelProgram* ShaderProgramRepository::getFPO(ComPtr<ID3D12Device> _device, const String& _fpoName)
    {
        return getOrLoadShaderProg(std::move(_device), m_fpoRepo, _fpoName);
    }

    void ShaderProgramRepository::clear()
    {
        m_vpoRepo.clear();
        m_fpoRepo.clear();
    }
}
