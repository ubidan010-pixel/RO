#include "precompiled_core.h"

#include <new>

#ifndef _ITF_MEMORY_H_
#include "core/memory/memory.h"
#endif //_ITF_MEMORY_H_

#ifndef ITF_MEMORYDEBUGGER_H_
    #include "core/memory/MemoryDebugger.h"
#endif //ITF_MEMORYDEBUGGER_H_

#ifndef _ITF_STACKWALKER_H_
#include "core/StackWalker.h"
#endif //_ITF_STACKWALKER_H_

#if defined (ITF_X360)
#include "memory_x360.h"
#elif defined (ITF_WINDOWS) || defined(ITF_XBOX_SERIES)
#include "memory_Win.inl"
#elif defined (ITF_PS3)
#include "memory_PS3.h"
#elif defined(ITF_IPAD)
#include "memory_iPad.inl"
#elif defined(ITF_CTR)
#include "memory_CTR.inl"
#elif defined(ITF_WII)
#include "memory_WII.h"
#elif defined(ITF_VITA)
#include "memory_VITA.inl"
#elif defined(ITF_PS5)
#include "memory_PS5.h"
#elif defined(ITF_NINTENDO)
#include "memory_Nintendo.h"
#else
#error Platform not supported
#endif

#ifndef ITF_MEMORYSTATSMANAGER_H_
#include "core/memory/memoryStatsManager.h"
#endif //ITF_MEMORYSTATSMANAGER_H_


// Regular repleaceable new
// More variants exists but are fallbacking on these.
[[nodiscard]] void* operator new(size_t size)
{
    return ITF::Memory::mallocCategory(size, ITF::MemoryId::mId_Unknow);
}

[[nodiscard]] void* operator new[](size_t size)
{
    return ITF::Memory::mallocCategory(size, ITF::MemoryId::mId_Unknow);
}

// Alignment explicit versions
// Called only if the alignment is greater than __STDCPP_DEFAULT_NEW_ALIGNMENT__ (16 on VS, might be something else on others)
#if !defined(ITF_PS5)
// On PS5, use of MemMng guaranty that the minimal alignment is aligned on size alignment up to 32.
static_assert(__STDCPP_DEFAULT_NEW_ALIGNMENT__ == 16, "Unexpected default new alignment. Please check that everything's functional.");
#endif
[[nodiscard]] void* operator new(size_t size, std::align_val_t align)
{
    return ITF::Memory::alignedMallocCategory(size, static_cast<size_t>(align), ITF::MemoryId::mId_Unknow);
}

[[nodiscard]] void* operator new[](std::size_t size, std::align_val_t align)
{
    return ITF::Memory::alignedMallocCategory(size, static_cast<size_t>(align), ITF::MemoryId::mId_Unknow);
}

// Regular replaceable delete
void operator delete(void* ptr) noexcept
{
    if (ptr != nullptr)
    {
        ITF::Memory::free(ptr);
    }
}

void operator delete[](void* ptr) noexcept
{
    if (ptr != nullptr)
    {
        ITF::Memory::free(ptr);
    }
}

// Alignment explicit versions
void operator delete(void* ptr, std::align_val_t al) noexcept
{
    if (ptr != nullptr)
    {
        ITF::Memory::alignedFree(ptr);
    }
}

void operator delete[](void* ptr, std::align_val_t al) noexcept
{
    if (ptr != nullptr)
    {
        ITF::Memory::alignedFree(ptr);
    }
}

// new with explicit category
[[nodiscard]] void* operator new(size_t size, ITF::MemoryId::ITF_ALLOCATOR_IDS _allocId)
{
    return ITF::Memory::mallocCategory(size, _allocId);
}
[[nodiscard]] void* operator new[](size_t size, ITF::MemoryId::ITF_ALLOCATOR_IDS _allocId)
{
   return ITF::Memory::mallocCategory(size, _allocId);
}

// Equivalent delete (called in case the constructor throws)
void operator delete(void * ptr, ITF::MemoryId::ITF_ALLOCATOR_IDS _allocId)
{
    if (ptr != nullptr)
    {
        ITF::Memory::free(ptr);
    }
}

void operator delete[](void* ptr, ITF::MemoryId::ITF_ALLOCATOR_IDS _allocId)
{
    if (ptr != nullptr)
    {
        ITF::Memory::free(ptr);
    }
}


// Alignment explicit versions.
[[nodiscard]] void* operator new(size_t size, std::align_val_t align, ITF::MemoryId::ITF_ALLOCATOR_IDS _allocId)
{
    return ITF::Memory::alignedMallocCategory(size, static_cast<size_t>(align), _allocId);
}

