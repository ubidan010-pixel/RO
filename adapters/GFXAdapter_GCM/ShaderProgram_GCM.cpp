#include "precompiled_GFXAdapter_GCM.h"

#include "GCM_API.h"

#include "adapters/FileManager_ITF/FileManager_ITF.h"
#include "engine/common/WarningManager.h"
#include "core/math/MathTools.h"
#include "VRAMAllocator_GCM.h"
#include "VertexDecl_GCM.h"
#include "ShaderProgram_GCM.h"
#include "RenderContext_GCM.h"

namespace ITF
{
    namespace GCM
    {
        // return a CGprogram that is an allocated u8[] (need to be freed)
        static CGprogram loadCGProgramRawData(const String & _fileName)
        {
            File* f = FILEMANAGER->openFile(_fileName, ITF_FILE_ATTR_READ);
            if (f)
            {
                u64 fileSize = f->getLength();
                u8 * readBuffer = newAlloc(mId_Temporary, u8[fileSize]);
                u32 sizeRead = 0;
                f->read(readBuffer,fileSize,&sizeRead);
                ITF_ASSERT(fileSize == sizeRead);
                FILEMANAGER->closeFile(f);
                return (CGprogram)readBuffer;
            }
            else
            {
                ITF_WARNING(NULL, bfalse, "CG program file %ls not found", _fileName.cStr());
                return NULL;
            }
        }

        void VertexProgram::load(const String & _vpoName)
        {
            instructionSlot = u16(VertexShaderCache::InvalidInstructionSlot); // not in any slot

            cg = loadCGProgramRawData(_vpoName);
            if (!cg)
            {
                return;
            }

            u8 * rawSrc = (u8 *)cg;
            CgBinaryProgram * header = (CgBinaryProgram *)cg;
            CgBinaryVertexProgram * vertexInfo = (CgBinaryVertexProgram *) (rawSrc + header->program);

            ITF_ASSERT(header->binaryFormatRevision == CG_BINARY_FORMAT_REVISION);

            // copy config
            instructionCount = u16(vertexInfo->instructionCount);
            attributeInputMask =  vertexInfo->attributeInputMask;
            registerCount = vertexInfo->registerCount;

            ucodeSize = header->ucodeSize;
            u32 ucodeOffset = header->ucode;
            ucode = rawSrc + ucodeOffset;

        #ifdef CHECK_VERTEXDECL_SHADER_COMPATIBILITY

            m_vpoName = _vpoName;

            #define CG_DATATYPE_MACRO(name, compiler_name, enum_name, base_name, nrows, ncols, pc_name) (ncols),
            static const u32 CG_Data_Sizes[] = 
            {
                #include <Cg/cg_datatypes.h>   
            };
            #undef CG_DATATYPE_MACRO

            // Now we add vertex input info to check vertex declarations
            // fill the vertex attribs from the shader
            for(CGparameter param = cellGcmCgGetFirstLeafParameter(cg, CG_PROGRAM);
                param!=0;
                param = cellGcmCgGetNextLeafParameter(cg, param))
            {
                if (    0 != cellGcmCgGetParameterReferenced(cg, param)
                    &&  CG_VARYING == cellGcmCgGetParameterVariability(cg, param)
                    &&  CG_IN == cellGcmCgGetParameterDirection(cg, param) )
                {
                    CGresource semantic = cellGcmCgGetParameterResource(cg, param);
                    u32 usage = u32(semantic - CG_ATTR0);

                    // add it only if not already present
                    bool addIt = true;
                    for(u32 i = 0, n = m_vertexInputs.size(); i!=n; ++i)
                    {
                        if (m_vertexInputs[i].usage == usage)
                        {
                            addIt = false;
                        }
                    }
                    if (addIt)
                    {
                        VertexInput vtxInput;
                        vtxInput.usage = usage;
                        CGtype cgType = cellGcmCgGetParameterType(cg, param);
                        vtxInput.size = CG_Data_Sizes[cgType - CG_TYPE_START_ENUM - 1];
                        m_vertexInputs.push_back(vtxInput);
                    }
                }
            }
        #endif // CHECK_VERTEXDECL_SHADER_COMPATIBILITY
        }

