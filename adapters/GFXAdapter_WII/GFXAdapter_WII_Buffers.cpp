#include "precompiled_GFXAdapter_WII.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////       implementation of various shapes drawing
///////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <revolution.h>
#include <revolution/sc.h>
#include <revolution/mtx.h>

#ifndef _ITF_WII_DRAWPRIM_H_
#include "adapters/GFXAdapter_WII/GFXAdapter_WII.h"
#endif //_ITF_WII_DRAWPRIM_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

namespace ITF
{
///----------------------------------------------------------------------------//
/// VertexBuffer.
///----------------------------------------------------------------------------//
void GFXAdapter_WII::createVertexBuffer(ITF_VertexBuffer* _vertexBuffer)
{
    u8* _data = (u8 *)Memory::alignedMallocCategory(_vertexBuffer->m_size, 32, 
                                                    _vertexBuffer->bIsDynamic() ? MemoryId::mId_VertexBuffer : MemoryId::mId_VertexBufferStatic);
    _vertexBuffer->mp_VertexBuffer = (u32)_data;
    _vertexBuffer->m_lockedsize = 0;
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::releaseVertexBuffer(ITF_VertexBuffer* _vertexBuffer)
{
    // Sync to be sure that VB is not in use by GPU
    syncGPU();

    u8* ptr = (u8*)_vertexBuffer->mp_VertexBuffer;
    ITF_ASSERT(ptr);
    Memory::alignedFree(ptr);
    _vertexBuffer->mp_VertexBuffer = 0;
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::setVertexBuffer(ITF_VertexBuffer* _vertexBuffer)
{
    setVertexDesc(_vertexBuffer, true, NeedDupUV());
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::LockVertexBuffer(ITF_VertexBuffer* _vertexBuffer, void** _data, u32 _offset, u32 _size, u32 _flag)
{
    ITF_ASSERT(!_vertexBuffer->m_lockedsize);
    ITF_ASSERT_MSG(_size, "requesting 0 size");
	u8* p = (u8*)_vertexBuffer->mp_VertexBuffer;
    *_data = p + _offset;
    _vertexBuffer->m_offset = _offset;
    _vertexBuffer->m_lockedsize = _size;
    //DCTouchRange(*_data, _size);
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::UnlockVertexBuffer(ITF_VertexBuffer* _vertexBuffer)
{
    ITF_ASSERT(_vertexBuffer->m_lockedsize);
    if(!_vertexBuffer->m_bnoflush)
    {
        const u8* p = (u8*)_vertexBuffer->mp_VertexBuffer + _vertexBuffer->m_offset;
        #if 0
        DCFlushRange((void*)p, _vertexBuffer->m_lockedsize);
        #else
        const u8* pa = (u8*)OSRoundDown32B(p);
        u32 i = 0;
        const u32 cnt = _vertexBuffer->m_lockedsize + p-pa;
        while(i<cnt)
        {
            __dcbf((void*)p, i);i+=32;
            __dcbf((void*)p, i);i+=32;
        }
        #endif
    }
    _vertexBuffer->m_lockedsize = 0;
}

///----------------------------------------------------------------------------//
/// IndexBuffer.
///----------------------------------------------------------------------------//

void GFXAdapter_WII::createIndexBuffer(ITF_IndexBuffer* _indexBuffer)
{
    u16* _data = (u16 *)Memory::alignedMallocCategory(sizeof(u16) * _indexBuffer->m_nIndices, 32, MemoryId::mId_IndexBuffer);
    _indexBuffer->mp_IndexBuffer = (u32)_data;
}

void GFXAdapter_WII::releaseIndexBuffer(ITF_IndexBuffer* _indexBuffer)
{
    // Sync to be sure that VB is not in use by GPU
    syncGPU();

    u16* ptr = (u16*)_indexBuffer->mp_IndexBuffer;
    ITF_ASSERT(ptr);
    Memory::alignedFree(ptr);
    _indexBuffer->mp_IndexBuffer = 0;
}

void GFXAdapter_WII::setIndexBuffer(ITF_IndexBuffer* _indexBuffer)
{
    ITF_ASSERT_MSG(0, "GFXAdapter_WII::setIndexBuffer not implemented");
}

void GFXAdapter_WII::LockIndexBuffer(ITF_IndexBuffer* _indexBuffer, void** _data)
{
    *_data = (void*)_indexBuffer->mp_IndexBuffer;
}

void GFXAdapter_WII::UnlockIndexBuffer(ITF_IndexBuffer* _indexBuffer)
{
    ITF_ASSERT(_indexBuffer);
    ITF_ASSERT(_indexBuffer->mp_IndexBuffer);
}
} // namespace ITF
