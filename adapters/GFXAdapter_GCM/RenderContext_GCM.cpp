#include "precompiled_GFXAdapter_GCM.h"

#include "GCM_API.h"

#include "ShaderProgram_GCM.h"
#include "RenderContext_GCM.h"

namespace ITF
{
    namespace GCM
    {
        RenderContext::RenderContext()
        {
            reset();
            assignGcmRenderContext(gCellGcmCurrentContext);
        }

        RenderContext::RenderContext(CellGcmContextData * _gcmCtxtData)
        {
            assignGcmRenderContext(_gcmCtxtData);
        }

        void RenderContext::assignGcmRenderContext(CellGcmContextData * _gcmCtxtData)
        {
            gcmCtxt = (cell::Gcm::Inline::CellGcmContext *)_gcmCtxtData;
            gcmCtxtNoInline = (cell::Gcm::CellGcmContext *)_gcmCtxtData;
            m_currentVertexProg = NULL;
            m_currentFragmentProg = NULL;
        }

        void RenderContext::reset()
        {
            m_currentVertexProg = NULL;
            m_currentFragmentProg = NULL;
            paramDBDirty = true;
            m_vtxShaderCache.flush();
        }
    }
}
