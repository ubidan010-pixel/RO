#ifndef ITF_MEMORYSTATSMANAGER_H_
#define ITF_MEMORYSTATSMANAGER_H_

#ifdef ITF_CATEGORY_MEMORY_ENABLE

#ifndef _ITF_BLOB_H_
#include "core/Blob.h"
#endif //_ITF_BLOB_H_

namespace ITF
{

class memoryStatsManager
{

public:

	static const u32 MAX_CATEGORY = 256;

	ITF_INLINE size_t getOverheadSize(uPtr _size, u32 _align)  { return ((sizeof(HeaderMem) + sizeof(HeaderPtr) + uPtr(_align-1)) & ~uPtr(_align-1)) + _size;}

	void* addAllocation(void* _ptrRealAlloc, uPtr _size, u32 _align, MemoryId::ITF_ALLOCATOR_IDS _id); //return a valid pointer to be used
	void* removeAllocation(void* _ptr);

	void addAllocationSize(uPtr _size, MemoryId::ITF_ALLOCATOR_IDS _id);
	void removeAllocationSize(uPtr _size, MemoryId::ITF_ALLOCATOR_IDS _id);

	uPtr getCategoryCurrentAllocated(MemoryId::ITF_ALLOCATOR_IDS _id) const { return m_CategoryCurrentAllocated[_id]; }

    u32 getGlobalProfileAllocCount() const { return m_globalProfileAllocCount; }
    u32 getGlobalProfileFreeCount() const { return m_globalProfileFreeCount; }

    void resetProfileCounter();

	static void init();

	void construct();

	static void disable()
	{
		m_bEnable = bfalse;
	}

	static memoryStatsManager& get()
	{
		return instance;
	}

	void dumpCategoryStats(ITF_VECTOR<String> &_strings);
	void dumpCategoryStats(Blob& blob);
	void dumpCategoryStatsName(Blob& blob);
	void dumpLoadAllocation();

	void flushFrameAllocation();
	

protected:

	static memoryStatsManager instance;

	static bool m_bEnable;

	ITF_THREAD_CRITICAL_SECTION				m_cs;
		
	char*									m_registerEnum[MAX_CATEGORY];

	i64										m_runtimeMemoryUsage;
	i64										m_runtimeMemoryUsagePeak;

    u32										m_globalProfileAllocCount;
    u32										m_globalProfileFreeCount;

	uPtr m_CategoryCurrentAllocated[MAX_CATEGORY];
	i32 m_CategoryCounterGlobal[MAX_CATEGORY];		//global count only add allocation
	i32 m_CategoryCurrentCounter[MAX_CATEGORY];		//global count add/remove allocation
	i32 m_CategoryAllocByFrame[MAX_CATEGORY];		//count allocator between one frame (average);
	uPtr m_CategoryOneAllocationPeak[MAX_CATEGORY];	//max size allocation detected;

	i32 m_CategoryProfileCounterAlloc[MAX_CATEGORY];
	i32 m_CategoryProfileCounterFree[MAX_CATEGORY];
};

}

#define INIT_ALLOCATION									memoryStatsManager::init();

#define ADD_ALLOCATION(ptr, size, id)					memoryStatsManager::get().addAllocation(ptr, size, 1, id)
#define GET_OVERHEADSIZE(size)							memoryStatsManager::get().getOverheadSize(size, 1)

#define ADD_ALLOCATION_ALIGN(ptr, size, align, id)		memoryStatsManager::get().addAllocation(ptr, size, align, id)
#define GET_OVERHEADSIZE_ALIGN(size, align)				memoryStatsManager::get().getOverheadSize(size, align)

#define REMOVE_ALLOCATION(ptr)							memoryStatsManager::get().removeAllocation(ptr)

#define GET_HEADER_FROM_USER_PTR(ptr)					((HeaderMem*)((HeaderPtr*)(uPtr(ptr) - sizeof(HeaderPtr)))->m_Ptr)

#define ADD_ALLOCATION_SIZE(size, id)					memoryStatsManager::get().addAllocationSize(size, id)
#define REMOVE_ALLOCATION_SIZE(size, id)				memoryStatsManager::get().removeAllocationSize(size, id)

#else // ITF_CATEGORY_MEMORY_ENABLE

#define GET_HEADER_FROM_USER_PTR(ptr)					(ptr)

#endif // ITF_CATEGORY_MEMORY_ENABLE

#endif //ITF_MEMORYSTATSMANAGER_H_
