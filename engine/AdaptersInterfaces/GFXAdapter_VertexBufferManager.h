#ifndef _ITF_GFXADAPTER_VERTEXBUFFERMANAGER_H_
#define _ITF_GFXADAPTER_VERTEXBUFFERMANAGER_H_

namespace ITF
{
    // Vertex buffers lock flags
    const int VB_LOCK_DEFAULT       = 0x00000000;     
    const int VB_LOCK_READONLY      = 0x00000001;     
    const int VB_LOCK_NOOVERWRITE   = 0x00000002;
    const int VB_LOCK_DISCARD       = 0x00000004;

    enum dbEnumType
    {
        VB_T_NOTDEFINED = 0,
        VB_T_PARTICLE,
        VB_T_FRIEZEANIM,
        VB_T_FRIEZESTATIC,
        VB_T_ANIMFRIZE,
        VB_T_FONT,
        VB_T_MESH,
        VB_T_ICONS,
        VB_T_INTERNAL,
        VB_T_FRIEZEOVERLAY,
        ENUM_FORCE_SIZE_32(dbEnumType)
    };

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Vertex Buffer.
    class ITF_VertexBuffer
    {
        friend class GFXAdapter_VertexBufferManager;
    public:
        ITF_VertexBuffer        (   );
        ~ITF_VertexBuffer       (   );

        void initVertexBuffer   (   u32 _vertexNumber, int _vertexformat, u32 _structVertexSize, bbool _dynamic);

        void createVertexBuffer (   u32 _vertexNumber, int _vertexformat, u32 _structVertexSize, bbool _dynamic  );
        void createVertexBuffer (   u32 _size, bbool _dynamic   );
        void release            (   );
        void Lock               ( void** _data  );
        void Lock               ( void** _data, u32 _offset, u32 _size, u32 _flag );
        void Unlock             (   );
        bbool                   bIsDynamic() const { return mb_dynamic; }
        i32                     m_vertexFormat;
        u32                     m_nVertex;
        u32                     m_structVertexSize;
        uPtr                    mp_VertexBuffer;    /// Data adress.
        u32                     m_Id;
        u32                     m_offset;
        u32                     m_size;
        u32                     m_wgpWrite;
        u8 *                    m_pLockedData;
        void                    wgp_write(const Vec2d& _v)
        {
            Vec2d* pV2 = (Vec2d*)(m_pLockedData + m_wgpWrite);
            *pV2 = _v;
            m_wgpWrite += 8;
        }

        void                    wgp_write(const Vec3d& _v)
        {
            Vec3d* pV2 = (Vec3d*)(m_pLockedData + m_wgpWrite);
            *pV2 = _v;
            m_wgpWrite += 12;
        }

        void                    wgp_write(f32 _v)
        {
            f32* pV = (f32*)(m_pLockedData + m_wgpWrite);
            *pV = _v;
            m_wgpWrite += 4;
        }

        void                    wgp_write(u32 _v)
        {
            u32* pV = (u32*)(m_pLockedData + m_wgpWrite);
            *pV = _v;
            m_wgpWrite += 4;
        }

        void                    wgp_write(u32 _color, f32 _z, const Vec2d& _p, const Vec2d& _uv)
        {
            f32* pV = (f32*)(m_pLockedData + m_wgpWrite);
            *pV++ = _p.m_x;
            *pV++ = _p.m_y;
            *pV++ = _z;            
            u32* pCol = (u32*)(pV++);
            *pCol = _color;
            *pV++ = _uv.m_x;
            *pV++ = _uv.m_y;

            m_wgpWrite += 24;
        }

        dbEnumType              m_debugType;
#ifndef ITF_FINAL
        bbool                   m_bUseRingBuffer;        
#endif
    protected:
        bbool                   mb_dynamic;
    };

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Index Buffer.
    class ITF_IndexBuffer
    {
        friend class GFXAdapter_VertexBufferManager;    
    public:
        ITF_IndexBuffer         (   );
        ~ITF_IndexBuffer        (   );

        void createIndexBuffer  (   u32 _indexNumber, bbool _dynamic  );
        void release            (   );
        void Lock               ( void** _data  );
        void Unlock             (   );
        bbool bIsDynamic        (   ) const { return mb_dynamic; }        
        u32                     m_nIndices;
        uPtr                    mp_IndexBuffer;    /// Data adress.
#if defined(ITF_WINDOWS)
        u32                     m_Id;
#endif // ITF_WINDOWS

