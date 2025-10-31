#ifndef ITF_GFXADAPTER_COOKER_H
#define ITF_GFXADAPTER_COOKER_H

#pragma once

#if defined (ITF_WINDOWS)

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_SYNCHRONIZE_H_
#include "engine/system/Synchronize.h"
#endif //_ITF_SYNCHRONIZE_H_

namespace ITF
{

class GFXAdapter_Cooker : public GFXAdapter
{
    /// VertexBuffer.
    void        createVertexBuffer              (   ITF_VertexBuffer* _vertexBuffer );
    void        releaseVertexBuffer             (   ITF_VertexBuffer* _vertexBuffer );
    void        LockVertexBuffer                (   ITF_VertexBuffer* _vertexBuffer, void** _data, u32 /*_offset*/, u32 /*_size*/, u32 /*_flag*/ );
    void        UnlockVertexBuffer              (   ITF_VertexBuffer* _vertexBuffer ) {};
 
    /// IndexBuffer.
    void        createIndexBuffer               (   ITF_IndexBuffer* _indexBuffer );
    void        releaseIndexBuffer              (   ITF_IndexBuffer* _indexBuffer );
    void        LockIndexBuffer                 (   ITF_IndexBuffer* _indexBuffer, void** _data );
    void        UnlockIndexBuffer               (   ITF_IndexBuffer* _indexBuffer) {}
 

public:

    bbool       createDXDevice            (int _alphaBits, int _depthBits, int _stencilBits, bbool _fullscreen  , void* _hwnd);
    GFXAdapter_Cooker()     {};
    ~GFXAdapter_Cooker()    {};
    const String getRenderName                 (   )   { return "COOKER";};


    bbool loadTexture(Texture* _texture, const char* _path, u32 _alhpa);
    void  cleanupTexture (Texture* /*_texture*/);

 protected:

};



} // namespace ITF


#endif //ITF_WINDOWS

#endif // ITF_GFXADAPTER_COOKER_H
