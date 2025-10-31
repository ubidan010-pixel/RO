#include "precompiled_core.h"

#ifndef _ITF_MEMORY_H_
#include "core/memory/memory.h"
#endif //_ITF_MEMORY_H_

#ifndef ITF_MEMORYDEBUGGER_H_
    #include "core/memory/MemoryDebugger.h"
#endif //ITF_MEMORYDEBUGGER_H_

#ifdef ITF_DEBUG_MEMORY

namespace ITF
{

    MemoryDebugger* MemoryDebugger::m_singleton = NULL;
    bool MemoryDebugger::m_singletonLocked = false;
    int MemoryDebugger::m_isInsideAllocationCounter = 0;
    int MemoryDebugger::m_currentBlockCount = 0;

    void MemoryDebugger::initialize()
    {
        if (!m_singleton && !m_singletonLocked)
        {
            m_singletonLocked = true;
            m_isInsideAllocationCounter = 1;
            m_currentBlockCount = 0;
            m_singleton = new MemoryDebugger;
            m_singletonLocked = false;
            m_singleton->m_scopedTracesEnabled = false;
        }
    }

    void MemoryDebugger::shutdown()
    {
        m_singletonLocked = true;
        MemoryDebugger* debbuger = m_singleton;
        m_singleton = NULL;
        delete debbuger;
        m_singletonLocked = false;
    }

    MemoryDebugger::MemoryDebugger() :
        m_stackPools(NULL),
        m_freeStacks(NULL),
        m_locked(true),
        m_blockPools(NULL),
        m_freeBlocks(NULL),
        m_maxMemoryUsage(0),
        m_maxMemoryUsageBlockCount(0),
        m_currentMemoryUsage(0),
        m_maxBlockCount(0),
        m_totalBlockCount(0),
        m_stackWalker()
    {        
        memset(m_stackMap, 0, STACK_MAP_SIZE * sizeof(Stack*));
        memset(m_blockMap, 0, BLOCK_MAP_SIZE * sizeof(Block*));
        m_stackWalker.initialize();
        m_locked = false;
    }
    
    MemoryDebugger::~MemoryDebugger()
    {
        m_locked = true;

        // delete all memory block pools
        BlockPool* blockPool = m_blockPools;
        while (blockPool)
        {
            BlockPool* next = blockPool->m_next;
            delete [] blockPool->m_blocks;
            delete blockPool;
            blockPool = next;
        }

        StackPool* stackPool = m_stackPools;
        while (stackPool)
        {
            StackPool* next = stackPool->m_next;
            delete [] stackPool->m_stacks;
            delete stackPool;
            stackPool = next;            
        }

        m_locked = false;
    }

    void MemoryDebugger::addBlock(void* ptr, size_t size, size_t align)
    {
        initialize();
        if (!m_singleton || m_singleton->m_locked)
        {
            return;
        }

        if (!m_singleton->m_freeBlocks)
        {
            m_singleton->createBlockPool(1024);
        }

        // assign memory block information
        Block* b = m_singleton->m_freeBlocks;
        m_singleton->m_freeBlocks = m_singleton->m_freeBlocks->m_next;
        b->m_pointer = ptr;
        b->m_size = size;
        b->m_alignement = align;
        b->m_id = m_singleton->m_totalBlockCount;
        b->m_allocatorSetupVersion = Memory::getAllocatorSetupVersion();
        b->m_stack = m_singleton->retrieveStack();
        if (b->m_stack)
        {
            b->m_stack->m_totalReferenceCount++;
            b->m_stack->m_referenceCount++;
            if (b->m_stack->m_referenceCount > b->m_stack->m_maxReferenceCount)
            {
                b->m_stack->m_maxReferenceCount = b->m_stack->m_referenceCount;
            }
        }

        // add block to map
        int h = hashPointer(ptr) % BLOCK_MAP_SIZE;
        b->m_next = m_singleton->m_blockMap[h];
        m_singleton->m_blockMap[h] = b;

        // log memory statistics
        m_singleton->m_totalBlockCount++;
        m_currentBlockCount++;
        if (m_singleton->m_maxBlockCount < m_currentBlockCount)
        {
            m_singleton->m_maxBlockCount = m_currentBlockCount;
        }

        m_singleton->m_currentMemoryUsage += size;
        if (m_singleton->m_maxMemoryUsage < m_singleton->m_currentMemoryUsage)
        {
            m_singleton->m_maxMemoryUsage = m_singleton->m_currentMemoryUsage;
            m_singleton->m_maxMemoryUsageBlockCount = m_currentBlockCount; 
        }

        if(m_singleton->m_scopedTracesEnabled)
        {
            m_singleton->m_scopedTraces.insert(b);
        }
    }

