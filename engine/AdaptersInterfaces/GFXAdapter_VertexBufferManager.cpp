#include "precompiled_engine.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifdef ITF_WII
    #ifndef _ITF_WII_DRAWPRIM_H_
    #include "adapters/GFXAdapter_WII/GFXAdapter_WII.h"
    #endif // _ITF_WII_DRAWPRIM_H_
#endif // ITF_WII

#ifndef _ITF_GFXADAPTER_VERTEXBUFFERMANAGER_H_
#include "engine/AdaptersInterfaces/GFXAdapter_VertexBufferManager.h"
#endif //_ITF_GFXADAPTER_VERTEXBUFFERMANAGER_H_

#ifdef ITF_WII
#ifndef __MEM_MACROS_WII_H__
#include "core/memory/WII/MEM_macros_WII.h"
#endif //__MEM_MACROS_WII_H__
#endif

#ifndef _ITF_STATSMANAGER_H_
#include "engine/stats/statsManager.h"
#endif //_ITF_STATSMANAGER_H_

#include "core/math/BitTweak.h"

namespace ITF
{
    //////////////////////////////////////////////////////////////////////////
    ///
    /// Vertex Buffer.
    ITF_VertexBuffer::ITF_VertexBuffer()
        : mp_VertexBuffer(0)
        , m_vertexFormat(0)
        , m_structVertexSize(0)
        , m_nVertex(0)
        , mb_dynamic(bfalse)
        , m_Id(0)
        , m_offset(0)
        , m_size(0)
#ifdef ITF_WII
        ,m_lockedsize(0)
        ,m_bnoflush(bfalse)
#endif // ITF_WII
        ,m_wgpWrite(0)
#ifndef USE_WGP
        ,m_pLockedData(0)
#endif
    {               
#ifdef ITF_CTR
        for (u32 i = 0; i < DYNAMIC_BUFFERS_COUNT; i++)
        {
	        m_dynamicBuffers[i] = 0;
        }
        m_currentBuffer = 0;
#endif
        m_debugType = VB_T_NOTDEFINED;
#ifndef ITF_FINAL
        m_bUseRingBuffer = bfalse;
#endif
    }

    ITF_VertexBuffer::~ITF_VertexBuffer()
    {        
        release();
    }

    void ITF_VertexBuffer::release()
    {
        if(mp_VertexBuffer)
            GFX_ADAPTER->releaseVertexBuffer(this);
    }

    void ITF_VertexBuffer::initVertexBuffer(u32 _vertexNumber, int _vertexformat, u32 _structVertexSize, bbool _dynamic)
    {
        m_nVertex = _vertexNumber;
        m_structVertexSize = _structVertexSize;
        m_size = m_nVertex * m_structVertexSize;
        m_vertexFormat = _vertexformat;
        mb_dynamic = _dynamic;
    }

    void ITF_VertexBuffer::createVertexBuffer(u32 _vertexNumber, int _vertexformat, u32 _structVertexSize, bbool _dynamic)
    {
        initVertexBuffer(_vertexNumber, _vertexformat, _structVertexSize, _dynamic);
        if (!m_size)
            return;
        GFX_ADAPTER->createVertexBuffer(this);
    }

    void ITF_VertexBuffer::createVertexBuffer(u32 _size, bbool _dynamic)
    {
        initVertexBuffer(0, 0, 0, _dynamic);
        m_size = _size;
        GFX_ADAPTER->createVertexBuffer(this);
    }

    void ITF_VertexBuffer::Lock( void** _data   )
    {
#ifndef ITF_FINAL
        ITF_ASSERT(!m_bUseRingBuffer);
#endif
        GFX_ADAPTER->LockVertexBuffer(this, _data, 0, this->m_nVertex * this->m_structVertexSize, VB_LOCK_DEFAULT);
#ifndef USE_WGP
        m_pLockedData  = (u8*)*_data;
#endif
    }

    void ITF_VertexBuffer::Lock( void** _data, u32 _offset, u32 _size, u32 _flag )
    {
#ifndef ITF_FINAL
        ITF_ASSERT(!m_bUseRingBuffer);
#endif
        GFX_ADAPTER->LockVertexBuffer(this, _data, _offset, _size, _flag);
#ifndef USE_WGP
        m_pLockedData  = (u8*)*_data;
#endif
    }

