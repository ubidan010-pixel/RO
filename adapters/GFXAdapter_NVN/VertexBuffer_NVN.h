#ifndef ITF_VERTEXBUFFER_NVN_H_
#define ITF_VERTEXBUFFER_NVN_H_

#pragma once

#include "core/types.h"
#include "NVN/Helpers.h"
#include "VRAMAllocator/VRAMRelocHdl.h"

namespace ITF
{
    struct NVNVertexBuffer
    {
        NVN::BufferVRAMRelocHandle vramHdl;

        // call this to tag the VB as being used at a specific frame
        void markAsUsedAtFrame(u64 _cpuFrameCount) { m_lastUsedFrameCount = static_cast<i64>(_cpuFrameCount); }
        i64 getLastUsedFrame() const { return m_lastUsedFrameCount; }
    private:
        i64 m_lastUsedFrameCount = -1;
    };

    struct NVNIndexBuffer
    {
        NVN::BufferVRAMRelocHandle vramHdl;

        // call this to tag the VB as being used at a specific frame
        void markAsUsedAtFrame(u64 _cpuFrameCount) { m_lastUsedFrameCount = static_cast<i64>(_cpuFrameCount); }
        i64 getLastUsedFrame() const { return m_lastUsedFrameCount; }

    private:
        i64 m_lastUsedFrameCount = -1;
    };
}

#endif // ITF_VERTEXBUFFER_NVN_H_
