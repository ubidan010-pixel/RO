#include "precompiled_core.h"

#include "MemMngTracerPlatform.h"
#include "MemMngPrivate.h"
#include "MemMngTracer.h"

namespace ITF
{

namespace MemMngDebug
{
#if MEMMNG_MEMORY_TRACER!=0

	// Init to go through stack
	void InitMemoryTracer()
	{
	}

	//-------------------------------------------------------------------------
	// This function collects the current callstack up do a maximum depth of
	// maxDepth. It allows to skip the n first functions (usually we dont
	// want to have "SStackFrameOrbis::GetFirstFrame()" and
	// "MemMngDebug::GetCallStack" in our callstack, and possibly others as
	// well.
	//-------------------------------------------------------------------------

	int GetCallStack(void** pBuffer, int maxDepth, int skip, const void* firstFrame)
	{
		int depth = skip;
        
#if MEMMNG_MEMORY_TRACER==1

        void * retAddress = nullptr;

        do
		{
			retAddress = __builtin_return_address(depth);
            if(depth < maxDepth)
			{
				pBuffer[depth] = retAddress;
				depth++;
			}
		} while(depth < maxDepth && retAddress != nullptr)
#endif
		return depth;
	}
#endif
}

//-----------------------------------------------------------------------------

#if MEMMNG_MEMORY_TRACER==1
extern MemMng::MemCounts * g_MemUsageStat;

FILE_HANDLE  MemMngTracer::m_fileHandle = (FILE_HANDLE)-1;

void MemMngTracer::Init()
{
    strncpy(m_dumpHeader.m_dumptype, "ENGINE", sizeof(m_dumpHeader.m_dumptype));
    strncpy(m_dumpHeader.m_platform, "<Platform>", sizeof(m_dumpHeader.m_platform));
    strncpy(m_dumpHeader.m_reserved, "....", sizeof(m_dumpHeader.m_reserved));

    InitCommon();
}

void MemMngTracer::Close()
{
    CloseCommon();
    cellFsClose(m_fileHandle);
}


bool MemMngTracer::ValidateFileHandle()
{
    if (m_fileHandle == (u32)-1)
    {
        return 0 <= cellFsOpen(SYS_APP_HOME "/heap_trace_0.raw", CELL_FS_O_CREAT|CELL_FS_O_TRUNC|CELL_FS_O_WRONLY, (int *)&m_fileHandle, NULL, 0);
    }

    return true;
}

u32 MemMngTracer::WriteBlock(u32 size)
{
    int compsize = MAX_TRACER_SIZE;
    if (lzo1x_1_compress(m_buffer, size, m_compBuffer, (lzo_uint*)&compsize, m_workBuffer) != LZO_E_OK)
    {
        return 0;
    }

    u32 writtenBytes = 0;
    int writesize = compsize;
    u64 nbWritten = 0;
    cellFsWrite(m_fileHandle, &writesize, u64(sizeof(writesize)), &nbWritten);
    writtenBytes += u32(nbWritten);
    cellFsWrite(m_fileHandle, m_compBuffer, u64(compsize), &nbWritten);
    writtenBytes += u32(nbWritten);
    return writtenBytes;
}

void MemMngTracer::Dump(const char* dumpname)
{
	if(m_dump)
	{
		char filename[CELL_FS_MAX_FS_PATH_LENGTH];
		sprintf(filename, SYS_APP_HOME "/%s.dump", dumpname);
        hwFile file;
        file.open(filename, ITF_FILE_ATTR_WRITE);

        //Write header
        file.write(&m_dumpHeader, sizeof(m_dumpHeader));

        // bucket
        u32 temp = g_MemUsageStat->BucketMemUsed.current;
        file.write(&temp, sizeof(temp));
        temp = g_MemUsageStat->BucketMemUsed.peak;
        file.write(&temp, sizeof(temp));

        temp = g_MemUsageStat->BucketPageMemAllocated.current;
        file.write(&temp, sizeof(temp));
        temp = g_MemUsageStat->BucketPageMemAllocated.peak;
        file.write(&temp, sizeof(temp));

        // chunk
        bool chunk = false;
        file.write(&chunk, sizeof(chunk));

        int currentorder = m_order;
        file.write(&currentorder, sizeof(currentorder));

		int modulecount = 0;
        file.write(&modulecount, sizeof(modulecount));

        WriteThreadInfo(file);

        file.close();
	}
}

void MemMngTracer::LogCallStack(void* addr, int maxDepth)
{
	int callStack[MAX_CALLSTACK_DEPTH];
	int depth = Min(MemMngDebug::GetCallStack((void**)callStack, MAX_CALLSTACK_DEPTH, 2, NULL), maxDepth);
	if(depth)
	{
		int swapdepth = depth - 1;
		WriteBuffer(&swapdepth, sizeof(swapdepth));
		WriteBuffer(callStack+1, sizeof(int)*(depth - 1));
	}
	else
	{
        printf("Error 0 depth callstack\n");
		WriteBuffer(&depth, sizeof(depth));
	}
}

#endif

}