    void ITF_VertexBuffer::Unlock(    )
    {
#ifndef ITF_FINAL
        ITF_ASSERT(!m_bUseRingBuffer);
#endif
        GFX_ADAPTER->UnlockVertexBuffer(this);

        m_wgpWrite = 0;
#ifndef USE_WGP
        m_pLockedData = 0;
#endif
    }

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Index Buffer.
    ITF_IndexBuffer::ITF_IndexBuffer()
    {
        mp_IndexBuffer = 0;
        m_nIndices = 0;
        mb_dynamic = false;
#if defined(ITF_WINDOWS) || defined(ITF_CTR)
        m_Id = 0;
#endif //
#ifdef ITF_CTR
        for (u32 i = 0; i < DYNAMIC_BUFFERS_COUNT; i++)
        {
            m_dynamicBuffers[i] = 0;
        }
        m_currentBuffer = 0;
#endif
    }

    ITF_IndexBuffer::~ITF_IndexBuffer()
    {
        release();
    }

    void ITF_IndexBuffer::release()
    {
        GFX_ADAPTER->releaseIndexBuffer(this);
    }

    void ITF_IndexBuffer::createIndexBuffer(u32 _indexNumber, bbool _dynamic)
    {
        m_nIndices = _indexNumber;
        mb_dynamic = _dynamic;
        GFX_ADAPTER->createIndexBuffer(this);
    }

    void ITF_IndexBuffer::Lock( void** _data   )
    {
        GFX_ADAPTER->LockIndexBuffer(this, _data);
    }

    void ITF_IndexBuffer::Unlock(    )
    {
        GFX_ADAPTER->UnlockIndexBuffer(this);
    }

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Manager Buffer.

    //////////////////////////////////////////////////////////////////////////
    /// Manage VertexBuffer.
    
    GFXAdapter_VertexBufferManager::GFXAdapter_VertexBufferManager()
    {
    }

    GFXAdapter_VertexBufferManager::~GFXAdapter_VertexBufferManager()
    {
    #ifdef VBMANAGER_USE_DYNAMICRING_VB
        deleteDynamicRingVB();
    #endif
    }

#ifdef VBMANAGER_USE_DYNAMICRING_VB
    void GFXAdapter_VertexBufferManager::createDynamicRingVB(u32 _Size, u32 _lockalign )
    {
        ITF_ASSERT(!mp_DynamicRingVB);
        ITF_ASSERT(_lockalign != 0);

        /// create discardable
        mp_DynamicRingVB = newAlloc(mId_VertexBuffer,ITF_VertexBuffer());
        mp_DynamicRingVB->createVertexBuffer(_Size, btrue);
        m_DynamicRingLockAlign = _lockalign;
        
        DynamicRingVBresetOffsets();
    }

    void GFXAdapter_VertexBufferManager::deleteDynamicRingVB( )
    {
        if (mp_DynamicRingVB)
            delete mp_DynamicRingVB;

        mp_DynamicRingVB = NULL;
    }
#endif // VBMANAGER_USE_DYNAMICRING_VB

    ITF_VertexBuffer* GFXAdapter_VertexBufferManager::createVertexBuffer(u32 _vertexNumber, int _vertexformat, u32 _structVertexSize, bbool _dynamic, dbEnumType _type)
    {
        ITF_VertexBuffer* VertexBuffer = newAlloc(mId_VertexBuffer,ITF_VertexBuffer());
        VertexBuffer->m_debugType = _type;
        addVertexBuffer(VertexBuffer);

    #ifdef VBMANAGER_USE_DYNAMICRING_VB
        if(_dynamic)
        {
        #ifndef ITF_FINAL
            VertexBuffer->m_bUseRingBuffer = btrue;
        #endif // ITF_FINAL
            VertexBuffer->m_nVertex = _vertexNumber;
            VertexBuffer->m_structVertexSize = _structVertexSize;
            VertexBuffer->m_size = _vertexNumber * _structVertexSize;
            VertexBuffer->m_vertexFormat = _vertexformat;
            VertexBuffer->mb_dynamic = _dynamic;
        }
        else
    #endif // VBMANAGER_USE_DYNAMICRING_VB
        {
            VertexBuffer->createVertexBuffer(_vertexNumber, _vertexformat, _structVertexSize, _dynamic);
        }
        return VertexBuffer;
    }