        void VertexProgram::destroy()
        {
            delete [] ((u8*)cg);
            cg = NULL;
        }


        bool VertexProgram::bind(const FxParameterDB & _paramDB)
        {
            ShaderParameterMapping & mapping = m_paramMapping[_paramDB.getType()];
            if (mapping.getBoundDBType() == FxParameterDB::TYPE_INVALID)
            {
                return mapping.bind(_paramDB, cg);
            }
            ITF_ASSERT(mapping.getBoundDBType() == _paramDB.getType());
            return true; // already mapped on this type
        }

        void VertexProgram::apply(RenderContext & _rdrCtxt)
        {
            m_paramMapping[_rdrCtxt.paramDB.getType()].applyParams(_rdrCtxt, *this);
            _rdrCtxt.setVertexProgram(this); // with cache
        }

    #ifdef CHECK_VERTEXDECL_SHADER_COMPATIBILITY
        void VertexProgram::checkVertexDeclCompatibility(const GCM::VertexDecl * _vertexDecl)
        {
            // this is checking that every semantic of the shader get a value from the vertex decl
            // first check if not already validated
            for (u64 i = 0, n = m_vdeclCompatibles.size();
                i!=n;
                ++i)
            {
                if (_vertexDecl == m_vdeclCompatibles[i])
                {
                    return;
                }
            }

            // one size
            for (u64 i = 0, n=m_vertexInputs.size(); i!=n; ++i)
            {
                const VertexInput & vInput = m_vertexInputs[i];
                bool usageFound = false;
                // search the same usage in the VDecl
                for(u64 i = 0, n = _vertexDecl->nbElements; i<n; ++i)
                {
                    const GCM::VertexDecl::Element & vDeclElt = _vertexDecl->elementArray[i];
                    if (vDeclElt.declType.size != 0 && vDeclElt.usage == vInput.usage)
                    {
                        usageFound = true;
                    }
                }
                if (!usageFound)
                {
                    ITF_WARNING(NULL, bfalse, "Vertex Declaration incompatible with technique %ls : %s not found in the vertex declaration.",
                                                m_vpoName.cStr(),
                                                VertexDecl::getSemanticNameFromUsage(VertexDecl::Usage::Enum(vInput.usage)));
                    // Just a warning as the vertex to shader mapping system will generate default
                }
            }

            // and the other direction
            for (u64 i = 0, n=_vertexDecl->nbElements; i!=n; ++i)
            {
                const GCM::VertexDecl::Element & vDeclElt = _vertexDecl->elementArray[i];
                bool usageFound = false;
                for (u64 i = 0, n=m_vertexInputs.size(); i!=n; ++i)
                {
                    const VertexInput & vInput = m_vertexInputs[i];
                    if (vDeclElt.usage == vInput.usage)
                    {
                        usageFound = true;
                        if (vDeclElt.declType.size > vInput.size)
                        {
                            ITF_WARNING(NULL, bfalse, "PERF : Vertex Declaration sub-optimal for technique %ls : %s smaller than what is provided by the vertex declaration.",
                                                       m_vpoName.cStr(),
                                                       VertexDecl::getSemanticNameFromUsage(vDeclElt.usage));
                        }
                    }
                }
                if (!usageFound)
                {
                    ITF_WARNING(NULL, bfalse, "PERF : Vertex Declaration sub-optimal for technique %ls : %s not requested by the shader.",
                                               m_vpoName.cStr(),
                                               VertexDecl::getSemanticNameFromUsage(vDeclElt.usage));

                }
            }
            m_vdeclCompatibles.push_back(_vertexDecl);
        }
    #endif

        FragmentProgram::FragmentProgram()
            : cg(NULL)
            , ucode(NULL)
            , ucodeSize(NULL)
            , uCodeHdl()
        {
        }

