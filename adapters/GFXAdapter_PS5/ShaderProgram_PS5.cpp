#include "precompiled_GFXAdapter_PS5.h"

#include "ShaderProgram_PS5.h"
#include "core/Macros.h"
#include "core/UnicodeTools.h"
#include "core/AdaptersInterfaces/FileManager.h"
#include "engine/common/WarningManager.h"

#include "Ags/AgsLoaderInStruct_PS5.h"


namespace ITF
{
    ShaderProgram::ShaderProgram() = default;
    ShaderProgram::~ShaderProgram() = default;

    static Vector<u8> readAgsFileContentInBundle(const String& _fileName)
    {
        File* f = FILEMANAGER->openFile(_fileName, ITF_FILE_ATTR_READ);
        if (f == nullptr)
        {
            ITF_WARNING(NULL, bfalse, "Shader program file %ls not found", _fileName.cStr());
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
    static Vector<u8> readAgsFileContentDirect(const String& _fileName)
    {
        Vector<u8> content;

        String directAccessFileName = String("/app0/") + _fileName;

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

    static Vector<u8> readAgsFileContent(const String & _fileName)
    {
        Vector<u8> content;

        // Try to read not bundled first to be able to change shaders without rebuilding the bundles
#ifndef ITF_FINAL
        content = readAgsFileContentDirect(_fileName);
#endif

        if (content.empty())
        {
            content = readAgsFileContentInBundle(_fileName);
        }

        return content;
    }

    static Gnmp::SimpleVRAMRelocHandle loadAgcShaderHeaderAddress(const SceShaderBinaryHandle shaderBinHdl)
    {
        const sce::Agc::Shader* header = reinterpret_cast<const sce::Agc::Shader*>(sceShaderGetProgramHeader(shaderBinHdl));
        uSize headerSize = sceShaderGetProgramHeaderSize(shaderBinHdl);

        // Copy header data
        Gnmp::SimpleVRAMRelocHandle headerAddr;
        headerAddr.allocAligned(headerSize, sce::Agc::Alignment::kShaderHeader, Gnmp::VRAM_Shader);
        sce::Agc::Shader* agcShader = reinterpret_cast<sce::Agc::Shader*>(headerAddr.getAddress());
        ITF_Memcpy(agcShader, header, headerSize);
        return headerAddr;
    }

    static Gnmp::SimpleVRAMRelocHandle loadShaderCodeAddress(const SceShaderBinaryHandle shaderBinHdl)
    {
        const u8* uCodeSrc = reinterpret_cast<const u8*>(sceShaderGetProgram(shaderBinHdl));
        uSize microcodeSize = sceShaderGetProgramSize(shaderBinHdl);

        // Copy shader microcode
        Gnmp::SimpleVRAMRelocHandle microCodeAddr;
        microCodeAddr.allocAligned(microcodeSize, sce::Agc::Alignment::kShaderCode, Gnmp::VRAM_Shader);
        ITF_Memcpy(microCodeAddr.getAddress(), uCodeSrc, microcodeSize);
        return microCodeAddr;
    }

#if defined(ITF_ENABLE_RAZOR_GPU_MARKERS) && ITF_ENABLE_RAZOR_GPU_MARKERS
    static String8 getShortFileName(const String8 & fileName)
    {
        const char* shortFileNamePtr = ::strrchr(fileName.cStr(), '/');
        if (shortFileNamePtr != nullptr)
        {
            shortFileNamePtr++;
            return String8(shortFileNamePtr);
        }
        return fileName;
    }
#endif

    void ShaderProgram::load(const String& fileName)
    {
        m_loadedFileName = UTF16ToUTF8(fileName.cStr()).get();
#if defined(ITF_ENABLE_RAZOR_GPU_MARKERS) && ITF_ENABLE_RAZOR_GPU_MARKERS
        m_markerString = String8("set shader ") + getShortFileName(m_loadedFileName);
#endif

        auto agsFileContent = readAgsFileContent(fileName);

        const SceShaderBinaryHandle shaderBinHdl = sceShaderGetBinaryHandle(agsFileContent.data());

        m_headerAddr = loadAgcShaderHeaderAddress(shaderBinHdl);
        m_microCodeAddr = loadShaderCodeAddress(shaderBinHdl);
        sce::Agc::Shader* agcShader = reinterpret_cast<sce::Agc::Shader*>(m_headerAddr.getAddress());
        ITF_VERIFY_SCE_CALL(sce::Agc::createShader(&m_agcShader, agcShader, m_microCodeAddr.getAddress()));
        ITF_ASSERT(agcShader == m_agcShader);

        m_resHandler.setResource(m_agcShader);
        m_resHandler.setFromPath(m_loadedFileName.cStr());

        m_metaDataResources = AGS::ResourceDesc::buildResourceCollection(sceShaderGetMetadataSection(shaderBinHdl), true);
        // $GB: work on linking of shaders ?
        // Maybe in technique ?
        // Might not be necessary, because handled by Gnmp?
    }

    bool ShaderProgram::bindParameterDB(const FxParameterDB& _paramDB)
    {
        String8 cbName = _paramDB.getName();
        if (cbName.isEmpty())
            cbName = "__GLOBAL_CB__"; // pssl convention

        if (m_paramMapping.find(_paramDB.getNameID()) != m_paramMapping.end())
        {
            return true; // already bound
        }

        bool success = false;

        auto itRes = std::find_if(m_metaDataResources.begin(), m_metaDataResources.end(), [&](const auto & resource)
        {
            return resource.resourceClass == SceShaderCb && resource.name == cbName;
        });

        auto & paramMappingForDB = m_paramMapping[_paramDB.getNameID()];

        if (itRes != m_metaDataResources.end())
        {
            success = paramMappingForDB.bindParameterDB(_paramDB, *itRes);
        }
        else
        {
            // No CB found in the shader, OK, it can happen (specialy for Pixel)
            success = true;
        }

        success = paramMappingForDB.bindTextureSamplersDB(_paramDB, m_metaDataResources) && success;

        ITF_ASSERT_MSG(success, "Cannot bind constant buffer %s or texture samplers to shader %s", cbName.cStr(), m_loadedFileName.cStr());

        return success;
    }

    template <Gnmp::ShaderStage stage>
    void ShaderProgram::applyParamDB(Gnmx::GfxContext& _rdrCtxt, const FxParameterDB& _paramDB)
    {
        auto itParamMapping = m_paramMapping.find(_paramDB.getNameID());
        ITF_ASSERT_MSG(itParamMapping != m_paramMapping.end(), "FX Parameter DB %s is not bound with shader %s", _paramDB.getName(), m_loadedFileName.cStr());
        if (itParamMapping != m_paramMapping.end())
        {
            itParamMapping->second.applyParams<stage>(_rdrCtxt, _paramDB);
        }
    }

    VertexProgram::VertexProgram() = default;
    VertexProgram::~VertexProgram() = default;

    void VertexProgram::load(const String& fileName)
    {
        ShaderProgram::load(fileName);

        // create the fetch shader (code taken from gnmp_basic_sample)
        sce::Agc::Gnmp::FetchShaderBuildState fsbs = { 0 };
        sce::Agc::Gnmp::generateVsFetchShaderBuildState
        (
            &fsbs,
            m_agcShader,
            m_agcShader->m_numInputSemantics,
            nullptr,
            0,
            0,
            0
        );

        // fill in the necessary semantics information in the fetch shader build state
        fsbs.m_numInputSemantics = (uint8_t)m_agcShader->m_numInputSemantics;
        fsbs.m_inputSemantics = m_agcShader->m_inputSemantics;
        fsbs.m_numInputUsageSlots = 0;
        fsbs.m_inputUsageSlots = nullptr;
        fsbs.m_numElementsInRemapTable = 0;
        fsbs.m_semanticsRemapTable = nullptr;

        u32 fetchShaderSize = fsbs.m_fetchShaderBufferSize;
        m_fetchShaderAddr.allocAligned(fetchShaderSize, Gnmp::kAlignmentOfFetchShaderInBytes, Gnmp::VRAM_Shader);

        sce::Agc::Gnmp::generateFetchShader(m_fetchShaderAddr.getAddress(), &fsbs);
    }

    void VertexProgram::checkVertexDeclCompatibility(const VertexFormatDescriptor& _vertexDecl)
    {
        // $GB: todo, parsing m_metaDataResources
#if 0
        for (auto & resDesc : m_metaDataResources)
        {
            
        };
#endif
    }


    void VertexProgram::applyParamDB(Gnmx::GfxContext& _rdrCtxt, const FxParameterDB& _paramDB)
    {
        ShaderProgram::applyParamDB<Gnmp::kShaderStageVs>(_rdrCtxt, _paramDB);
    }

    void VertexProgram::set(Gnmx::GfxContext& _context)
    {
        Gnmp::setMarker(&_context, m_markerString.cStr());
        _context.setVsShader(m_agcShader, 0, m_fetchShaderAddr.getAddress());
    }

    PixelProgram::PixelProgram() = default;
    PixelProgram::~PixelProgram() = default;

    void PixelProgram::applyParamDB(Gnmx::GfxContext& _rdrCtxt, const FxParameterDB& _paramDB)
    {
        ShaderProgram::applyParamDB<Gnmp::kShaderStagePs>(_rdrCtxt, _paramDB);
    }

    void PixelProgram::set(Gnmx::GfxContext& _context)
    {
        Gnmp::setMarker(&_context, m_markerString.cStr());
        _context.setPsShader(m_agcShader);
    }

    // Shader program repository

    template <typename Map>
    static typename Map::mapped_type * getOrLoadShaderProg(Map& map, const String& _shaderName)
    {
        using ShaderProg = typename Map::mapped_type;
        
        auto itFound = map.find(_shaderName);
        if (itFound == map.end())
        {
            ShaderProg & newShaderProg = map[_shaderName];
            newShaderProg.load(_shaderName);
            if (newShaderProg.getAgcShader() == nullptr) // not able to load ?
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

    VertexProgram* ShaderProgramRepository::getVPO(const String& _vpoName)
    {
        return getOrLoadShaderProg(m_vpoRepo, _vpoName);
    }

    PixelProgram* ShaderProgramRepository::getFPO(const String& _fpoName)
    {
        return getOrLoadShaderProg(m_fpoRepo, _fpoName);
    }

    void ShaderProgramRepository::clear()
    {
        m_vpoRepo.clear();
        m_fpoRepo.clear();
    }
}

