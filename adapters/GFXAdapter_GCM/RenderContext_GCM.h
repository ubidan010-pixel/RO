#if defined (ITF_PS3)

#ifndef ITF_RENDER_CONTEXT_GCM_H_
#define ITF_RENDER_CONTEXT_GCM_H_

#include "GCM_API.h"

#include "FxParameterDB.h"
#include "VertexShaderCache_GCM.h"

namespace ITF
{
    namespace GCM
    {
        class FragmentProgram;
        class VertexProgram;

        // this class is handling the command buffer and cache its states
        // (to avoid states change not useful)
        struct RenderContext
        {
            RenderContext();
            RenderContext(CellGcmContextData * _gcmCtxtData);
            void assignGcmRenderContext(CellGcmContextData * _gcmCtxtData);
            void reset(); // set the context to default state (start of each frame)

            cell::Gcm::Inline::CellGcmContext * gcmCtxt;
            cell::Gcm::CellGcmContext * gcmCtxtNoInline;

            FxParameterDB paramDB;
            bool paramDBDirty;

            // set vertex/fragment program, with caching
            inline void setVertexProgram(const VertexProgram * _vertexProg);
            inline void setFragmentProgram(const FragmentProgram * _fragmentProg);

            inline void invalidateVertexProgramCache() { m_currentVertexProg = NULL; }
            inline void invalidateFragmentProgramCache() { m_currentFragmentProg = NULL; }

        private:
            // cached shaders
            const VertexProgram * m_currentVertexProg;
            const FragmentProgram * m_currentFragmentProg;

            VertexShaderCache m_vtxShaderCache;
        };

        inline void RenderContext::setVertexProgram(const VertexProgram * _vertexProg)
        {
            if (_vertexProg != m_currentVertexProg)
            {
                m_currentVertexProg = _vertexProg;
                m_vtxShaderCache.setShader(gcmCtxt, *m_currentVertexProg);
            }
        }

        inline void RenderContext::setFragmentProgram(const FragmentProgram * _fragmentProg)
        {
            if (_fragmentProg != m_currentFragmentProg)
            {
                m_currentFragmentProg = _fragmentProg;
                gcmCtxt->SetFragmentProgramOffset(  m_currentFragmentProg->cg,
                    m_currentFragmentProg->uCodeHdl.GetOffset(),
                    m_currentFragmentProg->uCodeHdl.GetLocation());
                gcmCtxt->SetFragmentProgramControl( m_currentFragmentProg->cg,
                    CELL_GCM_FALSE,
                    1,
                    0);
            }
        }
    }
}

#endif // ITF_RENDER_CONTEXT_GCM_H_

#endif // ITF_PS3
