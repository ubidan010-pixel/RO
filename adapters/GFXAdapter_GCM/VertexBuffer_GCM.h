#ifndef ITF_VERTEXBUFFER_GCM_H_
#define ITF_VERTEXBUFFER_GCM_H_

#include "GCM_API.h"

#include "VRAMRelocHdl.h"

namespace ITF
{
    namespace GCM
    {
        struct VertexBuffer
        {
            VertexBuffer() : m_lastUsedFrameCount(0) {}
            SimpleVRAMRelocHandle vramHdl;
            // call this to tag the VB as being used at a specific frame
            void markAsUsedAtFrame(u32 _cpuFrameCount) const { m_lastUsedFrameCount = _cpuFrameCount; }
            u32 getLastUsedFrame() const { return m_lastUsedFrameCount; }
        private:
            mutable u32 m_lastUsedFrameCount;
        };
    }
}

#endif // ITF_VERTEXBUFFER_GCM_H_