[[nodiscard]] void* operator new[](size_t size, std::align_val_t align, ITF::MemoryId::ITF_ALLOCATOR_IDS _allocId)
{
    return ITF::Memory::alignedMallocCategory(size, static_cast<size_t>(align), _allocId);
}

// Equivalent delete (called in case the constructor throws)
void operator delete(void* ptr, std::align_val_t align, ITF::MemoryId::ITF_ALLOCATOR_IDS _allocId)
{
    if (ptr != nullptr)
    {
        ITF::Memory::free(ptr);
    }
}

void operator delete[](void* ptr, std::align_val_t align, ITF::MemoryId::ITF_ALLOCATOR_IDS _allocId)
{
    if (ptr != nullptr)
    {
        ITF::Memory::free(ptr);
    }
}

// A version with explicit filename and line (obsolete ?)
[[nodiscard]] void* operator new(size_t size, const char* filename, int nline)
{
    return ITF::Memory::mallocCategory(size,ITF::MemoryId::mId_Unknow);
}

[[nodiscard]] void* operator new(size_t size, std::align_val_t align, const char* filename, int nline)
{
    return ITF::Memory::alignedMallocCategory(size, static_cast<size_t>(align), ITF::MemoryId::mId_Unknow);
}
// Equivalent delete (called in case the constructor throws)
void operator delete(void* ptr, const char* filename, int nline)
{
    if (ptr != nullptr)
    {
        ITF::Memory::free(ptr);
    }
}

void operator delete[](void* ptr, const char* filename, int nline)
{
    if (ptr != nullptr)
    {
        ITF::Memory::free(ptr);
    }
}


// The allocator front end (with debug decorator)

namespace ITF
{

#if defined (ITF_WINDOWS) && defined (ITF_CATEGORY_MEMORY_ENABLE)
        Memory::MallocFunction Memory::m_mallocFunction                     = defaultMallocTracking;
        Memory::MallocCategoryFunction Memory::m_mallocCategoryFunction     = defaultMallocCategoryTracking;

        Memory::ReallocFunction Memory::m_reallocFunction                   = defaultReallocCategoryTracking;
        Memory::FreeFunction Memory::m_freeFunction                         = defaultFreeTracking;
#else 
        Memory::ReallocFunction Memory::m_reallocFunction                   = defaultRealloc;
        Memory::MallocFunction Memory::m_mallocFunction                     = defaultMalloc;
        Memory::MallocCategoryFunction Memory::m_mallocCategoryFunction     = defaultMallocCategory;
        Memory::FreeFunction Memory::m_freeFunction                         = defaultFree;
#endif // ITF_WINDOWS && ITF_CATEGORY_MEMORY_ENABLE

    Memory::SizeFunction Memory::m_sizeFunction                             = defaultSize;
    Memory::AlignedMallocFunction Memory::m_alignedMallocFunction = defaultAlignedMalloc;
    Memory::AlignedMallocCategoryFunction Memory::m_alignedMallocCategoryFunction = defaultAlignedMallocCategory;
    Memory::AlignedReallocFunction Memory::m_alignedReallocFunction = defaultAlignedRealloc;
    Memory::AlignedFreeFunction Memory::m_alignedFreeFunction = defaultAlignedFree;
    Memory::AlignedSizeFunction Memory::m_alignedSizeFunction               = defaultAlignedSize;
    int Memory::m_allocatorSetupVersion = 0;

    void Memory::setMallocFunction(MallocFunction funct)
    {
        m_mallocFunction = funct;
        m_allocatorSetupVersion++;
    }

    void Memory::setMallocCategoryFunction(MallocCategoryFunction funct)
    {
        m_mallocCategoryFunction = funct;
        m_allocatorSetupVersion++;
    }


    void Memory::setReallocFunction(ReallocFunction funct)
    {
        m_reallocFunction = funct;
        m_allocatorSetupVersion++;
    }

    void Memory::setFreeFunction(FreeFunction funct)
    {
        m_freeFunction = funct;
        m_allocatorSetupVersion++;
    }

    void Memory::setSizeFunction(SizeFunction funct)
    {
        m_sizeFunction = funct;
        m_allocatorSetupVersion++;
    }

    void Memory::setAlignedMallocFunction(AlignedMallocFunction funct)
    {
        m_alignedMallocFunction = funct;
        m_allocatorSetupVersion++;
    }

    void Memory::setAlignedMallocCategoryFunction(AlignedMallocCategoryFunction funct)
    {
        m_alignedMallocCategoryFunction = funct;
        m_allocatorSetupVersion++;
    }

    void Memory::setAlignedReallocFunction(AlignedReallocFunction funct)
    {
        m_alignedReallocFunction = funct;
        m_allocatorSetupVersion++;
    }

    void Memory::setAlignedFreeFunction(AlignedFreeFunction funct)
    {
        m_alignedFreeFunction = funct;
        m_allocatorSetupVersion++;
    }

