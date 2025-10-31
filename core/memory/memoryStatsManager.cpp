#include "precompiled_core.h"

#ifndef _ITF_CORE_MACROS_H_
#include "core/macros.h"
#endif //_ITF_CORE_MACROS_H_

#ifdef ITF_CATEGORY_MEMORY_ENABLE

#ifndef ITF_MEMORYSTATSMANAGER_H_
#include "core/memory/memoryStatsManager.h"
#endif //ITF_MEMORYSTATSMANAGER_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

namespace ITF
{
    memoryStatsManager memoryStatsManager::instance;

    bool   memoryStatsManager::m_bEnable = btrue;

    void memoryStatsManager::init()
    {
        static bbool isInit = bfalse;
        if (isInit)
            return;

        isInit= true;
        instance.construct();    
    }

    void memoryStatsManager::resetProfileCounter()
    {
        csAutoLock cs(m_cs);
        ITF_MemSet(m_CategoryProfileCounterAlloc,0,sizeof(m_CategoryProfileCounterAlloc));
        ITF_MemSet(m_CategoryProfileCounterFree,0,sizeof(m_CategoryProfileCounterFree));

        m_globalProfileAllocCount          = 0;
        m_globalProfileFreeCount           = 0;
    }

    void memoryStatsManager::construct()
    {
        Synchronize::createCriticalSection(&m_cs);
        ITF_MemSet(m_CategoryCurrentAllocated,0,sizeof(m_CategoryCurrentAllocated));
        ITF_MemSet(m_CategoryCounterGlobal,0,sizeof(m_CategoryCounterGlobal));
        ITF_MemSet(m_CategoryCurrentCounter,0,sizeof(m_CategoryCurrentCounter));
        ITF_MemSet(m_CategoryAllocByFrame,0,sizeof(m_CategoryAllocByFrame));
        ITF_MemSet(m_CategoryOneAllocationPeak,0,sizeof(m_CategoryOneAllocationPeak));
       
        resetProfileCounter();
        m_runtimeMemoryUsage        = 0;
        m_runtimeMemoryUsagePeak    = 0;

        m_globalProfileAllocCount          = 0;
        m_globalProfileFreeCount           = 0;

        #define REGISTER_ENUM(__enum) m_registerEnum[MemoryId::__enum] = #__enum;
        
        REGISTER_ENUM(mId_Unknow);
        REGISTER_ENUM(mId_String8);
        REGISTER_ENUM(mId_String16);
        REGISTER_ENUM(mId_IDServer);
        REGISTER_ENUM(mId_SafeArray);
        REGISTER_ENUM(mId_Scene);
        REGISTER_ENUM(mId_Animation);
        REGISTER_ENUM(mId_LogicDataBase);
        REGISTER_ENUM(mId_Temporary);
        REGISTER_ENUM(mId_Singleton);
        REGISTER_ENUM(mId_StringIDDatabase);
        REGISTER_ENUM(mId_Map);
        REGISTER_ENUM(mId_Vector);
        REGISTER_ENUM(mId_Phys);
        REGISTER_ENUM(mId_Vertex);
        REGISTER_ENUM(mId_List);
        REGISTER_ENUM(mId_Misc);
        REGISTER_ENUM(mId_Gameplay);
        REGISTER_ENUM(mId_Allocator);
        REGISTER_ENUM(mId_AudioData);
        REGISTER_ENUM(mId_TinyXML);
        REGISTER_ENUM(mId_UI);
        REGISTER_ENUM(mId_Resource);
        REGISTER_ENUM(mId_Mesh);
        REGISTER_ENUM(mId_Editor);
        REGISTER_ENUM(mId_System);
        REGISTER_ENUM(mId_Particle);
	    REGISTER_ENUM(mId_Polyline);
        REGISTER_ENUM(mId_Frieze);
        REGISTER_ENUM(mId_Blob);
        REGISTER_ENUM(mId_GfxAdapter);
        REGISTER_ENUM(mId_VectorAnimation);
        REGISTER_ENUM(mId_SavegameManager);
        REGISTER_ENUM(mId_Plugin);
        REGISTER_ENUM(mId_Textures);
        REGISTER_ENUM(mId_VertexBuffer);
        REGISTER_ENUM(mId_IndexBuffer);
        REGISTER_ENUM(mId_ExeSystem);
        REGISTER_ENUM(mId_SafeArrayCache);               
        REGISTER_ENUM(mId_VectorPointer);
        REGISTER_ENUM(mId_VectorObjectRef);
        REGISTER_ENUM(mId_VectorString);
        REGISTER_ENUM(mId_VectorInt);
        REGISTER_ENUM(mId_VectorAnimTrackBone);
        REGISTER_ENUM(mId_VectorAnimTrackBonePAS);
        REGISTER_ENUM(mId_VectorAnimTrackBoneZAL);
        REGISTER_ENUM(mId_VertexBufferStatic);
        REGISTER_ENUM(mId_GameplayTemplate);
        REGISTER_ENUM(mId_GameplayFactory);
        REGISTER_ENUM(mId_GameplayMisc);
        REGISTER_ENUM(mId_GameplayInputCriteria);
        REGISTER_ENUM(mId_ParticleVertex);
        REGISTER_ENUM(mId_Actor);
        REGISTER_ENUM(mId_Serialization);
        REGISTER_ENUM(mId_WiiSystem);
        REGISTER_ENUM(mId_FileCache);                        
        REGISTER_ENUM(mId_AudioStreamBuffers);
        REGISTER_ENUM(mId_AudioEngine);
        REGISTER_ENUM(mId_Archive);
    }