    void MemoryDebugger::removeBlock(void* ptr, bool expectedAlign)
    {
        if (!m_singleton || !ptr || m_singleton->m_locked)
        {
            return;
        }

        int h = hashPointer(ptr) % BLOCK_MAP_SIZE;

        Block* p = NULL;
        Block* b = m_singleton->m_blockMap[h];

        if(m_singleton->m_scopedTracesEnabled)
        {
            STD_SET<Block*>::iterator it = m_singleton->m_scopedTraces.find(b);
            if(it != m_singleton->m_scopedTraces.end())
            {
                m_singleton->m_scopedTraces.erase(it);
            }
        }

        while (b)
        {
            if (b->m_pointer == ptr)
            {
                if (expectedAlign && b->m_alignement == 0)
                {
                    ITF_ASSERT(!"You are using unaligned free on an aligned memory!");
                }
                else if (!expectedAlign && b->m_alignement != 0)
                {
                    ITF_ASSERT(!"You are using aligned free on an unaligned memory!");
                }

                if (b->m_allocatorSetupVersion != Memory::getAllocatorSetupVersion())
                {
                    ITF_ASSERT(!"You are trying to delete memory allocated with a different allocator!");
                }

                if (p)
                {
                    p->m_next = b->m_next;
                }
                else
                {
                    m_singleton->m_blockMap[h] = b->m_next;
                }
                
                if (b->m_stack)
                {
                    b->m_stack->m_referenceCount--;
                }

                b->m_next = m_singleton->m_freeBlocks;
                m_singleton->m_freeBlocks = b;
                m_singleton->m_currentMemoryUsage -= b->m_size;
                m_currentBlockCount--;
                return;
            }
            p = b;
            b = b->m_next;
        }

        ITF_ASSERT(!"Memory block not found");
    }

    int MemoryDebugger::getNumberOfAllocatedBlocks()
    {
        return m_currentBlockCount;
    }

    void MemoryDebugger::reportLeaks()
    {
        if (!m_singleton)
        {
            return;
        }

        m_singleton->m_locked = true;
        OutputDebugger d;

        d << "\n";
        d << "***********************************************\n";
        d << "***********************************************\n";
        d << "     ITF - Memory Debugger\n";
        d << "***********************************************\n";
        d << "***********************************************\n";
        d << "\n";

        static const int sSIZE = 255;
        char msg[sSIZE];
        sprintf_s(msg, sSIZE, "%i allocations done. %.1f Kb peak in %i blocks. %i blocks peak.\n", 
            m_singleton->m_totalBlockCount,
            m_singleton->m_maxMemoryUsage / 1024.0f,
            m_singleton->m_maxMemoryUsageBlockCount,
            m_singleton->m_maxBlockCount
            );
        d << msg;

        if (m_currentBlockCount == 0)
        {
            d <<"No leaks\n";
        }
        else
        {
            sprintf_s(msg, sSIZE, "Found %i memory leaks\n", m_currentBlockCount);
            d << msg;

            m_singleton->showLeaks();
        }

        m_singleton->showPopularAllocations(5);

        d << "\n";
        d << "***********************************************\n";
        d << "***********************************************\n";
        d << "\n";

        m_singleton->m_locked = false;

    }

    void MemoryDebugger::showBlockTrace(Block* b)
    {
        OutputDebugger d;

        char buf[1024];
        sprintf_s(buf, 1024, "ID: %i   Pointer: %08X   Size: %ib   Alignment: %i\n",
            b->m_id, reinterpret_cast<unsigned int>(b->m_pointer), b->m_size, b->m_alignement);
        d << buf;

        showStackTrace("   ", b->m_stack);
    }