    void Memory::setAlignedSizeFunction(AlignedSizeFunction funct)
    {
        m_alignedSizeFunction = funct;
        m_allocatorSetupVersion++;
    }

    int Memory::getAllocatorSetupVersion()
    {
        return m_allocatorSetupVersion;
    }

    Memory::MallocFunction Memory::getMallocFunction()
    {
        return m_mallocFunction;
    }


    Memory::MallocCategoryFunction Memory::getMallocCategoryFunction()
    {
       return m_mallocCategoryFunction;
    }

    Memory::ReallocFunction Memory::getReallocFunction()
    {
        return m_reallocFunction;
    }

    Memory::FreeFunction Memory::getFreeFunction()
    {
        return m_freeFunction;
    }

    Memory::SizeFunction Memory::getSizeFunction()
    {
        return m_sizeFunction;
    }

    Memory::AlignedMallocFunction Memory::getAlignedMallocFunction()
    {
        return m_alignedMallocFunction;
    }

    Memory::AlignedMallocCategoryFunction Memory::getAlignedMallocCategoryFunction()
    {
        return m_alignedMallocCategoryFunction;
    }

    Memory::AlignedReallocFunction Memory::getAlignedReallocFunction()
    {
        return m_alignedReallocFunction;
    }

    Memory::AlignedFreeFunction Memory::getAlignedFreeFunction()
    {
        return m_alignedFreeFunction;
    }

    Memory::AlignedSizeFunction Memory::getAlignedSizeFunction()
    {
        return m_alignedSizeFunction;
    }

#ifndef ITF_DEBUG_MEMORY

    void* Memory::malloc(size_t size)
    {
        return m_mallocFunction(size); 
    }


    void* Memory::mallocCategory(size_t size,MemoryId::ITF_ALLOCATOR_IDS id)
    {
        return m_mallocCategoryFunction(size,id);
    }

    void* Memory::realloc(void* ptr, size_t size)
    {
        return m_reallocFunction(ptr, size);
    }

    void Memory::free(void* ptr)
    {
        m_freeFunction(ptr);
    }

    size_t Memory::size(void* ptr)
    {
        return m_sizeFunction(ptr);
    }

    void* Memory::alignedMalloc(size_t size, size_t align)
    {
        return m_alignedMallocFunction(size, align);
    }

    void* Memory::alignedMallocCategory(size_t size, size_t align, MemoryId::ITF_ALLOCATOR_IDS id)
    {
        return m_alignedMallocCategoryFunction(size, align, id);
    }

    void* Memory::alignedRealloc(void* ptr, size_t size, size_t align)
    {
        return m_alignedReallocFunction(ptr, size, align);
    }

    void Memory::alignedFree(void* ptr)
    {
        m_alignedFreeFunction(ptr);
    }

    size_t Memory::alignedSize(void* ptr)
    {
        return m_alignedSizeFunction(ptr);
    }

#else // ITF_DEBUG_MEMORY

    void* Memory::malloc(size_t size)
    {       
        MemoryAllocationStateObject stateObject;
        void* block = m_mallocFunction(size);
        MemoryDebugger::addBlock(block, size, 0);
        return block;
    }

    void* Memory::realloc(void* ptr, size_t size)
    {
        MemoryAllocationStateObject stateObject;
        MemoryDebugger::removeBlock(ptr, false);
        void* block = m_reallocFunction(ptr, size);
        MemoryDebugger::addBlock(block, size, 0);
        return block;
    }

    void Memory::free(void* ptr)
    {
        MemoryAllocationStateObject stateObject;
        MemoryDebugger::removeBlock(ptr, false);
        m_freeFunction(ptr);
    }

    size_t Memory::size(void* ptr)
    {
        return m_sizeFunction(ptr);
    }

    void* Memory::alignedMalloc(size_t size, size_t align)
    {
        MemoryAllocationStateObject stateObject;
        void* block = m_alignedMallocFunction(size, align);
        MemoryDebugger::addBlock(block, size, align);
        return block;
    }

    void* Memory::alignedRealloc(void* ptr, size_t size, size_t align)
    {
        MemoryAllocationStateObject stateObject;
        MemoryDebugger::removeBlock(ptr, true);
        void* block = m_alignedReallocFunction(ptr, size, align);
        MemoryDebugger::addBlock(block, size, align);
        return block;
    }

    void Memory::alignedFree(void* ptr)
    {
        MemoryAllocationStateObject stateObject;
        MemoryDebugger::removeBlock(ptr, true);
        m_alignedFreeFunction(ptr);
    }

    size_t Memory::alignedSize(void* ptr)
    {
        return m_alignedSizeFunction(ptr);
    }

#endif // ITF_DEBUG_MEMORY
}
