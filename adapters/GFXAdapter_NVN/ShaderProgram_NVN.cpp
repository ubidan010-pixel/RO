#include "precompiled_GFXAdapter_NVN.h"

#include "ShaderProgram_NVN.h"

#include "NVN/Helpers.h"
#include <nvnTool/nvnTool_GlslcInterface.h>
#include "core/Macros.h"
#include "core/UnicodeTools.h"
#include "core/AdaptersInterfaces/FileManager.h"
#include "engine/common/WarningManager.h"

#include "core/Math/BitTweak.h"

#include "VRAMAllocator/VRAMAllocator.h"

#include "GLSLC/MetaData.h"

namespace ITF
{

    ShaderProgram::ShaderProgram() = default;
    ShaderProgram::~ShaderProgram() = default;

    static Vector<u8> readGLSLCBinaryFileContentInBundle(const String& _fileName)
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
    static Vector<u8> readGLSLCBinaryFileContentDirect(const String& _fileName)
    {
        Vector<u8> content;

        String directAccessFileName = String("rom:/") + _fileName;

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

    static Vector<u8> readGLSLCBinaryFileContent(const String & _fileName)
    {
        Vector<u8> content;

        // Try to read not bundled first to be able to change shaders without rebuilding the bundles
#ifndef ITF_FINAL
        content = readGLSLCBinaryFileContentDirect(_fileName);
#endif

        if (content.empty())
        {
            content = readGLSLCBinaryFileContentInBundle(_fileName);
        }

        return content;
    }

#if defined(ITF_ENABLE_NVN_GRAPHICS_DEBUGGING) && ITF_ENABLE_NVN_GRAPHICS_DEBUGGING
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

    void ShaderProgram::loadShaderGPUCode(const u8 * _gpuCodeDataBlock, u32 _dataSize)
    {
        using NVN::VRAMAllocator::ChunkSize;
        static NVN::VRAMAllocator::ChunkSize gpuCodeMemAlignment = NVN::VRAMAllocator::getDeviceProperty(nvn::DeviceInfo::SHADER_CODE_ALIGNMENT);
        static NVN::VRAMAllocator::ChunkSize gpuCodePaddingSize = NVN::VRAMAllocator::getDeviceProperty(nvn::DeviceInfo::SHADER_CODE_MEMORY_POOL_PADDING_SIZE);

        m_effectiveGPUCodeSize = _dataSize;
        u32 allocatedSize = BitTweak::GetAboveAlign(_dataSize + gpuCodePaddingSize, gpuCodeMemAlignment);
        m_gpuCode.allocAligned<NVN::VRAMResourceType::SHADER>(allocatedSize, gpuCodeMemAlignment, NVN::VRAM_Shader);

        void* cpuMap = m_gpuCode.getBuffer()->Map();
        ITF_Memcpy(cpuMap, _gpuCodeDataBlock, m_effectiveGPUCodeSize);

    #if defined(ITF_ENABLE_NVN_GRAPHICS_DEBUGGING) && ITF_ENABLE_NVN_GRAPHICS_DEBUGGING
        m_debugShortName = getShortFileName(m_loadedFileName);
        m_gpuCode.getBuffer()->SetDebugLabel(m_debugShortName.cStr());
    #endif
    }

    void ShaderProgram::loadControlData(const u8* _controlDataBlock, u32 _controlSize)
    {
        m_controlData.reserve(_controlSize);
        m_controlData.assign(_controlDataBlock, _controlDataBlock + _controlSize);
    }

    void ShaderProgram::load(const String& fileName)
    {
        m_loadedFileName.clear();
        const Vector<u8> fileContent = readGLSLCBinaryFileContent(fileName);

        if (fileContent.empty())
        {
            ITF_WARNING(NULL, bfalse, "Shader program file %s not found or empty", StringToUTF8(fileName).get());
            return;
        }

        m_loadedFileName = StringToUTF8(fileName.cStr()).get();

        const GLSLCoutput* glslcOut = reinterpret_cast<const GLSLCoutput*>(fileContent.data());

        for (uint32_t i = 0; i < glslcOut->numSections; ++i)
        {
            const GLSLCsectionHeaderUnion& currHeader = glslcOut->headers[i];
            if (currHeader.genericHeader.common.type == GLSLC_SECTION_TYPE_GPU_CODE)
            {
                ITF_ASSERT(!m_gpuCode.isValid());

                if (m_gpuCode.isValid())
                    continue;

                const GLSLCgpuCodeHeader& gpuCodeHeader = currHeader.gpuCodeHeader;

                const u8* gpuCodeDataBlock = reinterpret_cast<const u8*>(glslcOut) + currHeader.genericHeader.common.dataOffset;

                ITF_ASSERT(gpuCodeHeader.scratchMemBytesRecommended == 0);
                ITF_ASSERT(gpuCodeHeader.scratchMemBytesPerWarp == 0);
                loadShaderGPUCode(gpuCodeDataBlock + gpuCodeHeader.dataOffset, gpuCodeHeader.dataSize);
                loadControlData(gpuCodeDataBlock + gpuCodeHeader.controlOffset, gpuCodeHeader.controlSize);
            }
            else if (currHeader.genericHeader.common.type == GLSLC_SECTION_TYPE_REFLECTION)
            {
                const GLSLCprogramReflectionHeader& reflectionHeader = currHeader.programReflectionHeader;
                const u8 * reflectionDataBlock = reinterpret_cast<const u8*>(glslcOut) + reflectionHeader.common.dataOffset;
                const char* stringPool = reinterpret_cast<const char*>(reflectionDataBlock + reflectionHeader.stringPoolOffset);

                m_metaDataUniformBlocks = NVN::ShaderMetaData::UniformBlockDesc::buildCollection(
                    reinterpret_cast<const GLSLCuniformBlockInfo*>(reflectionDataBlock + reflectionHeader.uniformBlockOffset),
                    reflectionHeader.numUniformBlocks,
                    stringPool);

                m_metaDataUniforms = NVN::ShaderMetaData::UniformDesc::buildCollection(
                    reinterpret_cast<const GLSLCuniformInfo*>(reflectionDataBlock + reflectionHeader.uniformOffset),
                    reflectionHeader.numUniforms,
                    stringPool);

                m_metaDataInputs = NVN::ShaderMetaData::InputDesc::buildCollection(
                    reinterpret_cast<const GLSLCprogramInputInfo*>(reflectionDataBlock + reflectionHeader.programInputsOffset),
                    reflectionHeader.numProgramInputs,
                    stringPool);

                m_metaDataOutputs = NVN::ShaderMetaData::OutputDesc::buildCollection(
                    reinterpret_cast<const GLSLCprogramOutputInfo*>(reflectionDataBlock + reflectionHeader.programInputsOffset),
                    reflectionHeader.numProgramInputs,
                    stringPool);
            }
        }
    }

    nvn::ShaderData ShaderProgram::getShaderData() const
    {
        nvn::ShaderData shaderData{};
        shaderData.control = m_controlData.data();
        if (m_gpuCode.getBuffer() != nullptr)
            shaderData.data = m_gpuCode.getBuffer()->GetAddress();

        return shaderData;
    }

    bool ShaderProgram::bindParameterDB(const FxParameterDB& _paramDB)
    {
        String8 cbName = _paramDB.getName();
        if (cbName.isEmpty())
            cbName = "type_$Globals.$Globals"; // glslc convention

        if (m_paramMapping.end() != m_paramMapping.find(_paramDB.getNameID()))
            return true; // already bound

        bool success = false;

        auto & paramMappingForDB = m_paramMapping[_paramDB.getNameID()];

        success = paramMappingForDB.bindParameterDB(_paramDB, m_metaDataUniformBlocks, m_metaDataUniforms);

        ITF_ASSERT_MSG(success, "Cannot bind constant buffer %s or texture samplers to shader %s", cbName.cStr(), m_loadedFileName.cStr());

        return success;
    }

    template <nvn::ShaderStage::Enum stage>
    void ShaderProgram::applyParamDB(nvn::CommandBuffer& _rdrCtxt, NVNUniformBufferPool& _uniformPool, const FxParameterDB& _paramDB)
    {
        auto itParamMapping = m_paramMapping.find(_paramDB.getNameID());
        ITF_ASSERT_MSG(itParamMapping != m_paramMapping.end(), "FX Parameter DB %s is not bound with shader %s", _paramDB.getName(), m_loadedFileName.cStr());
        if (itParamMapping != m_paramMapping.end())
        {
            itParamMapping->second.applyParams<stage>(_rdrCtxt, _uniformPool, _paramDB);
        }
    }

    VertexProgram::VertexProgram() = default;
    VertexProgram::~VertexProgram() = default;

    void VertexProgram::checkVertexDeclCompatibility(const VertexFormatDescriptor& _vertexDecl)
    {
        // $GB: todo, parsing m_metaDataResources
#if 0
        for (auto & resDesc : m_metaDataResources)
        {
            
        };
#endif
    }

    void VertexProgram::applyParamDB(nvn::CommandBuffer& _rdrCtxt, NVNUniformBufferPool& _uniformPool, const FxParameterDB& _paramDB)
    {
        ShaderProgram::applyParamDB<nvn::ShaderStage::VERTEX>(_rdrCtxt, _uniformPool, _paramDB);
    }

    PixelProgram::PixelProgram() = default;
    PixelProgram::~PixelProgram() = default;

    void PixelProgram::applyParamDB(nvn::CommandBuffer& _rdrCtxt, NVNUniformBufferPool& _uniformPool, const FxParameterDB& _paramDB)
    {
        ShaderProgram::applyParamDB<nvn::ShaderStage::FRAGMENT>(_rdrCtxt, _uniformPool, _paramDB);
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

