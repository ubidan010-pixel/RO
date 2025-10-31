#include <nn/fnd.h>
#include <nn/os.h>
#include <nn/init.h>
#include <nn/config.h>
#include <nn/init.h>
#include <nn/os.h>
#include <nn/ndm.h>

#define RESERVED_MEMORY_FOR_GX (1*1024*1024)

namespace ITF
{

    /*
    static bbool g_isCTRMemoryInitialized = bfalse;

    static void getHeaps(nn::fnd::ExpHeap *&_outAppHeap, nn::fnd::ExpHeap *&_outMallocHeap)
    {
        static nn::fnd::ExpHeap applicationHeap;
        static nn::fnd::ExpHeap mallocHeap;

        _outAppHeap = &applicationHeap;
        _outMallocHeap = &mallocHeap;
    }


    ///////////////////////////////////////////////////////////////////////////////////////////
    void mayInitCTRMemory()
    {
        nn::fnd::ExpHeap *appHeap=NULL, *mallocHeap=NULL;

        if (g_isCTRMemoryInitialized)
        {
            return;
        }

        nn::os::Initialize();        

        g_isCTRMemoryInitialized = btrue;

        nn::os::InitializeDeviceMemory();
        //initialize total memory
        uptr baseAddress = nn::os::GetDeviceMemoryAddress();
        size_t memorySize = NN_OS_DEVICE_MEMORY_SIZE;
        getHeaps(appHeap, mallocHeap);

        appHeap->Initialize(baseAddress, memorySize, nn::os::ALLOCATE_OPTION_LINEAR);

        //We reserve mem but dont use yet
        appHeap->Allocate(RESERVED_MEMORY_FOR_GX);

        //
        size_t availableMem = appHeap->GetAllocatableSize();
        uptr memForMalloc = (uptr)appHeap->Allocate(availableMem);
        mallocHeap->Initialize(memForMalloc, availableMem);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void* defaultMallocCategory(size_t size,MemoryId::ITF_ALLOCATOR_IDS)
    {
        mayInitCTRMemory();

        nn::fnd::ExpHeap *appHeap, *mallocHeap;
        getHeaps(appHeap, mallocHeap);
        void *r = mallocHeap->Allocate(size);
        ITF_ASSERT(r!=NULL);
        return r;
    }

    void* defaultMalloc(size_t size)
    {
        mayInitCTRMemory();

        nn::fnd::ExpHeap *appHeap, *mallocHeap;
        getHeaps(appHeap, mallocHeap);
        void *r = mallocHeap->Allocate(size);
        ITF_ASSERT(r!=NULL);
        return r;
    }

    void defaultFree(void* ptr)
    {
        if (ptr!=NULL)
        {
            mayInitCTRMemory();

            nn::fnd::ExpHeap *appHeap, *mallocHeap;
            getHeaps(appHeap, mallocHeap);
            return mallocHeap->Free(ptr);
        }
    }
    void* defaultMallocCategory(size_t size,MemoryId::ITF_ALLOCATOR_IDS)
    {
        mayInitCTRMemory();

        nn::fnd::ExpHeap *appHeap, *mallocHeap;
        getHeaps(appHeap, mallocHeap);
        void *r = mallocHeap->Allocate(size);
        ITF_ASSERT(r!=NULL);
        return r;
    }

    void* defaultMalloc(size_t size)
    {
        mayInitCTRMemory();

        nn::fnd::ExpHeap *appHeap, *mallocHeap;
        getHeaps(appHeap, mallocHeap);
        void *r = mallocHeap->Allocate(size);
        ITF_ASSERT(r!=NULL);
        return r;
    }

    void defaultFree(void* ptr)
    {
        if (ptr!=NULL)
        {
            mayInitCTRMemory();

            nn::fnd::ExpHeap *appHeap, *mallocHeap;
            getHeaps(appHeap, mallocHeap);
            return mallocHeap->Free(ptr);
        }
    }

*/
void mayInitCTRMemory()
{
}

void* defaultMallocCategory(size_t size,MemoryId::ITF_ALLOCATOR_IDS)
{
    void *r = ::malloc(size);
    ITF_ASSERT(r!=NULL);
	if(r==NULL)
	{
		while(1){};
	}
    return r;
}

void* defaultMalloc(size_t size)
{
    void *r = ::malloc(size);
    ITF_ASSERT(r!=NULL);
	if(r==NULL)
	{
		while(1){};
	}
    return r;
}

void* defaultRealloc(void* ptr, size_t size)
{
    return ::realloc(ptr, size);
}

void defaultFree(void* ptr)
{
    if (ptr!=NULL)
    {
        return ::free(ptr);
    }
}

}

extern "C"
{
  /*  void nninitStartUp(void)
    {        
        const u32 memoryMargin=5*1024*1024;
        nn::os::InitializeDeviceMemory();
        nnosMemoryBlock block;
        u32 blockSize = 64*1024*1024-memoryMargin;
        nnosMemoryBlockAllocate(&block, blockSize);
        NN_ASSERT(nnosMemoryBlockGetSize(&block)==blockSize);
        uptr blockAddr = nnosMemoryBlockGetAddress(&block);
#ifndef ITF_FINAL
        NN_LOG("Heap memory : %08X-%08X\n", blockAddr,blockAddr+blockSize);
        NN_LOG("Device memory : %08X-%08X\n", nn::os::GetDeviceMemoryAddress(),nn::os::GetDeviceMemoryAddress()+NN_OS_DEVICE_MEMORY_SIZE);
#endif
        nn::init::InitializeAllocator(blockAddr, blockSize);
    }*/


	//extern size_t g_codeSizeInMemory;
	//extern size_t g_heapSize; 
	//extern size_t g_defaultHeapSize ;

	//void nninitStartUp()
	//{
	//	g_heapSize = nn::os::GetAppMemorySize();// - NN_OS_DEVICE_MEMORY_SIZE;
	//	g_codeSizeInMemory = nn::os::GetUsingMemorySize();

	//	const size_t assingment   = nn::os::GetAppMemorySize();
	//	const size_t currentUsing = nn::os::GetUsingMemorySize();

	//	const size_t available = assingment - currentUsing;
	//	const size_t heapSize  = available - nn::os::DEVICE_MEMORY_SIZE;

	//	nn::os::SetupHeapForMemoryBlock(heapSize);
	//	nn::os::InitializeDeviceMemory();

	//	//g_defaultHeapSize = heapSize;
 
	//	nn::init::InitializeAllocator( g_defaultHeapSize );
	//}
}

namespace std
{
extern "C"
{
    void *realloc(void *_memory, size_t _s)
    {
        if (_memory)
        {
            typedef nn::fnd::ExpHeapTemplate<nn::os::LockPolicy::Object<nn::os::Mutex> > SysHeap;
            SysHeap *heap = (SysHeap*)nn::init::GetAllocator();
            size_t oldSize = heap->GetSizeOf(_memory);
            if (oldSize>=_s)
            {
                return _memory;
            }

            void *newMemory = malloc(_s);
            ITF_ASSERT(newMemory);
            ITF_Memcpy(newMemory, _memory, oldSize);
            free(_memory);
            return newMemory;
            
        }
        return malloc(_s);
    }
}
}

#include "Memory_Align.inl"