    void GFXAdapter_VertexBufferManager::removeVertexBuffer( ITF_VertexBuffer* _VertexBuffer  )
    {
        i32 index = mp_VertexBufferList.find(_VertexBuffer);
        ITF_ASSERT(index >=0);
        if (index >= 0)
        {
            ITF_VertexBuffer* vb = mp_VertexBufferList[index];
        #if defined(VBMANAGER_USE_DYNAMICRING_VB)
            if(mp_DynamicRingVB) // this vb use the ring buffer.
                if(vb->mp_VertexBuffer == mp_DynamicRingVB->mp_VertexBuffer)
                    vb->mp_VertexBuffer = 0;
        #endif

            delete vb;
            mp_VertexBufferList.eraseNoOrder(index);
        }
    }

    void GFXAdapter_VertexBufferManager::LockVertexBuffer( ITF_VertexBuffer* _VB, void** _data  )
    {
        #ifdef VBMANAGER_USE_DYNAMICRING_VB
        if(_VB->bIsDynamic())
        {
            #ifndef ITF_FINAL
            ITF_ASSERT(_VB->m_bUseRingBuffer);
            #endif
            LockDynamicRingVB(_VB->m_nVertex * _VB->m_structVertexSize, _VB, _data);
            #ifdef ITF_WII
            //MEM_M_DCZeroRangeSafe(*_data, _VB->m_nVertex * _VB->m_structVertexSize);
            #endif // ITF_WII
        }
        else
        #endif // VBMANAGER_USE_DYNAMICRING_VB
        {
            #ifndef ITF_FINAL
            ITF_ASSERT(!_VB->m_bUseRingBuffer);
            #endif
            _VB->Lock(_data);
        }
    }

    void GFXAdapter_VertexBufferManager::LockVertexBuffer( ITF_VertexBuffer* _VB, void** _data, u32 _offset, u32 _size, u32 _flag )
    {
        #ifdef VBMANAGER_USE_DYNAMICRING_VB
        if(_VB->bIsDynamic())
        {
            #ifndef ITF_FINAL
            ITF_ASSERT(_VB->m_bUseRingBuffer);
            ITF_ASSERT(_offset == 0);
            #endif
            LockDynamicRingVB(_size, _VB, _data);
            #ifdef ITF_WII
            //MEM_M_DCZeroRangeSafe(*_data, _size);
            #endif // ITF_WII
        }
        else
        #endif // VBMANAGER_USE_DYNAMICRING_VB
        {
            #ifndef ITF_FINAL
            ITF_ASSERT(!_VB->m_bUseRingBuffer);
            #endif
            _VB->Lock(_data, _offset, _size, _flag );
        }
    }

    void GFXAdapter_VertexBufferManager::UnlockVertexBuffer( ITF_VertexBuffer* _VB )
    {
        #ifdef VBMANAGER_USE_DYNAMICRING_VB
        if(_VB->bIsDynamic())
        {
            #ifndef ITF_FINAL
            ITF_ASSERT(_VB->m_bUseRingBuffer);
            #endif
            UnlockDynamicRingVB(_VB);
        }
        else
        #endif // VBMANAGER_USE_DYNAMICRING_VB
        {
            #ifndef ITF_FINAL
            ITF_ASSERT(!_VB->m_bUseRingBuffer);
            #endif
            _VB->Unlock();        
        }
    }

#if defined(VBMANAGER_USE_DYNAMICRING_VB) && (PATCH32_WORKINGBUFF==0)
    void GFXAdapter_VertexBufferManager::UnlockVertexBufferUsage(ITF_VertexBuffer* _VB, u32 _usage)
    {
        if(_VB->bIsDynamic())
        {
#ifndef ITF_FINAL
            ITF_ASSERT(_VB->m_bUseRingBuffer);
#endif

            if(_usage == 0)
            {
                _VB->m_bnoflush = btrue;
                m_DynamicRingVBcpuOffset -= _VB->m_lockedsize;
#ifdef SUPPORT_STATS_MANAGER
                m_DynamicRingVBusage -= _VB->m_lockedsize;
#endif // SUPPORT_STATS_MANAGER
            }
            else
            {
                _usage = (_usage + m_DynamicRingLockAlign - 1) & ~(m_DynamicRingLockAlign - 1);
                m_DynamicRingVBcpuOffset -= (_VB->m_lockedsize - _usage);
#ifdef SUPPORT_STATS_MANAGER
                m_DynamicRingVBusage -= (_VB->m_lockedsize - _usage);
#endif // SUPPORT_STATS_MANAGER
            }
#ifdef SUPPORT_STATS_MANAGER
            StatsManager::getptr()->dynRingVBUsageSet(m_DynamicRingVBusage);
#endif // SUPPORT_STATS_MANAGER

            UnlockDynamicRingVB(_VB);
        }
        else
        {
#ifndef ITF_FINAL
            ITF_ASSERT(!_VB->m_bUseRingBuffer);
#endif
            _VB->Unlock();        
        }
    }
#endif // VBMANAGER_USE_DYNAMICRING_VB

