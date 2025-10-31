#include "precompiled_GFXAdapter_Cooker.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#include "adapters/GFXAdapter_Cooker/GFXAdapter_Cooker.h"

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_


namespace ITF
{


///----------------------------------------------------------------------------//
/// VertexBuffer.
///----------------------------------------------------------------------------//

void GFXAdapter_Cooker::createVertexBuffer( ITF_VertexBuffer* _vertexBuffer )
{
    u8* pData = new u8[_vertexBuffer->m_nVertex * _vertexBuffer->m_structVertexSize];
    _vertexBuffer->mp_VertexBuffer = uPtr(pData);
}

void GFXAdapter_Cooker::releaseVertexBuffer( ITF_VertexBuffer* _vertexBuffer )
{
    u8* pData = (u8*) _vertexBuffer->mp_VertexBuffer;
    SF_DEL_ARRAY(pData);
    _vertexBuffer->mp_VertexBuffer = 0;
}

void GFXAdapter_Cooker::LockVertexBuffer( ITF_VertexBuffer* _vertexBuffer, void** _data, u32 _offset, u32 _size, u32 _flag )
{
    *_data = (void*)(_vertexBuffer->mp_VertexBuffer);
}

///----------------------------------------------------------------------------//
/// IndexBuffer.
///----------------------------------------------------------------------------//

void GFXAdapter_Cooker::createIndexBuffer( ITF_IndexBuffer* _indexBuffer )
{
    u8* pData = new u8[_indexBuffer->m_nIndices * sizeof(u16)];
    _indexBuffer->mp_IndexBuffer = uPtr(pData);

}

void GFXAdapter_Cooker::releaseIndexBuffer( ITF_IndexBuffer* _indexBuffer )
{
    u8* pData = (u8*) _indexBuffer->mp_IndexBuffer;
    SF_DEL_ARRAY(pData);
    _indexBuffer->m_nIndices = 0;
}

void GFXAdapter_Cooker::LockIndexBuffer( ITF_IndexBuffer* _indexBuffer, void** _data )
{
    *_data = (void*)(_indexBuffer->mp_IndexBuffer);
}

bbool GFXAdapter_Cooker::loadTexture(Texture* _texture, const char* _path, u32 _alhpa)
{
    RESOURCE_MANAGER->addResourceAsLoaded(_texture);
    return btrue; //fake loading for dependent textures. Used in anim texture packer
}

void  GFXAdapter_Cooker::cleanupTexture (Texture* /*_texture*/)
{
}

} // namespace ITF
