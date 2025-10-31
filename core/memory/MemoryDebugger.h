#ifndef ITF_MEMORYDEBUGGER_H_
#define ITF_MEMORYDEBUGGER_H_

#ifndef _ITF_MEMORY_H_
#include "core/memory/memory.h"
#endif //_ITF_MEMORY_H_

#ifndef _ITF_STACKWALKER_H_
#include "core/StackWalker.h"
#endif //_ITF_STACKWALKER_H_

#include <set>

#ifdef ITF_DEBUG_MEMORY

namespace ITF
{
    class MemoryDebugger
    {
    public:
        ///
        /// Destructor must be public for placement delete
        ///
        ~MemoryDebugger();

        ///
        /// Initialize the memory debugger
        ///
        static void initialize();

        ///
        /// Shutdown the memory debugger
        ///
        static void shutdown();

        ///
        /// Add a block of memory to keep track.  If the block is not removed
        /// before a call to reportLeaks, it will be reported as a leak
        /// @param ptr the pointer to the memory block
        /// @param size of the memory block
        /// @param alignement of the memory block
        ///
        static void addBlock(void* ptr, size_t size, size_t align);

        ///
        /// Remove a previously added memory block
        /// @param ptr the pointer to the memory block
        /// @param expectedAlign set to true if that was supposed to be an align memory block
        ///
        static void removeBlock(void* ptr, bool expectedAlign);

        ///
        /// Report all memory blocks that were not removed as a leak.
        ///
        static void reportLeaks();

        ///
        /// Start a scoped trace.
        ///
        static void startScopedTraces();

        ///
        /// Stop a scoped trace.
        ///
        static void stopScopedTraces();

        ///
        /// Get the allocation's count not freed between start/stop scoped traces.
        /// @return The scoped traces count.
        ///
        static unsigned int getScopedTracesCount();

        ///
        /// Report all allocation's not freed between start/stop scoped traces.
        ///
        static void reportScopedTraces();

        ///
        /// @return the number of allocated blocks still not freed
        ///
        static int getNumberOfAllocatedBlocks(); 

        ///
        /// @return true if we are currently doing an allocation
        ///
        static bool isInsideAllocation();
        static void setAllocationState(bool isInside);

    private:
        MemoryDebugger();

        static MemoryDebugger* m_singleton;
        static bool m_singletonLocked;
        static int m_currentBlockCount;
        static int m_isInsideAllocationCounter;
        
        struct Stack
        {
            enum { MAX_STACK_DEPTH = 32 };
            Stack* m_next;
            StackWalker::ProcessAddress m_addresses[MAX_STACK_DEPTH];
            u16 m_stackDepth;
            int m_referenceCount;
            int m_maxReferenceCount;
            int m_totalReferenceCount;
        };

        struct StackPool
        {
            StackPool* m_next;
            Stack* m_stacks;
        };
        StackPool* m_stackPools;
        Stack* m_freeStacks;
        enum { STACK_MAP_SIZE = 1024 };
        Stack* m_stackMap[STACK_MAP_SIZE];
        void createStackPool(size_t size);
        Stack* retrieveStack();
        static unsigned int hashAddress(StackWalker::ProcessAddress* addr);

        struct Block
        {
            Block* m_next;
            void* m_pointer;
            size_t m_size;
            size_t m_alignement;
            int m_id;
            int m_allocatorSetupVersion;
            Stack* m_stack;
        };

        struct BlockPool
        {
             BlockPool* m_next;
             Block* m_blocks;
        };
        bool m_locked;
        BlockPool* m_blockPools;
        Block* m_freeBlocks;
        void createBlockPool(size_t size);

        enum { BLOCK_MAP_SIZE = 1024 };
        Block* m_blockMap[BLOCK_MAP_SIZE];
        static unsigned int hashPointer(void* ptr);

        size_t m_maxMemoryUsage;
        size_t m_maxMemoryUsageBlockCount;
        size_t m_currentMemoryUsage;
        int m_maxBlockCount;
        int m_totalBlockCount;
        StackWalker m_stackWalker;

        void showBlockTrace(Block* b);
        void showStackTrace(const char* header, Stack* b);

        void showLeaks();
        void showPopularAllocations(unsigned int maxToShow);
        static bool stackTotalReferenceCountLess(Stack* lhs, Stack* rhs);
        static bool stackPeakReferenceCountLess(Stack* lhs, Stack* rhs);
        static bool blockAgeLess(Block* lhs, Block* rhs);

        bool                m_scopedTracesEnabled;
        std::set<Block*>    m_scopedTraces;
    };

    class MemoryAllocationStateObject
    {
    public:
        MemoryAllocationStateObject();
        ~MemoryAllocationStateObject();
    };
} // namespace ITF

#endif // ITF_DEBUG_MEMORY

#endif // ITF_MEMORYDEBUGGER_H_