    void MemoryDebugger::showStackTrace(const char* header, Stack* s)
    {
        if (s)
        {
            OutputDebugger d;
            char buf[1024];
            ProcessAddressDescriptor desc;
            for(u16 i = 0; i < s->m_stackDepth; i++)
            {        
                m_stackWalker.fillDescriptor(s->m_addresses[i], desc);
                if (desc.m_line == 0)
                    break;

                sprintf_s(buf, 1024, "%s%s(%u): %s\n", header, desc.m_filename, 
                    (int)desc.m_line, desc.m_symbolName);
                d << buf;
            }
        }
    }

    bool MemoryDebugger::isInsideAllocation()
    {
        return m_isInsideAllocationCounter != 0;
    }

    void MemoryDebugger::setAllocationState(bool isInside)
    {
        if (isInside)
        {
            m_isInsideAllocationCounter++;
        }
        else
        {
            ITF_ASSERT(m_isInsideAllocationCounter > 0);
            m_isInsideAllocationCounter--;
        }
    }

    void MemoryDebugger::createStackPool(size_t size)
    {
        m_locked = true;

        // create a new pool with size number of blocks.
        m_stackPools = new StackPool;
        m_stackPools->m_next = NULL;
        m_stackPools->m_stacks = new Stack[size];

        // initialize all blocks and add them to the free list
        unsigned int last = size-1;
        for (unsigned int i = 0; i < last; i++)
        {
            m_stackPools->m_stacks[i].m_next = &m_stackPools->m_stacks[i + 1];
        }
        m_stackPools->m_stacks[last].m_next = m_freeStacks;
        m_freeStacks = m_stackPools->m_stacks;

        m_locked = false;
    }

    void MemoryDebugger::createBlockPool(size_t size)
    {
        m_locked = true;

        // create a new pool with size number of blocks.
        m_blockPools = new BlockPool;
        m_blockPools->m_next = NULL;
        m_blockPools->m_blocks = new Block[size];

        // initialize all blocks and add them to the free list
        unsigned int last = size-1;
        for (unsigned int i = 0; i < last; i++)
        {
            m_blockPools->m_blocks[i].m_next = &m_blockPools->m_blocks[i + 1];
        }
        m_blockPools->m_blocks[last].m_next = m_freeBlocks;
        m_freeBlocks = m_blockPools->m_blocks;

        m_locked = false;
    }
              
    MemoryDebugger::Stack* MemoryDebugger::retrieveStack()
    {
        if (!m_freeStacks)
        {
            createStackPool(256);
        }

        m_freeStacks->m_stackDepth = (u16)m_stackWalker.retrieveStack(
            m_freeStacks->m_addresses, Stack::MAX_STACK_DEPTH, 4);
       
        if (m_freeStacks->m_stackDepth > 3)
        {
            int h = hashAddress(m_freeStacks->m_addresses) % STACK_MAP_SIZE;
            Stack* res = NULL;
            Stack* stack = m_stackMap[h];
            while (stack)
            {
                if (m_freeStacks->m_stackDepth == stack->m_stackDepth &&
                    memcmp(m_freeStacks->m_addresses, stack->m_addresses, 
                        m_freeStacks->m_stackDepth * 
                        sizeof(StackWalker::ProcessAddress)) == 0)
                {
                    res = stack;
                    break;
                }               
                stack = stack->m_next;
            }

            if (!res)
            {
                res = m_freeStacks;
                m_freeStacks = m_freeStacks->m_next;

                res->m_referenceCount = 0;
                res->m_maxReferenceCount = 0;
                res->m_totalReferenceCount = 0;

                res->m_next = m_stackMap[h];
                m_stackMap[h] = res;
            }
            return res;

        }
        return NULL;
    }

    unsigned int MemoryDebugger::hashAddress(StackWalker::ProcessAddress* addr)
    {
        u64 hash = addr[0] ^ addr[1];
        return (unsigned int)(hash >> 32 ^ hash);
    }

    unsigned int MemoryDebugger::hashPointer(void* ptr)
    {
        return (unsigned int)((u32)ptr >> 16 ^ 
            (u32)ptr);
    }

