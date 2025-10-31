#ifndef ITF_CORE_MEMMNGTRACER_H_
#define ITF_CORE_MEMMNGTRACER_H_

#pragma once

#include "MemMngPrivate.h"
#include "Core/File/File.h"


namespace ITF
{

const u32 MAX_CALLSTACK_DEPTH = 256;
const u32 MAX_TRACER_SIZE = 65536;

struct SCallStack
{
    int m_callStackIdx;
    int m_callStack[MAX_CALLSTACK_DEPTH];
};

enum
{
    TRACETYPE_ALLOC = 0,
    TRACETYPE_FREE  = 1,
    TRACETYPE_FRAME = 2,
};

enum NMallocType
{
    NMALLOC_BUCKET,
    NMALLOC_STD,
    NMALLOC_BIG,
    NMALLOC_SIZE
};

struct SAllocTrace
{
    u32   m_threadId;
    u32   m_order;
    u32   m_addr;
    u32   m_type;
    u32   m_asked;
    u32   m_size;
};

const u32 MaxFilePath = 260;

// Dump module info.
struct SModuleEntry
{
    int baseAddress;
    int size;
    char modulePath[MaxFilePath];
};

class MemMngTracerGroup
{
public:
    MemMngTracerGroup(const char* groupId);
    ~MemMngTracerGroup();

    char    m_groupId[MaxFilePath];
};

class MemMngTracer
{
public:
    struct MemDumpHeader // 64 Bytes
    {
        char m_platform[8];
        char m_dumptype[8];
        char m_reserved[48];
    };

    static Mutex      * m_pMutex;
    static u32        m_order;
    static FILE_HANDLE  m_fileHandle;
    static bool       m_dump;

    static u8*        m_buffer;
    static u8*        m_bufferptr;

    static u8*        m_compBuffer;
    static u8*        m_workBuffer;

    static bool         m_enabled;
    static MemDumpHeader m_dumpHeader;
    static void Init();
    static void InitCommon();
    static void Close();
    static void CloseCommon();
    static void Enable();
    static void EnableMemDump();
    static void Dump();
    static void Dump(const char*);
    static void LogAlloc(void*, u32 asked, NMallocType mtype, u32 attrib);
    static void LogCallStack(void*, int maxDepth);
    static void LogFree(void*, u32 size);
    static void FlushLog(bool forceFlush = false);

    static bool   ValidateFileHandle();
    static void   WriteBuffer(const void* buffer, u32 size);
    static u32    WriteBlock(u32 size);
    static u32    WriteThreadInfo(File & _file);

    static void UpdateFrame();
};

} // namespace ITF

#if defined(MEMMNG_MEMORY_TRACER) && MEMMNG_MEMORY_TRACER==0
    #define MEMMNG_TRACER_ASKEDSIZE(x)
    #define MEMMNG_TRACER_ALLOC(x, flag)
    #define MEMMNG_TRACER_ALLOC_SIZE(x, size)
    #define MEMMNG_TRACER_FREE(x, size)
    #define MEMMNG_TRACER_INIT
    #define MEMMNG_TRACER_CLOSE
    #define MEMMNG_TRACER_ENABLEDUMP
    #define MEMMNG_TRACER_DUMP(x)
    #define MEMMNG_TRACER_DUMPS()
    #define MEMMNG_TRACER_UPDATE()
#else
    #define MEMMNG_TRACER_ASKEDSIZE(x) u32 memtracer_askedsize_ = x;
    #define MEMMNG_TRACER_ALLOC(x, flag) MemMngTracer::LogAlloc(x, memtracer_askedsize_, flag, 0)
    #define MEMMNG_TRACER_ALLOC_SIZE(x, size) MemMngTracer::LogAlloc(x, memtracer_askedsize_, NMALLOC_SIZE, size)
    #define MEMMNG_TRACER_FREE(x, size) MemMngTracer::LogFree(x, size)
    #define MEMMNG_TRACER_INIT MemMngTracer::Init()
    #define MEMMNG_TRACER_CLOSE MemMngTracer::Close()
    #define MEMMNG_TRACER_ENABLEDUMP MemMngTracer::EnableMemDump()
    #define MEMMNG_TRACER_DUMP(x)  MemMngTracer::Dump(x)
    #define MEMMNG_TRACER_DUMPS()  MemMngTracer::Dump()
    #define MEMMNG_TRACER_UPDATE() MemMngTracer::UpdateFrame();
#endif


#endif // ITF_CORE_MEMMNGTRACER_H_
