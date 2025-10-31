#if defined (ITF_PS3)

#ifndef ITF_SHADER_PROGRAM_GCM_H_
#define ITF_SHADER_PROGRAM_GCM_H_

#include <Cg/cg.h>
#include "VRAMRelocHdl.h"
#include "ShaderParameterMapping_GCM.h"

#ifndef ITF_FINAL
    #define CHECK_VERTEXDECL_SHADER_COMPATIBILITY
#endif

namespace ITF
{
    namespace GCM
    {
        class VertexDecl;
        class RenderContext;

        struct VertexProgram
        {
            void load(const String & _vpoName);
            void destroy(); // release resources/memory

        #ifdef CHECK_VERTEXDECL_SHADER_COMPATIBILITY
            void checkVertexDeclCompatibility(const GCM::VertexDecl * _vertexDecl);
        #else
            void checkVertexDeclCompatibility(const GCM::VertexDecl * _vertexDecl) {}
        #endif

            CGprogram       cg;         // CG binary program
            u32 ucodeSize;
            void * ucode;
            mutable u16     instructionSlot;
            u16 instructionCount;
            u16 attributeInputMask;
            u8 registerCount;

            bool bind(const FxParameterDB & _paramDB);
            void apply(RenderContext & _rdrCtxt);
        
        private:
            ShaderParameterMapping m_paramMapping[FxParameterDB::TYPE_COUNT];

        #ifdef CHECK_VERTEXDECL_SHADER_COMPATIBILITY
            // to avoid excessive checking, we keep validated vertex decls
            SafeArray<const GCM::VertexDecl *> m_vdeclCompatibles;
            struct VertexInput
            {
                u32 usage; 
                u32 size;

                u32 offset;
            };
            SafeArray<VertexInput> m_vertexInputs;
            String m_vpoName;
        #endif
        };

        struct FragmentProgram
        {
            FragmentProgram();
            FragmentProgram(const FragmentProgram & _fragProg);
            
            FragmentProgram & operator = (const FragmentProgram & _fragProg);

            void load(const String & _fpoName);
            void destroy(); // release resources/memory

            bool bind(const FxParameterDB & _paramDB);
            void apply(RenderContext & _rdrCtxt);

            CGprogram       cg;         // CG binary program
            void *          ucode;      // EA ptr, pointing inside cg
            u32             ucodeSize;
            SimpleVRAMRelocHandle uCodeHdl; // fragment in RSX map memory
        
        private:
            ShaderParameterMapping m_paramMapping[FxParameterDB::TYPE_COUNT];
        };

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

            VertexProgram * getVPO(const String & _vpoName);
            FragmentProgram * getFPO(const String & _fpoName);

        private:
            CGprogram loadCGProgramRawData(const String & _fileName);

            typedef ITF_MAP<String, VertexProgram> VertexProgRepo;
            typedef ITF_MAP<String, FragmentProgram> FragmentProgRepo;
            
            VertexProgRepo m_vpoRepo;
            FragmentProgRepo m_fpoRepo;
        };
    }
}

#endif // ITF_SHADER_PROGRAM_GCM_H_

#endif // ITF_PS3
