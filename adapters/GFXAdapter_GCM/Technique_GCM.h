#if defined (ITF_PS3)

#ifndef ITF_TECHNIQUE_GCM_H_
#define ITF_TECHNIQUE_GCM_H_

#include <Cg/cg.h>
#include "ShaderProgram_GCM.h"

namespace ITF
{
    class FxParameterDB;

    namespace GCM
    {
        class VertexDecl;
        class RenderContext;

        #ifndef ITF_FINAL
            #define CHECK_VERTEXDECL_SHADER_COMPATIBILITY
        #endif

        class Technique
        {
        public:

            Technique();
            ~Technique();

            i32 readNode(class PugiXMLWrap & _xmlParser, const String & _fxPathAndName); // return technique number

            void loadVertexProgram(const String & _fxPathAndName, const char * _entry);
            void loadFragmentProgram(const String & _fxPathAndName, const char * _entry);
            bool bind(const FxParameterDB & _paramDB); // return false if parameters are missing

            // the following method is checking validity of the technique for a vertex decl
    #ifdef CHECK_VERTEXDECL_SHADER_COMPATIBILITY
            void checkVertexDeclCompatibility(const GCM::VertexDecl * _vertexDecl);
    #else
            void checkVertexDeclCompatibility(const GCM::VertexDecl * _vertexDecl) {}
    #endif
            bool isValid() const { return m_vertexProg && m_fragmentProg; }
            const VertexProgram * getVertexProgram() const { return m_vertexProg; }
            const FragmentProgram * getFragmentProgram() const { return m_fragmentProg; }

            inline void apply(RenderContext & _rdrCtxt); // set the technique in the command buffer

            const String & getName() const { return m_techFullName; }

        private:

            VertexProgram * m_vertexProg;
            FragmentProgram * m_fragmentProg;

            String m_techFullName;
        };

        inline void Technique::apply(RenderContext & _rdrCtxt)
        {
            if(m_vertexProg)
            {
                m_vertexProg->apply(_rdrCtxt);
            }
            if (m_fragmentProg)
            {
                m_fragmentProg->apply(_rdrCtxt);
            }
        }


    } // GCM
} // ITF

#endif // ITF_TECHNIQUE_GCM_H_

#endif // ITF_PS3