    void* memoryStatsManager::addAllocation(void* _ptrRealAlloc, uPtr _size, u32 _align,MemoryId::ITF_ALLOCATOR_IDS  _id)
    {
        HeaderMem* header = (HeaderMem*)_ptrRealAlloc;

        header->m_Ptr = ((uPtr) (_ptrRealAlloc) + getOverheadSize(_size,_align) - _size);
        header->m_size = _size;
        
        ITF_ASSERT(!(header->m_Ptr & (_align-1)));
        
        ITF_AllocatorAttributes attribute = {0};
        attribute.dwAllocatorId = _id;
        attribute.dwAlignment = _align;
      

        header->m_attribute = attribute;

        HeaderPtr* headerPtr = (HeaderPtr*)(header->m_Ptr - sizeof(HeaderPtr));
        headerPtr->m_Ptr = (uPtr) header;

        addAllocationSize(_size, _id);

        return (void*)(header->m_Ptr);
    }

    void* memoryStatsManager::removeAllocation(void* _ptr)
    {
        HeaderMem* header = GET_HEADER_FROM_USER_PTR(_ptr);
        ITF_ASSERT(header->m_Ptr==(uPtr)_ptr);
 
        removeAllocationSize(header->m_size, (MemoryId::ITF_ALLOCATOR_IDS)header->m_attribute.dwAllocatorId);

        return (void*) header; //start of the allocation block
    }

    void memoryStatsManager::addAllocationSize(uPtr _size, MemoryId::ITF_ALLOCATOR_IDS _id)
    {
        if (m_bEnable)
        {
            //track is enable

            csAutoLock cs(m_cs);
            m_CategoryCurrentAllocated[_id] += _size;
            m_CategoryCounterGlobal[_id] ++;
            m_CategoryCurrentCounter[_id]++;
            m_CategoryAllocByFrame[_id]++;
            m_CategoryProfileCounterAlloc[_id]++;
            m_globalProfileAllocCount++;

            if (_size>m_CategoryOneAllocationPeak[_id])
                m_CategoryOneAllocationPeak[_id] = _size;

            m_runtimeMemoryUsage += _size;

            if (m_runtimeMemoryUsage > m_runtimeMemoryUsagePeak)
            {
                m_runtimeMemoryUsagePeak = m_runtimeMemoryUsage;
            }
        }
    }
    