    void GFXAdapter_VertexBufferManager::addVertexBuffer( ITF_VertexBuffer* _VertexBuffer )
    {
        mp_VertexBufferList.push_back(_VertexBuffer);
    }

    ITF_VertexBuffer* GFXAdapter_VertexBufferManager::getVertexBufferByIndex( u32 _id )
    {
        return mp_VertexBufferList[_id];
    }
    
    void GFXAdapter_VertexBufferManager::destroyAllVertexBuffer()
    {
        for (u32 i = 0; i < mp_VertexBufferList.size(); i++)
            delete mp_VertexBufferList[i];

        mp_VertexBufferList.clear();
    }

    void GFXAdapter_VertexBufferManager::UnallocAllDynamicVB(   )
    {
        ITF_VertexBuffer* pVertexBuffer;

        for (u32 i = 0; i < mp_VertexBufferList.size(); i++)
        {
            pVertexBuffer = mp_VertexBufferList[i];
            
            if (pVertexBuffer->bIsDynamic())
                GFX_ADAPTER->releaseVertexBuffer(pVertexBuffer);
        }
    }
    void GFXAdapter_VertexBufferManager::ReallocAllDynamicVB(   )
    {
        ITF_VertexBuffer* pVertexBuffer;
        
        for (u32 i = 0; i < mp_VertexBufferList.size(); i++)
        {
            pVertexBuffer = mp_VertexBufferList[i];
            if (pVertexBuffer->bIsDynamic() && pVertexBuffer->m_size != 0)
                GFX_ADAPTER->createVertexBuffer(pVertexBuffer);
        }
    }

    //////////////////////////////////////////////////////////////////////////
    /// Manage IndexBuffer.

    ITF_IndexBuffer* GFXAdapter_VertexBufferManager::createIndexBuffer(u32 _indexNumber, bbool _dynamic)
    {
        ITF_IndexBuffer* IndexBuffer = newAlloc(mId_GfxAdapter,ITF_IndexBuffer());
        IndexBuffer->createIndexBuffer(_indexNumber, _dynamic);
        addIndexBuffer(IndexBuffer);
        return IndexBuffer;
    }

    void GFXAdapter_VertexBufferManager::removeIndexBuffer( ITF_IndexBuffer* _IndexBuffer  )
    {
        i32 index = mp_IndexBufferList.find(_IndexBuffer);
        ITF_ASSERT(index >=0);
        if (index >= 0)
        {
#ifdef ITF_WII
            GFX_ADAPTER_WII->ForceDeprecatedDL(mp_IndexBufferList[index]);
#endif // ITF_WII
            delete mp_IndexBufferList[index];
            mp_IndexBufferList.eraseNoOrder(index);
        }
    }

    void GFXAdapter_VertexBufferManager::addIndexBuffer( ITF_IndexBuffer* _IndexBuffer  )
    {
        mp_IndexBufferList.push_back(_IndexBuffer);
    }

    ITF_IndexBuffer* GFXAdapter_VertexBufferManager::getIndexBufferByIndex( u32 _id )
    {
        return mp_IndexBufferList[_id];
    }
    