    void MemoryDebugger::showLeaks()
    {
        OutputDebugger d;

        // first dump hash map into a ITF_VECTOR
        ITF_VECTOR<Block*> array;
        array.reserve(m_currentBlockCount);
        for (unsigned int i = 0; i < BLOCK_MAP_SIZE; i++)
        {
            Block* b = m_singleton->m_blockMap[i];
            while (b)
            {
                array.push_back(b);
                b = b->m_next;
            }
        }

        std::sort(array.begin(), array.end(), blockAgeLess);

        char buf[255];
        for (unsigned int i = 0; i < array.size(); i++)
        {
            sprintf_s(buf, 255, "\n\nLeak #%i:\n", i+1);
            d << buf;
            m_singleton->showBlockTrace(array[i]);
        }
    }

    void MemoryDebugger::showPopularAllocations(unsigned int maxToShow)
    {
        // first dump hash map into a ITF_VECTOR
        ITF_VECTOR<Stack*> array;
        array.reserve(1024);
        for (unsigned int i = 0; i < STACK_MAP_SIZE; i++)
        {
            Stack* s = m_singleton->m_stackMap[i];
            while (s)
            {
                array.push_back(s);
                s = s->m_next;
            }
        }


        char buf[1024];
        OutputDebugger d;

        unsigned int count = array.size() > maxToShow ? maxToShow : array.size();

        std::sort(array.begin(), array.end(), stackTotalReferenceCountLess);
        std::reverse(array.begin(), array.end());
        d << "\n***********************************************\n";
        d << "\n\nMost performed allocations:\n";
        d << "***********************************************\n";
        for (unsigned int i = 0; i < count; i++)
        {
            sprintf_s(buf, 1024, "\n\n#%i   Total: %i   Peak: %i\n",
                i+1, array[i]->m_totalReferenceCount, array[i]->m_maxReferenceCount);
            d << buf;
            showStackTrace("   ", array[i]);
        }

        std::sort(array.begin(), array.end(), stackPeakReferenceCountLess);
        std::reverse(array.begin(), array.end());
        d << "\n***********************************************\n";
        d << "\n\nMost accumulated allocations:\n";
        d << "***********************************************\n";
        for (unsigned int i = 0; i < count; i++)
        {
            sprintf_s(buf, 1024, "\n\n#%i   Total: %i   Peak: %i\n",
                i+1, array[i]->m_totalReferenceCount, array[i]->m_maxReferenceCount);
            d << buf;
            showStackTrace("   ", array[i]);
        }
    }

    bool MemoryDebugger::stackTotalReferenceCountLess(Stack* lhs, Stack* rhs)
    {
        return lhs->m_totalReferenceCount < rhs->m_totalReferenceCount;
    }

    bool MemoryDebugger::stackPeakReferenceCountLess(Stack* lhs, Stack* rhs)
    {
        return lhs->m_maxReferenceCount < rhs->m_maxReferenceCount;
    }

    bool MemoryDebugger::blockAgeLess(Block* lhs, Block* rhs)
    {
        return lhs->m_id < rhs->m_id;
    }

    MemoryAllocationStateObject::MemoryAllocationStateObject()
    {
        MemoryDebugger::setAllocationState(true);
    }

    MemoryAllocationStateObject::~MemoryAllocationStateObject()
    {
        MemoryDebugger::setAllocationState(false);
    }

    void MemoryDebugger::startScopedTraces()
    {
        m_singleton->m_scopedTraces.clear();
        m_singleton->m_scopedTracesEnabled = true;
    }

    void MemoryDebugger::stopScopedTraces()
    {
        m_singleton->m_scopedTracesEnabled = false;
    }

    unsigned int MemoryDebugger::getScopedTracesCount()
    {
        return m_singleton->m_scopedTraces.size();
    }

    void MemoryDebugger::reportScopedTraces()
    {
        OutputDebugger d;
 
        d << "\n";
        d << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
        d << "               Scoped traces\n";
        d << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
        d << "\n";


        char buf[255];
        std::set<Block*>::iterator it = m_singleton->m_scopedTraces.begin(), itEnd = m_singleton->m_scopedTraces.end();

        int i = 0;
        while(it != itEnd)
        {
            sprintf_s(buf, 255, "\n\nScoped leak #%i:\n", i+1);
            d << buf;
            m_singleton->showBlockTrace(*it);
            ++i;
            it++;
        }

        d << "\n";
        d << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
        d << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
        d << "\n";
    }
}

#endif // ITF_DEBUG_MEMORY