        FragmentProgram::FragmentProgram(const FragmentProgram & _fragProg)
            : cg(_fragProg.cg)
            , ucode(_fragProg.ucode)
            , ucodeSize(_fragProg.ucodeSize)
            , uCodeHdl()
        {
            uCodeHdl = _fragProg.uCodeHdl;
        }

        FragmentProgram & FragmentProgram::operator = (const FragmentProgram & _fragProg)
        {
            cg = _fragProg.cg;
            ucode = _fragProg.ucode;
            ucodeSize = _fragProg.ucodeSize;
            uCodeHdl = _fragProg.uCodeHdl;
            return *this;
        }

        void FragmentProgram::load(const String & _fpoName)
        {
            cg = loadCGProgramRawData(_fpoName);
            if (!cg)
            {
                return;
            }

            // adjust the register count to a minimal value
            // Best perf would set that per draw,
            // but a minimal value set at draw is easier to do and giving good results
            u32 curRegCount = cellGcmCgGetRegisterCount(cg);
            enum { MinimalFragmentRegCount = 7 };
            cellGcmCgSetRegisterCount(cg, Max(curRegCount, u32(MinimalFragmentRegCount)));

            cellGcmCgGetUCode(cg, &ucode, &ucodeSize);
            uCodeHdl.AllocAligned(ucodeSize, 64, VRAM_Shader);
            memcpy(uCodeHdl.GetAddress(), ucode, ucodeSize);  // ! upload to VRAM
        }

        void FragmentProgram::destroy()
        {
            uCodeHdl = InvalidRelocHandle; // release handle (free memory)
            delete [] ((u8 *)cg);
            cg = NULL;
        }

        bool FragmentProgram::bind(const FxParameterDB & _paramDB)
        {
            ShaderParameterMapping & mapping = m_paramMapping[_paramDB.getType()];
            if (mapping.getBoundDBType() == FxParameterDB::TYPE_INVALID)
            {
                return mapping.bind(_paramDB, cg);
            }
            ITF_ASSERT(mapping.getBoundDBType() == _paramDB.getType());
            return true; // already mapped on this type
        }

        void FragmentProgram::apply(RenderContext & _rdrCtxt)
        {
            m_paramMapping[_rdrCtxt.paramDB.getType()].applyParams(_rdrCtxt, *this);
            _rdrCtxt.setFragmentProgram(this); // with cache            
        }

        ShaderProgramRepository::ShaderProgramRepository()
        {
        }

        ShaderProgramRepository::~ShaderProgramRepository()
        {
            for(VertexProgRepo::iterator it = m_vpoRepo.begin(), itEnd = m_vpoRepo.end();
                it!=itEnd;
                ++it)
            {
                it->second.destroy();
            }

            for(FragmentProgRepo::iterator it = m_fpoRepo.begin(), itEnd = m_fpoRepo.end();
                it!=itEnd;
                ++it)
            {
                it->second.destroy();
            }
        }

        template <typename Map>
        static typename Map::mapped_type * getOrLoadShaderProg(Map & map, const String & _shaderName)
        {
            typedef typename Map::mapped_type ShaderProg;
            ShaderProg newShaderProg;
            std::pair<typename Map::iterator, bool> insertResPair = map.insert(Map::value_type(_shaderName, newShaderProg));
            if (insertResPair.second)
            {
                ShaderProg & prog = insertResPair.first->second;
                prog.load(_shaderName);
                if (!prog.cg) // not able to load ?
                {
                    map.erase(insertResPair.first); // remove it and return NULL
                    return NULL;
                }
            }
            return &insertResPair.first->second;
        }

        VertexProgram * ShaderProgramRepository::getVPO(const String & _vpoName)
        {
            return getOrLoadShaderProg(m_vpoRepo, _vpoName);
        }

        FragmentProgram * ShaderProgramRepository::getFPO(const String & _fpoName)
        {
            return getOrLoadShaderProg(m_fpoRepo, _fpoName);
        }
    }
}