    void GFXAdapter_VertexBufferManager::destroyAllIndexBuffer()
    {
        for (u32 i = 0; i < mp_IndexBufferList.size(); i++)
            delete mp_IndexBufferList[i];

        mp_IndexBufferList.clear();
    }

    void GFXAdapter_VertexBufferManager::UnallocAllDynamicIB(   )
    {
        ITF_IndexBuffer* pIndexBuffer;

        for (u32 i = 0; i < mp_IndexBufferList.size(); i++)
        {
            pIndexBuffer = mp_IndexBufferList[i];
            if (pIndexBuffer->bIsDynamic())
                GFX_ADAPTER->releaseIndexBuffer(pIndexBuffer);
        }
    }
    void GFXAdapter_VertexBufferManager::ReallocAllDynamicIB(   )
    {
        ITF_IndexBuffer* pIndexBuffer;
        
        for (u32 i = 0; i < mp_IndexBufferList.size(); i++)
        {
            pIndexBuffer = mp_IndexBufferList[i];
            if (pIndexBuffer->bIsDynamic())
                GFX_ADAPTER->createIndexBuffer(pIndexBuffer);
        }
    }

#if defined(VBMANAGER_USE_DYNAMICRING_VB)
    //////////////////////////////////////////////////////////////////////////
    /// Vertex Buffer Ring.
    bbool GFXAdapter_VertexBufferManager::LockDynamicRingVB(u32 _size, ITF_VertexBuffer *_vertexBuffer, void **_data)
    {
    
        ITF_ASSERT(mp_DynamicRingVB != NULL);
        _size = (_size + m_DynamicRingLockAlign - 1) & ~(m_DynamicRingLockAlign - 1);
        ITF_ASSERT(_size <= mp_DynamicRingVB->m_size);

#ifdef SUPPORT_STATS_MANAGER
        u32 predOffset = m_DynamicRingVBcpuOffset;        
        ITF_ASSERT(m_DynamicRingVBusage+_size <= mp_DynamicRingVB->m_size);
#endif
        
        if(m_DynamicRingVBcpuOffset < m_DynamicRingVBgpuOffset)
        {                    
            if( (m_DynamicRingVBgpuOffset - m_DynamicRingVBcpuOffset) <= _size)
                DynamicRingVBsyncGPU(); // Need to wait for gpu to free buffer.
            
            // round buffer.
            if(m_DynamicRingVBcpuOffset + _size > mp_DynamicRingVB->m_size)
                m_DynamicRingVBcpuOffset = 0;
        }
        else if(m_DynamicRingVBcpuOffset > m_DynamicRingVBgpuOffset)
        {
            if(m_DynamicRingVBcpuOffset + _size > mp_DynamicRingVB->m_size)
            { // round buffer.
                m_DynamicRingVBcpuOffset = 0;
                
                if(_size >= m_DynamicRingVBgpuOffset)                    
                    DynamicRingVBsyncGPU(); // Need to wait for gpu to free buffer.
            }                    
        }
        else // if(m_DynamicRingVBcpuOffset == m_DynamicRingVBgpuOffset)
        {
            ITF_ASSERT_MSG(0,"Dynamic ring vertex buffer error.");
            DynamicRingVBsyncGPU();
        }

        mp_DynamicRingVB->Lock(_data, m_DynamicRingVBcpuOffset, _size, VB_LOCK_NOOVERWRITE);
        ITF_ASSERT_MSG(BitTweak::IsAligned(*_data, m_DynamicRingLockAlign), "Ring vertex buffer VB must be aligned");
#ifndef USE_WGP
        _vertexBuffer->m_pLockedData = (u8*)*_data;
#endif
	    _vertexBuffer->mp_VertexBuffer = mp_DynamicRingVB->mp_VertexBuffer;
	    _vertexBuffer->m_offset = m_DynamicRingVBcpuOffset;
#ifdef ITF_WII
        _VertexBuffer->m_lockedsize = _Size;
#endif // ITF_WII

	    m_DynamicRingVBcpuOffset += _size;
	    
#ifdef SUPPORT_STATS_MANAGER
        if(predOffset < m_DynamicRingVBcpuOffset)
            m_DynamicRingVBusage += m_DynamicRingVBcpuOffset - predOffset;
        else
            m_DynamicRingVBusage += m_DynamicRingVBcpuOffset + (mp_DynamicRingVB->m_size-predOffset);

        StatsManager::getptr()->dynRingVBUsageSet(m_DynamicRingVBusage);

        ITF_ASSERT(m_DynamicRingVBusage <= mp_DynamicRingVB->m_size);
#endif // SUPPORT_STATS_MANAGER

#ifdef USE_WGP
        GXRedirectWriteGatherPipe( *_data );
#endif
        return true;
    }

