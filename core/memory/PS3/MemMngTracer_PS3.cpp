#include "precompiled_core.h"
#include "engine/lzo/minilzo.h"
#include "MemMngPrivate_PS3.h"
#include "MemMngTracer_PS3.h"

namespace ITF
{

#if MEMMNG_MEMORY_TRACER==1

Mutex * MemMngTracer::m_pMutex = 0;
u32   MemMngTracer::m_order = 0;
bool  MemMngTracer::m_dump = true;
u8*   MemMngTracer::m_buffer = NULL;
u8*   MemMngTracer::m_bufferptr = NULL;
u8*   MemMngTracer::m_compBuffer = NULL;
u8*   MemMngTracer::m_workBuffer = NULL;
MemMngTracer::MemDumpHeader MemMngTracer::m_dumpHeader;

bool    MemMngTracer::m_enabled = false;

void MemMngTracer::InitCommon()
{
    static Mutex mutex;
    m_pMutex = &mutex;
#if MEMMNG_MEMORY_TRACERALL==1
    Enable();
#endif
}

void MemMngTracer::CloseCommon()
{
    FlushLog(true);
}

void MemMngTracer::FlushLog(bool forceFlush)
{
    ScopeLockMutex locker(*m_pMutex);
    if(ValidateFileHandle())
    {
        if(m_bufferptr > m_buffer)
        {
            WriteBlock(m_bufferptr-m_buffer);
        }
    }
    m_bufferptr = m_buffer;
}

// the size of buffer must be smaller than MAX_TRACER_SIZE
void MemMngTracer::WriteBuffer(const void* buffer, u32 size)
{
    const u8* src = (const u8*)buffer;
    while(m_bufferptr + size > m_buffer + MAX_TRACER_SIZE)
    {
        int copy_len = m_buffer + MAX_TRACER_SIZE - m_bufferptr;
        memcpy(m_bufferptr, src, copy_len);
        if(ValidateFileHandle())
        {
            WriteBlock(MAX_TRACER_SIZE);
            m_bufferptr = m_buffer;
            src += copy_len;
            size = size - copy_len;
        }
    }

    memcpy(m_bufferptr, src, size);
    m_bufferptr += size;
}

void MemMngTracer::Dump()
{
    if(m_enabled)
    {
        ScopeLockMutex locker(*m_pMutex);
        static int dumpcount = 0;
        char dumpfile[MaxFilePath];
        sprintf(dumpfile, "MemDump%d", dumpcount++);
        Dump(dumpfile);
    }
}

void MemMngTracer::LogAlloc(void* addr, u32 asked, NMallocType mtype, u32 attrib)
{
    if(m_enabled)
    {
        SAllocTrace allocnode;
        if(mtype == NMALLOC_SIZE)
            allocnode.m_size = attrib;
        else
            allocnode.m_size = MemMng::Size(addr);

        ScopeLockMutex locker(*m_pMutex);
        allocnode.m_type = TRACETYPE_ALLOC;
        allocnode.m_type |= (mtype<<8);

        allocnode.m_addr = (u32)addr;
        allocnode.m_asked = asked;

        allocnode.m_order = m_order++;
        allocnode.m_threadId = Synchronize::getCurrentThreadId();
        WriteBuffer(&allocnode, sizeof(allocnode));

        LogCallStack(addr, 255);
    }
}

void MemMngTracer::LogFree(void* addr, u32 size)
{
    if(m_enabled)
    {
        ScopeLockMutex locker(*m_pMutex);
        SAllocTrace allocnode;
        allocnode.m_type = TRACETYPE_FREE;
        allocnode.m_addr = (u32)addr;
        allocnode.m_asked = 0;
	    allocnode.m_size = size;
        allocnode.m_order = m_order++;
        allocnode.m_threadId = Synchronize::getCurrentThreadId();
        WriteBuffer(&allocnode, sizeof(allocnode));
    }
}

void MemMngTracer::EnableMemDump()
{ 
    m_dump = true;
}

void MemMngTracer::Enable()
{
    if(!m_buffer)
    {
        m_enabled = false; // be sure to avoid reentrancy
        m_workBuffer = (u8*)MemMng::Malloc(LZO1X_MEM_COMPRESS * sizeof(lzo_align_t));
        m_buffer = (u8*)MemMng::Malloc(MAX_TRACER_SIZE);
        m_compBuffer = (u8*)MemMng::Malloc(LZO1X_MEM_COMPRESS_MAX_EXPANSION(MAX_TRACER_SIZE));
        
        m_bufferptr = m_buffer;
    }

    m_enabled = true;
}

void MemMngTracer::UpdateFrame()
{
    if(m_enabled)
    {
        SAllocTrace allocnode;

        ScopeLockMutex locker(*m_pMutex);
        allocnode.m_type = TRACETYPE_FRAME;
        allocnode.m_size = 0;   // use 0 to indicate it is end.
        allocnode.m_addr = 0;
        allocnode.m_asked = 0;
        allocnode.m_order = m_order;
        allocnode.m_threadId = Synchronize::getCurrentThreadId();
        WriteBuffer(&allocnode, sizeof(allocnode));
    }
}

u32 MemMngTracer::WriteThreadInfo(File & _file)
{
#if 0
    u32 writtenBytes = 0;
    u32 threadnb = 0;
    {
        u32 threadID;
        while( Gear::ThreadRegistry::Instance().Enumerate(threadnb,threadID) )
        {
            threadnb++;
        }

        u32 threadCount = threadnb;
        _file.write(&threadCount, sizeof(threadCount));
    }

    for(u32 i=0; i<threadnb; i++)
    {
        u32 threadID;
        Gear::ThreadRegistry::Instance().Enumerate(i,threadID);

        _file.write(&threadID, sizeof(u32));

        Gear::Thread* thread = Gear::ThreadRegistry::Instance().GetThreadByID(threadID);
        const char* threadName = "unknown";

        if (thread)
        {
            threadName = thread->GetThreadName();
        }

        u32 length = strlen(threadName);
        u32 swappedLength = length;
        SwapByte(swappedLength);
        CFileSystem::WriteFile(handle, &swappedLength, sizeof(u32), &writtenBytes);
        CFileSystem::WriteFile(handle, threadName, length, &writtenBytes);
    }

    return writtenBytes;
#else
    return 0;
#endif
}

#else

void MemMngTracer::LogAlloc(void*, u32 asked, NMallocType mtype, u32 attrib)
{
}

void MemMngTracer::LogFree(void*, u32 size)
{
}

void MemMngTracer::Close()
{
}

void MemMngTracer::Dump()
{
}

void MemMngTracer::Init()
{
}

void MemMngTracer::EnableMemDump()
{
}

void MemMngTracer::Dump(const char*)
{
}

void MemMngTracer::Enable() { }
void MemMngTracer::UpdateFrame() {}
#endif

} // namespace ITF