    protected:
        bbool                   mb_dynamic;
   };

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Manager Buffer.
    class GFXAdapter_VertexBufferManager
    {
    public:
        GFXAdapter_VertexBufferManager              (   );
        ~GFXAdapter_VertexBufferManager             (   );

        ITF_VertexBuffer* createVertexBuffer        (   u32 _vertexNumber, int _vertexformat, u32 _structVertexSize, bbool _dynamic, dbEnumType _type = VB_T_NOTDEFINED );
        ITF_IndexBuffer* createIndexBuffer          (   u32 _indexNumber, bbool _dynamic    );
        void removeVertexBuffer                     (   ITF_VertexBuffer* _VertexBuffer     );
        void removeIndexBuffer                      (   ITF_IndexBuffer* _IndexBuffer   );
        void destroyAllVertexBuffer                 (   );
        void destroyAllIndexBuffer                  (   );
        ITF_VertexBuffer* getVertexBufferByIndex    (   u32 _id );
        ITF_IndexBuffer* getIndexBufferByIndex      (   u32 _id );
        void UnallocAllDynamicVB                    (   );
        void ReallocAllDynamicVB                    (   );
        void UnallocAllDynamicIB                    (   );
        void ReallocAllDynamicIB                    (   );

        void LockVertexBuffer  ( ITF_VertexBuffer* _VB, void** _data  );
        void LockVertexBuffer  ( ITF_VertexBuffer* _VB, void** _data, u32 _offset, u32 _size, u32 _flag );
        void UnlockVertexBuffer( ITF_VertexBuffer* _VB  );
#if defined(VBMANAGER_USE_DYNAMICRING_VB)
        void UnlockVertexBufferUsage(ITF_VertexBuffer* _VB, u32 _usage);
#endif // VBMANAGER_USE_DYNAMICRING_VB

        void removeVertexBufferDelay                    (   ITF_VertexBuffer* _VertexBuffer ,u32 _frameCount    );
        void removeIndexBufferDelay                  (   ITF_IndexBuffer* _IndexBuffer ,u32 _frameCount     );

        void cleanEndOfFrame();

#ifdef VBMANAGER_USE_DYNAMICRING_VB
        void createDynamicRingVB                           ( u32 _Size = 2*1024*1024, u32 _lockalign = 4 );
        void deleteDynamicRingVB                           (   );

        u32  getDynamicRingVBSize()                        {return mp_DynamicRingVB?mp_DynamicRingVB->m_size:0;}

        bbool LockDynamicRingVB(u32 _Size, ITF_VertexBuffer *_VertexBuffer, void **_Data);
        void UnlockDynamicRingVB(ITF_VertexBuffer *_VertexBuffer);
#endif

#ifndef ITF_FINAL
        void drawDebugVBInfo                        (   );
#endif // ITF_FINAL

#ifdef VBMANAGER_USE_DYNAMICRING_VB
        void DynamicRingVBBeginDraw();
        void DynamicRingVBEndDraw();
#endif

    private:
        void addVertexBuffer                        (   ITF_VertexBuffer* _VertexBuffer );
        SafeArray<ITF_VertexBuffer*>                mp_VertexBufferList;
        SafeArray<ITF_IndexBuffer*>                 mp_IndexBufferList;

        SafeArray<ITF_VertexBuffer*>                mp_RequestDelayVertexFreeList;
        SafeArray<ITF_IndexBuffer*>                 mp_RequestDelayIndexFreeList;
        void addIndexBuffer                         (   ITF_IndexBuffer* _IndexBuffer );

    #ifdef VBMANAGER_USE_DYNAMICRING_VB
        ITF_VertexBuffer*   mp_DynamicRingVB = nullptr;
        u32                 m_DynamicRingLockAlign = 0;

        void                DynamicRingVBresetOffsets();
        void                DynamicRingVBsyncGPU();
        u32                 m_DynamicRingVBcpuOffset = 0;
        volatile u32        m_DynamicRingVBgpuOffset = 0;
        u32                 m_DynamicRingVBgpuNextOffset = 0;
    #ifdef SUPPORT_STATS_MANAGER
        u32                 m_DynamicRingVBusage = 0;
    #endif // SUPPORT_STATS_MANAGER
    #endif // VBMANAGER_USE_DYNAMICRING_VB
     };

} // namespace ITF

#endif // _ITF_GFXADAPTER_VERTEXBUFFERMANAGER_H_