    void GFXAdapter_VertexBufferManager::DynamicRingVBresetOffsets()
    {
        m_DynamicRingVBcpuOffset = 0;
        m_DynamicRingVBgpuOffset = (u32)-1;
        m_DynamicRingVBgpuNextOffset = (u32)-1;
        
#ifdef SUPPORT_STATS_MANAGER
        m_DynamicRingVBusage = 0;
        StatsManager::getptr()->dynRingVBUsageSet(m_DynamicRingVBusage);
#endif // SUPPORT_STATS_MANAGER
    }
    
    void GFXAdapter_VertexBufferManager::DynamicRingVBsyncGPU()
    {
        ITF_WARNING_CATEGORY(Engine,NULL, bfalse, "GFXAdapter_VertexBufferManager::DynamicRingVBsyncGPU called.");
        
        GFX_ADAPTER->syncGPU();
        m_DynamicRingVBgpuOffset = (u32)-1;
        m_DynamicRingVBgpuNextOffset = (u32)-1;
    }
    
    void GFXAdapter_VertexBufferManager::UnlockDynamicRingVB(ITF_VertexBuffer *_VertexBuffer)
    {
        ITF_ASSERT(mp_DynamicRingVB != NULL);

#ifdef ITF_WII
        mp_DynamicRingVB->m_bnoflush = _VertexBuffer->m_bnoflush;
#ifdef USE_WGP
        if(_VertexBuffer->m_wgpWrite)
        {
            mp_DynamicRingVB->m_bnoflush = btrue;
            u32 padcnt = OSRoundUp32B(_VertexBuffer->m_wgpWrite) - _VertexBuffer->m_wgpWrite;
            padcnt /= 4;
            for(int i = 0; i < padcnt; ++i) GXWGFifo.u32 = 0;
            _VertexBuffer->m_lockedsize = 0;
        }
        GXRestoreWriteGatherPipe();
#endif // USE_WGP
#endif // ITF_WII
        _VertexBuffer->m_wgpWrite = 0;
#ifndef USE_WGP
        _VertexBuffer->m_pLockedData = 0;
#endif
        mp_DynamicRingVB->Unlock();

#ifdef ITF_WII
        _VertexBuffer->m_lockedsize = 0;
#endif // ITF_WII
    }

    
    void GFXAdapter_VertexBufferManager::DynamicRingVBBeginDraw()
    {
#ifdef SUPPORT_STATS_MANAGER
        m_DynamicRingVBusage = 0;
        StatsManager::getptr()->dynRingVBUsageSet(m_DynamicRingVBusage);
#endif // SUPPORT_STATS_MANAGER
    }

    void GFXAdapter_VertexBufferManager::DynamicRingVBEndDraw()
    {
        m_DynamicRingVBgpuNextOffset = m_DynamicRingVBcpuOffset - 1;
        m_DynamicRingVBgpuOffset = m_DynamicRingVBgpuNextOffset;
    }


#endif // VBMANAGER_USE_DYNAMICRING_VB

    void GFXAdapter_VertexBufferManager::cleanEndOfFrame()
    {
        for (u32 index = 0;index<mp_RequestDelayVertexFreeList.size();index++)
        {
            removeVertexBuffer(mp_RequestDelayVertexFreeList[index]);
        }

        mp_RequestDelayVertexFreeList.clear();

        for (u32 index = 0;index<mp_RequestDelayIndexFreeList.size();index++)
        {
            removeIndexBuffer(mp_RequestDelayIndexFreeList[index]);
        }

        mp_RequestDelayIndexFreeList.clear();

    }

    void GFXAdapter_VertexBufferManager::removeVertexBufferDelay  (   ITF_VertexBuffer* _VertexBuffer ,u32 _frameCount    )
    {
        _frameCount = 1;//not supported yet
        mp_RequestDelayVertexFreeList.push_back(_VertexBuffer);
    }