    void memoryStatsManager::removeAllocationSize(uPtr _size, MemoryId::ITF_ALLOCATOR_IDS _id)
    {
        if (m_bEnable)
        {
            csAutoLock cs(m_cs);
            m_CategoryCurrentAllocated[_id] -= _size;
            m_CategoryCurrentCounter[_id] --;
            m_CategoryProfileCounterFree[_id] ++;
            m_globalProfileFreeCount++;

            if ( m_CategoryCurrentAllocated[_id]<0)
            {
    #ifdef ITF_WINDOWS
                __debugbreak();
    #endif
            }

            m_runtimeMemoryUsage -= _size;

            ITF_ASSERT(m_CategoryCurrentAllocated[_id]>=0);
        }
    }

    void memoryStatsManager::flushFrameAllocation()
    {
        csAutoLock cs(m_cs);

        ITF_MemSet(m_CategoryAllocByFrame,0,sizeof m_CategoryAllocByFrame);
    }

    void memoryStatsManager::dumpCategoryStats(ITF_VECTOR<String> &_strings)
    {
        _strings.reserve(MemoryId::mId_Max);
        String tmp;
        for (u32 index =MemoryId::mId_Unknow ;index <MemoryId::mId_Max;++index)
        {
            const String& key =  m_registerEnum[index];
            tmp.setTextFormat("%-20ls %zu" ,key.cStr(),m_CategoryCurrentAllocated[index]);
            _strings.push_back(tmp);
        }

        tmp.setTextFormat("Global %d Mb  (%d)" ,((m_runtimeMemoryUsage>>10)>>10),m_runtimeMemoryUsage);
        _strings.push_back(tmp);

        tmp.setTextFormat("Peak %d Mb  (%d)" ,((m_runtimeMemoryUsagePeak>>10)>>10),m_runtimeMemoryUsagePeak);
        _strings.push_back(tmp);
    }

    void memoryStatsManager::dumpCategoryStatsName(Blob& blob)
    {
        blob.pushBool(btrue);
        blob.pushUInt32(MemoryId::mId_Max);
        for (u32 index =0 ;index <MemoryId::mId_Max;++index)
        {
            const String& key =  m_registerEnum[index];
            blob.pushString(key.cStr());
        }
    }
    
    //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void memoryStatsManager::dumpLoadAllocation()
    {
        String tmp;

        u32 alloc = 0;
        u32 free  = 0;
        LOG(">>---------------------------------------------------------------------------------------------------------------------------------------------------");
        for (u32 index =MemoryId::mId_Unknow ;index <MemoryId::mId_Max;++index)
        {
            const String& key =  m_registerEnum[index];
            alloc+=m_CategoryProfileCounterAlloc[index];
            free+=m_CategoryProfileCounterFree[index];
            tmp.setTextFormat("%-30ls %10d %10d" ,key.cStr(),m_CategoryProfileCounterAlloc[index],m_CategoryProfileCounterFree[index]);
            LOG(">>%s", StringToUTF8(tmp).get());
        }

        tmp.setTextFormat("%-30s %10d %10d" ,"global sum",alloc,free);
        LOG(">>%s", StringToUTF8(tmp).get());

        tmp.setTextFormat("%-30s %10d %10d" ,"global real",m_globalProfileAllocCount,m_globalProfileFreeCount);
        LOG(">>%s", StringToUTF8(tmp).get());
        LOG(">>---------------------------------------------------------------------------------------------------------------------------------------------------");
    }


    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------- 

    void memoryStatsManager::dumpCategoryStats(Blob& blob)
    {
        blob.pushBool(btrue);
        blob.pushUInt32(MemoryId::mId_Max);
        for (u32 index =0 ;index <MemoryId::mId_Max;++index)
        {
            blob.pushUInt64(m_CategoryCurrentAllocated[index]);
            blob.pushInt32(m_CategoryCounterGlobal[index]);
            blob.pushInt32(m_CategoryCurrentCounter[index]);
            blob.pushUInt64(m_CategoryOneAllocationPeak[index]);
            blob.pushInt32(m_CategoryAllocByFrame[index]);
        }
    }
}
#endif //ITF_CATEGORY_MEMORY_ENABLE
