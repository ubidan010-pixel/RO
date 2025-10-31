#include "precompiled_core.h"

#ifndef _ITF_MEMORYSETTINGS_H_
#include "core/memory/memorySettings.h"
#endif //_ITF_MEMORYSETTINGS_H_

#ifndef _ITF_SLOTALLOCATORMANAGER_H_
#include "core/memory/slotallocatorManager.h"
#endif //_ITF_SLOTALLOCATORMANAGER_H_

namespace ITF
{ 
    

    void memorySetting::init()
    {
        #define KB 1024
        #define MB 1024*1024L
     
        slotAllocatorManager* sallocatorManager = newAlloc(mId_Allocator,slotAllocatorManager());
        
        memSettings settings[] = 
        {
            memSettings(MemoryId::PHYSICAL_MEMORY,32,1024,MEM_ALIGNMENT_4,ITF_PAGE_WRITECOMBINE),
            memSettings(MemoryId::PHYSICAL_MEMORY,256,1024,MEM_ALIGNMENT_4,ITF_PAGE_WRITECOMBINE),
#ifdef ITF_X360
           /*
            memSettings(MemoryId::PHYSICAL_MEMORY,128,2000,MEM_ALIGNMENT_2K,ITF_PAGE_READWRITE),
            memSettings(MemoryId::PHYSICAL_MEMORY,256,200,MEM_ALIGNMENT_2K,ITF_PAGE_READWRITE),
            memSettings(MemoryId::PHYSICAL_MEMORY,512,100,MEM_ALIGNMENT_2K,ITF_PAGE_READWRITE),
            memSettings(MemoryId::PHYSICAL_MEMORY,1*KB,1000,MEM_ALIGNMENT_2K,ITF_PAGE_READWRITE),*/
            
            memSettings(MemoryId::PHYSICAL_MEMORY,2*KB,3000,MEM_ALIGNMENT_2K,ITF_PAGE_READWRITE),
            memSettings(MemoryId::PHYSICAL_MEMORY,4*KB,100,MEM_ALIGNMENT_2K,ITF_PAGE_READWRITE),
            memSettings(MemoryId::PHYSICAL_MEMORY,8*KB,200,MEM_ALIGNMENT_2K,ITF_PAGE_READWRITE),
            memSettings(MemoryId::PHYSICAL_MEMORY,16*KB,200,MEM_ALIGNMENT_2K,ITF_PAGE_READWRITE),
            memSettings(MemoryId::PHYSICAL_MEMORY,32*KB,100,MEM_ALIGNMENT_2K,ITF_PAGE_READWRITE),
            memSettings(MemoryId::PHYSICAL_MEMORY,64*KB,64,MEM_ALIGNMENT_2K,ITF_PAGE_READWRITE),
            memSettings(MemoryId::PHYSICAL_MEMORY,128*KB,24,MEM_ALIGNMENT_2K,ITF_PAGE_READWRITE),

            memSettings(MemoryId::PHYSICAL_MEMORY,393216,56,MEM_ALIGNMENT_2K,ITF_PAGE_READWRITE),
            //sound buffer
          
#endif //ITF_X360
     
            
        };
       
        size_t size_of_array = (sizeof settings)/(sizeof settings[0]); 

        u32 countHeap       = 0;
        u32 countPhysical   = 0;
        u32 indexHeap       = 0;
        u32 indexPhysical   = 0;
        

        for (u32 index =0;index < size_of_array;++index)
        {
           if (settings[index].m_MemoryType == MemoryId::PHYSICAL_MEMORY)
                countPhysical++;
           else
                countHeap++;
        }

        //allocate slot allocator in a continuous memory,parsing allocator must be faster
        sallocatorManager->prepare(countHeap,countPhysical);
            
        for (u32 index = 0; index<size_of_array ;++index)
        {
            u32& indexSlot = (settings[index].m_MemoryType == MemoryId::PHYSICAL_MEMORY) ? indexPhysical : indexHeap;
            sallocatorManager->init(settings[index],indexSlot);
            indexSlot++;
        }

        


   }


} // namespace ITF
	