    void GFXAdapter_VertexBufferManager::removeIndexBufferDelay (   ITF_IndexBuffer* _IndexBuffer ,u32 _frameCount     )
    {
        _frameCount = 1;//not supported yet
        mp_RequestDelayIndexFreeList.push_back(_IndexBuffer);
    }

    //////////////////////////////////////////////////////////////////////////
    /// Debug Info.
#ifndef ITF_FINAL
    void GFXAdapter_VertexBufferManager::drawDebugVBInfo()
    {
        String text;
        ITF_VertexBuffer* vb;
        u32 svbnodef = 0;
        u32 svbpart = 0;
        u32 svbfrizeover = 0;
        u32 svbfrizestat = 0;
        u32 svbmesh = 0;
        u32 svbfont = 0;
        u32 svbintern = 0;
        u32 svbIcons = 0;
        u32 global = 0;

        for (u32 i = 0; i < mp_VertexBufferList.size(); i++)
        {
            vb = mp_VertexBufferList[i];

            switch(vb->m_debugType)
            {
            default:
            case VB_T_NOTDEFINED:
            svbnodef += vb->m_size;
            break;
            case VB_T_PARTICLE:
            svbpart += vb->m_size;
            break;
            case VB_T_FRIEZEANIM:
            svbfrizeover += vb->m_size;
            break;
            case VB_T_FRIEZESTATIC:
            svbfrizestat += vb->m_size;
            break;
            case VB_T_MESH:
            svbmesh += vb->m_size;
            break;
            case VB_T_FONT:
            svbfont += vb->m_size;
            break;
            case VB_T_INTERNAL:
            svbintern += vb->m_size;
            break;
            case VB_T_ICONS:
            svbIcons += vb->m_size;
            break;
            case VB_T_FRIEZEOVERLAY:
                svbfrizestat += vb->m_size;
            break;            
            }
        
            global += vb->m_size;
        }

        /// Draw.
        f32 pos = GFX_ADAPTER->getScreenHeight()- 90.f;
        f32 initpos = pos;
        f32 add = -10.f;

        if (svbnodef)
        {
            text.setTextFormat("NOT DEFINED: %d", svbnodef);
            GFX_ADAPTER->drawDBGText(text, 50, pos);    
            pos += add;
        }
        if (svbpart)
        {
            text.setTextFormat("PARTICLES: %d", svbpart);
            GFX_ADAPTER->drawDBGText(text, 50, pos);    
            pos += add;
        }
        if (svbfrizeover)
        {
            text.setTextFormat("FRIZE OVERLAY: %d", svbfrizeover);
            GFX_ADAPTER->drawDBGText(text, 50, pos);    
            pos += add;
        }
        if (svbfrizestat)
        {
            text.setTextFormat("FRIZE: %d", svbfrizestat);
            GFX_ADAPTER->drawDBGText(text, 50, pos);    
            pos += add;
        }
        if (svbmesh)
        {
            text.setTextFormat("MESH: %d", svbmesh);
            GFX_ADAPTER->drawDBGText(text, 50, pos);    
            pos += add;
        }
        if (svbfont)
        {
            text.setTextFormat("FONT: %d", svbfont);
            GFX_ADAPTER->drawDBGText(text, 50, pos);    
            pos += add;
        }
        if (svbintern)
        {
            text.setTextFormat("INTERNAL: %d", svbintern);
            GFX_ADAPTER->drawDBGText(text, 50, pos);    
            pos += add;
        }
        if (svbIcons)
        {
            text.setTextFormat("ICONS: %d", svbIcons);
            GFX_ADAPTER->drawDBGText(text, 50, pos);    
            pos += add;
        }

        text.setTextFormat("Global VB size: %d", global);
        GFX_ADAPTER->drawDBGText(text, 50, pos, 1.f,1.f,0.f);    

        u32 color = ITFCOLOR_FROM_ARGB(0x80000000);
        GFX_ADAPTER->draw2dBox( Vec2d(40 , pos - 10), 160, (initpos - pos) + 40, color, color, color, color);
    }
#endif // ITF_FINAL

} // namespace ITF
